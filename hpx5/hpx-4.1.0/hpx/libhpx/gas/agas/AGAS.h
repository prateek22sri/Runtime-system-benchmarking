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

#ifndef LIBHPX_GAS_AGAS_H
#define LIBHPX_GAS_AGAS_H

#include "BlockTranslationTable.h"
#include "ChunkAllocator.h"
#include "ChunkTable.h"
#include "GlobalVirtualAddress.h"
#include "libhpx/GAS.h"
#include "libhpx/util/Bitmap.h"
#include "libhpx/util/math.h"

namespace libhpx {
namespace gas {
namespace agas {
class AGAS : public GAS {
  using GVA = GlobalVirtualAddress;
  static constexpr uint64_t THERE_OFFSET = uint64_t(4398046511103);
  static constexpr uint64_t HEAP_SIZE = uint64_t(1) << GVA_OFFSET_BITS;
  static constexpr uint64_t HEAP_ALIGN = util::ceil_log2(HEAP_SIZE);

 public:
  AGAS(const config_t* config, const boot::Network* const boot);
  ~AGAS();

  /// Interpret the here->gas as an AGAS instance.
  static AGAS* Instance() {
    dbg_assert(Instance_);
    return Instance_;
  }

  /// Initialize the global and cyclic allocators (from memory.h)
  void initAllocators(unsigned rank);

  libhpx_gas_t type() const {
    return HPX_GAS_AGAS;
  }

  uint64_t maxBlockSize() const;

  void* pinHeap(MemoryOps&, void*) const {
    return nullptr;
  }

  void unpinHeap(MemoryOps&) const {
  }

  hpx_gas_ptrdiff_t sub(hpx_addr_t lhs, hpx_addr_t rhs, size_t bsize) const {
    dbg_assert(bsize <= maxBlockSize());
    return GVA(lhs).sub(rhs, bsize);
  }

  hpx_addr_t add(hpx_addr_t gva, hpx_gas_ptrdiff_t n, size_t bsize) const {
    dbg_assert(bsize <= maxBlockSize());
    return GVA(gva).add(n, bsize);
  }

  hpx_addr_t there(uint32_t i) const {
    dbg_assert(i < here->ranks);
    return GVA(THERE_OFFSET, 0, 0, i);
  }

  uint32_t ownerOf(hpx_addr_t gva) const;

  bool tryPin(hpx_addr_t gva, void **local) {
    return btt_.tryPin(gva, local);
  }

  void unpin(hpx_addr_t gva) {
    btt_.unpin(gva);
  }

  uint32_t getAttribute(hpx_addr_t gva) const {
    return btt_.getAttr(GVA(gva));
  }

  void setAttribute(hpx_addr_t gva, uint32_t attr) {
    btt_.setAttr(GVA(gva), attr);
  }

  void move(hpx_addr_t src, hpx_addr_t dst, hpx_addr_t lco);

  /// Implement the allocator interface.
  /// @{
  void free(hpx_addr_t gva, hpx_addr_t rsync);
  hpx_addr_t alloc_cyclic(size_t n, size_t bsize, uint32_t boundary, uint32_t attr);
  hpx_addr_t calloc_cyclic(size_t n, size_t bsize, uint32_t boundary, uint32_t attr);
  hpx_addr_t alloc_blocked(size_t n, size_t bsize, uint32_t boundary,
                           uint32_t attr) {
    return alloc_user(n, bsize, boundary, Blocked, attr);
  }

  hpx_addr_t calloc_blocked(size_t n, size_t bsize, uint32_t boundary,
                            uint32_t attr) {
    return calloc_user(n, bsize, boundary, Blocked, attr);
  }

  hpx_addr_t alloc_local(size_t n, size_t bsize, uint32_t boundary,
                         uint32_t attr) {
    return allocateLocal(n, bsize, boundary, attr, false);
  }

  hpx_addr_t calloc_local(size_t n, size_t bsize, uint32_t boundary,
                          uint32_t attr) {
    return allocateLocal(n, bsize, boundary, attr, true);
  }

