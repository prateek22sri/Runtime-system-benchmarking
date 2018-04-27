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

#ifndef LIBHPX_GAS_BITMAP_H
#define LIBHPX_GAS_BITMAP_H

#include "libhpx/util/Aligned.h"
#include <cstddef>
#include <mutex>

/// @file libhpx/gas/bitmap.h
/// @brief Implement a simple parallel bitmap allocator to manage chunk
///        allocation for jemalloc.
///
/// We use jemalloc directly to manage our symmetric heaps. Unlike dlmalloc,
/// which provides a "memory space" abstraction, jemalloc operates in terms of
/// large "chunks," which are typically provided through the system mmap/munmap
/// functionality.
///
/// In our case, we want jemalloc to use chunks that we provide from our
/// global heap.
///
/// @note Our implementation is currently single-lock based. If this becomes a
///       synchronization bottleneck we can move to a non-blocking or more
///       fine-grained approach.

namespace libhpx {
namespace util {
/// The bitmap structure.
///
/// The bitmap is fundamentally an array of bits chunked up into blocks combined
/// with some header data describing the block array and a lock for
/// concurrency.
class Bitmap : public libhpx::util::Aligned<HPX_CACHELINE_SIZE>
{
 public:
  /// Allocate and initialize a bitmap.
  ///
  /// @param        nbits The number of bits we need to manage.
  /// @param    min_align The minimum alignment.
  /// @param   base_align The base alignment.
  ///
  /// @returns The new bitmap or NULL if there was an error.
  Bitmap(unsigned nbits, unsigned minAlign, unsigned baseAlign);

  /// Delete a bitmap that was previously allocated with bitmap_new().
  ~Bitmap();

  /// Allocate @p nbits contiguous bits from the bitmap, aligned to a @p align
  /// boundary.
  ///
  /// This performs a least->most significant bit search for space.
  ///
  /// @param[in]    nbits The number of continuous bits to allocate.
  /// @param[in]     align The alignment we need to return
  /// @param[out]       i The offset of the start of the allocation.
  ///
  /// @returns LIBHPX_OK, LIBHPX_ENOMEM
  int reserve(unsigned nbits, unsigned align, unsigned *i);

  /// Allocate @p nbits contiguous bits from the bitmap, aligned to a @p align
  /// boundary.
  ///
  /// This performs a most->least significant bit search.
  ///
  /// @param[in]    nbits The number of continuous bits to allocate.
  /// @param[in]    align The alignment we need to find.
  /// @param[out]       i The offset of the start of the allocation.
  ///
  /// @returns LIBHPX_OK, LIBHPX_ENOMEM
  int rreserve(unsigned nbits, unsigned align, unsigned *i);

  /// Free @p nbits contiguous bits of memory, starting at offset @p i.
  ///
  /// @param            i The offset to start freeing from.
  /// @param        nbits The number of bits to free.
  void release(unsigned i, unsigned nbits);

  /// Determine if a particular region has been allocated.
  ///
  /// @param          bit The bit offset to check.
  /// @param        nbits The number of bits.
  ///
  /// @returns true if the bit is set, false otherwise.
  bool isSet(unsigned bit, unsigned nbits) const;

 private:

  /// Constants related to the block type.
  static constexpr unsigned BLOCK_BYTES = sizeof(uintptr_t);
  static constexpr unsigned  BLOCK_BITS = sizeof(uintptr_t) * 8;

  /// Count the number of unused blocks in the bitmap.
  int countUnusedBlocks() const;

  /// Handle an out-of-memory condition.
  ///
  /// @param        nbits The request size that triggered the OOM.
  /// @param        align The alignment that triggered OOM.
  ///
  /// @returns LIBHPX_ENOMEM
  int oom(unsigned nbits, unsigned align) const;

  /// Set a contiguous number of bits in the bitmap.
  ///
  /// @param          bit The absolute bit index we're starting with.
  /// @param        nbits The number of bits that we're setting.
  void set(unsigned bit, unsigned nbits);

  /// Try to match the range of bits from [@p bit, @p bit + @p nbits).
  ///
  /// This will scan the bitmap for @p nbits free bits starting at @p bit. If it
  /// succeeds, it will return @p nbits. It failure mode is controlled by the @p
  /// f parameter. If @p f is BlockLFS(), it will return the number of bits
  /// successfully matched. If @p f is BlockILS(), it will return the number of
  /// bits matched to get to the last failure.
  ///
  /// This @p f customization is an optimization. The caller knows if it's going
  /// to shift the test left or right in the presence of a failure, and the index
  /// of the first failure or last failure can help avoid useless retries,
  /// depending on which way the shift will be.
  ///
  /// @param          bit The base offset.
  /// @param        nbits The number of bits to match.
  /// @param            f On failure, count to the first or last mismatch.
  ///
  /// @returns @p nbits for success, customizable for failure (see description).
  unsigned match(unsigned bit, unsigned nbits, unsigned (*f)(uintptr_t)) const;

  /// Clear a continuous number of bits in the bitmap.
  ///
  /// @param          bit The absolute bit index we're starting with.
  /// @param        nbits The number of bits that we're clearing.
  void clear(unsigned bit, unsigned nbits);

  /// Scan through the blocks looking for the first free bit.
  ///
  /// @param          bit The starting bit index.
  /// @param          max The end bit index.
  ///
  /// @returns The first free bit, or max if there are no free bits.
  unsigned findFirstFree(unsigned bit, unsigned max) const;

  /// Return the 0-based index of the first 1 bit in the block. At least one bit
  /// in @p b must be set.
  ///
  /// @code
  /// BlockIFS(0001 0001 0001 0000) -> 4
  /// BlockIFS(0001 0001 0000 0000) -> 8
  /// @endcode
  static unsigned BlockIFS(uintptr_t b);

  /// Return the 0-based index of the last 1 bit in the block. At least one bit in
  /// @p b must be set.
  ///
  /// @code
  /// BlockILS(0001 0001 0001 0000) -> 12
  /// BlockILS(0000 0001 0001 0000) -> 8
  /// @endcode
  static unsigned BlockILS(uintptr_t b);

  /// Create a block bitmask of min(@p length, BLOCK_BITS - offset) 1s starting at
  /// @p offset.
  ///
  /// @param       offset The index of the least-significant 1 in the mask.
  /// @param       length The number of contiguous 1s in the mask.
  ///
  /// @returns A constructed mask.
  static uintptr_t CreateMask(unsigned offset, unsigned length);

  mutable std::mutex  lock_;  //!< A single lock to serialize access to the bitmap.
  const unsigned  minAlign_;
  const unsigned baseAlign_;
  const unsigned     nBits_;  //!< The number of bits in the bitmap.
  const unsigned   nBlocks_;  //!< The number of blocks in the block array.
  unsigned             min_;  //!< An index such that there are no free bits < min.
  unsigned             max_;  //!< An index such that there are no free bits >= max.
  uintptr_t        *blocks_;  //!< The block array.
};

} // namespace util
} // namespace libhpx

#endif
