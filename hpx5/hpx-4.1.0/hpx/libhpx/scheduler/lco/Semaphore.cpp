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
#include "config.h"
#endif

/// @file libhpx/scheduler/sema.c
/// @brief Implements the semaphore LCO.

#include "LCO.h"
#include "Condition.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include <mutex>

namespace {
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;

class Semaphore final : public LCO
{
 public:
  Semaphore(unsigned init);

  ~Semaphore() {
    lock();                                     // Released in ~LCO()
  }

  int set(size_t size, const void *value) {
    std::lock_guard<LCO> _(*this);
    if (0 == count_++) {
      nonZero_.signal();                        // just wake one waiter
    }
    return 1;
  }

  void error(hpx_status_t code) {
    std::lock_guard<LCO> _(*this);
    nonZero_.signalError(code);
  }

  hpx_status_t get(size_t size, void *value, int reset) {
    dbg_assert(size == 0);
    return wait(reset);
  }

  hpx_status_t wait(int reset);

  hpx_status_t attach(hpx_parcel_t *p) {
    // @todo: what does it mean to create an error here?
    dbg_error("Attaching to a semaphore is unsupported.\n");
  }

  void reset() {
    std::lock_guard<LCO> _(*this);
    resetNonZero();
  }

  size_t size(size_t) const {
    return sizeof(*this);
  }

 public:
  /// Static action interface.
  /// @{
  static int NewHandler(void* buffer, unsigned count) {
    auto lco = new(buffer) Semaphore(count);
    return HPX_THREAD_CONTINUE(lco);
  }
  /// @}

 private:
  void resetNonZero() {
    nonZero_.signalAll();                         // wake all waiters
    nonZero_.reset();
    count_ = init_;
  }

  Condition   nonZero_;
  unsigned      count_;
  const unsigned init_;
};

LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, New, Semaphore::NewHandler,
              HPX_POINTER, HPX_UINT);
} // namespace

Semaphore::Semaphore(unsigned count)
    : LCO(LCO_SEMA),
      nonZero_(),
      count_(count),
      init_(count)
{
}

hpx_status_t
Semaphore::wait(int reset)
{
  std::lock_guard<LCO> _(*this);
  while (count_ == 0) {
    if (auto status = waitFor(nonZero_)) {
      return status;
    }
  }

  count_--;

  if (reset) {
    resetNonZero();
  }

  return HPX_SUCCESS;
}

/// Allocate a semaphore LCO.
hpx_addr_t
hpx_lco_sema_new(unsigned count)
{
  hpx_addr_t gva = HPX_NULL;
  Semaphore* lco = nullptr;
  try {
    lco = new(gva) Semaphore(count);
    hpx_gas_unpin(gva);
  }
  catch (const LCO::NonLocalMemory&) {
    hpx_call_sync(gva, New, &lco, sizeof(lco), &count);
  }
  LCO_LOG_NEW(gva, lco);
  return gva;
}

/// Decrement a semaphore.
///
/// Just forward to the equivalent lco_wait() operation.
hpx_status_t
hpx_lco_sema_p(hpx_addr_t sema)
{
  return hpx_lco_wait(sema);
}

/// Increment a semaphore.
///
/// If the semaphore is local, then we can use the _sema_set operation directly,
/// otherwise we perform the operation as an asynchronous remote call using the
/// _sema_v action.
void
hpx_lco_sema_v(hpx_addr_t sema, hpx_addr_t rsync)
{
  hpx_lco_set(sema, 0, NULL, HPX_NULL, rsync);
}

/// Increment a semaphore synchronously.
///
/// Just forwards on.
void
hpx_lco_sema_v_sync(hpx_addr_t sema)
{
  hpx_lco_set_rsync(sema, 0, NULL);
}
