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

#include "AGAS.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/events.h"
#include "libhpx/gpa.h"
#include "libhpx/locality.h"
#include "libhpx/memory.h"
#include "libhpx/rebalancer.h"
#include "libhpx/Worker.h"                      // self->getCurrentParcel()
#include "libhpx/util/math.h"
#include <cstdlib>
#include <cstring>

namespace {
using libhpx::util::ceil_div;
using libhpx::util::ceil_log2;
using libhpx::gas::agas::AGAS;
using libhpx::gas::agas::ChunkAllocator;
using libhpx::gas::agas::GlobalVirtualAddress;

LIBHPX_ACTION(HPX_DEFAULT, 0, InsertTranslation, AGAS::InsertTranslationHandler,
              HPX_ADDR, HPX_UINT, HPX_SIZE_T, HPX_UINT32);
LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_PINNED, UpsertBlock,
              AGAS::UpsertBlockHandler, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);
LIBHPX_ACTION(HPX_DEFAULT, 0, InvalidateMapping, AGAS::InvalidateMappingHandler,
              HPX_ADDR, HPX_INT);
LIBHPX_ACTION(HPX_DEFAULT, 0, Move, AGAS::MoveHandler, HPX_ADDR);
LIBHPX_ACTION(HPX_DEFAULT, 0, FreeBlock, AGAS::FreeBlockHandler, HPX_ADDR);
LIBHPX_ACTION(HPX_DEFAULT, 0, FreeSegment, AGAS::FreeSegmentHandler, HPX_ADDR);
LIBHPX_ACTION(HPX_DEFAULT, 0, InsertUserBlock, AGAS::InsertUserBlockHandler,
              HPX_ADDR, HPX_SIZE_T, HPX_SIZE_T, HPX_UINT32, HPX_POINTER,
              HPX_INT);
LIBHPX_ACTION(HPX_DEFAULT, 0,
              InsertCyclicSegment, AGAS::InsertCyclicSegmentHandler, HPX_ADDR,
              HPX_SIZE_T, HPX_SIZE_T, HPX_UINT32, HPX_POINTER, HPX_INT);
LIBHPX_ACTION(HPX_DEFAULT, 0, AllocateCyclic, AGAS::AllocateCyclicHandler,
              HPX_SIZE_T, HPX_SIZE_T, HPX_UINT32, HPX_UINT32, HPX_INT);
}

__thread size_t AGAS::BlockSizePassthrough_;
AGAS* AGAS::Instance_;

AGAS::AGAS(const config_t* config, const boot::Network* const boot)
    : btt_(0),
      chunks_(0),
      global_(chunks_, HEAP_SIZE),
      cyclic_(nullptr),
      rank_(boot->getRank()),
      ranks_(boot->getNRanks())
{
  dbg_assert(!Instance_);
  Instance_ = this;

  if (rank_ == 0) {
    cyclic_ = new ChunkAllocator(chunks_, HEAP_SIZE);
  }

  initAllocators(rank_);
  rebalancer_init();

  GlobalVirtualAddress gva(there(rank_));
  btt_.insert(gva, rank_, here, 1, HPX_GAS_ATTR_NONE);
}

AGAS::~AGAS()
{
  delete cyclic_;
  rebalancer_finalize();
}

uint64_t
AGAS::maxBlockSize() const
{
  return uint64_t(1) << GPA_MAX_LG_BSIZE;
}

uint32_t
AGAS::ownerOf(hpx_addr_t addr) const
{
  GVA gva(addr);

  if (gva.offset == THERE_OFFSET) {
    return gva.home;
  }

  bool cached;
  uint32_t owner = btt_.getOwner(gva, cached);
  if (!cached) {
    EVENT_GAS_MISS(addr, owner);
  }
  dbg_assert(owner < ranks_);
  return owner;
}

int
AGAS::upsertBlock(GVA src, uint32_t attr, size_t bsize, const char block[])
{
  void *lva = std::malloc(bsize);
  std::memcpy(lva, block, bsize);
  btt_.upsert(src, rank_, lva, 1, attr);
  return HPX_SUCCESS;
}

