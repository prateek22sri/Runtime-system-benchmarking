// ==================================================================-*- C++ -*-
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifndef LIBHPX_GAS_ALLOCATOR_H
#define LIBHPX_GAS_ALLOCATOR_H

#include "hpx/hpx.h"

namespace libhpx {
namespace gas {
class Allocator {
 public:
  virtual ~Allocator();

  virtual void free(hpx_addr_t gca, hpx_addr_t rsync) = 0;

  virtual hpx_addr_t alloc_cyclic(size_t n, size_t bsize, uint32_t boundary,
                                  uint32_t attr) = 0;

  virtual hpx_addr_t calloc_cyclic(size_t n, size_t bsize, uint32_t boundary,
                                   uint32_t attr) = 0;

  virtual hpx_addr_t alloc_blocked(size_t n, size_t bsize, uint32_t boundary,
                                   uint32_t attr) = 0;

  virtual hpx_addr_t calloc_blocked(size_t n, size_t bsize, uint32_t boundary,
                                    uint32_t attr) = 0;

  virtual hpx_addr_t alloc_local(size_t n, size_t bsize, uint32_t boundary,
                                 uint32_t attr) = 0;

  virtual hpx_addr_t calloc_local(size_t n, size_t bsize, uint32_t boundary,
                                  uint32_t attr) = 0;

  virtual hpx_addr_t alloc_user(size_t n, size_t bsize, uint32_t boundary,
                                hpx_gas_dist_t dist, uint32_t attr) = 0;

  virtual hpx_addr_t calloc_user(size_t n, size_t bsize, uint32_t boundary,
                                 hpx_gas_dist_t dist, uint32_t attr) = 0;
};
}
}

#endif // LIBHPX_GAS_ALLOCATOR_H
