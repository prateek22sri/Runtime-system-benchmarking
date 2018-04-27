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

#include "ParcelBlock.h"
#include "libhpx/debug.h"
#include "libhpx/locality.h"
#include "libhpx/memory.h"
#include <cstdint>

namespace {
using libhpx::network::pwc::EagerBlock;
using libhpx::network::pwc::InplaceBlock;
using libhpx::network::pwc::PhotonTransport;
}

EagerBlock::EagerBlock()
    : end_(nullptr),
      next_(nullptr),
      key_()
{
}

EagerBlock::EagerBlock(size_t capacity, char* buffer)
    : end_(buffer + capacity),
      next_(buffer),
      key_(PhotonTransport::FindKey(buffer, capacity))
{
}

void*
EagerBlock::operator new[](size_t bytes)
{
  void *p = registered_memalign(HPX_CACHELINE_SIZE, bytes);
  return p;
}

void
EagerBlock::operator delete[](void* ptr)
{
  registered_free(ptr);
}

void*
EagerBlock::allocate(size_t& bytes, int rank)
{
  size_t padded = bytes + util::align(bytes, HPX_CACHELINE_SIZE);
  dbg_assert(bytes <= padded);
  dbg_assert(next_ <= end_);
  size_t capacity = size_t(end_ - next_);
  if (capacity <= padded) {
    bytes = capacity;
    return nullptr;
  }
  else {
    if (capacity == padded) {
      log_net("perfect use of eager buffer targeting rank %d\n", rank);
    }
    void* buffer = next_;
    next_ += padded;
    dbg_assert(buffer <= next_);
    return buffer;
  }
}

bool
EagerBlock::put(unsigned rank, const hpx_parcel_t* p, size_t& n)
{
  void *dest = allocate(n, rank);
  if (!dest) {
    return false;
  }

  PhotonTransport::Op op;
  op.rank = rank;
  op.n = n;
  op.dest = dest;
  op.dest_key = &key_;
  op.src = p;
  op.src_key = PhotonTransport::FindKeyRef(p, n);
  op.lop = Command::DeleteParcel(p);
  op.rop = Command::RecvParcel(static_cast<hpx_parcel_t*>(dest));

  if (!op.src_key) {
    dbg_error("no rdma key for local parcel (%p, %zu)\n", (void*)p, n);
  }
  dbg_check( op.put() );
  return true;
}

InplaceBlock::InplaceBlock(size_t n)
    : EagerBlock(n - sizeof(InplaceBlock), bytes_),
      remaining_(n - sizeof(InplaceBlock)),
      padding_()
{
}

InplaceBlock::~InplaceBlock()
{
  if (remaining_.load()) {
    log_parcel("block %p freed with %zu bytes remaining\n", this,
               remaining_.load());
  }
  log_parcel("deleting parcel block at %p\n", (void*)this);
}

void*
InplaceBlock::operator new(size_t bytes, size_t n)
{
  dbg_assert(sizeof(InplaceBlock) == bytes);
  dbg_assert(n >= bytes);
  size_t align = size_t(1) << ceil_log2_size_t(n);
  dbg_assert_str(align == here->config->pwc_parcelbuffersize,
                 "Parcel block alignment is currently limited to "
                 "--hpx-pwc-parcelbuffersize (%zu), %zu requested\n",
                 here->config->pwc_parcelbuffersize, align);
  void *p = registered_memalign(align, n);
  log_net("allocating parcel block %p\n", p);
  return p;
}

void
InplaceBlock::operator delete(void* block)
{
  log_net("freeing parcel block %p\n", block);
  registered_free(block);
}

void
InplaceBlock::deallocate(size_t bytes)
{
  dbg_assert(0 < bytes && bytes < SIZE_MAX/2);
  size_t remaining = remaining_.fetch_sub(bytes, std::memory_order_acq_rel);
  dbg_assert(remaining >= bytes);
  remaining -= bytes;
  log_parcel("deducting %zu bytes from parcel block %p (%zu remain)\n", bytes,
             (void*)this, remaining);
  if (!remaining) {
    delete this;
  }
}

void
InplaceBlock::finalize()
{
  if (size_t remaining = remaining_) {
    log_net("recovering %zu bytes from parcel block %p during finalize\n", remaining, this);
    deallocate(remaining);
  }
}

void
InplaceBlock::DeleteParcel(const hpx_parcel_t *p)
{
    uintptr_t block_size = here->config->pwc_parcelbuffersize;
    dbg_assert(1lu << ceil_log2_uintptr_t(block_size) == block_size);
    uintptr_t block_mask = ~(block_size - 1);
    auto block = reinterpret_cast<InplaceBlock*>((uintptr_t)p & block_mask);
    size_t bytes = parcel_size(p);
    size_t padded = bytes + util::align(bytes, HPX_CACHELINE_SIZE);
    block->deallocate(padded);
}

