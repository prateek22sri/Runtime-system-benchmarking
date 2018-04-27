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

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "photon_backend.h"
#include "photon_exchange.h"
#include "logging.h"
#include "verbs_util.h"
#include "verbs_buffer.h"

int __verbs_handle_config(verbs_cnct_ctx *ctx, photonConfig cfg) {
  ctx->max_cqe  = _LEDGER_SIZE * _photon_nproc * VERBS_DEF_MULT;
  ctx->tx_depth = _LEDGER_SIZE * VERBS_DEF_MULT;
  ctx->rx_depth = _LEDGER_SIZE * VERBS_DEF_MULT;
  ctx->ib_dev   = cfg->ibv.ib_dev;
  ctx->eth_dev  = cfg->ibv.eth_dev;
  ctx->use_cma  = cfg->ibv.use_cma;
  ctx->use_ud   = cfg->ibv.use_ud;
  ctx->num_srq  = (cfg->ibv.num_srq < 0) ? VERBS_DEF_NUM_SRQ : cfg->ibv.num_srq;
  ctx->num_cq   = cfg->cap.num_cq;
  ctx->use_rcq  = cfg->cap.use_rcq;
  
  if (ctx->num_srq > _photon_nproc) {
    ctx->num_srq = _photon_nproc;
    one_info("Requesting (num_srq > nproc), setting num_srq to nproc");
  }
  
  if (cfg->ibv.use_cma && !cfg->ibv.eth_dev) {
    log_err("CMA specified but Ethernet dev missing");
    goto error_exit;
  }

  return PHOTON_OK;

 error_exit:
  return PHOTON_ERROR;
}

int __verbs_adjust_ctx(verbs_cnct_ctx *ctx) {

  if (ctx->num_srq > 0) {
    if ((2 * _LEDGER_SIZE * _photon_nproc / ctx->num_srq) > ctx->max_cqe) {
      one_debug("Adjusting SRQ count to avoid possible overrun with specified "
		"config (nproc=%d, nledger=%d, nsrq=%d)",
		_photon_nproc, _LEDGER_SIZE, ctx->num_srq);
      do {
	ctx->num_srq++;
	if (ctx->num_srq > _photon_nproc) {
	  one_err("SRQ count exceeds number of ranks, reduce ledger size!");
	  goto error_exit;
	}
      } while ((2 * _LEDGER_SIZE * _photon_nproc / ctx->num_srq) > ctx->max_cqe);
      one_debug("SRQ count is now set to %d", ctx->num_srq);    }
  }

  if ((2 * _LEDGER_SIZE * _photon_nproc / ctx->num_cq) > ctx->max_cqe) {
    one_debug("Adjusting CQ count to avoid possible overrun with specified "
	      "config (nproc=%d, nledger=%d, ncq=%d)",
	      _photon_nproc, _LEDGER_SIZE, ctx->num_cq);
    do {
      ctx->num_cq++;
      if (ctx->num_cq > _photon_nproc) {
	one_err("CQ count exceeds number of ranks, reduce ledger size!");
	goto error_exit;
      }
    } while ((2 * _LEDGER_SIZE * _photon_nproc / ctx->num_cq) > ctx->max_cqe);
    one_debug("CQ count is now set to %d", ctx->num_cq);
  }

  if (ctx->num_srq > 0) {
    if ((2 * _LEDGER_SIZE * _photon_nproc / ctx->num_srq) > ctx->max_srq_wr) {
      one_warn("Possible SRQ WR overrun with current config (2*nledger*"
	       "nproc/nsrq=%d > max_srq_wr=%d)",
	       2*_LEDGER_SIZE*_photon_nproc/ctx->num_srq, ctx->max_srq_wr);
      one_warn("Decrease Photon ledger size on this sytem!");
    }
  }
  else {
    if ((2 * _LEDGER_SIZE) > ctx->max_qp_wr) {
      one_warn("Possible QP WR overrun with current config (2*nledger=%d > "
	       "max_qp_wr=%d)", 2*_LEDGER_SIZE, ctx->max_qp_wr);
      one_warn("Decrease Photon ledger size on this sytem!");
    }
  }

  return PHOTON_OK;

 error_exit:
  return PHOTON_ERROR;
}

int __verbs_post_srq_recv(verbs_cnct_ctx *ctx, uint64_t id, int proc, int num) {
  int i, j, start, end;
  int nposts = (num < 0) ? ctx->max_srq_wr : num;

  if (proc < 0) {
    start = 0;
    end = _photon_nproc;
  }
  else {
    start = PHOTON_GET_CQ_IND(ctx->num_srq, proc);
    end = start+1;
  }

  for (i = start; i < end; i++) {
    struct ibv_sge sg;
    struct ibv_recv_wr wr;
    struct ibv_recv_wr *bad_wr;
 
    memset(&sg, 0, sizeof(sg));
    memset(&wr, 0, sizeof(wr));

    wr.sg_list = &sg;
    wr.num_sge = 0;
    
    for (j = 0; j < nposts; j++) {
      wr.wr_id = (num < 0) ? j : id;
      if (ibv_post_srq_recv(ctx->ib_srq[i], &wr, &bad_wr)) {
	log_err("Could not post %d of %d SRQ RRs", j, nposts);
	return PHOTON_ERROR;
      }
    }
  }
  return PHOTON_OK;
}

