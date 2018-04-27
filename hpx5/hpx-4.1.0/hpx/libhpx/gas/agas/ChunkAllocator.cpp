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

#include "ChunkAllocator.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include "libhpx/system.h"
#include "libhpx/util/math.h"

namespace {
using libhpx::util::Bitmap;
using libhpx::gas::agas::ChunkAllocator;
using libhpx::util::ceil_log2;
using libhpx::util::ceil_div;
}

ChunkAllocator::ChunkAllocator(ChunkTable& chunks, size_t heapSize)
    : Bitmap(ceil_div(heapSize, as_bytes_per_chunk()),
             ceil_log2(as_bytes_per_chunk()),
             ceil_log2(heapSize)),
      chunkSize_(as_bytes_per_chunk()),
      chunks_(chunks)
{
}

ChunkAllocator::~ChunkAllocator()
{
}

void*
ChunkAllocator::allocate(void* addr, size_t n, size_t align, size_t bsize)
{
  // 1) get gva placement for this allocation
  uint32_t nbits = ceil_div(n, chunkSize_);
  uint32_t log2_align = ceil_log2(std::max(align, bsize));
  uint32_t bit;
  if (reserve(nbits, log2_align, &bit)) {
    dbg_error("Could not reserve gva for %zu bytes\n", n);
  }
  uint64_t offset = bit * chunkSize_;

  // 2) get backing memory
  align = 1 << log2_align;
  void *base = system_mmap(NULL, addr, n, align);
  dbg_assert(base);
  dbg_assert(((uintptr_t)base & (align - 1)) == 0);

  // 3) insert the inverse mappings
  char* chunk = static_cast<char*>(base);
  for (int i = 0, e = nbits; i < e; ++i) {
    chunks_.insert(chunk, offset);
    offset += chunkSize_;
    chunk += chunkSize_;
  }
  return base;
}

void
ChunkAllocator::deallocate(void* addr, size_t n)
{
  // 1) release the bits
  uint64_t offset = chunks_.offsetOf(addr);
  uint32_t nbits = ceil_div(n, chunkSize_);
  uint32_t bit = offset / chunkSize_;
  release(bit, nbits);

  // 2) unmap the backing memory
  system_munmap(NULL, addr, n);

  // 3) remove the inverse mappings
  char *chunk = static_cast<char*>(addr);
  for (int i = 0, e = nbits; i < e; ++i) {
    chunks_.remove(chunk);
    chunk += chunkSize_;
  }
}
