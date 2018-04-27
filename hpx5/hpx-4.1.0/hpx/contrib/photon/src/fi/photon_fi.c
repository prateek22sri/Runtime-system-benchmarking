// =============================================================================
//  Photon RDMA Library (libphoton)
//
//  Copyright (c) 2016, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#define _GNU_SOURCE
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <limits.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include <rdma/fi_rma.h>

#include "photon_backend.h"
#include "photon_buffer.h"
#include "photon_exchange.h"
#include "photon_event.h"

#include "photon_fi.h"
#include "photon_fi_connect.h"

#include "logging.h"
#include "libsync/locks.h"

#include "photon_buffertable.h"

#define MAX_RETRIES 1

static int __initialized = 0;

static int fi_initialized(void);
static int fi_init(photonConfig cfg, ProcessInfo *photon_processes, photonBI ss);
static int fi_finalize(void);
static int fi_get_info(ProcessInfo *pi, int proc, void **info, int *size, photon_info_t type);
static int fi_set_info(ProcessInfo *pi, int proc, void *info, int size, photon_info_t type);
static int fi_rdma_put(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
		       photonBuffer lbuf, photonBuffer rbuf, uint64_t id, uint64_t imm_data,
		       int flags);
static int fi_rdma_get(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
		       photonBuffer lbuf, photonBuffer rbuf, uint64_t id, int flags);
static int fi_rdma_send(photonAddr addr, uintptr_t laddr, uint64_t size,
			photonBuffer lbuf, uint64_t id, uint64_t imm_data, int flags);
static int fi_rdma_recv(photonAddr addr, uintptr_t laddr, uint64_t size,
			photonBuffer lbuf, uint64_t id, int flags);
static int pfi_tx_size_left(int proc);
static int pfi_rx_size_left(int proc);
static int fi_get_event(int proc, int max, photon_rid *ids, int *n);
static int fi_get_revent(int proc, int max, photon_rid *ids, uint64_t *imms, int *n);

static fi_cnct_ctx fi_ctx = {
  .thread_safe = 0,
  .node = NULL,
  .service = NULL,
  .domain = NULL,
  .provider = NULL,
  .num_cq = 1,
};

// we are now a Photon backend
static struct photon_backend_attr_t fi_attr = {
  .rdma_put_align = PHOTON_FI_PUT_ALIGN,
  .rdma_get_align = PHOTON_FI_GET_ALIGN,
  .comp_order     = PHOTON_FI_COMP_ORDER
};

struct photon_backend_t photon_fi_backend = {
  .attr = &fi_attr,
  .context = &fi_ctx,
  .initialized = fi_initialized,
  .init = fi_init,
  .finalize = fi_finalize,
  .connect = NULL,
  .get_info = fi_get_info,
  .set_info = fi_set_info,
  .get_dev_addr = NULL,
  .get_dev_name = NULL,
  .register_addr = NULL,
  .unregister_addr = NULL,
  .rdma_put = fi_rdma_put,
  .rdma_get = fi_rdma_get,
  .rdma_send = fi_rdma_send,
  .rdma_recv = fi_rdma_recv,
  .tx_size_left = pfi_tx_size_left,
  .rx_size_left = pfi_rx_size_left,
  .get_event = fi_get_event,
  .get_revent = fi_get_revent
};

static void cq_readerr(struct fid_cq *cq, char *cq_str)
{ 
  struct fi_cq_err_entry cq_err;
  const char *err_str;
  int ret;
  
  ret = fi_cq_readerr(cq, &cq_err, 0);
  if (ret < 0) {
    dbg_err("Could not read error from CQ");
  } else {
    err_str = fi_cq_strerror(cq, cq_err.prov_errno, cq_err.err_data, NULL, 0);
    log_err("%s: %d %s\n", cq_str, cq_err.err, fi_strerror(cq_err.err));
    log_err("%s: prov_err: %s (%d)\n", cq_str, err_str, cq_err.prov_errno);
  }
}

