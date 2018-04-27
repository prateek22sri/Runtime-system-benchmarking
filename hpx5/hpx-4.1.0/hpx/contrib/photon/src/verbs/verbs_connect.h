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

#ifndef VERBS_CONNECT_H
#define VERBS_CONNECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "verbs.h"

typedef struct verbs_cnct_info_t {
  unsigned lid;
  unsigned qpn;
  unsigned psn;
  int cma_port;
  struct in_addr ip;
  union ibv_gid gid;
} verbs_cnct_info;

typedef struct verbs_cnct_ctx_t {
  const char                *ib_dev;
  const char                *eth_dev;
  int                        ib_port;
  struct ibv_context        *ib_context;
  struct ibv_pd             *ib_pd;
  struct ibv_cq            **ib_cq;
  struct ibv_cq            **ib_rq;
  struct ibv_cq             *ib_ud_cq;
  struct ibv_srq           **ib_srq;
  struct ibv_comp_channel   *ib_cc;
  int                        ib_lid;
  int                        ib_mtu;
  enum ibv_mtu               ib_mtu_attr;

  struct rdma_event_channel *cm_schannel;
  struct rdma_cm_id        **cm_id;

  struct ibv_qp            **qp;
  struct ibv_qp             *ud_qp;
  int                        psn;
  int                        num_qp;
  int                        use_cma;
  int                        use_ud;

  int                        tx_depth;
  int                        rx_depth;
  int                        atomic_depth;
  int                        max_sge;
  int                        max_inline;
  int                        max_qp_wr;
  int                        max_srq_wr;
  int                        max_cqe;
  int                        num_cq;
  int                        num_srq;
  int                        use_rcq;

  verbs_cnct_info          **local_ci;
  verbs_cnct_info          **remote_ci;
  
  uint32_t                  *tx_cnt;
  uint32_t                  *rx_cnt;
} verbs_cnct_ctx;

PHOTON_INTERNAL int __verbs_init_context(verbs_cnct_ctx *ctx);
PHOTON_INTERNAL int __verbs_connect_peers(verbs_cnct_ctx *ctx);
PHOTON_INTERNAL int __verbs_connect_single(verbs_cnct_ctx *ctx,
					   verbs_cnct_info *local_info,
					   verbs_cnct_info *remote_info,
					   int pindex, verbs_cnct_info **ret_ci,
					   int *ret_len,
					   photon_connect_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
