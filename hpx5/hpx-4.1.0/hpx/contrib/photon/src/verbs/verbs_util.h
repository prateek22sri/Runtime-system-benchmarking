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

#ifndef VERBS_EXCHANGE_H
#define VERBS_EXCHANGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "verbs.h"
#include "verbs_connect.h"

PHOTON_INTERNAL int __verbs_adjust_ctx(verbs_cnct_ctx *ctx);
PHOTON_INTERNAL int __verbs_handle_config(verbs_cnct_ctx *ctx, photonConfig cfg);
PHOTON_INTERNAL int __verbs_post_srq_recv(verbs_cnct_ctx *ctx, uint64_t id,
					  int proc, int num);
PHOTON_INTERNAL int __verbs_post_rq_recv(verbs_cnct_ctx *ctx, uint64_t id,
					 int proc, int num);
PHOTON_INTERNAL int __verbs_sync_qpn(verbs_cnct_ctx *ctx);
  PHOTON_INTERNAL int __verbs_find_max_inline(verbs_cnct_ctx *ctx, int *ret_mi);

#ifdef __cplusplus
}
#endif

#endif
