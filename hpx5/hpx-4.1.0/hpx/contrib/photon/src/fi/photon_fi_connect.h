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

#ifndef FI_CONNECT_H
#define FI_CONNECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rdma/fabric.h>
#include <rdma/fi_domain.h>
#include <rdma/fi_errno.h>

#include "photon_attributes.h"
#include "libsync/locks.h"

typedef struct fi_cnct_ctx_t {
  struct fi_info         *fi;
  struct fi_info         *hints;
  struct fid_fabric      *fab;
  struct fid_domain      *dom;
  struct fid_av          *av;
  struct fid_ep         **eps;
  struct fid_cq         **lcq;
  struct fid_cq         **rcq;
  struct fi_context       fi_ctx_av;
  fi_addr_t              *addrs;

  int                     thread_safe;
  void                   *local_addr;
  size_t                  addr_len;
  
  uint64_t                flags;
  const char             *eth_dev;
  const char             *node;
  const char             *service;
  const char             *domain;
  const char             *provider;

  int                     num_cq;
  int                     use_rcq;

  tatas_lock_t            lock;
} fi_cnct_ctx;

PHOTON_INTERNAL int __fi_init_context(fi_cnct_ctx *ctx);
PHOTON_INTERNAL int __fi_connect_peers(fi_cnct_ctx *ctx, struct fi_info *fi);

#ifdef __cplusplus
}
#endif

#endif
