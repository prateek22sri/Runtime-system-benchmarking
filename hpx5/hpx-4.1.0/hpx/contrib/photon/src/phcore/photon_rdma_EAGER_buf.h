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

#ifndef PHOTON_RDMA_EAGER_BUF_H
#define PHOTON_RDMA_EAGER_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "photon.h"
#include "photon_buffer.h"

typedef struct photon_rdma_eager_buf_entry_t {
  uint8_t *data;
} photon_rdma_eager_buf_entry;

typedef struct photon_rdma_eager_buf_t {
  uint64_t prog;
  uint64_t curr;
  uint64_t tail;
  uint32_t size;
  uint8_t *data;
  struct photon_buffer_t remote;
  struct photon_acct_t   acct;
} photon_rdma_eager_buf;

typedef struct photon_rdma_eager_buf_entry_t * photonEagerBufEntry;
typedef struct photon_rdma_eager_buf_t       * photonEagerBuf;

PHOTON_INTERNAL photonEagerBuf photon_rdma_eager_buf_create_reuse(uint8_t *eager_buffer, int size, int prefix);
PHOTON_INTERNAL void photon_rdma_eager_buf_free(photonEagerBuf buf);
PHOTON_INTERNAL int photon_rdma_eager_buf_get_offset(int proc, photonEagerBuf buf, int size, int lim);

#ifdef __cplusplus
}
#endif

#endif
