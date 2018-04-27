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

#ifndef HPX_CXX_SYSTEM_H
#define HPX_CXX_SYSTEM_H

#include <hpx/rpc.h>
#include <hpx/cxx/global_ptr.h>

namespace hpx {
namespace system {
template <typename A, typename... Params>
inline void
broadcast(A action, Params... params)
{
  hpx_pid_t pid = hpx_thread_current_pid();
  size_t n = sizeof...(Params);
  if (int e = _hpx_process_broadcast_rsync(pid, action, n, params...)) {
    throw Error(e);
  }
}

template <typename A, typename T, typename... Params>
inline void
broadcast(A act, const global_ptr<T>& rsync, Params... params)
{
  hpx_pid_t pid = hpx_thread_current_pid();
  size_t n = sizeof...(Params);
  hpx_addr_t raddr = rsync.ptr();
  if (int e = _hpx_process_broadcast_lsync(pid, act, raddr, n, params...)) {
    throw Error(e);
  }
}

template <typename A, typename T, typename U, typename... Params>
inline void
broadcast(A act, const global_ptr<T>& lsync, const global_ptr<U>& rsync,
          Params... params)
{
  hpx_pid_t pid = hpx_thread_current_pid();
  size_t n = sizeof...(Params);
  hpx_addr_t laddr = lsync.ptr();
  hpx_addr_t raddr = rsync.ptr();
  if (int e = _hpx_process_broadcast_lsync(pid, act, laddr, raddr, n, params...)) {
    throw Error(e);
  }
}
} // namespace system
} // namespace hpx

#endif // HPX_CXX_SYSTEM_H
