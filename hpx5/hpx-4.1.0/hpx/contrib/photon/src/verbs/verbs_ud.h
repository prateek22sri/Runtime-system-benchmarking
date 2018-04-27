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

#ifndef VERBS_UD_H
#define VERBS_UD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "verbs.h"
#include "verbs_connect.h"

PHOTON_INTERNAL int __verbs_ud_create_qp(verbs_cnct_ctx *ctx);
PHOTON_INTERNAL int __verbs_ud_attach_addr(verbs_cnct_ctx *ctx,
					   union ibv_gid *gid);
PHOTON_INTERNAL int __verbs_ud_detach_addr(verbs_cnct_ctx *ctx,
					   union ibv_gid *gid);
PHOTON_INTERNAL struct ibv_ah *__verbs_ud_create_ah(verbs_cnct_ctx *ctx,
						    union ibv_gid *gid, int lid,
						    struct ibv_ah **ret_ah);

#ifdef __cplusplus
}
#endif

#endif