int __verbs_post_rq_recv(verbs_cnct_ctx *ctx, uint64_t id, int proc, int num) {
  int i, j, start, end;
  int nposts = (num < 0) ? ctx->rx_depth : num;

  if (proc < 0) {
    start = 0;
    end = _photon_nproc;
  }
  else {
    start = proc;
    end = proc+1;
  }

  for (i = start; i < end; i++) {
    struct ibv_sge sg;
    struct ibv_recv_wr wr;
    struct ibv_recv_wr *bad_wr;
 
    memset(&sg, 0, sizeof(sg));
    memset(&wr, 0, sizeof(wr));

    wr.sg_list = &sg;
    wr.num_sge = 0;
    
    for (j = 0; j < nposts; j++) {
      wr.wr_id = (num < 0) ? j : id;
      if (ibv_post_recv(ctx->qp[i], &wr, &bad_wr)) {
	log_err("Could not post %d of %d QP RRs", j, nposts);
	return PHOTON_ERROR;
      }
    }
  }
  return PHOTON_OK;
}

int __verbs_find_max_inline(verbs_cnct_ctx *ctx, int *ret_mi) {
  // find out what our max inline data size is
  // this is not defined in the spec and is vendor specific
  if (ctx->max_inline == -1) {
    struct ibv_qp *qp;
    struct ibv_cq *cq;
    struct ibv_qp_init_attr init_attr;
    uint32_t max_inline_data;
    
    cq = ibv_create_cq(ctx->ib_context, 1, NULL, NULL,  0);
    if (!cq) {
      dbg_err("Could not create completion queue");
      return PHOTON_ERROR;
    }
    
    memset(&init_attr, 0, sizeof(init_attr));
    init_attr.qp_type = IBV_QPT_RC;
    init_attr.send_cq = cq;
    init_attr.recv_cq = cq;
    init_attr.srq = 0;
    init_attr.cap.max_send_sge = 1;
    init_attr.cap.max_recv_sge = 1;
    init_attr.cap.max_recv_wr = 1;
    
    init_attr.cap.max_inline_data = max_inline_data = 1 << 20;
    while (max_inline_data > 0) {
      qp = ibv_create_qp(ctx->ib_pd, &init_attr); 
      if (qp != NULL) {
        break;
      }
      max_inline_data >>= 1;
      init_attr.cap.max_inline_data = max_inline_data;
    }
    
    if (qp != NULL) {
      *ret_mi = max_inline_data;
      ibv_destroy_qp(qp);
    } else {
      *ret_mi = 0;
    }
  }
  else {
    *ret_mi = ctx->max_inline;
  } 
  
  return PHOTON_OK;
}

int __verbs_sync_qpn(verbs_cnct_ctx *ctx) {
  unsigned *qp_numbers = (unsigned *)calloc(_photon_nproc, sizeof(unsigned));
  unsigned local_qpn;
  unsigned max_qpn = 0;
  struct ibv_qp *tmp_qp;
  int status, i;

  struct ibv_qp_init_attr attr = {
    .send_cq        = ctx->ib_cq[0],
    .recv_cq        = ctx->ib_cq[0],
    .cap            = {
      .max_send_wr     = 1,
      .max_send_sge    = 1,
      .max_recv_wr     = 1,
      .max_recv_sge    = 1,
    },
    .qp_type        = IBV_QPT_UD
  };
  
  tmp_qp = ibv_create_qp(ctx->ib_pd, &attr);
  if (!tmp_qp) {
    dbg_err("Could not create temp QP");
    ibv_destroy_qp(tmp_qp);
    free(qp_numbers);
    return PHOTON_ERROR;
  }
  
  local_qpn = tmp_qp->qp_num;

  status = ibv_destroy_qp(tmp_qp);
  if (status) {
    dbg_err("Could not destroy temp QP: %s", strerror(status));
  }

  status = photon_exchange_allgather(&local_qpn, qp_numbers, sizeof(local_qpn));
  if (status != PHOTON_OK) {
    dbg_err("Could not allgather QP numbers");
    return PHOTON_ERROR;
  }
  
  for (i = 0; i < _photon_nproc; i++) {
    if (qp_numbers[i] > max_qpn) {
      max_qpn = qp_numbers[i];
    }
  }
  
  dbg_info("my qpn: %u, max qpn: %u", local_qpn, max_qpn);
  //printf("%d: my_qpn: %u, max_qpn: %u\n", _photon_myrank, local_qpn, max_qpn);

  if (local_qpn != max_qpn) {
    for (i = local_qpn; i <= max_qpn; i++) {
      tmp_qp = ibv_create_qp(ctx->ib_pd, &attr);
      local_qpn = tmp_qp->qp_num;
      ibv_destroy_qp(tmp_qp);
    }
  }

  dbg_info("new qpn: %u", local_qpn);
  //printf("%d: new qpn: %d\n", _photon_myrank, local_qpn);

  //exit(1);
  return PHOTON_OK;
}