  hpx_addr_t alloc_user(size_t n, size_t bsize, uint32_t boundary,
                        hpx_gas_dist_t dist, uint32_t attr) {
    return allocateUser(n, bsize, boundary, dist, attr, false);
  }

  hpx_addr_t calloc_user(size_t n, size_t bsize, uint32_t boundary,
                         hpx_gas_dist_t dist, uint32_t attr) {
    return allocateUser(n, bsize, boundary, dist, attr, true);
  }
  /// @}

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

  /// Update the owner for a gva.
  void updateOwner(GVA gva, uint16_t owner) {
    btt_.updateOwner(gva, owner);
  }

  /// Allocate a chunk from the appropriate chunk allocator.
  ///
  /// This uses the BlockSizePassthrough variable internally.
  ///
  /// @param       addr A requested address for the chunk.
  /// @param          n The number of bytes to allocate.
  /// @param      align The minimum alignment for the chunk.
  /// @param     cyclic A flag indicating this should be a cyclic allocation.
  ///
  /// @returns          The base address of the chunk, or nullptr on error.
  void* chunkAllocate(void* addr, size_t n, size_t align, bool cyclic);

  /// Deallocate a chunk from the appropriate chunk allocator.
  ///
  /// @param       addr The base address to deallocate.
  /// @param          n The number of bytes to deallocate.
  /// @param     cyclic A flag indicating this should be a cyclic allocation.
  void chunkDeallocate(void* addr, size_t n, bool cyclic);

 public:
  /// Updating a block during the move operation.
  /// @{
  struct UpsertBlockArgs {
    static void* operator new(size_t bytes, size_t bsize) {
      return new char[bytes + bsize];
    }

    static void operator delete(void* ptr) {
      delete [] static_cast<char*>(ptr);
    }

    GVA        src;                             //!< The source GVA to upsert
    uint32_t  attr;                             //!< The attributes
    char     block[];                           //!< The block data
  };

  static int UpsertBlockHandler(void*, const UpsertBlockArgs& args, size_t n) {
    return Instance()->upsertBlock(args.src, args.attr, n - sizeof(args),
                                   args.block);
  }
  /// @}

  /// The first step of the move operation is to invalidate the mapping for the
  /// destination.
  static int InvalidateMappingHandler(hpx_addr_t dst, int to) {
    return Instance()->invalidateMapping(dst, to);
  }

  static int MoveHandler(hpx_addr_t src);

  static int FreeBlockHandler(hpx_addr_t block) {
    return Instance()->freeBlock(block);
  }

  static int FreeSegmentHandler(hpx_addr_t base) {
    Instance()->freeSegment(base);
    return HPX_SUCCESS;
  }

  static int InsertTranslationHandler(hpx_addr_t addr, unsigned rank,
                                      size_t blocks, uint32_t attr) {
    Instance()->insertTranslation(addr, rank, blocks, attr);
    return HPX_SUCCESS;
  }

  static int InsertUserBlockHandler(hpx_addr_t addr, size_t n, size_t padded,
                                    uint32_t attr, void *lva, int zero) {
    return Instance()->insertUserBlock(addr, n, padded, attr, lva, zero);
  }

  static int InsertCyclicSegmentHandler(hpx_addr_t addr, size_t blocks,
                                        size_t padded, uint32_t attr, void* lva,
                                        int zero) {
    Instance()->insertCyclicSegment(addr, blocks, padded, attr, lva, zero);
    return HPX_SUCCESS;
  }

  static int AllocateCyclicHandler(size_t blocks, size_t bsize,
                                   uint32_t boundary, uint32_t attr, int zero)
  {
    GVA gva = Instance()->allocateCyclic(blocks, bsize, boundary, attr, zero);
    return HPX_THREAD_CONTINUE(gva);
  }

 private:
  /// We leverage the user distribution to implement the blocked distribution.
  static hpx_addr_t Blocked(uint32_t i, size_t n, size_t bsize) {
    int blocks_per_locality = n / HPX_LOCALITIES;
    return HPX_THERE((i/blocks_per_locality) % HPX_LOCALITIES);
  }

