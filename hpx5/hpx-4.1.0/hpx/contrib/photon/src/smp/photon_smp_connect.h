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

#ifndef PHOTON_SMP_CONNECT_H
#define PHOTON_SMP_CONNECT_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CQ_ENTRIES 262144

typedef struct smp_cnct_ctx_t {
  int                 use_rcq;
} smp_cnct_ctx;

#ifdef __cplusplus
}
#endif

#endif
