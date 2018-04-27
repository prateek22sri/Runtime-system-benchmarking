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

#ifndef LIBHPX_SCHEDULER_TATAS_LOCK_H
#define LIBHPX_SCHEDULER_TATAS_LOCK_H

#include "arch/common/asm.h"
#include <algorithm>
#include <atomic>

namespace libhpx {
namespace scheduler {
static constexpr unsigned BASE = 16;
static constexpr unsigned LIMIT = 65536;

template <typename T>
class TatasLock {
 public:
  TatasLock() : data_(0) {
  }

  ~TatasLock() {
  }

  T try_lock() {
    return (data_.exchange(1, std::memory_order_acquire) == 0);
  }

  void lock() {
    unsigned spins = BASE;
    while (!try_lock()) {
      for (unsigned i = 0, e = spins; i < e; ++i) {
        pause_nop();
      }
      spins <<= 1;
      spins = std::min(spins, LIMIT);
    }
  }

  void unlock() {
    data_.store(0, std::memory_order_release);
  }

 private:
  std::atomic<T> data_;
};

} // namespace scheduler
} // namespace libhpx

#endif // LIBHPX_SCHEDULER_TATAS_LOCK_H