int
AGAS::invalidateMapping(GVA dst, unsigned to)
{
  // unnecessary to move to the same locality
  if (rank_ == to) {
    return HPX_SUCCESS;
  }

  GVA src(hpx_thread_current_target());

  // instrument the move event
  EVENT_GAS_MOVE(src, HPX_HERE, dst);

  if (btt_.getOwner(src) != rank_) {
    return HPX_RESEND;
  }

  size_t bsize = src.getBlockSize();
  std::unique_ptr<UpsertBlockArgs> args(new(bsize) UpsertBlockArgs());
  args->src = src;
  void* lva = btt_.move(src, to, args->attr);
  std::memcpy(args->block, lva, bsize);
  if (src.home != rank_) {
    std::free(lva);
  }

  // @todo We could explicitly send a continuation parcel to avoid a
  //       malloc/memcpy/free here.
  size_t bytes = sizeof(UpsertBlockArgs) + bsize;
  void* buffer = args.get();
  return hpx_call_cc(dst, UpsertBlock, buffer, bytes);
}

int
AGAS::MoveHandler(hpx_addr_t src)
{
  hpx_addr_t dst = hpx_thread_current_target();
  return hpx_call_cc(src, InvalidateMapping, &dst, &here->rank);
}

void
AGAS::move(hpx_addr_t src, hpx_addr_t dst, hpx_addr_t sync)
{
  bool found;
  uint32_t owner = btt_.getOwner(dst, found);
  if (found) {
    hpx_call(src, InvalidateMapping, sync, &dst, &owner);
  }
  else {
    hpx_call(dst, Move, sync, &src);
  }
}

