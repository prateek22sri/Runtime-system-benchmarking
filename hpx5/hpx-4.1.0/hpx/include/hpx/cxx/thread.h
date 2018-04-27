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

#ifndef HPX_CXX_THREAD_H
#define HPX_CXX_THREAD_H

#include <hpx/parcel.h>
#include <hpx/cxx/global_ptr.h>

namespace hpx {
class Thread {
 public:
  Thread() = delete;
  Thread(const Thread&) = default;
  Thread& operator=(const Thread&) = delete;

  ~Thread() {
  }

  static const Thread Current() {
    return Thread(hpx_thread_current_parcel());
  }

  template <typename T>
  global_ptr<T> target(unsigned bsize) const {
    return global_ptr<T>(hpx_parcel_get_target(_p), bsize);
  }

  global_ptr<void> target() const {
    return global_ptr<void>(hpx_parcel_get_target(_p));
  }

  global_ptr<void> continuation() const {
    return global_ptr<void>(hpx_parcel_get_cont_target(_p));
  }

  template <typename T>
  static global_ptr<T> CurrentTarget(unsigned bsize) {
    return Current().target<T>(bsize);
  }

  static global_ptr<void> CurrentTarget() {
    return Current().target();
  }

 private:

  Thread(const hpx_parcel_t *p) : _p(p) {
  }

  const hpx_parcel_t *_p;
}; // class Thread

} // namespace hpx

#endif // HPX_CXX_THREAD_H
