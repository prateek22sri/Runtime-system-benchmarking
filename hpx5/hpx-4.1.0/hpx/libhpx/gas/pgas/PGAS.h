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

#ifndef LIBHPX_GAS_PGAS_H
#define LIBHPX_GAS_PGAS_H

#include "Allocator.h"
#include "HeapSegment.h"
#include "libhpx/GAS.h"
#include "libhpx/debug.h"
#include "libhpx/gpa.h"
#include "libhpx/locality.h"

namespace libhpx {
namespace gas {
namespace pgas {
class PGAS : public GAS, public HeapSegment {
 public:
  PGAS(const config_t* config);
  ~PGAS();

  libhpx_gas_t type() const {
    return HPX_GAS_PGAS;
  }

  uint64_t maxBlockSize() const {
    return HeapSegment::maxBlockSize();
  }

  void* pinHeap(MemoryOps& ops, void* key) const {
    ops.pin(base_, nBytes_, key);
    return base_;
  }

  void unpinHeap(MemoryOps& ops) const {
    ops.unpin(base_, nBytes_);
  }

  hpx_gas_ptrdiff_t sub(hpx_addr_t lhs, hpx_addr_t rhs, size_t bsize) const {
    bool cyclic = isCyclicAddress(lhs);
    dbg_assert(cyclic == isCyclicAddress(rhs));
    return (cyclic) ? gpa_sub_cyclic(lhs, rhs, bsize) : gpa_sub(lhs, rhs);
  }

  hpx_addr_t add(hpx_addr_t gpa, hpx_gas_ptrdiff_t n, size_t bsize) const {
    bool cyclic = isCyclicAddress(gpa);
    return (cyclic) ? gpa_add_cyclic(gpa, n, bsize) : gpa_add(gpa, n);
  }

  hpx_addr_t there(uint32_t i) const {
    return offset_to_gpa(i, UINT64_MAX);
  }

  uint32_t ownerOf(hpx_addr_t gpa) const {
    return gpa_to_rank(gpa);
  }

  bool tryPin(hpx_addr_t gva, void **local);

  void unpin(hpx_addr_t gva) {
    dbg_assert(tryPin(gva, nullptr));
  }

  uint32_t getAttribute(hpx_addr_t gva) const {
    log_error("PGAS setAttribute unimplemented");
    return HPX_ATTR_NONE;
  }

  void setAttribute(hpx_addr_t gva, uint32_t attr) {
    log_error("PGAS setAttribute unimplemented");
  }

  void move(hpx_addr_t src, hpx_addr_t dst, hpx_addr_t lco) {
    hpx_lco_set(lco, 0, NULL, HPX_NULL, HPX_NULL);
  }

  void free(hpx_addr_t gva, hpx_addr_t rsync);

  hpx_addr_t alloc_cyclic(size_t n, size_t bsize, uint32_t boundary,
                          uint32_t attr);

  hpx_addr_t calloc_cyclic(size_t n, size_t bsize, uint32_t boundary,
                           uint32_t attr);

  hpx_addr_t alloc_local(size_t n, size_t bsize, uint32_t boundary,
                         uint32_t attr);

  hpx_addr_t calloc_local(size_t n, size_t bsize, uint32_t boundary,
                          uint32_t attr);

  hpx_addr_t alloc_blocked(size_t n, size_t bsize, uint32_t boundary,
                           uint32_t attr) {
    dbg_error("Blocked GAS distributions are not supported.\n");
  }

  hpx_addr_t calloc_blocked(size_t n, size_t bsize, uint32_t boundary,
                            uint32_t attr) {
    dbg_error("Blocked GAS distributions are not supported.\n");
  }

  hpx_addr_t alloc_user(size_t n, size_t bsize, uint32_t boundary,
                        hpx_gas_dist_t dist, uint32_t attr) {
    dbg_error("User-defined GAS distributions not supported for PGAS.\n");
  }

  hpx_addr_t calloc_user(size_t n, size_t bsize, uint32_t boundary,
                         hpx_gas_dist_t dist, uint32_t attr) {
    dbg_error("User-defined GAS distributions not supported for PGAS.\n");
  }

  /// Implement the string interface.
  /// @{
  void memget(void *dest, hpx_addr_t src, size_t n, hpx_addr_t lsync, hpx_addr_t rsync);
  void memget(void *dest, hpx_addr_t src, size_t n, hpx_addr_t lsync);
  void memget(void *dest, hpx_addr_t src, size_t n);
  void memput(hpx_addr_t dest, const void *src, size_t n, hpx_addr_t lsync, hpx_addr_t rsync);
  void memput(hpx_addr_t dest, const void *src, size_t n, hpx_addr_t rsync);
  void memput(hpx_addr_t dest, const void *src, size_t n);
  void memcpy(hpx_addr_t dest, hpx_addr_t src, size_t n, hpx_addr_t sync);
  void memcpy(hpx_addr_t dest, hpx_addr_t src, size_t n);
  /// @}

