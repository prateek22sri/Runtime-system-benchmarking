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

#ifndef HPX_CXX_PROCESS_H
#define HPX_CXX_PROCESS_H

#include <hpx/process.h>
#include <hpx/thread.h>
#include <hpx/cxx/errors.h>

namespace hpx {
namespace process {
/// Synchronous broadcast to all of the localities within a process.
template <typename A, typename... Params>
inline void
broadcast(A action, Params... params)
{
  const size_t n = sizeof...(Params);
  const hpx_pid_t pid = hpx_thread_current_pid();
  if (int e = _hpx_process_broadcast_rsync(pid, action, n, params...)) {
    throw Error(e);
  }
} // template broadcast
} // namespace process
} // namespace hpx

#endif // HPX_CXX_PROCESS_H
