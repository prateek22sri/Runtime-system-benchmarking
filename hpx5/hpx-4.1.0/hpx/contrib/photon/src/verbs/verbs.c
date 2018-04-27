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
#include <arpa/inet.h>

#include "photon_backend.h"
#include "photon_forwarder.h"
#include "photon_buffer.h"
#include "photon_exchange.h"
#include "photon_event.h"

#include "verbs.h"
#include "verbs_connect.h"
#include "verbs_util.h"
#include "verbs_ud.h"
#include "htable.h"
#include "logging.h"

#define MAX_RETRIES    1

struct rdma_args_t {
  int proc;
  uint64_t id;
  struct ibv_sge *sg_list;
  int num_sge;
  uintptr_t raddr;
  uint32_t rkey;
  uint32_t imm_data;
};

struct sr_args_t {
  int proc;
  uint64_t id;
  struct ibv_sge *sg_list;
  int num_sge;
  struct ibv_ah *ah;
  uint32_t qpn;
  uint32_t qkey;
  uint32_t imm_data;
};

static int __initialized = 0;
static htable_t *ah_table;

static int verbs_initialized(void);
static int verbs_init(photonConfig cfg, ProcessInfo *photon_processes, photonBI ss);
static int verbs_finalize(void);
static int verbs_connect_single(void *local_ci, void *remote_ci, int pindex, void **ret_ci, int *ret_len, photon_connect_mode_t mode);
static int verbs_get_info(ProcessInfo *pi, int proc, void **info, int *size, photon_info_t type);
static int verbs_set_info(ProcessInfo *pi, int proc, void *info, int size, photon_info_t type);
static int verbs_rdma_put(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                          photonBuffer lbuf, photonBuffer rbuf, uint64_t id, uint64_t imm, int flags);
static int verbs_rdma_get(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                          photonBuffer lbuf, photonBuffer rbuf, uint64_t id, int flags);
static int verbs_rdma_send(photonAddr addr, uintptr_t laddr, uint64_t size,
                           photonBuffer lbuf, uint64_t id, uint64_t imm, int flags);
static int verbs_rdma_recv(photonAddr addr, uintptr_t laddr, uint64_t size,
                           photonBuffer lbuf, uint64_t id, int flags);
static int verbs_tx_size_left(int proc);
static int verbs_rx_size_left(int proc);
static int verbs_get_event(int proc, int max, photon_rid *ids, int *n);
static int verbs_get_revent(int proc, int max, photon_rid *ids, uint64_t *imms, int *n);
static int verbs_get_dev_addr(int af, photonAddr addr);
static int verbs_register_addr(photonAddr addr, int af);
static int verbs_unregister_addr(photonAddr addr, int af);
static int verbs_get_dev_name(char **ib_dev);

static int __verbs_do_rdma(struct rdma_args_t *args, int opcode, int flags);
static int __verbs_do_send(struct sr_args_t *args, int opcode, int flags);
static int __verbs_do_recv(struct sr_args_t *args, int flags);

static verbs_cnct_ctx verbs_ctx = {
  .ib_dev ="ib0",
  .ib_port = 1,
  .ib_context = NULL,
  .ib_pd = NULL,
  .ib_cq = NULL,
  .ib_srq = NULL,
  .ib_cc = NULL,
  .ib_lid = 0,
  .cm_schannel = NULL,
  .cm_id = NULL,
  .qp = NULL,
  .ud_qp = NULL,
  .psn = 0,
  .num_qp = 0,
  .use_ud = 0,
  .tx_depth = DEF_LEDGER_SIZE,
  .rx_depth = DEF_LEDGER_SIZE,
  .atomic_depth = 16,
  .max_sge = 1,
  .max_inline = -1,
  .num_cq = DEF_NUM_CQ,
  .num_srq = VERBS_DEF_NUM_SRQ,
};

// we are now a Photon backend
struct photon_backend_attr_t verbs_attr = {
  .rdma_put_align = PHOTON_VERBS_PUT_ALIGN,
  .rdma_get_align = PHOTON_VERBS_GET_ALIGN,
  .comp_order     = PHOTON_VERBS_COMP_ORDER
};

