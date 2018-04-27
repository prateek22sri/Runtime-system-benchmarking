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

#include "HeapSegment.h"
#include "libhpx/gpa.h"
#include "libhpx/libhpx.h"
#include "libhpx/locality.h"
#include "libhpx/system.h"
#include "libhpx/util/math.h"
#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <exception>

namespace {
using libhpx::gas::pgas::HeapSegment;
using libhpx::util::Bitmap;
using libhpx::util::ceil_log2;
using libhpx::util::ceil_div;
const uint64_t MAX_HEAP_BYTES = UINT64_C(1) << GPA_OFFSET_BITS;
}

HeapSegment* HeapSegment::Instance_;

HeapSegment::HeapSegment(size_t size)
    : bytesPerChunk_(as_bytes_per_chunk()),
      nBytes_(size - (size % bytesPerChunk_)),
      csbrk_(nBytes_),
      nChunks_(ceil_div(nBytes_, bytesPerChunk_)),
      logMaxBlockSize_(std::min(GPA_MAX_LG_BSIZE, ceil_log2(nBytes_))),
      base_(newSegment()),
      chunks_(nChunks_, ceil_log2(bytesPerChunk_), logMaxBlockSize_)
{
  if (Instance_) {
    dbg_error("HPX only supports a singleton heap segment in PGAS\n");
  }
  else {
    Instance_ = this;
  }

  if (nBytes_ > MAX_HEAP_BYTES) {
    dbg_error("%zu > max heap bytes of %" PRIu64 "\n", nBytes_, MAX_HEAP_BYTES);
  }

  log_gas("heap bytes per chunk is %zu\n", bytesPerChunk_);
  log_gas("heap nbytes is aligned as %zu\n", nBytes_);
  log_gas("heap nchunks is %u\n", nChunks_);
  log_gas("allocated %zu bytes for the global heap\n", nBytes_);
  log_gas("allocated chunk bitmap to manage %u chunks.\n", nChunks_);
  log_gas("allocated heap.\n");
}

HeapSegment::~HeapSegment()
{
  if (base_) {
    system_munmap_huge_pages(nullptr, base_, nBytes_);
  }
}

char*
HeapSegment::newSegment() const
{
  size_t align = maxBlockSize();
  void* addr = system_mmap_huge_pages(nullptr, nullptr, nBytes_, align);
  if (char* base = static_cast<char*>(addr)) {
    assert((uintptr_t)base % bytesPerChunk_ == 0);
    return base;
  }
  log_error("could not allocate %zu bytes for the global heap\n", nBytes_);
  throw std::exception();
}

void*
HeapSegment::allocateChunk(void *addr, size_t n, size_t align, ChunkType type)
{
  assert(n % bytesPerChunk_ == 0);
  assert(n / bytesPerChunk_ < UINT32_MAX);
  uint32_t bits = n / bytesPerChunk_;
  uint32_t log2_align = ceil_log2(align);

  uint32_t bit = 0;
  if (type == GLOBAL) {
    if (chunks_.reserve(bits, log2_align, &bit)) {
      dbg_error("out-of-memory detected\n");
    }
  }
  else {
    if (chunks_.rreserve(bits, log2_align, &bit)) {
      dbg_error("out-of-memory detected\n");
    }
  }

  uint64_t offset = bit * bytesPerChunk_;
  assert(offset % align == 0);

  if (type == GLOBAL) {
    if (offset + n > csbrk_) {
      dbg_error("out-of-memory detected\n");
    }
  }
  else {
    setCsbrk(offset);
  }

  void *p = offsetToLVA(offset);
  dbg_assert(((uintptr_t)p & (align - 1)) == 0);
  return p;
}

bool
HeapSegment::deallocateChunk(void *chunk, size_t size)
{
  const uint64_t offset = static_cast<char*>(chunk) - base_;
  assert(offset % bytesPerChunk_ == 0);
  assert(size % bytesPerChunk_ == 0);

  const uint64_t    bit = offset / bytesPerChunk_;
  const uint64_t  nbits = size / bytesPerChunk_;
  chunks_.release(bit, nbits);
  return true;
}

void *
HeapSegment::offsetToLVA(uint64_t offset) const
{
  DEBUG_IF (nBytes_ < offset) {
    dbg_error("offset %" PRIu64 " out of range (0,%zu)\n", offset, nBytes_);
  }
  return base_ + offset;
}

uint64_t
HeapSegment::lvaToOffset(const void* const lva) const
{
  if (lva < base_ || base_ + nBytes_ <= lva) {
    dbg_error("lva %p not in the heap [%p, %p)\n", lva, base_, base_ + nBytes_);
  }
  auto rhs = static_cast<const char* const>(lva);
  return uint64_t(rhs - base_);
}

bool
HeapSegment::isCyclicOffset(uint64_t offset) const
{
  if (offset >= nBytes_) {
    log_gas(" offset %" PRIu64 " is not in the heap\n", offset);
    return false;
  }
  return (offset >= csbrk_);
}

bool
HeapSegment::chunksAreUsed(uint64_t offset, size_t n) const
{
  uint32_t from = offset / bytesPerChunk_;
  auto t = ceil_div(offset + n, uint64_t(bytesPerChunk_)); // ILP32
  dbg_assert(t < UINT32_MAX);
  uint32_t to = (uint32_t)t;
  return chunks_.isSet(from, to - from);
}

bool
HeapSegment::setCsbrk(uint64_t offset)
{
  // csbrk is monotonically decreasing
  uint64_t old = csbrk_.load(std::memory_order_relaxed);
  if (old > offset) {
    csbrk_.compare_exchange_weak(old, offset, std::memory_order_relaxed,
                                 std::memory_order_relaxed);
  }
  return chunksAreUsed(offset, old - offset);
}