static int fi_initialized() {
  if (__initialized == 1)
    return PHOTON_OK;
  else
    return PHOTON_ERROR_NOINIT;
}

static int fi_init(photonConfig cfg, ProcessInfo *photon_processes, photonBI ss) {

  __initialized = -1;

  //if (_photon_myrank == 1)
  //  photon_dbg_wait();

  sync_tatas_init(&fi_ctx.lock);
  
  fi_ctx.num_cq  = cfg->cap.num_cq;
  fi_ctx.use_rcq = cfg->cap.use_rcq;
  fi_ctx.eth_dev = cfg->fi.eth_dev;
  fi_ctx.node    = NULL;
  fi_ctx.service = NULL;
  fi_ctx.domain  = NULL;

  if (cfg->attr.comp_order == PHOTON_ORDER_DEFAULT) {
    fi_attr.comp_order = PHOTON_FI_DEF_ORDER;
  }
  else  if (cfg->attr.comp_order & PHOTON_FI_COMP_ORDER) {
    fi_attr.comp_order = cfg->attr.comp_order;
  }
  else {
    dbg_err("specified completion order not supported");
    goto error_exit;
  }

  if (fi_ctx.eth_dev) {
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
      dbg_err("Cannot get interface addrs");
      goto error_exit;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr == NULL)
	continue;
      
      if (!strcmp(ifa->ifa_name, fi_ctx.eth_dev) &&
	  ifa->ifa_addr->sa_family == AF_INET) {
	
	fi_ctx.node = inet_ntoa(((struct sockaddr_in*)(ifa->ifa_addr))->sin_addr);
	dbg_info("Found matching ethernet device: %s (%s)",
		 ifa->ifa_name, fi_ctx.node);
	break;
      }
    }
  }

  fi_ctx.hints = fi_allocinfo();
  if (!fi_ctx.hints) {
    log_err("Could not allocate space for fi hints");
    goto error_exit;
  }

  fi_ctx.hints->fabric_attr->prov_name = strdup(cfg->fi.provider);
  fi_ctx.hints->domain_attr->mr_mode = FI_MR_BASIC;
  //fi_ctx.hints->domain_attr->threading = FI_THREAD_SAFE;
  //fi_ctx.hints->rx_attr->comp_order = FI_ORDER_STRICT | FI_ORDER_DATA;
  fi_ctx.hints->ep_attr->type = FI_EP_RDM;
  fi_ctx.hints->caps = FI_MSG | FI_RMA | FI_RMA_EVENT;
  fi_ctx.hints->mode = FI_CONTEXT | FI_LOCAL_MR | FI_RX_CQ_DATA;

  if(__fi_init_context(&fi_ctx)) {
    log_err("Could not initialize libfabric context");
    goto error_exit;
  }

  if (photon_buffer_register(ss, &fi_ctx, BUFFER_FLAG_NOTIFY) != 0) {
    log_err("Could not register local buffer for the ledger entries");
    goto error_exit;
  }

  buffertable_insert(ss);

  if (photon_exchange_ledgers(photon_processes, LEDGER_ALL) != PHOTON_OK) {
    log_err("Could not exchange ledgers");
    goto error_exit;
  }

  fi_freeinfo(fi_ctx.hints);

  __initialized = 1;

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

static int fi_finalize() {
  return PHOTON_OK;
}

static int fi_get_info(ProcessInfo *pi, int proc, void **ret_info, int *ret_size, photon_info_t type) {
  switch (type) {
  case PHOTON_GET_ALIGN:
    {
      *ret_info = &fi_attr.rdma_get_align;
      *ret_size = sizeof(fi_attr.rdma_get_align);
    }
    break;
  case PHOTON_PUT_ALIGN:
    {
      *ret_info = &fi_attr.rdma_put_align;
      *ret_size = sizeof(fi_attr.rdma_put_align);
    }
    break;
  case PHOTON_COMP_ORDER:
    {
      *ret_info = &fi_attr.comp_order;
      *ret_size = sizeof(fi_attr.comp_order);
    }
    break;
  default:
    goto error_exit;
    break;
  }

  return PHOTON_OK;
  
 error_exit:
  return PHOTON_ERROR;
}