struct photon_backend_t photon_verbs_backend = {
  .attr = &verbs_attr,
  .context = &verbs_ctx,
  .initialized = verbs_initialized,
  .init = verbs_init,
  .finalize = verbs_finalize,
  .connect = verbs_connect_single,
  .get_info = verbs_get_info,
  .set_info = verbs_set_info,
  .get_dev_addr = verbs_get_dev_addr,
  .get_dev_name = verbs_get_dev_name,
  .register_addr = verbs_register_addr,
  .unregister_addr = verbs_unregister_addr,
  .rdma_put = verbs_rdma_put,
  .rdma_get = verbs_rdma_get,
  .rdma_send = verbs_rdma_send,
  .rdma_recv = verbs_rdma_recv,
  .tx_size_left = verbs_tx_size_left,
  .rx_size_left = verbs_rx_size_left,
  .get_event = verbs_get_event,
  .get_revent = verbs_get_revent
};

static int verbs_initialized() {
  if (__initialized == 1)
    return PHOTON_OK;
  else
    return PHOTON_ERROR_NOINIT;
}

static int verbs_init(photonConfig cfg, ProcessInfo *photon_processes, photonBI ss) {
  // __initialized: 0 - not; -1 - initializing; 1 - initialized 
  __initialized = -1;

  if (__verbs_handle_config(&verbs_ctx, cfg)) {
    log_err("Failed to set valid verbs backend config");
    goto error_exit;
  }

  if (verbs_ctx.use_ud) {
    dbg_trace("create ah_table");
    ah_table = htable_create(1009);
    if (!ah_table) {
      log_err("Failed to allocate AH table");
      goto error_exit;
    }
  }

  if (cfg->attr.comp_order == PHOTON_ORDER_DEFAULT) {
    verbs_attr.comp_order = PHOTON_VERBS_DEF_ORDER;
  }
  else if (cfg->attr.comp_order & PHOTON_VERBS_COMP_ORDER) {
    verbs_attr.comp_order = cfg->attr.comp_order;
  }
  else {
    dbg_err("specified completion order not supported");
    goto error_exit;
  }
  
  if(__verbs_init_context(&verbs_ctx)) {
    goto error_exit;
  }

  // a forwarder connects to itself so we get a protection domain
  if (_forwarder) {
    __verbs_connect_single(&verbs_ctx, verbs_ctx.local_ci[_photon_myrank],
                           verbs_ctx.local_ci[_photon_myrank], _photon_myrank,
                           NULL, NULL,
                           PHOTON_CONN_ACTIVE);
  }
  else {
    if(__verbs_connect_peers(&verbs_ctx)) {
      log_err("Could not connect peers");
      goto error_exit;
    }

    // setup forwarder if requested
    if (cfg->forwarder.use_forwarder && __photon_forwarder) {
      if (__photon_forwarder->init(cfg, photon_processes)) {
        log_err("Could not initialize forwarder(s)");
        goto error_exit;
      }

      if (__photon_forwarder->connect(photon_processes)) {
        log_err("Could not perform connect with forwarder(s)");
        goto error_exit;
      }
    }
  }

  // This shared buffer needs to be registered before any exchanges
  // since we share a common rkey across ledgers
  // and to register, we need a protection domain, hence at least one
  // connect must be made first (CMA case)
  if (photon_buffer_register(ss, &verbs_ctx, BUFFER_FLAG_NOTIFY) != 0) {
    log_err("couldn't register local buffer for the ledger entries");
    goto error_exit;
  }

  if (!_forwarder) {
    if (cfg->forwarder.use_forwarder && __photon_forwarder) {
      if (__photon_forwarder->exchange(photon_processes)) {
        log_err("Could not perform ledger exchange with forwarder(s)");
        goto error_exit;
      }
    }

    if (photon_exchange_ledgers(photon_processes, LEDGER_ALL) != PHOTON_OK) {
      log_err("couldn't exchange ledgers");
      goto error_exit;
    }
  }

  // prime the recv queue pump if enabled
  if (verbs_ctx.use_rcq) {
    if (verbs_ctx.num_srq > 0) {
      if (__verbs_post_srq_recv(&verbs_ctx, 0, PHOTON_ANY_SOURCE, -1) != PHOTON_OK) {
	log_err("Could not initialize recv work requests");
	goto error_exit;
      }
    }
    else {
      if (__verbs_post_rq_recv(&verbs_ctx, 0, PHOTON_ANY_SOURCE, -1) != PHOTON_OK) {
	log_err("Could not initialize recv work requests");
	goto error_exit;
      }
    }
  }

  // counters to track tx/rx operations
  verbs_ctx.tx_cnt = malloc(_photon_nproc * sizeof(uint32_t));
  verbs_ctx.rx_cnt = malloc(_photon_nproc * sizeof(uint32_t));
  if (!verbs_ctx.tx_cnt || !verbs_ctx.rx_cnt) {
    log_err("Could not allocate tx/rx counters");
    goto error_exit;
  }

  for (int i=0; i<_photon_nproc; i++) {
    verbs_ctx.tx_cnt[i] = verbs_ctx.tx_depth;
    verbs_ctx.rx_cnt[i] = verbs_ctx.rx_depth;
  }
  
  __initialized = 1;

  dbg_trace("ended successfully =============");

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

static int verbs_finalize() {
  int i;
  verbs_cnct_ctx *ctx = &verbs_ctx;
  
  // free CQs
  free(ctx->ib_cq);
  if (ctx->use_rcq)
    free(ctx->ib_rq);
  
  // free connect info
  for (i=0; i<(_photon_nproc+_photon_nforw); i++) {
    free(ctx->remote_ci[i]);
    free(ctx->local_ci[i]);
  }
  
  // free counters
  free(ctx->tx_cnt);
  free(ctx->rx_cnt);

  free(ctx->qp);
  free(ctx->remote_ci);
  free(ctx->local_ci);

  return PHOTON_OK;
}

static int verbs_connect_single(void *local_ci, void *remote_ci, int pindex, void **ret_ci, int *ret_len, photon_connect_mode_t mode) {
  return __verbs_connect_single(&verbs_ctx, local_ci, remote_ci, pindex, (verbs_cnct_info**)ret_ci, ret_len, mode);
}

static int verbs_get_info(ProcessInfo *pi, int proc, void **ret_info, int *ret_size, photon_info_t type) {
  int i;
  struct photon_buffer_t *info = NULL;

  switch (type) {
  case PHOTON_GET_ALIGN:
    {
      *ret_info = &verbs_attr.rdma_get_align;
      *ret_size = sizeof(verbs_attr.rdma_get_align);
    }
    break;
  case PHOTON_PUT_ALIGN:
    {
      *ret_info = &verbs_attr.rdma_put_align;
      *ret_size = sizeof(verbs_attr.rdma_put_align);
    }
    break;
  case PHOTON_COMP_ORDER:
    {
      *ret_info = &verbs_attr.comp_order;
      *ret_size = sizeof(verbs_attr.comp_order);
    }
    break;
  case PHOTON_MTU:
    {
      *ret_info = &verbs_ctx.ib_mtu;
      *ret_size = sizeof(verbs_ctx.ib_mtu);
    }
    break;
  case PHOTON_CI: {
    verbs_cnct_info *cinfo;
    cinfo = (verbs_cnct_info *)malloc(MAX_QP * sizeof(verbs_cnct_info));
    if(!cinfo) {
      goto error_exit;
    }
    for (i=0; i<MAX_QP; i++) {
      memcpy(&cinfo[i], verbs_ctx.local_ci[proc], sizeof(verbs_cnct_info));
    }
    *ret_info = (void*)cinfo;
    *ret_size = MAX_QP * sizeof(*cinfo);
  }
  break;
  case PHOTON_RI:
  case PHOTON_SI:
  case PHOTON_FI: {
    info = (struct photon_buffer_t *)malloc(sizeof(struct photon_buffer_t));
    if (!info) {
      goto error_exit;
    }
    info->priv = shared_storage->buf.priv;
    *ret_info = (void*)info;
    *ret_size = sizeof(*info);
  }
  break;
  default:
    break;
  }

  switch (type) {
  case PHOTON_RI:
    info->addr = (uintptr_t)pi->local_rcv_info_ledger->entries;
    break;
  case PHOTON_SI:
    info->addr = (uintptr_t)pi->local_snd_info_ledger->entries;
    break;
  case PHOTON_FI:
    info->addr = (uintptr_t)pi->local_fin_ledger->entries;
  default:
    break;
  }

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

static int verbs_set_info(ProcessInfo *pi, int proc, void *info, int size, photon_info_t type) {
  photonBuffer li = (photonBuffer)info;

  switch (type) {
  case PHOTON_CI: {
    int i;
    for (i=0; i<MAX_QP; i++) {
      memcpy(&verbs_ctx.remote_ci[proc][i], info, sizeof(verbs_cnct_info));
    }
  }
  break;
  case PHOTON_RI:
    pi->remote_rcv_info_ledger->remote.addr = li->addr;
    pi->remote_rcv_info_ledger->remote.priv = li->priv;
    break;
  case PHOTON_SI:
    pi->remote_snd_info_ledger->remote.addr = li->addr;
    pi->remote_snd_info_ledger->remote.priv = li->priv;
    break;
  case PHOTON_FI:
    pi->remote_fin_ledger->remote.addr = li->addr;
    pi->remote_fin_ledger->remote.priv = li->priv;
    break;
  default:
    goto error_exit;
    break;
  }

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

static int __verbs_do_rdma(struct rdma_args_t *args, int opcode, int flags) {
  int err, retries;
  struct ibv_send_wr *bad_wr;

  struct ibv_send_wr wr = {
    .wr_id               = args->id,
    .sg_list             = args->sg_list,
    .num_sge             = args->num_sge,
    .opcode              = opcode,
    .send_flags          = flags,
    .imm_data            = args->imm_data,
    .wr.rdma.remote_addr = args->raddr,
    .wr.rdma.rkey        = args->rkey
  };

  retries = MAX_RETRIES;
  do {
    err = ibv_post_send(verbs_ctx.qp[args->proc], &wr, &bad_wr);
  }
  while(err && --retries);

  if (err != 0) {
    log_err("Failure in ibv_post_send(): %s", strerror(err));
    return PHOTON_ERROR;
  }
  
  if (args->id) {
    uint32_t p = (uint32_t)(args->id>>32);
    if (! (p & REQUEST_COOK_MASK)) {
      sync_fadd(&verbs_ctx.tx_cnt[args->proc], -1, SYNC_RELAXED);
    }
  }


  return PHOTON_OK;
}

// send/recv use UD service_qp by default at the moment
static int __verbs_do_send(struct sr_args_t *args, int opcode, int flags) {
  int err, retries;
  struct ibv_send_wr *bad_wr;

  struct ibv_send_wr wr = {
    .wr_id               = args->id,
    .sg_list             = args->sg_list,
    .num_sge             = args->num_sge,
    .opcode              = opcode,
    .send_flags          = IBV_SEND_SIGNALED,
    .wr = {
      .ud = {
        .ah              = args->ah,
        .remote_qpn      = args->qpn,
        .remote_qkey     = args->qkey
      }
    }
  };
  
  retries = MAX_RETRIES;
  do {
    err = ibv_post_send(verbs_ctx.ud_qp, &wr, &bad_wr);
  }
  while(err && --retries);

  if (err != 0) {
    dbg_err("Failure in ibv_post_send(): %s", strerror(err));
    return PHOTON_ERROR;
  }

  return PHOTON_OK;
}

static int __verbs_do_recv(struct sr_args_t *args, int flags) {
  int err, retries;
  struct ibv_recv_wr *bad_wr;

  struct ibv_recv_wr wr = {
    .wr_id   = args->id,
    .sg_list = args->sg_list,
    .num_sge = args->num_sge,
    .next    = NULL
  };
  
  retries = MAX_RETRIES;
  do {
    err = ibv_post_recv(verbs_ctx.ud_qp, &wr, &bad_wr);
  }
  while(err && --retries);

  if (err != 0) {
    dbg_err("Failure in ibv_post_recv(): (%d) %s", err, strerror(err));
    return PHOTON_ERROR;
  }

  return PHOTON_OK;
}

static int verbs_rdma_put(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                          photonBuffer lbuf, photonBuffer rbuf, uint64_t id,
			  uint64_t imm, int flags) {
  int ibv_flags = IBV_SEND_SIGNALED;
  int op = (flags & RDMA_FLAG_WITH_IMM) && (verbs_ctx.use_rcq) ?
    IBV_WR_RDMA_WRITE_WITH_IMM : IBV_WR_RDMA_WRITE;
  struct rdma_args_t args;
  struct ibv_sge list = {
    .addr   = laddr,
    .length = size,
    .lkey   = lbuf->priv.key0
  };

  if (size <= verbs_ctx.max_inline) {
    ibv_flags |= IBV_SEND_INLINE;
  }

  if (flags & RDMA_FLAG_NO_CQE)
    ibv_flags ^= IBV_SEND_SIGNALED;
  
  args.proc = proc;
  args.id = id;
  args.sg_list = &list;
  args.num_sge = 1;
  args.raddr = raddr;
  args.rkey = rbuf->priv.key1;
  args.imm_data = (uint32_t)imm;
  return __verbs_do_rdma(&args, op, ibv_flags);
}

static int verbs_rdma_get(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                          photonBuffer lbuf, photonBuffer rbuf, uint64_t id, int flags) {
  struct rdma_args_t args;
  struct ibv_sge list = {
    .addr   = laddr,
    .length = size,
    .lkey   = lbuf->priv.key0
  };

  args.proc = proc;
  args.id = id;
  args.sg_list = &list;
  args.num_sge = 1;
  args.raddr = raddr;
  args.rkey = rbuf->priv.key1;
  return __verbs_do_rdma(&args, IBV_WR_RDMA_READ, IBV_SEND_SIGNALED);
}

static int verbs_rdma_send(photonAddr addr, uintptr_t laddr, uint64_t size,
                           photonBuffer lbuf, uint64_t id, uint64_t imm, int flags) {
  void *rc;
  int op = (flags & RDMA_FLAG_WITH_IMM) && (verbs_ctx.use_rcq) ? 
    IBV_WR_SEND_WITH_IMM : IBV_WR_SEND;
  struct sr_args_t args;
  struct ibv_sge list = {
    .addr = laddr,
    .length = size,
    .lkey = lbuf->priv.key0
  };
  
  // cache the address handles since it takes forever to create them
  if (htable_lookup(ah_table,
		    (uint64_t)(addr->global.proc_id + addr->global.prefix),
		    (void**)&args.ah) != 0) {
    rc = __verbs_ud_create_ah(&verbs_ctx, (union ibv_gid *)addr, 0x0, &args.ah);
    if (!rc) {
      return PHOTON_ERROR;
    }
    htable_insert(ah_table, addr->s_addr, args.ah);
  }
  
  args.proc = addr->global.proc_id;
  args.id = id;
  args.sg_list = &list;
  args.num_sge = 1;
  args.qpn = 0xffffff;
  args.qkey = 0x11111111;
  args.imm_data = (uint32_t)imm;
  return __verbs_do_send(&args, op, flags);
}

static int verbs_rdma_recv(photonAddr addr, uintptr_t laddr, uint64_t size,
                           photonBuffer lbuf, uint64_t id, int flags) {
  struct sr_args_t args;
  struct ibv_sge list = {
    .addr = laddr,
    .length = size,
    .lkey = lbuf->priv.key0
  };
  
  if (addr)
    args.proc = addr->global.proc_id;
  args.id = id;
  args.sg_list = &list;
  args.num_sge = 1;
  return __verbs_do_recv(&args, flags);
}

static int verbs_tx_size_left(int proc) {
  return sync_load(&verbs_ctx.tx_cnt[proc], SYNC_RELAXED);
}

static int verbs_rx_size_left(int proc) {
  return sync_load(&verbs_ctx.rx_cnt[proc], SYNC_RELAXED);
}

static int verbs_get_event(int proc, int max, photon_rid *ids, int *n) {
  int i, j, ne, comp;
  int start, end;
  int retries;
  struct ibv_wc wc[MAX_CQ_POLL];
  
  *n = 0;
  comp = 0;

  if (verbs_ctx.num_cq == 1) {
    start = 0;
    end = 1;
  }
  else if (proc == PHOTON_ANY_SOURCE) {
    start = 0;
    end = verbs_ctx.num_cq;
  }
  else {
    start = PHOTON_GET_CQ_IND(verbs_ctx.num_cq, proc);
    end = start+1;
  }

  for (i=start; i<end && comp<max; i++) {
    retries = MAX_RETRIES;
    do {
      ne = ibv_poll_cq(verbs_ctx.ib_cq[i], max, wc);
      if (ne < 0) {
	log_err("ibv_poll_cq() failed");
	goto error_exit;
      }
    }
    while ((ne < 1) && --retries);
    
    for (j=0; j<ne && j<MAX_CQ_POLL; j++) {
      if (wc[j].status != IBV_WC_SUCCESS) {
	log_err("(status==%d) != IBV_WC_SUCCESS: %s",
		wc[j].status, ibv_wc_status_str(wc[j].status));
	if (wc[j].status == IBV_WC_WR_FLUSH_ERR) {
	  goto error_exit;
	}
      }
      uint64_t id = wc[j].wr_id;
      if (id) {
	uint32_t p = (uint32_t)(id>>32);
	if (! (p & REQUEST_COOK_MASK)) {
	  sync_fadd(&verbs_ctx.tx_cnt[p], 1, SYNC_RELAXED);
	}
      }
      ids[j+comp] = id;
    }
    comp += ne;
  }

  *n = comp;

  // CQs are empty
  if (comp == 0) {
    return PHOTON_EVENT_NONE;
  }
  
  return PHOTON_EVENT_OK;
  
error_exit:
  return PHOTON_EVENT_ERROR;
}

static int verbs_get_revent(int proc, int max, photon_rid *ids, uint64_t *imms, int *n) {
  int i, j, ne, comp;
  int start, end;
  int retries;
  struct ibv_wc wc[MAX_CQ_POLL];

  *n = 0;
  comp = 0;

  if (!verbs_ctx.use_rcq) {
    return PHOTON_EVENT_NOTIMPL;
  }

  if (verbs_ctx.num_cq == 1) {
    start = 0;
    end = 1;
  }
  else if (proc == PHOTON_ANY_SOURCE) {
    start = 0;
    end = verbs_ctx.num_cq;
  }
  else {
    start = PHOTON_GET_CQ_IND(verbs_ctx.num_cq, proc);
    end = start+1;
  }
  
  for (i=start; i<end && comp<max; i++) {
    retries = MAX_RETRIES;
    do {
      ne = ibv_poll_cq(verbs_ctx.ib_rq[i], max, wc);
      if (ne < 0) {
	log_err("ibv_poll_cq() failed");
	goto error_exit;
      }
    }
    while ((ne < 1) && --retries);
    
    for (j=0; j<ne && j<MAX_CQ_POLL; j++) {
      if (wc[j].status != IBV_WC_SUCCESS) {
	log_err("(status==%d) != IBV_WC_SUCCESS: %s",
		wc[j].status, ibv_wc_status_str(wc[j].status));
      }
      ids[j+comp] = wc[j].wr_id;
      imms[j+comp] = wc[j].imm_data;
      // re-arm the recv, we encode the source in the immediate data
      int src = (int)DECODE_RCQ_32_PROC(wc[j].imm_data);
      if (verbs_ctx.num_srq)
	__verbs_post_srq_recv(&verbs_ctx, wc[j].wr_id, src, 1);
      else
	__verbs_post_rq_recv(&verbs_ctx, wc[j].wr_id, src, 1);
    }
    comp += ne;
  }

  *n = comp;
  
  // CQs are empty
  if (comp == 0) {
    return PHOTON_EVENT_NONE;
  }	
  
  return PHOTON_EVENT_OK;
  
 error_exit:
  return PHOTON_EVENT_ERROR;
}

static int verbs_get_dev_addr(int af, photonAddr addr) {
  if (!addr) {
    return PHOTON_ERROR;
  }
  if (af == AF_INET) {
    addr->s_addr = verbs_ctx.local_ci[_photon_myrank][0].ip.s_addr;
  }
  else {
    memcpy(addr, &(verbs_ctx.local_ci[_photon_myrank][0].gid), sizeof(union ibv_gid));
  }
  return PHOTON_OK;
}

static int verbs_register_addr(photonAddr addr, int af) {
  int rc;
  photon_addr saddr;
  if (verbs_ctx.use_ud) {
    rc = _photon_handle_addr(addr, &saddr);
    if (rc != PHOTON_OK) {
      return PHOTON_ERROR;
    }
    return __verbs_ud_attach_addr(&verbs_ctx, (union ibv_gid*)&saddr);
  }
  else {
    dbg_warn("Unknown action");
  }
  
  return PHOTON_OK;
}

static int verbs_unregister_addr(photonAddr addr, int af) {
  int rc;
  photon_addr saddr;
  if (verbs_ctx.use_ud) {
    rc = _photon_handle_addr(addr, &saddr);
    if (rc != PHOTON_OK) {
      return PHOTON_ERROR;
    }
    return __verbs_ud_detach_addr(&verbs_ctx, (union ibv_gid*)&saddr);
  }
  else {
    dbg_warn("Unknown action");
  }

  return PHOTON_OK;
}

static int verbs_get_dev_name(char **ib_dev) {
  *ib_dev = (char*)verbs_ctx.ib_dev;
  return PHOTON_OK;
}
