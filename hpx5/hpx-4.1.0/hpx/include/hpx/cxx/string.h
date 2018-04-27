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

#ifndef HPX_CXX_STRING_H
#define HPX_CXX_STRING_H

/// @file include/hpx/cxx/string.h
/// @brief Wrappers for the gas string functions (memget/memput/memcpy)

#include <hpx/cxx/global_ptr.h>
#include <hpx/cxx/lco.h>

namespace hpx {
namespace gas {

/// These wrap the hpx_gas_memget operations, basically just allowing the client
/// to pass in a global pointer instead of hpx_addr_t. They don't try and type
/// check the @p to address (though one would expect it to be T*), and they
/// don't take @p n in terms of T elements, because that doesn't match the
/// semantics of memget.
///
/// @{

/// The memget_sync wrapper.
///
/// This version of memget is completely synchronous.
///
/// @param           to The local pointer we're copying to.
/// @param         from The global pointer we're copying from.
/// @param            n The number of bytes to copy
template <typename T>
inline void
memget(void* to, global_ptr<T>&& from, size_t n)
{
  if (int e = hpx_gas_memget_sync(to, from.get(), n)) {
    throw e;
  }
} // template memget

/// This version of memget is completely asynchronous, i.e., it will return
/// immediately. The user must provide an @p lsync LCO to test for memget
/// completion, as there is no other safe mechanism to determine that the local
/// buffer is safe to read.
///
/// There is currently no way to wait independently for remote completion.
///
/// @param           to The local pointer we're copying to.
/// @param         from The global pointer we're copying from.
/// @param            n The number of bytes to copy
/// @param        lsync An LCO to test for local completion
template <typename T, typename U>
inline void
memget(void* to, global_ptr<T>&& from, size_t n, const global_ptr<U>& lsync)
{
  static_assert(lco::is_lco<U>::value, "lsync must be an LCO");
  if (int e = hpx_gas_memget(to, from.get(), n, lsync.get())) {
    throw e;
  }
} // template memget<T, U>

/// @} memget

/// The memput wrappers.
///
/// These wrap the hpx_gas_memget operations, allowing the client to pass in
/// global pointers instead of hpx_addr_t. The don't try and type check the @p
/// from address (though one would expect it to be a T*), and they don't take @p
/// n in terms of T-typed elements, because that doesn't match the semantics of
/// memput.
///
/// @{

/// The synchronous memput operation.
///
/// This version of memput is completely synchronous, it will not return until
/// the remote write has completed.
///
/// @param           to The global address we're putting to.
/// @param         from The local address we're putting from.
/// @param            n The number of bytes to put.
template <typename T>
inline void
memput(global_ptr<T>&& to, const void *from, size_t n)
{
  if (int e = hpx_gas_memput_rsync(to.get(), from, n)) {
    throw e;
  }
}

/// This version of memput is locally synchronous, it will return when the @p
/// from buffer can be modified or freed. The user must an @p rsync LCO to test
/// for remote completion---without an @p rsync there is no way to determine if
/// the remote operation has completed.
///
/// @param           to The global address we're putting to.
/// @param         from The local address we're putting from.
/// @param            n The number of bytes to put.
/// @param        rsync An LCO that will be set when the put is complete.
template <typename T, typename U>
inline void
memput(global_ptr<T>&& to, const void *from, size_t n,
       const global_ptr<U>& rsync)
{
  static_assert(lco::is_lco<U>::value, "rsync must be an LCO");
  if (int e = hpx_gas_memput_lsync(to.get(), from, n, rsync.get())) {
    throw e;
  }
}

/// This version of memput is fully asynchronous. The user will supply a
/// required @p rsync LCO that they must use remote completion, and may supply
/// an optional @p lsync LCO that can be used to test for local
/// completion. Remote completion implies local completion.
///
/// @param           to The global address we're putting to.
/// @param         from The local address we're putting from.
/// @param            n The number of bytes to put.
/// @param        lsync An LCO that will be set when @p from can be modified.
/// @param        rsync An LCO that will be set when the put is complete.
template <typename T, typename U, typename V>
inline void
memput(global_ptr<T>&& to, const void *from, size_t n,
       const global_ptr<U>& lsync, const global_ptr<V>& rsync)
{
  static_assert(lco::is_lco<U>::value, "lsync must be an LCO");
  static_assert(lco::is_lco<V>::value, "rsync must be an LCO");
  if (int e = hpx_gas_memput(to.get(), from, n, lsync.get(), rsync.get())) {
    throw e;
  }
}

/// This version of memput is fully asynchronous. The user will supply a
/// required @p rsync LCO that they must use remote completion, and may supply
/// an optional @p lsync LCO that can be used to test for local
/// completion. Remote completion implies local completion.
///
/// @param           to The global address we're putting to.
/// @param         from The local address we're putting from.
/// @param            n The number of bytes to put.
/// @param        lsync An LCO that will be set when @p from can be modified.
/// @param        rsync An LCO that will be set when the put is complete.
template <typename T, typename U>
inline void
memput(global_ptr<T>&& to, const void *from, size_t n, std::nullptr_t,
       const global_ptr<U>& rsync)
{
  static_assert(lco::is_lco<U>::value, "rsync must be an LCO");
  if (int e = hpx_gas_memput(to.get(), from, n, HPX_NULL, rsync.get())) {
    throw e;
  }
}

/// @}

} // namespace gas
} // namespace hpx

#endif // HPX_CXX_STRING_H
