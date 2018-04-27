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
#include <errno.h>

#include "verbs_ud.h"
#include "logging.h"

int __verbs_ud_create_qp(verbs_cnct_ctx *ctx) {
  
  struct ibv_device_attr dattr;
  if (ibv_query_device(ctx->ib_context, &dattr)) {
    dbg_err("Could not query IB device");
    return PHOTON_ERROR;
  }
  
  struct ibv_qp_init_attr attr = {
    .qp_context     = ctx,
    .send_cq        = ctx->ib_cq[0],
    .recv_cq        = ctx->ib_cq[0],
    .srq            = NULL,
    .cap            = {
      .max_send_wr     = dattr.max_qp_wr - 16,
      .max_recv_wr     = dattr.max_qp_wr - 16,
      .max_send_sge    = dattr.max_sge,
      .max_recv_sge    = dattr.max_sge,
      .max_inline_data = 0
    },
    .qp_type        = IBV_QPT_UD
  };

  ctx->ud_qp = ibv_create_qp(ctx->ib_pd, &attr);
  if (!(ctx->ud_qp)) {
    log_err("Could not create UD QP!");
    return PHOTON_ERROR;
  }
  
  {  
    struct ibv_qp_attr attr;
    
    attr.qp_state        = IBV_QPS_INIT;
    attr.pkey_index	 = 0;
    attr.qkey            = 0x11111111;
    attr.port_num	 = ctx->ib_port;
    
    if (ibv_modify_qp(ctx->ud_qp, &attr,
                      IBV_QP_STATE		 |
                      IBV_QP_PKEY_INDEX          |
                      IBV_QP_QKEY                |
                      IBV_QP_PORT)) {
      dbg_err("Failed to modify UD QP to INIT: %s", strerror(errno));
      return PHOTON_ERROR;
    }
  }

  dbg_trace("Created UD QP, qpn=%d", ctx->ud_qp->qp_num);

  {
    struct ibv_qp_attr attr = {
      .qp_state		= IBV_QPS_RTR
    };
    
    if (ibv_modify_qp(ctx->ud_qp, &attr, IBV_QP_STATE)) {
      dbg_err("Failed to modify UD QP to RTR: %s", strerror(errno));
      return PHOTON_ERROR;
    }
    
    attr.qp_state   = IBV_QPS_RTS;
    attr.sq_psn     = lrand48() & 0xffffff;
    
    if (ibv_modify_qp(ctx->ud_qp, &attr,
                      IBV_QP_STATE              |
                      IBV_QP_SQ_PSN)) {
      dbg_err("Failed to modify QP to RTS: %s", strerror(errno));
      return PHOTON_ERROR;
    }
  }

  dbg_trace("UD QP is transitioned to RTR and RTS");

  return PHOTON_OK;
}

struct ibv_ah *__verbs_ud_create_ah(verbs_cnct_ctx *ctx, union ibv_gid *gid, int lid, struct ibv_ah **ret_ah) {
  struct ibv_ah *ah = NULL;

  struct ibv_ah_attr ah_attr = {
    .is_global     = 0,
    .dlid          = lid,
    .sl            = 0,
    .src_path_bits = 0,
    .port_num      = ctx->ib_port
  };
  
  if (gid->global.interface_id) {
    ah_attr.is_global = 1;
    ah_attr.grh.hop_limit = 1;
    ah_attr.grh.dgid = *gid;
    ah_attr.grh.sgid_index = 0;
  }
  
  ah = ibv_create_ah(ctx->ib_pd, &ah_attr);
  if (!ah) {
    dbg_err("Failed to create AH: %s", strerror(errno));   
    *ret_ah = NULL;
    return NULL;
  }

  *ret_ah = ah;
  return ah;
}
int __verbs_ud_attach_addr(verbs_cnct_ctx *ctx, union ibv_gid *gid) {

  //char buf[40];
  //inet_ntop(AF_INET6, gid->raw, buf, 40);
  //dbg_trace("(%s)", buf);

  int ret;
  ret = ibv_attach_mcast(ctx->ud_qp, gid, 0x0);
  if (ret) {
    dbg_err("%s", strerror(ret));
    return PHOTON_ERROR;
  }
  
  return PHOTON_OK;
}

int __verbs_ud_detach_addr(verbs_cnct_ctx *ctx, union ibv_gid *gid) {
  int ret;
  ret = ibv_detach_mcast(ctx->ud_qp, gid, 0x0);
  if (ret) {
    dbg_err("%s", strerror(ret));
    return PHOTON_ERROR;
  }

  return PHOTON_OK;
}