static int fi_set_info(ProcessInfo *pi, int proc, void *info, int size, photon_info_t type) {
  return PHOTON_OK;
}

static int fi_rdma_put(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
		       photonBuffer lbuf, photonBuffer rbuf, uint64_t id,
		       uint64_t imm_data, int flags) {
  uint64_t *lid = malloc(sizeof(uint64_t));
  int rc;

  dbg_trace("(size=%lu, id=0x%016lx, raddr: %p)", size, id, raddr);

  memcpy(lid, &id, sizeof(*lid));

  (!fi_ctx.thread_safe) ? sync_tatas_acquire(&fi_ctx.lock):NULL;
  {  
    if (fi_ctx.use_rcq && (flags & RDMA_FLAG_WITH_IMM)) {
      rc = fi_writedata(fi_ctx.eps[_photon_myrank], (void*)laddr, size,
			NULL, imm_data, fi_ctx.addrs[proc],
			raddr, rbuf->priv.key1, lid);
    }
    else {
      rc = fi_write(fi_ctx.eps[_photon_myrank], (void*)laddr, size,
		    NULL, fi_ctx.addrs[proc], raddr,
		    rbuf->priv.key1, lid);
    }
    if (rc) {
      dbg_err("Could not PUT to %p, size %lu: %s", (void*)raddr, size,
	      fi_strerror(-rc));
      goto error_exit;
    }
  }
  (!fi_ctx.thread_safe) ? sync_tatas_release(&fi_ctx.lock):NULL;
  
  return PHOTON_OK;
  
 error_exit:
  return PHOTON_ERROR;
}

static int fi_rdma_get(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
		       photonBuffer lbuf, photonBuffer rbuf, uint64_t id, int flags) {
  uint64_t *lid = malloc(sizeof(uint64_t));
  int rc;

  dbg_trace("(size=%lu, id=0x%016lx, raddr: %p)", size, id, raddr);

  memcpy(lid, &id, sizeof(*lid));

  (!fi_ctx.thread_safe) ? sync_tatas_acquire(&fi_ctx.lock):NULL;
  {  
    rc = fi_read(fi_ctx.eps[_photon_myrank], (void*)laddr, size,
		 NULL, fi_ctx.addrs[proc], raddr,
		 rbuf->priv.key1, lid);
    if (rc) {
      dbg_err("Could not GET from %p, size %lu: %s", (void*)raddr, size,
	      fi_strerror(-rc));
      goto error_exit;
    }
  }
  (!fi_ctx.thread_safe) ? sync_tatas_release(&fi_ctx.lock):NULL;


  return PHOTON_OK;
  
 error_exit:
  return PHOTON_ERROR;
}

static int fi_rdma_send(photonAddr addr, uintptr_t laddr, uint64_t size,
			photonBuffer lbuf, uint64_t id, uint64_t imm_data, int flags) {
  return PHOTON_OK;
}

static int fi_rdma_recv(photonAddr addr, uintptr_t laddr, uint64_t size,
			photonBuffer lbuf, uint64_t id, int flags) {
  return PHOTON_OK;
}

static int pfi_tx_size_left(int proc) {
  int c;
  (!fi_ctx.thread_safe) ? sync_tatas_acquire(&fi_ctx.lock):NULL;
  {
    c = (int)fi_tx_size_left(fi_ctx.eps[_photon_myrank]);
  }
  (!fi_ctx.thread_safe) ? sync_tatas_release(&fi_ctx.lock):NULL;
  return c;
}

static int pfi_rx_size_left(int proc) {
  int c;
  (!fi_ctx.thread_safe) ? sync_tatas_acquire(&fi_ctx.lock):NULL;
  {
    c = (int)fi_rx_size_left(fi_ctx.eps[_photon_myrank]);
  }
  (!fi_ctx.thread_safe) ? sync_tatas_release(&fi_ctx.lock):NULL;
  return c;
}

