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

#ifndef LIBHPX_GAS_AGAS_CHUNK_ALLOCATOR_H
#define LIBHPX_GAS_AGAS_CHUNK_ALLOCATOR_H

#include "ChunkTable.h"
#include "libhpx/util/Bitmap.h"
#include <cstddef>

namespace libhpx {
namespace gas {
namespace agas {

/// A chunk allocator handles allocating large chunks for the high performance
/// allocators. Each chunk allocator takes the full extent of the virtual
/// address space and breaks it up into aligned chunks. It uses a bitmap
/// allocator to keep track of which chunks have been used. When a new chunk is
/// allocated the chunk allocator will go through the system_mmap()
/// functionality to get backing memory, and it will update the chunk table with
/// the mapping.
class ChunkAllocator : public util::Bitmap {
 public:
  ChunkAllocator(ChunkTable& chunks, size_t heapSize);
  ~ChunkAllocator();

  /// Allocate a chunk from the global address space.
  ///
  /// @param       addr A hinted virtual location for the allocation.
  /// @param          n The number of bytes we want.
  /// @param      align The minimum alignment for the chunk.
  /// @param      bsize The block size that triggered the current allocation.
  ///
  /// @returns          nullptr on error, otherwise a new chunk (possibly at @p
  ///                   addr)
  void* allocate(void* addr, size_t n, size_t align, size_t bsize);

  /// Releases a previously allocated chunk.
  ///
  /// This will update the bits for this chunk, remove the chunk mapping from
  /// the chunks table, and return the memory to the system using
  /// system_munmap().
  void deallocate(void* addr, size_t n);

 private:
  const size_t chunkSize_;
  ChunkTable& chunks_;
};

} // namespace agas
} // namespace gas
} // namespace libhpx

#endif // LIBHPX_GAS_AGAS_CHUNK_ALLOCATOR_H
