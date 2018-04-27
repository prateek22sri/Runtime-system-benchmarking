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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/// @file libhpx/scheduler/and.c
/// Defines the AND LCO.

#include "LCO.h"
#include "Condition.h"
#include "TatasLock.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/gpa.h"
#include <atomic>
#include <mutex>

namespace {
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;

class And final : public LCO {
 public:
  And(int inputs);

  ~And() {
    lock();                                     // released in ~LCO()
  }

  void error(hpx_status_t code) {
    std::lock_guard<LCO> _(*this);
    barrier_.signalError(code);
  }

  hpx_status_t get(size_t size, void *value, int reset) {
    dbg_assert(!size && !value);
    return wait(reset);
  }

  void reset() {
    std::lock_guard<LCO> _(*this);
    unlockedReset();
  }

  size_t size(size_t) const {
    return sizeof(And);
  }

  int set(size_t size, const void *value);
  hpx_status_t wait(int reset);
  hpx_status_t attach(hpx_parcel_t *p);

 public:
  /// Static action interface.
  /// @{
  static int NewHandler(void* buffer, int inputs) {
    auto lco = new(buffer) And(inputs);
    LCO_LOG_NEW(hpx_thread_current_target(), lco);
    return HPX_SUCCESS;
  }
  /// @}

 private:
  void unlockedReset();

  Condition      barrier_;                      //<! the condition
  std::atomic<int> count_;                      //<! the current count
  const int        value_;                      //<! the number of inputs
};

LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, New, And::NewHandler, HPX_POINTER,
              HPX_INT);
}

And::And(int count)
    : LCO(LCO_AND),
      barrier_(),
      count_(count),
      value_(count)
{
  log_lco("initialized with %d inputs lco %p\n", count_.load(), (void*)this);
  if (!count_) {
    setTriggered();
  }
}

void
And::unlockedReset()
{
  log_lco("%p resetting lco %p\n", hpx_thread_current_parcel(), this);
  count_.store(value_, std::memory_order_release);
  barrier_.reset();
  resetTriggered();
  if (!value_) {
    setTriggered();
  }
}

hpx_status_t
And::wait(int reset)
{
  std::lock_guard<LCO> _(*this);

  hpx_status_t status = barrier_.getError();
  if (status != HPX_SUCCESS) {
    return status;
  }

  // wait for the lco if its not triggered
  if (!getTriggered()) {
    status = waitFor(barrier_);
    log_lco("%p resuming in lco %p (reset=%d)\n", hpx_thread_current_parcel(),
            (void*)this, reset);
  }

  if (reset && status == HPX_SUCCESS) {
    unlockedReset();
  }

  return status;
}

hpx_status_t
And::attach(hpx_parcel_t *p)
{
  std::lock_guard<LCO> _(*this);

  if (hpx_status_t status = barrier_.getError()) {
    return status;
  }

  if (getTriggered()) {
    return hpx_parcel_send(p, HPX_NULL);
  }

  return barrier_.push(p);
}

int
And::set(size_t size, const void *from)
{
  dbg_assert(!size || from);
  auto* p = static_cast<const int*>(from);
  const int n = (p) ? *p : 1;

  // We interact with the counter atomically without the lock, but acquire the
  // lock if we need to modify the condition.
  const int count = count_.fetch_sub(n, std::memory_order_acq_rel);
  log_lco("%p reduced count to %d lco %p\n", hpx_thread_current_parcel(),
          count - n, this);

  if (count > n) {
    return 0;
  }

  if (count == n) {
    std::lock_guard<LCO> _(*this);
    log_lco("%p triggering lco %p\n", hpx_thread_current_parcel(), this);
    dbg_assert(!getTriggered());
    setTriggered();
    barrier_.signalAll();
    return 1;
  }

  dbg_error("too many threads joined (%d).\n", count - n);
  unreachable();
}

hpx_addr_t
hpx_lco_and_new(int64_t limit)
{
  dbg_assert(limit < INT_MAX);
  int inputs(limit);
  hpx_addr_t gva = HPX_NULL;
  try {
    And* lco = new(gva) And(inputs);
    hpx_gas_unpin(gva);
    LCO_LOG_NEW(gva, lco);
  }
  catch (const LCO::NonLocalMemory&) {
    hpx_call_sync(gva, New, nullptr, 0, &inputs);
  }
  return gva;
}

/// Join the and.
void
hpx_lco_and_set(hpx_addr_t cand, hpx_addr_t rsync)
{
  hpx_lco_set(cand, 0, NULL, HPX_NULL, rsync);
}

/// Set an "and" @p num times.
void
hpx_lco_and_set_num(hpx_addr_t cand, int sum, hpx_addr_t rsync)
{
  hpx_addr_t lsync = hpx_lco_future_new(0);
  hpx_lco_set(cand, sizeof(sum), &sum, lsync, rsync);
  hpx_lco_wait(lsync);
  hpx_lco_delete(lsync, HPX_NULL);
}

hpx_addr_t
hpx_lco_and_local_array_new(int n, int limit)
{
  hpx_addr_t base = lco_alloc_local(n, sizeof(And), 0);
  if (!base) {
    throw std::bad_alloc();
  }

  hpx_addr_t bcast = hpx_lco_and_new(n);
  for (int i = 0, e = n; i < e; ++i) {
    hpx_addr_t addr = hpx_addr_add(base, i * sizeof(And), sizeof(And));
    dbg_check( hpx_call(addr, New, bcast, &limit) );
  }
  hpx_lco_wait(bcast);
  hpx_lco_delete_sync(bcast);
  return base;
}

