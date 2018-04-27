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

#ifndef LIBHPX_GPA_H
#define LIBHPX_GPA_H

/// @file libhpx/gas/pgas/addr.h
/// @brief Declaration of the PGAS-specific global physical address.
#include <stdint.h>
#include <hpx/hpx.h>
#include <libhpx/debug.h>

#ifdef __cplusplus
extern "C" {
#endif

#define   GPA_RANK_BITS 16
#define GPA_OFFSET_BITS 48

typedef union {
  hpx_addr_t addr;
  struct {
    uint64_t offset : GPA_OFFSET_BITS;
    uint64_t   home : GPA_RANK_BITS;
  } bits;
} gpa_t;

#define GPA_MAX_LG_BSIZE (sizeof(uint32_t)*8)

/// Extract the locality from a gpa.
static inline uint32_t gpa_to_rank(hpx_addr_t addr) {
  gpa_t gpa = { .addr = addr };
  return gpa.bits.home;
}

/// Extract the heap offset of a gpa, given the number of ranks.
///
/// The heap_offset is the complete relative offset of the global physical
/// address in its global heap. It encodes both the gpa offset and the gpa
/// phase, and can be used to extract either of those in the future.
///
/// @param         addr The global address.
///
/// @returns            The offset within the global heap that corresponds to
///                     the address.
static inline uint64_t gpa_to_offset(hpx_addr_t addr) {
  gpa_t gpa = { .addr = addr };
  return gpa.bits.offset;
}

/// Create a global physical address from a locality and heap offset pair.
///
/// @param     locality The locality where we want the address to point.
/// @param       offset The offset into the heap.
///
/// @returns            A global address representing the offset at the
///                     locality.
static inline hpx_addr_t offset_to_gpa(uint32_t locality, uint64_t offset)
{
  dbg_assert(locality < UINT16_MAX);
  gpa_t gpa = {
    .bits = {
      .offset = offset,
      .home = locality
    }
  };
  return gpa.addr;
}

/// Compute the (signed) distance, in bytes, between two global addresses.
///
/// This is only valid if @p lhs and @p rhs come from the same global
/// allocation.
///
/// @param          lhs The left-hand-side address.
/// @param          rhs The right-hand-side address.
///
/// @returns            (lhs - rhs)
static inline int64_t gpa_sub(hpx_addr_t lhs, hpx_addr_t rhs) {
    return (lhs - rhs);
}

/// Compute standard address arithmetic on the global address.
static inline hpx_addr_t gpa_add(hpx_addr_t gpa, int64_t bytes) {
  return gpa + bytes;
}

/// Compute the (signed) distance, in bytes, between two global addresses from a
/// cyclic allocation.
///
/// This is only valid if @p lhs and @p rhs come from the same global
/// allocation.
///
/// @param          lhs The left-hand-side address.
/// @param          rhs The right-hand-size address.
/// @param        bsize The block size for the allocation.
///
/// @returns            The difference between the two addresses such that
///                     @code
///                       lhs == gpa_add_cyclic(rhs,
///                                             gpa_sub_cyclic(lhs, rhs, bsize),
///                                             bsize)
///                     @endcode
int64_t gpa_sub_cyclic(hpx_addr_t lhs, hpx_addr_t rhs, uint32_t bsize);

/// Compute cyclic address arithmetic on the global address.
///
/// @param          gpa The global address base.
/// @param        bytes The displacement in the number of bytes (may be
///                     negative).
/// @param        bsize The block size in bytes for the allocation.
///
/// @returns            The global address representation that is @p bytes away
///                     from @p gpa.
hpx_addr_t gpa_add_cyclic(hpx_addr_t gpa, int64_t bytes, uint32_t bsize);

#ifdef __cplusplus
}
#endif

#endif // LIBHPX_GPA_H
