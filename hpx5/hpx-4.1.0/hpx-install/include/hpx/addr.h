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

#ifndef HPX_ADDR_H
#define HPX_ADDR_H

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup agas
/// @{

/// @file include/hpx/addr.h
/// @brief Types and functions specific to dealing with global addresses.

/// An HPX global address.
typedef uint64_t hpx_addr_t;

// An HPX global pointer difference type for global pointer arithmetic
// and global array indexing.
typedef int64_t hpx_gas_ptrdiff_t;

/// The equivalent of NULL for global memory
#define HPX_NULL ((hpx_addr_t)0)

/// An address representing this locality in general, that is suitable for use
/// as a parcel target.
extern hpx_addr_t HPX_HERE HPX_PUBLIC;

/// An address representing some other locality, that is suitable for use as a
/// parcel target.
///
/// @param         i A locality number.
///
/// @returns An address representing that locality.
hpx_addr_t HPX_THERE(uint32_t i) HPX_PUBLIC;

/// Perform global address displacement arithmetic.
///
/// Get the address of @p bytes into memory with address @p addr. As with normal
/// C pointer arithmetic, the @p bytes displacement must result in an address
/// associated with the same allocation as @p addr, or one-off-the-end if the
/// allocation was an array. The @p bsize must correspond to the block size
/// specified during the initial allocation (or the size of the object for
/// allocations performed by hpx_gas_alloc_local()).
///
/// @param     addr A valid  global address.
/// @param    bytes An offset in bytes into the memory referenced by @p addr.
/// @param    bsize The block size of the allocation associated with @p addr.
///
/// @returns The address of the memory at offset @p bytes from @p addr.
hpx_addr_t hpx_addr_add(hpx_addr_t addr, hpx_gas_ptrdiff_t bytes, size_t bsize)
  HPX_PUBLIC;

/// Perform global address distance arithmetic.
///
/// Computes the (signed) distance between @p lhs and @p rhs, in bytes. As with
/// standard C pointer arithmetic, both @p lhs and @p rhs must be part of the
/// same allocation. Furthermore, @p bsize must correspond to the block size
/// specified during the initial allocation (or the size of the object for
/// allocations performed by hpx_gas_alloc_local()).
///
/// @param      lhs The "left-hand-size" address for the computation.
/// @param      rhs The "right-hand-size" address for the computation.
/// @param    bsize The block size associated with the initial allocation.
///
/// @returns The number of bytes between @p lhs and @p rhs.
///          - positive if @p lhs > @p rhs
///          - 0 if @p lhs == @p rhs
///          - negative if @p lhs < @p rhs
hpx_gas_ptrdiff_t hpx_addr_sub(hpx_addr_t lhs, hpx_addr_t rhs, size_t bsize)
  HPX_PUBLIC;

/// @}

#ifdef __cplusplus
}
#endif

#endif