/// This will free an allocation.
///
/// This must be called on the base address in the allocation. It will attempt
/// to perform a synchronous fastpath free operation for single blocks that have
/// not been relocated. This is an important use case that is mostly successful
/// for LCOs.
///
/// If the fastpath fails then it will use the FreeSegment handler to free the
/// allocation. For cyclic allocations, the FreeSegment is broadcast to all of
/// the ranks, while for global allocations the FreeSegment is sent *only* to
/// the home for the segment.
void
AGAS::free(hpx_addr_t addr, hpx_addr_t rsync) {
  GVA gva(addr);
  if (addr == HPX_NULL) {
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
  else if (void* lva = btt_.tryRemoveForFastpathFree(gva)) {
    global_free(lva);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
  else if (gva.cyclic) {
    // We can't return early here, because the FreeSegment operation at rank 0
    // returns the entire allocation to the cyclic allocator, and it could be
    // reused before the rest of the segments have been freed.
    hpx_bcast_rsync(FreeSegment, &gva);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
  else {
    hpx_call(HPX_THERE(gva.home), FreeSegment, rsync, &gva);
  }
}

int
AGAS::freeBlock(GVA gva)
{
  void *lva = btt_.remove(gva);
  if (gva.home == rank_) {
    return HPX_SUCCESS;
  }

  // This was a relocated block, free the clone and then forward the request
  // back to the home locality.
  dbg_assert(lva);
  std::free(lva);
  hpx_addr_t block = gva;
  return hpx_call_cc(block, FreeBlock, &block);
}

void
AGAS::freeSegment(GVA gva)
{
  size_t bsize = gva.getBlockSize();

  // For cyclic allocations the gva is actually the base of the entire
  // allocation, so we adjust that to be the base of the segment at this rank.
  if (gva.cyclic) {
    gva += (rank_ * bsize);
  }

  // Get the number of blocks and the lva backing the segment.
  size_t blocks;
  char* lva = btt_.getSegment(gva, blocks);

  // Iterate through the correct subset of blocks.
  size_t stride = (gva.cyclic) ? ranks_ * bsize : bsize;

  // Free all the blocks.
  hpx_addr_t done = hpx_lco_and_new(blocks);
  for (unsigned i = 0; i < blocks; ++i, gva += stride) {
    dbg_assert(gva.home == rank_);
    hpx_call(gva, FreeBlock, done, &gva);
  }
  hpx_lco_wait(done);
  hpx_lco_delete_sync(done);

  // Return the segment memory to the appropriate allocator.
  if (!gva.cyclic) {
    global_free(lva);
  }
  else if (rank_) {
    std::free(lva);
  }
  else {
    cyclic_free(lva);
  }
}

GlobalVirtualAddress
AGAS::lvaToGVA(const void *lva, size_t bsize) const
{
  dbg_assert(bsize <= maxBlockSize());
  uint64_t offset = chunks_.offsetOf(lva);
  return GVA(offset, ceil_log2(bsize), 0, rank_);
}

void
AGAS::insertTranslation(GVA gva, unsigned rank, size_t blocks, uint32_t attr)
{
  btt_.upsert(gva, rank, nullptr, blocks, attr);
}

int
AGAS::insertUserBlock(GVA gva, size_t blocks, size_t padded, uint32_t attr,
                      void *lva, int zero)
{
  dbg_assert(gva);
  dbg_assert(blocks);
  dbg_assert(padded);
  dbg_assert(lva);

  const hpx_parcel_t *p = self->getCurrentParcel();

  // If the block isn't being inserted at the source of the allocation request
  // then it's basically a moved block, and we need to allocate backing space
  // for it locally.
  if (p->src != rank_) {
    size_t boundary = std::max(size_t(8u), padded);
    if (posix_memalign(&lva, boundary, padded)) {
      dbg_error("Failed memalign\n");
    }
  }

  // Zero the memory if necessary.
  if (zero) {
    std::memset(lva, 0, padded);
  }

  // Insert the translation
  btt_.insert(gva, rank_, lva, blocks, attr);

  // And push the translation back to the source locality if we're
  // out-of-place from the initial call.
  if (p->src != rank_) {
    hpx_addr_t src = HPX_THERE(p->src);
    return hpx_call_cc(src, InsertTranslation, &gva, &rank_, &blocks, &attr);
  }
  else {
    return HPX_SUCCESS;
  }
}

void
AGAS::insertCyclicSegment(GVA gva, size_t blocks, size_t padded, uint32_t attr,
                          void *lva, int zero)
{
  dbg_assert(gva);
  dbg_assert(blocks);
  dbg_assert(padded);
  dbg_assert(lva);

  size_t bytes = blocks * padded;
  if (rank_ != 0) {
    size_t boundary = std::max(size_t(8u), padded);
    if (posix_memalign(&lva, boundary, bytes)) {
      dbg_error("Failed memalign\n");
    }
  }

  if (zero) {
    std::memset(lva, 0, bytes);
  }

  // Adjust the gva to point at the first block on this rank.
  gva = gva.add(rank_ * padded, padded);

  // And insert translations for all of the blocks.
  for (unsigned i = 0; i < blocks; ++i) {
    btt_.insert(gva, rank_, lva, blocks, attr);
    gva = gva.add(ranks_ * padded, padded);
    lva = static_cast<char*>(lva) + padded;
  }
}

/// @todo why are we ignoring boundary here?
GlobalVirtualAddress
AGAS::allocateUser(size_t n, size_t bsize, uint32_t, hpx_gas_dist_t dist,
                   uint32_t attr, int zero)
{
  dbg_assert(bsize <= maxBlockSize());

  // determine the padded block size
  size_t padded = uintptr_t(1) << ceil_log2(bsize);

  // Out of band communication to the chunk allocator infrastructure.
  BlockSizePassthrough_ = padded;

  // Allocate the blocks as a contiguous, aligned array from local memory.
  void *local = global_memalign(padded, n * padded);
  if (!local) {
    dbg_error("failed user-defined allocation\n");
  }

  // Iterate through all of the blocks and insert them into the block
  // translation tables. We use an asynchronous interface here in order to mask
  // the difference between local blocks and global blocks.
  GVA gva = lvaToGVA(local, padded);
  hpx_addr_t done = hpx_lco_and_new(n);
  for (unsigned i = 0; i < n; ++i) {
    hpx_addr_t where = (i == 0) ? HPX_HERE : dist(i, n, bsize);
    GVA addr = gva.add(i * padded, padded);
    const void* lva = static_cast<char*>(local) + i * padded;
    hpx_call(where, InsertUserBlock, done, &addr, &n, &padded, &attr, &lva,
             &zero);
  }
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);
  return gva;
}

GlobalVirtualAddress
AGAS::allocateLocal(size_t n, size_t bsize, uint32_t boundary, uint32_t attr,
                    bool zero)
{
  dbg_assert(bsize < maxBlockSize());

  // use the local allocator to get some memory that is part of the global
  // address space
  uint32_t padded = uintptr_t(1) << ceil_log2(bsize);
  uint32_t aligned = max_u32(boundary, padded);

  // Out of band communication to the chunk allocator infrastructure.
  BlockSizePassthrough_ = aligned;

  void *lva = global_memalign(aligned, n * padded);
  if (!lva) {
    return HPX_NULL;
  }

  if (zero) {
    std::memset(lva, 0, n * padded);
  }

  GVA gva = lvaToGVA(lva, padded);
  for (unsigned i = 0; i < n; ++i) {
    GVA block = gva.add(i * padded, padded);
    void* local = static_cast<char*>(lva) + i * padded;
    btt_.insert(block, rank_, local, n, attr);
  }
  return gva;
}

/// @todo: why do we ignore boundary?
GlobalVirtualAddress
AGAS::allocateCyclic(size_t n, size_t bsize, uint32_t, uint32_t attr, int zero)
{
  dbg_assert(rank_ == 0);
  dbg_assert(bsize <= maxBlockSize());

  // Figure out how many blocks per node we need, and what the size is.
  auto blocks = ceil_div(n, size_t(ranks_));
  auto  align = ceil_log2(bsize);
  auto padded = size_t(1u) << align;

  // Transmit the padded block size through to the chunk allocator.
  BlockSizePassthrough_ = padded;

  // Allocate the blocks as a contiguous, aligned array from cyclic memory. This
  // only happens at rank 0, where the cyclic_memalign is functional. We use
  // this segment to define the GVA of the allocation, and then broadcast a
  // request for the entire system to allocate their own segment.
  void *lva = cyclic_memalign(padded, blocks * padded);
  if (!lva) {
    dbg_error("failed cyclic allocation\n");
  }

  GVA gva = lvaToGVA(lva, padded);
  gva.cyclic = 1;
  if (hpx_bcast_rsync(InsertCyclicSegment, &gva, &blocks, &padded, &attr, &lva,
                      &zero)) {
    dbg_error("failed to insert btt entries.\n");
  }
  return gva;
}

hpx_addr_t
AGAS::alloc_cyclic(size_t n, size_t bsize, uint32_t boundary, uint32_t attr) {
  int zero = 0;
  GVA gva;
  if (hpx_call_sync(HPX_THERE(0), AllocateCyclic, &gva, sizeof(gva), &n, &bsize,
                    &boundary, &attr, &zero)) {
    dbg_error("Failed to allocate.\n");
  }
  return gva;
}

hpx_addr_t
AGAS::calloc_cyclic(size_t n, size_t bsize, uint32_t boundary, uint32_t attr) {
  int zero = 1;
  GVA gva;
  if (hpx_call_sync(HPX_THERE(0), AllocateCyclic, &gva, sizeof(gva), &n, &bsize,
                    &boundary, &attr, &zero)) {
    dbg_error("Failed to allocate.\n");
  }
  return gva;
}

void
AGAS::memput(hpx_addr_t to, const void *from, size_t n, hpx_addr_t lsync,
             hpx_addr_t rsync)
{
  void *lto;
  if (!n) {
    hpx_lco_set(lsync, 0, NULL, HPX_NULL, HPX_NULL);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
  else if (!tryPin(to, &lto)) {
    here->net->memput(to, from, n, lsync, rsync);
  }
  else {
    std::memcpy(lto, from, n);
    unpin(to);
    hpx_lco_set(lsync, 0, NULL, HPX_NULL, HPX_NULL);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
}

void
AGAS::memput(hpx_addr_t to, const void *from, size_t n, hpx_addr_t rsync)
{
  void *lto;
  if (!n) {
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
  else if (!tryPin(to, &lto)) {
    here->net->memput(to, from, n, rsync);
  }
  else {
    std::memcpy(lto, from, n);
    unpin(to);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
}

void
AGAS::memput(hpx_addr_t to, const void *from, size_t n) {
  void *lto;
  if (!n) {
  }
  else if (!tryPin(to, &lto)) {
    here->net->memput(to, from, n);
  }
  else {
    std::memcpy(lto, from, n);
    unpin(to);
  }
}

void
AGAS::memget(void *to, hpx_addr_t from, size_t n, hpx_addr_t lsync,
             hpx_addr_t rsync)
{
  void *lfrom;
  if (!n) {
    hpx_lco_set(lsync, 0, NULL, HPX_NULL, HPX_NULL);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
  } else if (!tryPin(from, &lfrom)) {
    here->net->memget(to, from, n, lsync, rsync);
  }
  else {
    std::memcpy(to, lfrom, n);
    unpin(from);
    hpx_lco_set(lsync, 0, NULL, HPX_NULL, HPX_NULL);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
}

void
AGAS::memget(void *to, hpx_addr_t from, size_t n, hpx_addr_t lsync)
{
  void *lfrom;
  if (!n) {
    hpx_lco_set(lsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
  else if (!tryPin(from, &lfrom)) {
    here->net->memget(to, from, n, lsync);
  }
  else {
    std::memcpy(to, lfrom, n);
    unpin(from);
    hpx_lco_set(lsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
}

void
AGAS::memget(void *to, hpx_addr_t from, size_t n)
{
  void *lfrom;
  if (!n) {
  }
  else if (!tryPin(from, &lfrom)) {
    here->net->memget(to, from, n);
  }
  else {
    std::memcpy(to, lfrom, n);
    unpin(from);
  }
}

void
AGAS::memcpy(hpx_addr_t to, hpx_addr_t from, size_t size, hpx_addr_t sync)
{
  void *lto;
  void *lfrom;

  if (!size) {
    hpx_lco_set(sync, 0, NULL, HPX_NULL, HPX_NULL);
  }
  else if (!tryPin(to, &lto)) {
    here->net->memcpy(to, from, size, sync);
  }
  else if (!tryPin(from, &lfrom)) {
    unpin(to);
    here->net->memcpy(to, from, size, sync);
  }
  else {
    std::memcpy(lto, lfrom, size);
    hpx_gas_unpin(to);
    hpx_gas_unpin(from);
    hpx_lco_set(sync, 0, NULL, HPX_NULL, HPX_NULL);
  }
}

void
AGAS::memcpy(hpx_addr_t to, hpx_addr_t from, size_t size)
{
  if (size) {
    hpx_addr_t sync = hpx_lco_future_new(0);
    memcpy(to, from, size, sync);
    dbg_check(hpx_lco_wait(sync));
    hpx_lco_delete(sync, HPX_NULL);
  }
}

void*
AGAS::chunkAllocate(void* addr, size_t n, size_t align, bool cyclic)
{
  if (cyclic) {
    dbg_assert(cyclic_);
    return cyclic_->allocate(addr, n, align, BlockSizePassthrough_);
  }
  else {
    return global_.allocate(addr, n, align, BlockSizePassthrough_);
  }
}

void
AGAS::chunkDeallocate(void* addr, size_t n, bool cyclic)
{
  if (cyclic) {
    dbg_assert(cyclic_);
    cyclic_->deallocate(addr, n);
  }
  else {
    global_.deallocate(addr, n);
  }
}
