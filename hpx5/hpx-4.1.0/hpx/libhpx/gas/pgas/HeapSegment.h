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

#ifndef LIBHPX_GAS_PGAS_HEAP_H
#define LIBHPX_GAS_PGAS_HEAP_H

/// @file libhpx/gas/heap.h
/// @brief Implementation of the global address space with a PGAS model.
///
/// This implementation is similar to, and inspired by, the PGAS heap
/// implementation for UPC.
///
/// The PGAS heap implementation allocates one large region for the symmetric
/// heap, as requested by the application programmer. This region is dynamically
/// divided into cyclic and acyclic regions. Each locality manages its acyclic
/// region with a combination of jemalloc and a simple, locking-bitmap-based
/// jemalloc chunk allocator. The cyclic region is managed via an sbrk at the
/// root locality. The regions start out opposite each other in the space, and
/// grow towards each other.
///
///   +------------------------
///   | cyclic
///   |
///   | ...
///   |
///   |
///   | acyclic
///   +------------------------
///
/// We do not currently have any way to detect intersection of the cyclic and
/// acyclic regions, because the cyclic allocations are not broadcast. The root
/// has no way of knowing how much acyclic allocation each locality has
/// performed, which it would need to know to do the check.
///
/// @todo Implement a debugging mode where cyclic allocation is broadcast, so we
///       can detect intersections and report meaningful errors. Without this,
///       intersections lead to untraceable errors.
///

#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include "libhpx/util/Bitmap.h"
#include <cstddef>
#include <algorithm>
#include <atomic>

#define HEAP_USE_CYCLIC_CSBRK_BARRIER 0

namespace libhpx {
namespace gas {
namespace pgas {
/// A type corresponding to the local heap type.
///
/// The pgas heap manages a contiguous range of virtual memory using a
/// bitmap. The "bottom" of the heap is used to satisfy cyclic allocation
/// requests, while the "top" of the heap is used to satisfy normal global
/// allocation. The csbrk value indicates the upper bound on cyclic allocations,
/// so that we can quickly tell if an address is cylic or not.
/// @{
class HeapSegment {
 public:
  /// The heap segment manages cyclic and global heap chunks.
  enum ChunkType {
    CYCLIC,
    GLOBAL
  };

  /// Initialize a heap to manage the specified number of bytes.
  ///
  /// @param         size The number of bytes to allocate for the heap.
  HeapSegment(size_t size);

  ~HeapSegment();

  /// Allocate a chunk of the global address space.
  ///
  /// This satisfies requests from jemalloc's chunk allocator for global memory.
  ///
  /// @param         addr A "suggested" address---ignored by the heap for now.
  /// @param         size The number of bytes to allocate.
  /// @param        align The alignment required for the chunk.
  /// @param         type The type of chunk requested.
  ///
  /// @returns The address of the base of the allocated chunk, or NULL if we are
  ///          out of memory.
  void *allocateChunk(void *addr, size_t size, size_t align, ChunkType type);

  /// Release a chunk of the global address space.
  ///
  /// This satisfies requests from jemalloc's chunk allocator to release global
  /// memory.
  ///
  /// @param        chunk The chunk base pointer to release.
  /// @param         size The number of bytes associated with the chunk.
  ///
  /// @returns I have no idea what the return value should be used for---Luke.
  bool deallocateChunk(void *chunk, size_t size);

  /// Return the number of heap bytes (used in dlmalloc).
  size_t getNBytes() const {
    return nBytes_;
  }

  /// Return the number of bytes per chunk (used in tbbmalloc).
  size_t getBytesPerChunk() const {
    return bytesPerChunk_;
  }

  /// Get the base pointer for the heap (used in dlmalloc).
  void* getBase() const {
    return base_;
  }

  /// Access the singleton heap segment.
  static HeapSegment* Instance() {
    dbg_assert(Instance_);
    return Instance_;
  }

  /// Set the csbrk to correspond to the given heap_offset value.
  bool setCsbrk(uint64_t offset);

 protected:
  /// Get the csbrk.
  ///
  /// @returns The current value of csbrk.
  uint64_t getCsbrk() const {
    return csbrk_.load(std::memory_order_relaxed);
  }

  /// Convert a heap offset into a local virtual address.
  ///
  /// @param       offset The absolute offset within the heap.
  ///
  /// @returns The local virtual address corresponding to the offset.
  void* offsetToLVA(uint64_t offset) const;

  /// Convert a local virtual address into an offset.
  uint64_t lvaToOffset(const void* const lva) const;

  uint64_t maxBlockSize() const {
    return (uint64_t(1) << logMaxBlockSize_);
  }

  /// Check to see if the given offset is cyclic.
  ///
  /// This will verify that the @p heap_offset is in the heap, out-of-bounds
  /// offsets result in false.
  ///
  /// @param       offset The heap-relative offset to check.
  ///
  /// @returns TRUE if the offset is a cyclic offset, FALSE otherwise.
  bool isCyclicOffset(uint64_t offset) const;

 private:

  /// Check to see if the heap contains the given local virtual address.
  ///
  /// @param          lva The local virtual address to test.
  ///
  /// @returns TRUE if the @p lva is contained in the global heap.
  bool contains(const void *lva) const {
    const ptrdiff_t d = static_cast<const char*>(lva) - base_;
    return (0 <= d && uintptr_t(d) < nBytes_);

  }

  /// Check to see if the heap contains the given offset.
  ///
  /// @param       offset The offset to check.
  ///
  /// @returns TRUE if the @p offset is contained in the global heap.
  bool contains(uint64_t offset) const {
    return (offset < nBytes_);
  }

  /// Check to see if any of the chunks associated range of bytes are used.
  bool chunksAreUsed(uint64_t offset, size_t n) const;

  /// Utility function to encapsulate allocating the segment.
  ///
  /// This is used in the constructor to mmap the heap segment. It is used in an
  /// initializer list so we go ahead an have it return the segment. Order of
  /// initialization matters.
  char* newSegment() const;

  static HeapSegment* Instance_;

 protected:
  const size_t bytesPerChunk_;
  const size_t nBytes_;
  std::atomic<uint64_t> csbrk_;
  const uint32_t nChunks_;
  const uint32_t logMaxBlockSize_;
  char *base_;
  util::Bitmap chunks_;
};
} // namespace pgas
} // namespace gas
} // namespace libhpx
#endif
