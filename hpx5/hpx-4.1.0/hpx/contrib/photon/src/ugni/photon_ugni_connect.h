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

#ifndef PHOTON_UGNI_CONNECT_H
#define PHOTON_UGNI_CONNECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <netinet/in.h>
#include "photon_ugni.h"
#include "libsync/locks.h"

#define MAX_CQ_ENTRIES 262144

typedef struct ugni_cnct_ctx_t {
  const char         *gemini_dev;
  uint32_t            cdm_id;
  gni_cdm_handle_t    cdm_handle;
  gni_nic_handle_t    nic_handle;
  gni_cq_handle_t    *local_cq_handles;
  gni_cq_handle_t    *remote_cq_handles;
  gni_ep_handle_t    *ep_handles;
  int                 num_cq;
  int                 use_rcq;
  unsigned int        max_cqd;

  tatas_lock_t        lock;
} ugni_cnct_ctx;

typedef struct ugni_cnct_info_t {
  unsigned int lid;
  struct in_addr ip;
} ugni_cnct_info;

PHOTON_INTERNAL int __ugni_init_context(ugni_cnct_ctx *ctx);
PHOTON_INTERNAL int __ugni_connect_peers(ugni_cnct_ctx *ctx);

#ifdef __cplusplus
}
#endif

#endif
