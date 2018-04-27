// =============================================================================
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/// @file  libhpx/gas/smp/smp.c
///
/// @brief This file contains an implementation of the GAS interface for use
///        when no network is available, or when we are running on a single
///        locality. It simply forwards all requests to the system allocator.
#include "SMP.h"
#include <cstring>
#include <cstdlib>

namespace {
using libhpx::gas::SMP;
}

void
SMP::free(hpx_addr_t addr, hpx_addr_t sync)
{
  std::free(ToLVA(addr));
  hpx_lco_set(sync, 0, NULL, HPX_NULL, HPX_NULL);
}

hpx_addr_t
SMP::alloc_local(size_t n, size_t bsize, uint32_t boundary, uint32_t attr)
{
  size_t bytes = n * bsize;
  void *p = NULL;
  if (boundary) {
    dbg_check(posix_memalign(&p, boundary, bytes));
  } else {
    p = std::malloc(bytes);
  }
  return ToGVA(p);
}

hpx_addr_t
SMP::calloc_local(size_t n, size_t bsize, uint32_t boundary, uint32_t attr)
{
  void *p = NULL;
  if (boundary) {
    auto bytes = n * bsize;
    dbg_check(posix_memalign(&p, boundary, bytes));
    p = std::memset(p, 0, bytes);
  } else {
    p = std::calloc(n, bsize);
  }
  return ToGVA(p);
}

void
SMP::copy(void* dest, const void* src, size_t n, hpx_addr_t lsync,
          hpx_addr_t rsync)
{
  if (n) {
    dbg_assert(dest != nullptr);
    dbg_assert(src != nullptr);
    std::memcpy(dest, src, n);
  }
  hpx_lco_error(lsync, HPX_SUCCESS, HPX_NULL);
  hpx_lco_error(rsync, HPX_SUCCESS, HPX_NULL);
}
