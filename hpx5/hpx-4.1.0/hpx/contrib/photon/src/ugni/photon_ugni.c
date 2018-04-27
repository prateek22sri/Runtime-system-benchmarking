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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <limits.h>

#include "photon_backend.h"
#include "photon_buffer.h"
#include "photon_exchange.h"
#include "photon_event.h"

#include "photon_ugni.h"
#include "photon_ugni_connect.h"
#include "logging.h"
#include "util.h"
#include "utility_functions.h"
#include "libsync/locks.h"

#define UGNI_PD_FREE   1
#define MAX_RETRIES    1

struct rdma_args_t {
  int proc;
  uint64_t id;
  uint64_t laddr;
  uint64_t raddr;
  uint64_t size;
  gni_mem_handle_t lmdh;
  gni_mem_handle_t rmdh;
  uint32_t imm_data;
};

typedef struct {
  gni_post_descriptor_t d;
  uint64_t flags;
} photon_gni_descriptor_t;

typedef struct {
  uint64_t size;
  uint64_t count;
  uint64_t curr;
  photon_gni_descriptor_t *pds;
} photon_gni_post_descriptors_t;

static int __initialized = 0;

static int ugni_initialized(void);
static int ugni_init(photonConfig cfg, ProcessInfo *photon_processes, photonBI ss);
static int ugni_finalize(void);
static int ugni_get_info(ProcessInfo *pi, int proc, void **info, int *size, photon_info_t type);
static int ugni_set_info(ProcessInfo *pi, int proc, void *info, int size, photon_info_t type);
static int ugni_rdma_put(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                         photonBuffer lbuf, photonBuffer rbuf, uint64_t id, uint64_t imm, int flags);
static int ugni_rdma_get(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                         photonBuffer lbuf, photonBuffer rbuf, uint64_t id, int flags);
static int ugni_rdma_send(photonAddr addr, uintptr_t laddr, uint64_t size,
                          photonBuffer lbuf, uint64_t id, uint64_t imm, int flags);
static int ugni_rdma_recv(photonAddr addr, uintptr_t laddr, uint64_t size,
                          photonBuffer lbuf, uint64_t id, int flags);
static int ugni_tx_size_left(int proc);
static int ugni_rx_size_left(int proc);
static int ugni_get_event(int proc, int max, photon_rid *ids, int *n);
static int ugni_get_revent(int proc, int max, photon_rid *ids, uint64_t *imms, int *n);

static int __ugni_do_rdma(struct rdma_args_t *args, int opcode, int flags);
static int __ugni_do_fma(struct rdma_args_t *args, int opcode, int flags);

static ugni_cnct_ctx ugni_ctx;
static photon_gni_post_descriptors_t dpool = {
  .curr = 0,
  .count = 0,
  .size = 0,
  .pds = NULL
};

// we are now a Photon backend
static struct photon_backend_attr_t ugni_attr = {
  .rdma_put_align = PHOTON_UGNI_PUT_ALIGN,
  .rdma_get_align = PHOTON_UGNI_GET_ALIGN,
  .comp_order     = PHOTON_UGNI_COMP_ORDER
};
    
struct photon_backend_t photon_ugni_backend = {
  .attr = &ugni_attr,
  .context = &ugni_ctx,
  .initialized = ugni_initialized,
  .init = ugni_init,
  .finalize = ugni_finalize,
  .connect = NULL,
  .get_info = ugni_get_info,
  .set_info = ugni_set_info,
  .get_dev_addr = NULL,
  .get_dev_name = NULL,
  .register_addr = NULL,
  .unregister_addr = NULL,
  .rdma_put = ugni_rdma_put,
  .rdma_get = ugni_rdma_get,
  .rdma_send = ugni_rdma_send,
  .rdma_recv = ugni_rdma_recv,
  .tx_size_left = ugni_tx_size_left,
  .rx_size_left = ugni_rx_size_left,
  .get_event = ugni_get_event,
  .get_revent = ugni_get_revent
};