static int fi_get_event(int proc, int max, photon_rid *ids, int *n) {
  int i, j, ne, comp;
  int start, end;
  int retries;
  struct fi_cq_data_entry entries[MAX_CQ_POLL];
  
  *n = 0;
  comp = 0;

  if (fi_ctx.num_cq == 1) {
    start = 0;
    end = 1;
  }
  else if (proc == PHOTON_ANY_SOURCE) {
    start = 0;
    end = fi_ctx.num_cq;
  }
  else {
    start = PHOTON_GET_CQ_IND(fi_ctx.num_cq, proc);
    end = start+1;
  }

  (!fi_ctx.thread_safe) ? sync_tatas_acquire(&fi_ctx.lock):NULL;
  {
    for (i=start; i<end && comp<max; i++) {
      retries = MAX_RETRIES;
      do {
	ne = fi_cq_read(fi_ctx.lcq[i], entries, max);
	if (ne < 0) {
	  if (ne == -FI_EAGAIN) {
	    ne = 0;
	    continue;
	  }
	  else if (ne == -FI_EAVAIL) {
	    cq_readerr(fi_ctx.lcq[i], "local CQ");
	    goto error_exit;
	  }
	  else {
	    log_err("fi_cq_read() failed: %s", fi_strerror(-ne));
	    goto error_exit;
	  }
	}
      }
      while ((ne < 1) && --retries);
      
      for (j=0; j<ne && j<MAX_CQ_POLL; j++) {
	ids[j+comp] = *((uint64_t*)entries[j].op_context);
	free(entries[j].op_context);
      }
      comp += ne;
    }
    
    *n = comp;
  }
  (!fi_ctx.thread_safe) ? sync_tatas_release(&fi_ctx.lock):NULL;

  // CQs are empty
  if (comp == 0) {
    return PHOTON_EVENT_NONE;
  }
  
  return PHOTON_EVENT_OK;
  
error_exit:
  return PHOTON_EVENT_ERROR;
}

static int fi_get_revent(int proc, int max, photon_rid *ids, uint64_t *imms, int *n) {
  int i, j, ne, comp;
  int start, end;
  int retries;
  struct fi_cq_data_entry entries[MAX_CQ_POLL];

  if (!fi_ctx.use_rcq) {
    return PHOTON_EVENT_NOTIMPL;
  }
  
  *n = 0;
  comp = 0;

  if (fi_ctx.num_cq == 1) {
    start = 0;
    end = 1;
  }
  else if (proc == PHOTON_ANY_SOURCE) {
    start = 0;
    end = fi_ctx.num_cq;
  }
  else {
    start = PHOTON_GET_CQ_IND(fi_ctx.num_cq, proc);
    end = start+1;
  }

  (!fi_ctx.thread_safe) ? sync_tatas_acquire(&fi_ctx.lock):NULL;
  {
    for (i=start; i<end && comp<max; i++) {
      retries = MAX_RETRIES;
      do {
	ne = fi_cq_read(fi_ctx.rcq[i], entries, max);
	if (ne < 0) {
	  if (ne == -FI_EAGAIN) {
	    ne = 0;
	    continue;
	  }
	  else if (ne == -FI_EAVAIL) {
	    cq_readerr(fi_ctx.rcq[i], "remote CQ");
	    goto error_exit;
	  }
	  else {
	    log_err("fi_cq_read() failed: %s", fi_strerror(-ne));
	    goto error_exit;
	  }
	}
      }
      while ((ne < 1) && --retries);
      
      for (j=0; j<ne && j<MAX_CQ_POLL; j++) {
	// there are no revent IDs associated with this context
	ids[j+comp] = 0x0;
	imms[j+comp] = entries[j].data;
      }
      comp += ne;
    }
    *n = comp;
  }
  (!fi_ctx.thread_safe) ? sync_tatas_release(&fi_ctx.lock):NULL;
  
  // CQs are empty
  if (comp == 0) {
    return PHOTON_EVENT_NONE;
  }

  return PHOTON_EVENT_OK;

 error_exit:
  return PHOTON_EVENT_ERROR;
}