 public:
  /// Asynchronous entry point for the allocate operation
  static int AllocateCyclicBlocksHandler(size_t n, size_t bsize, uint32_t align,
                                         uint32_t attr, int zero)
  {
    dbg_assert(here->gas->type() == HPX_GAS_PGAS);
    PGAS* pgas = static_cast<PGAS*>(here->gas);
    hpx_addr_t addr = pgas->allocateCyclicBlocks(n, bsize, align, attr, zero);
    return HPX_THREAD_CONTINUE(addr);
  }

  /// Asynchronous entry point for the setCsbrk operation.
  static int SetCsbrkHandler(size_t offset) {
    dbg_assert(here->gas->type() == HPX_GAS_PGAS);
    PGAS* pgas = static_cast<PGAS*>(here->gas);
    if (pgas->setCsbrk(offset)) {
      dbg_error("cyclic allocation ran out of memory at rank %u", here->rank);
    }
    return HPX_SUCCESS;
  }

  /// Asynchronous entry point for the zeroBlocks operation.
  ///
  /// @param     offset The base offset for the cyclic allocation.
  /// @param          n The total number of cyclic blocks.
  /// @param      bsize The block size.
  static int ZeroBlocksHandler(uint64_t offset, size_t n, size_t bsize) {
    dbg_assert(here->gas->type() == HPX_GAS_PGAS);
    PGAS* pgas = static_cast<PGAS*>(here->gas);
    pgas->zeroBlocks(offset, n, bsize);
    return HPX_SUCCESS;
  }

  /// Asynchronous entry point for the free operation.
  ///
  /// We could encode the @p gpa as the thread target and thus save some space
  /// on the message, but for symmetries sake we don't.
  ///
  /// @param        gpa The gpa to free.
  /// @param        lco An lco to signal when the free operation completes.
  /// @returns             HPX_SUCCSS
  static int FreeHandler(hpx_addr_t gpa, hpx_addr_t lco) {
    dbg_assert(here->gas->type() == HPX_GAS_PGAS);
    PGAS* pgas = static_cast<PGAS*>(here->gas);
    pgas->free(gpa, lco);
    return HPX_SUCCESS;
  }

 private:
  /// Convert a global address into a local virtual address.
  ///
  /// @param          gpa The global physical address.
  ///
  /// @returns            The corresponding local virtual address.
  void* gpaToLVA(hpx_addr_t gpa) const {
    return offsetToLVA(gpa_to_offset(gpa));
  }

  /// Convert a heap segment offset into a global address.
  hpx_addr_t offsetToGPA(uint64_t offset) const {
    return offset_to_gpa(here->rank, offset);
  }

  /// Check if a global address is in the cyclic heap.
  bool isCyclicAddress(hpx_addr_t gpa) const {
    return isCyclicOffset(gpa_to_offset(gpa));
  }

  /// The core allocation routine.
  ///
  /// This allocates contiguous blocks of memory from the designated address
  /// space, zeroing them if necessary. Blocks are always aligned to a 2^k
  /// boundary, so the total number of bytes allocated may be more than n *
  /// bsize.
  ///
  /// @param        n The number of blocks to allocate.
  /// @param    bsize The block size for this allocation.
  /// @param boundary The alignment boundary.
  /// @param     attr Attributes for the allocation.
  /// @param     zero A flag that indicates if we should zero the allocation.
  /// @param   cyclic A flag indicating the blocks must be cyclic.
  ///
  /// @returns        The base offset of the global allocation.
  uint64_t allocateBlocks(size_t n, size_t bsize, uint32_t boundary,
                            uint32_t attr, bool zero, bool cyclic);

  /// Zero the memory for a sequence of blocks.
  ///
  /// @param     offset The base offset for the blocks.
  /// @param          n The number of contiguous blocks.
  /// @param      bsize The block size.
  void zeroBlocks(uint64_t offset, size_t n, size_t bsize);

  /// The cyclic block allocator.
  ///
  /// This allocates n blocks in a cyclic distribution. It must only be called
  /// at rank 0.
  ///
  /// @param        n The number of blocks to allocate.
  /// @param    bsize The block size for this allocation.
  /// @param boundary The alignment boundary.
  /// @param     attr Attributes for the allocation.
  /// @param     zero A flag that indicates if we should zero the allocation.
  ///
  /// @returns        The based address of the global allocation.
  uint64_t allocateCyclicBlocks(size_t n, size_t bsize, uint32_t boundary,
                                uint32_t attr, int zero);

  GlobalAllocator global_;
  CyclicAllocator cyclic_;
};
} // namespace pgas
} // namespace gas
} // namespace libhpx

#endif // LIBHPX_GAS_PGAS_H