static int ugni_initialized() {
  if (__initialized == 1)
    return PHOTON_OK;
  else
    return PHOTON_ERROR_NOINIT;
}

static int ugni_init(photonConfig cfg, ProcessInfo *photon_processes, photonBI ss) {
  // __initialized: 0 - not; -1 - initializing; 1 - initialized
  __initialized = -1;

  sync_tatas_init(&ugni_ctx.lock);

  if (cfg->ugni.bte_thresh < 0)
    cfg->ugni.bte_thresh = UGNI_DEF_BTE_THRESH;
  else if (cfg->ugni.bte_thresh == 0)
    cfg->ugni.bte_thresh = INT_MAX;

  if (cfg->ugni.eth_dev) {
    ugni_ctx.gemini_dev = cfg->ugni.eth_dev;
  }
  else {
    ugni_ctx.gemini_dev = "ipogif0";
  }
  
  if (cfg->attr.comp_order == PHOTON_ORDER_DEFAULT) {
    ugni_attr.comp_order = PHOTON_UGNI_DEF_ORDER;
  }
  else if (cfg->attr.comp_order & PHOTON_UGNI_COMP_ORDER) {
    ugni_attr.comp_order = cfg->attr.comp_order;
  }
  else {
    dbg_err("specified completion order not supported");
    goto error_exit;
  }

  ugni_ctx.num_cq  = cfg->cap.num_cq;
  ugni_ctx.use_rcq = cfg->cap.use_rcq;

  if ((4 * _LEDGER_SIZE * _photon_nproc / ugni_ctx.num_cq) > MAX_CQ_ENTRIES) {
    one_debug("Adjusting CQ count to avoid possible overrun with specified "
	      "config (nproc=%d, nledger=%d, ncq=%d)",
	      _photon_nproc, _LEDGER_SIZE, ugni_ctx.num_cq);
    do {
      ugni_ctx.num_cq++;
      if (ugni_ctx.num_cq > _photon_nproc) {
	one_err("CQ count exceeds number of ranks, reduce ledger size!");
	goto error_exit;
      }
    } while ((4 * _LEDGER_SIZE * _photon_nproc / ugni_ctx.num_cq) > MAX_CQ_ENTRIES);
    one_debug("CQ count is now set to %d", ugni_ctx.num_cq);
  }

  if(__ugni_init_context(&ugni_ctx)) {
    log_err("Could not initialize ugni context");
    goto error_exit;
  }

  if(__ugni_connect_peers(&ugni_ctx)) {
    log_err("Could not connect peers");
    goto error_exit;
  }

  if (photon_buffer_register(ss, &ugni_ctx, BUFFER_FLAG_NOTIFY) != 0) {
    log_err("couldn't register local buffer for the ledger entries");
    goto error_exit;
  }

  if (photon_exchange_ledgers(photon_processes, LEDGER_ALL) != PHOTON_OK) {
    log_err("couldn't exchange ledgers");
    goto error_exit;
  }
  
  // initialize the available dpool
  dpool.size = next_power_of_2(UGNI_DEF_NUM_PDS*_photon_nproc);
  dpool.pds = (photon_gni_descriptor_t*)calloc(dpool.size,
					       sizeof(photon_gni_descriptor_t));
  if (!dpool.pds) {
    dbg_err("Could not allocate ugni descriptors");
    goto error_exit;
  }
  
  __initialized = 1;

  dbg_trace("ended successfully =============");

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

static int ugni_finalize() {
  // should clean up allocated buffers
  free(dpool.pds);
  return PHOTON_OK;
}

static int ugni_get_info(ProcessInfo *pi, int proc, void **ret_info, int *ret_size, photon_info_t type) {
  switch(type) {
  case PHOTON_GET_ALIGN:
    {
      *ret_info = &ugni_attr.rdma_get_align;
      *ret_size = sizeof(ugni_attr.rdma_get_align);
    }
    break;
  case PHOTON_PUT_ALIGN:
    {
      *ret_info = &ugni_attr.rdma_put_align;
      *ret_size = sizeof(ugni_attr.rdma_put_align);
    }
    break;
  case PHOTON_COMP_ORDER:
    {
      *ret_info = &ugni_attr.comp_order;
      *ret_size = sizeof(ugni_attr.comp_order);
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

static int ugni_set_info(ProcessInfo *pi, int proc, void *info, int size, photon_info_t type) {

  return PHOTON_OK;
}

static int __ugni_set_desc(struct rdma_args_t *args, int opcode, int flags,
			   gni_post_descriptor_t **rdesc) {
  int err, cqind;
  uint64_t cind;
  gni_post_descriptor_t *desc;
  photon_gni_descriptor_t *pd;
  
  // dynamically allocate if descriptor pool is exhausted
  if (dpool.count >= dpool.size) {
    pd = calloc(1, sizeof(photon_gni_descriptor_t));
    if (!pd) {
      log_err("Could not allocate new post descriptor");
      return PHOTON_ERROR;
    }
    pd->flags = UGNI_PD_FREE;
    desc = &(pd->d);
  }
  else {
    cind = ++dpool.curr & (dpool.size - 1);
    pd = &(dpool.pds[cind]);
    pd->flags = 0;
    desc = &(pd->d);
    dpool.count++;
  }

  cqind = PHOTON_GET_CQ_IND(ugni_ctx.num_cq, args->proc);
  
  if (flags & RDMA_FLAG_NO_CQE) {
    desc->cq_mode = GNI_CQMODE_SILENT;
    desc->src_cq_hndl = NULL;
  }
  else {
    desc->cq_mode = GNI_CQMODE_GLOBAL_EVENT;
    desc->src_cq_hndl = ugni_ctx.local_cq_handles[cqind];
  }

  desc->type = opcode;
  desc->local_addr = args->laddr;
  desc->local_mem_hndl = args->lmdh;
  desc->remote_addr = args->raddr;
  desc->remote_mem_hndl = args->rmdh;
  desc->length = args->size;
  desc->post_id = args->id;
  desc->rdma_mode = 0;
  desc->dlvr_mode = (ugni_attr.comp_order & PHOTON_ORDER_NONE) ? 
    GNI_DLVMODE_PERFORMANCE : GNI_DLVMODE_IN_ORDER;

  if (ugni_ctx.use_rcq && (flags & RDMA_FLAG_WITH_IMM)) {
    err = GNI_EpSetEventData(ugni_ctx.ep_handles[args->proc],
			     (uint32_t)0x0,
			     (uint32_t)args->imm_data);
    if (err != GNI_RC_SUCCESS) {
      log_err("Could not set immediate RCQ event data");
      return PHOTON_ERROR;
    }
    desc->cq_mode |= GNI_CQMODE_REMOTE_EVENT;
  }

  *rdesc = desc;

  return PHOTON_OK;
}

static int __ugni_do_rdma(struct rdma_args_t *args, int opcode, int flags) {
  gni_post_descriptor_t *desc;
  const int max_trials = 1000;
  int err, trials = 0;

  sync_tatas_acquire(&ugni_ctx.lock);
  {
    if (__ugni_set_desc(args, opcode, flags, &desc) != PHOTON_OK) {
      goto error_exit;
    }

    do {
      err = GNI_PostRdma(ugni_ctx.ep_handles[args->proc], desc);
      if (err == GNI_RC_SUCCESS) {
	dbg_trace("GNI_PostRdma data transfer successful: %"PRIx64, args->id);
	break;
      }
      if (err != GNI_RC_ERROR_RESOURCE) {
	log_err("GNI_PostRdma data ERROR status: %s (%d)\n", gni_err_str[err], err);
	goto error_exit;
      }
      sched_yield();
    } while (++trials < max_trials);
    
    if (err == GNI_RC_ERROR_RESOURCE) {
      log_err("GNI_PostFma retries exceeded: %s (%d)", gni_err_str[err], err);
      goto error_exit;
    }
  }
  sync_tatas_release(&ugni_ctx.lock);

  return PHOTON_OK;

error_exit:
  sync_tatas_release(&ugni_ctx.lock);
  return PHOTON_ERROR;
}

static int __ugni_do_fma(struct rdma_args_t *args, int opcode, int flags) {
  gni_post_descriptor_t *desc;
  const int max_trials = 1000;
  int err, trials = 0;

  sync_tatas_acquire(&ugni_ctx.lock);
  {
    if (__ugni_set_desc(args, opcode, flags, &desc) != PHOTON_OK) {
      goto error_exit;
    }

    do {
      err = GNI_PostFma(ugni_ctx.ep_handles[args->proc], desc);
      if (err == GNI_RC_SUCCESS) {
	dbg_trace("GNI_PostFma data transfer successful: %"PRIx64, args->id);
	break;
      }
      if (err != GNI_RC_ERROR_RESOURCE) {
	log_err("GNI_PostFma data ERROR status: %s (%d)", gni_err_str[err], err);
	goto error_exit;
      }
      sched_yield();
    } while (++trials < max_trials);
    
    if (err == GNI_RC_ERROR_RESOURCE) {
      log_err("GNI_PostFma retries exceeded: %s (%d)", gni_err_str[err], err);
      goto error_exit;
    }
  }
  sync_tatas_release(&ugni_ctx.lock);

  return PHOTON_OK;

error_exit:
  sync_tatas_release(&ugni_ctx.lock);
  return PHOTON_ERROR;
}

static int ugni_rdma_put(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                         photonBuffer lbuf, photonBuffer rbuf, uint64_t id,
			 uint64_t imm, int flags) {
  struct rdma_args_t args;
  args.proc = proc;
  args.id = id;
  args.laddr = (uint64_t)laddr;
  args.raddr = (uint64_t)raddr;
  args.size = size;
  args.lmdh.qword1 = lbuf->priv.key0;
  args.lmdh.qword2 = lbuf->priv.key1;
  args.rmdh.qword1 = rbuf->priv.key0;
  args.rmdh.qword2 = rbuf->priv.key1;
  args.imm_data = (uint32_t)imm;

  if (size < __photon_config->ugni.bte_thresh)
    return __ugni_do_fma(&args, GNI_POST_FMA_PUT, flags);
  else
    return __ugni_do_rdma(&args, GNI_POST_RDMA_PUT, flags);
}

static int ugni_rdma_get(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                         photonBuffer lbuf, photonBuffer rbuf, uint64_t id, int flags) {
  struct rdma_args_t args;
  args.proc = proc;
  args.id = id;
  args.laddr = (uint64_t)laddr;
  args.raddr = (uint64_t)raddr;
  args.size = size;
  args.lmdh.qword1 = lbuf->priv.key0;
  args.lmdh.qword2 = lbuf->priv.key1;
  args.rmdh.qword1 = rbuf->priv.key0;
  args.rmdh.qword2 = rbuf->priv.key1;

  if (size < __photon_config->ugni.bte_thresh)
    return __ugni_do_fma(&args, GNI_POST_FMA_GET, flags);
  else
    return __ugni_do_rdma(&args, GNI_POST_RDMA_GET, flags);
}

static int ugni_rdma_send(photonAddr addr, uintptr_t laddr, uint64_t size,
                          photonBuffer lbuf, uint64_t id, uint64_t imm, int flags) {
  return PHOTON_OK;
}

static int ugni_rdma_recv(photonAddr addr, uintptr_t laddr, uint64_t size,
                          photonBuffer lbuf, uint64_t id, int flags) {
  return PHOTON_OK;
}

static int ugni_tx_size_left(int proc) {
  return MAX_CQ_ENTRIES;
}

static int ugni_rx_size_left(int proc) {
  return MAX_CQ_ENTRIES;
}

static int ugni_get_event(int proc, int max, photon_rid *ids, int *n) {
  gni_post_descriptor_t *event_post_desc_ptr;
  photon_gni_descriptor_t *pd;
  gni_cq_entry_t current_event;
  uint64_t cookie = NULL_REQUEST;
  int i, rc, start, end, comp;

  *n = 0;
  comp = 0;

  if (ugni_ctx.num_cq == 1) {
    start = 0;
    end = 1;
  }
  else if (proc == PHOTON_ANY_SOURCE) {
    start = 0;
    end = ugni_ctx.num_cq;
  }
  else {
    start = PHOTON_GET_CQ_IND(ugni_ctx.num_cq, proc);
    end = start+1;
  }
  
  sync_tatas_acquire(&ugni_ctx.lock);
  for (i=start; i<end && comp<max; i++) {
    rc = get_cq_event(ugni_ctx.local_cq_handles[i], 1, 0, &current_event);
    if (rc == 0) {
      rc = GNI_GetCompleted(ugni_ctx.local_cq_handles[i], current_event, &event_post_desc_ptr);
      if (rc != GNI_RC_SUCCESS) {
	dbg_err("GNI_GetCompleted data ERROR status: %s (%d)", gni_err_str[rc], rc);
	goto error_exit;
      }
      cookie = event_post_desc_ptr->post_id;
      pd = (photon_gni_descriptor_t*)event_post_desc_ptr;
      if (pd->flags == UGNI_PD_FREE) {
	free(pd);
      }
      else {
	dpool.count--;
      }
    }
    else if (rc == 3) {
      // nothing available
      continue;
    }
    else {
      // rc == 2 is an overrun
      dbg_trace("Error getting CQ event: %d", rc);
      goto error_exit;
    }
    
    dbg_trace("received event with cookie:%"PRIx64, cookie);
    ids[comp] = cookie;
    comp++;
  }
  sync_tatas_release(&ugni_ctx.lock);  

  *n = comp;
  
  if (comp == 0) {
    return PHOTON_EVENT_NONE;
  }
  
  return PHOTON_EVENT_OK;
  
 error_exit:
  sync_tatas_release(&ugni_ctx.lock);
  return PHOTON_EVENT_ERROR;
}

static int ugni_get_revent(int proc, int max, photon_rid *ids, uint64_t *imms, int *n) {
  gni_cq_entry_t current_event;
  uint64_t cookie = NULL_REQUEST;
  uint64_t imm_data;
  int rc, comp;

  if (!ugni_ctx.use_rcq) {
    return PHOTON_EVENT_NOTIMPL;
  }
  
  *n = 0;
  comp = 0;

  sync_tatas_acquire(&ugni_ctx.lock);
  do {
    rc = get_cq_event(ugni_ctx.remote_cq_handles[0], 1, 0, &current_event);
    if (rc == 0) {
      imm_data = GNI_CQ_GET_REM_INST_ID(current_event);
      ids[comp] = cookie;
      imms[comp] = imm_data;
      comp++;
    }
    else if (rc == 3) {
      // nothing available
      break;
    }
    else {
      // rc == 2 is an overrun
      dbg_trace("Error getting CQ event: %d", rc);
      goto error_exit;
    }
  } while (comp < max);
  sync_tatas_release(&ugni_ctx.lock);  
  
  *n = comp;
  
  if (comp == 0) {
    return PHOTON_EVENT_NONE;
  }
  
  return PHOTON_EVENT_OK;
  
 error_exit:
  sync_tatas_release(&ugni_ctx.lock);
  return PHOTON_EVENT_ERROR;
}

