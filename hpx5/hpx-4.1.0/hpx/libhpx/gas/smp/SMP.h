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

#ifndef LIBHPX_GAS_SMP_SMP_H
#define LIBHPX_GAS_SMP_SMP_H

#include "libhpx/GAS.h"
#include "libhpx/debug.h"
#include "hpx/hpx.h"

namespace libhpx {
namespace gas {
class SMP : public GAS {
 public:
  libhpx_gas_t type() const {
    return HPX_GAS_SMP;
  }

  uint64_t maxBlockSize() const {
    return UINT64_MAX;
  }

  void* pinHeap(MemoryOps&, void*) const {
    dbg_error("SMP execution should not call this function\n");
  }

  void unpinHeap(MemoryOps&) const {
    dbg_error("SMP execution should not call this function\n");
  }

  hpx_gas_ptrdiff_t sub(hpx_addr_t lhs, hpx_addr_t rhs, size_t bsize) const {
    return lhs - rhs;
  }

  hpx_addr_t add(hpx_addr_t gva, hpx_gas_ptrdiff_t n, size_t bsize) const {
    return gva + n;
  }

  hpx_addr_t there(uint32_t i) const {
    dbg_assert_str(i == 0, "Rank %d does not exist in the SMP GAS\n", i);
    // We use the address of the global "here" locality to represent HPX_HERE.
    return ToGVA(here);
  }

  uint32_t ownerOf(hpx_addr_t gva) const {
    return 0;
  }

  bool tryPin(hpx_addr_t gva, void **local) {
    if (local) {
      *local = ToLVA(gva);
    }
    return true;
  }

  void unpin(hpx_addr_t gva) {
  }

  uint32_t getAttribute(hpx_addr_t gva) const {
    dbg_error("SMP execution should not call this function\n");
  }

  void setAttribute(hpx_addr_t gva, uint32_t attr) {
        dbg_error("SMP execution should not call this function\n");
  }

  void move(hpx_addr_t src, hpx_addr_t dst, hpx_addr_t sync) {
    hpx_lco_set(sync, 0, NULL, HPX_NULL, HPX_NULL);
  }

  /// Implement the StringOps interface.
  /// @{
  void memget(void *dest, hpx_addr_t src, size_t n, hpx_addr_t lsync,
              hpx_addr_t rsync) {
    copy(dest, ToLVA(src), n, lsync, rsync);
  }

  void memget(void *dest, hpx_addr_t src, size_t n, hpx_addr_t lsync) {
    copy(dest, ToLVA(src), n, lsync);
  }

  void memget(void *dest, hpx_addr_t src, size_t n) {
    copy(dest, ToLVA(src), n);
  }

  void memput(hpx_addr_t dest, const void *src, size_t n, hpx_addr_t lsync,
              hpx_addr_t rsync) {
    copy(ToLVA(dest), src, n, lsync, rsync);
  }

  void memput(hpx_addr_t dest, const void *src, size_t n, hpx_addr_t rsync) {
    copy(ToLVA(dest), src, n, rsync);
  }

  void memput(hpx_addr_t dest, const void *src, size_t n) {
    copy(ToLVA(dest), src, n);
  }

  void memcpy(hpx_addr_t dest, hpx_addr_t src, size_t n, hpx_addr_t sync) {
    copy(ToLVA(dest), ToLVA(src), n, sync);
  }

  void memcpy(hpx_addr_t dest, hpx_addr_t src, size_t n) {
    copy(ToLVA(dest), ToLVA(src), n);
  }
  /// @}

  /// Implement the Allocator interface.
  /// @{
  void free(hpx_addr_t gca, hpx_addr_t rsync);

  hpx_addr_t alloc_local(size_t n, size_t bsize, uint32_t boundary,
                         uint32_t attr);

  hpx_addr_t calloc_local(size_t n, size_t bsize, uint32_t boundary,
                          uint32_t attr);

  hpx_addr_t alloc_cyclic(size_t n, size_t bsize, uint32_t boundary,
                          uint32_t attr) {
    return alloc_local(n, bsize, boundary, attr);
  }

  hpx_addr_t calloc_cyclic(size_t n, size_t bsize, uint32_t boundary,
                           uint32_t attr) {
    return calloc_local(n, bsize, boundary, attr);
  }

  hpx_addr_t alloc_blocked(size_t n, size_t bsize,
                           uint32_t boundary, uint32_t attr) {
    return alloc_local(n, bsize, boundary, attr);
  }

  hpx_addr_t calloc_blocked(size_t n, size_t bsize,
                            uint32_t boundary, uint32_t attr) {
    return calloc_local(n, bsize, boundary, attr);
  }

  hpx_addr_t alloc_user(size_t n, size_t bsize, uint32_t boundary,
                        hpx_gas_dist_t dist, uint32_t attr) {
    return alloc_local(n, bsize, boundary, attr);
  }

  hpx_addr_t calloc_user(size_t n, size_t bsize, uint32_t boundary,
                         hpx_gas_dist_t dist, uint32_t attr) {
    return calloc_local(n, bsize, boundary, attr);
  }
  /// @}

 private:
  void copy(void* dest, const void* src, size_t n, hpx_addr_t lsync,
            hpx_addr_t rsync);

  void copy(void* dest, const void* src, size_t n, hpx_addr_t sync) {
    copy(dest, src, n, sync, HPX_NULL);
  }

  void copy(void* dest, const void* src, size_t n) {
    copy(dest, src, n, HPX_NULL, HPX_NULL);
  }

  static hpx_addr_t ToGVA(void* lva) {
    return hpx_addr_t(reinterpret_cast<uintptr_t>(lva));
  }

  static void* ToLVA(hpx_addr_t gva) {
    return reinterpret_cast<void*>(uintptr_t(gva));
  }
};
}
}

#endif // LIBHPX_GAS_SMP_SMP_H