  /// Free a block.
  ///
  /// This will remove the block's translation table mapping once its reference
  /// count hits zero. If the block was relocated (i.e., the parcel to free block
  /// wasn't sent to the home locality for the block), it then frees the cloned
  /// memory and forwards the request on to the home locality for the block,
  /// recursively.
  ///
  /// This will terminate the current lightweight thread and should only be
  /// called through the FreeBlockHandler.
  int freeBlock(GVA gva);

  /// Free a segment associated with a cyclic allocation.
  ///
  /// This function is broadcast during cyclic de-allocation. Each locality will
  /// clean up all of the blocks in the allocation that are "home" to the
  /// locality, blocking until it is completely cleaned up, and then continue
  /// back to the root.
  ///
  /// This function is also used by non-cyclic array allocations to clean up the
  /// blocks associated with the array.
  void freeSegment(GVA gva);

  int upsertBlock(GVA src, uint32_t attr, size_t bsize, const char block[]);
  int invalidateMapping(GVA dst, unsigned to);

  // Insert a translation.
  void insertTranslation(GVA gva, unsigned rank, size_t blocks, uint32_t attr);

  /// Convert a local virtual address to a global virtual address.
  GVA lvaToGVA(const void* lva, size_t bsize) const;

  /// Insert a block into the translation table for a user distribution.
  ///
  /// This will possibly create a new allocation if the block is not being
  /// inserted at the same place as the user allocation it being done. It may
  /// execute an hpx_call_cc(), so it should only be called from the
  /// InsertUserBlockHandler routine.
  ///
  /// @param        gva The address of the block to insert.
  /// @param     blocks The total number of blocks in the allocation.
  /// @param     padded The size of the block.
  /// @param       attr The allocation's attributes.
  /// @param        lva The virtual address of the block at the source rank.
  /// @param       zero True if we should zero the block.
  int insertUserBlock(GVA gva, size_t blocks, size_t padded, uint32_t attr,
                      void *lva, int zero);

  /// Insert a segment into the translation table.
  ///
  /// This will possibly create a new segment and insert it into the local
  /// translation table. For rank 0 it will just insert the lva as passed here.
  ///
  /// @param        gva The base global virtual address of the allocation.
  /// @param     blocks The number of blocks in the segment.
  /// @param     padded The padded block size.
  /// @param       attr The attributes for the segment.
  /// @param        lva The local virtual address of the segment for rank 0.
  /// @param       zero True if we should zero the segment.
  void insertCyclicSegment(GVA gva, size_t blocks, size_t padded, uint32_t attr,
                           void* lva, int zero);

  /// The basic user allocate.
  GVA allocateUser(size_t n, size_t bsize, uint32_t boundary,
                   hpx_gas_dist_t dist, uint32_t attr, int zero);

  /// The basic local allocator.
  GVA allocateLocal(size_t n, size_t bsize, uint32_t boundary, uint32_t attr,
                    bool zero);

  /// The cyclic allocator.
  GVA allocateCyclic(size_t n, size_t bsize, uint32_t boundary, uint32_t attr,
                     int zero);

  /// The agas instance needs to be available very early, before the here->gas
  /// is set, so we store this pointer as a static instance memeber.
  static AGAS* Instance_;

  /// This thread local is used to pass block sizes through to the chunk
  /// allocator, because we need to know if the chunk needs to have a very large
  /// alignment, however the callback interface used by jemalloc can't help us.
  static __thread size_t BlockSizePassthrough_;

  BlockTranslationTable btt_;                   //!< maps gva to lva
  ChunkTable         chunks_;                   //!< maps from chunk to gva
  ChunkAllocator     global_;                   //!< allocates global chunks
  ChunkAllocator    *cyclic_;                   //!< allocates cyclic chunks
  const unsigned       rank_;                   //!< current rank
  const unsigned      ranks_;                   //!< total number of ranks
};
} // namespace agas
} // namespace gas
} // namespace libhpx

#endif
