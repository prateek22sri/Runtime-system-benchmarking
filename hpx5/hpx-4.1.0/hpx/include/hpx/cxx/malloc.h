// ================================================================= -*- C++ -*-
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

#ifndef HPX_CXX_MALLOC_H
#define HPX_CXX_MALLOC_H

#include <hpx/cxx/global_ptr.h>
#include <hpx/cxx/lco.h>

namespace hpx {

inline void
free(const global_ptr<void>& gva)
{
  hpx_gas_free_sync(gva.get());
}

template <typename T>
inline void
free(const global_ptr<void>& gva, const global_ptr<T>& rsync)
{
  static_assert(lco::is_lco<T>::value, "rsync must be an LCO");
  hpx_gas_free(gva.get(), rsync.get());
}

inline void
free(const global_ptr<void>& gva, std::nullptr_t)
{
  hpx_gas_free(gva.get(), HPX_NULL);
}

/// The generic gas allocation routine.
template <typename T>
inline global_ptr<T>
malloc(size_t n, unsigned block, unsigned boundary, hpx_gas_dist_t dist,
       unsigned attr)
{
  hpx_addr_t gva = hpx_gas_alloc(n, block * sizeof(T), boundary, dist, attr);
  return global_ptr<T>(gva, block);
}

template <>
inline global_ptr<void>
malloc<void>(size_t n, unsigned block, unsigned boundary, hpx_gas_dist_t dist,
             unsigned attr)
{
  hpx_addr_t gva = hpx_gas_alloc(n, block, boundary, dist, attr);
  return global_ptr<void>(gva, block);
}

/// The generic gas allocation routine.
template <typename T>
inline global_ptr<T>
calloc(size_t n, unsigned block, unsigned boundary, hpx_gas_dist_t dist,
       unsigned attr)
{
  hpx_addr_t gva = hpx_gas_calloc(n, block * sizeof(T), boundary, dist, attr);
  return global_ptr<T>(gva, block);
}

/// The generic gas allocation routine.
template <>
inline global_ptr<void>
calloc<void>(size_t n, unsigned block, unsigned boundary, hpx_gas_dist_t dist,
             unsigned attr)
{
  hpx_addr_t gva = hpx_gas_calloc(n, block, boundary, dist, attr);
  return global_ptr<void>(gva, block);
}

/// Allocate a local array of elements.
template <typename T>
inline global_ptr<T>
alloc_local(size_t n, unsigned boundary)
{
  return malloc<T>(n, 1, boundary, HPX_GAS_DIST_LOCAL, HPX_GAS_ATTR_NONE);
}

template <typename T>
inline global_ptr<T>
alloc_local(size_t n)
{
  return malloc<T>(n, 1, 0, HPX_GAS_DIST_LOCAL, HPX_GAS_ATTR_NONE);
}

template <typename T>
inline global_ptr<T>
calloc_local(size_t n, unsigned boundary)
{
  return calloc<T>(n, 1, boundary, HPX_GAS_DIST_LOCAL, HPX_GAS_ATTR_NONE);
}

template <typename T>
inline global_ptr<T>
calloc_local(size_t n)
{
  return calloc<T>(n, 1, 0, HPX_GAS_DIST_LOCAL, HPX_GAS_ATTR_NONE);
}

template <typename T>
inline global_ptr<T>
alloc_cyclic(size_t n, unsigned block, unsigned boundary)
{
  return malloc<T>(n, block, boundary, HPX_GAS_DIST_CYCLIC, HPX_GAS_ATTR_NONE);
}

template <typename T>
inline global_ptr<T>
alloc_cyclic(size_t n, unsigned block)
{
  return malloc<T>(n, block, 0, HPX_GAS_DIST_CYCLIC, HPX_GAS_ATTR_NONE);
}

template <typename T>
inline global_ptr<T>
calloc_cyclic(size_t n, unsigned block, unsigned boundary)
{
  return calloc<T>(n, block, boundary, HPX_GAS_DIST_CYCLIC, HPX_GAS_ATTR_NONE);
}

template <typename T>
inline global_ptr<T>
calloc_cyclic(size_t n, unsigned block)
{
  return calloc<T>(n, block, 0, HPX_GAS_DIST_CYCLIC, HPX_GAS_ATTR_NONE);
}

namespace registered {
template <typename T>
inline T*
malloc(size_t bytes)
{
  return reinterpret_cast<T*>(hpx_malloc_registered(bytes));
}

template <typename T>
inline T*
memalign(size_t align, size_t bytes)
{
  return reinterpret_cast<T*>(hpx_memalign_registered(align, bytes));
}

inline void
free(void* p)
{
  hpx_free_registered(p);
}

template <typename T>
inline std::unique_ptr<T, decltype(free)*>
malloc_unique(size_t bytes)
{
  return std::unique_ptr<T, decltype(free)*>(malloc<T>(bytes), free);
}

template <typename T>
inline std::unique_ptr<T, decltype(free)*>
memalign_unique(size_t align, size_t bytes)
{
  return std::unique_ptr<T, decltype(free)*>(memalign<T>(align, bytes), free);
}

} // namespace registered
} // namespace hpx

#endif // HPX_CXX_MALLOC_H
