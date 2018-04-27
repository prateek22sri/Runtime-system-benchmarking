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

#ifndef NBC_PWC_H
#define NBC_PWC_H

#include <nbc.h>
#include "photon_coll.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEMPOOL_INIT_SZ 500

extern photon_coll_interface nbc_collectives;

struct MemPool;

PHOTON_INTERNAL NBC_Handle *photon_pwc_get_nbc_handle(photon_cid);
PHOTON_INTERNAL void photon_nbc_map_handle(photonRequest req,
                                           NBC_Handle *handle);
PHOTON_INTERNAL void add_nbc_comp_event(completion_info_t *comp);
PHOTON_INTERNAL completion_info_t *get_nbc_comp_event();
PHOTON_INTERNAL void push_back_to_test_queue(NBC_Handle *handle);
PHOTON_INTERNAL int test_some_nbc_handles();

/// A simple expanding memory pool
typedef struct MemPool {
  void **ptr_array;
  size_t array_sz;
  void (*put)(struct MemPool *p, size_t idx, void *ptr);
  void *(*get)(struct MemPool *p, size_t idx);
  void (*clean)(struct MemPool *p);
} MemPool_t;

/// external init Iface
void Init_MemPool(MemPool_t *p) ;

#ifdef __cplusplus
}
#endif

#endif
