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

/// @file libhpx/scheduler/reduce.c
/// @brief Defines the reduction LCO.

#include "LCO.h"
#include "Condition.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include <mutex>
#include <cstring>

namespace {
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;

struct Reduce final : public LCO {
 public:
  Reduce(unsigned inputs, size_t size, hpx_action_t id, hpx_action_t op);

  ~Reduce() {
    lock();
  }

  hpx_status_t attach(hpx_parcel_t *p);
  hpx_status_t get(size_t size, void *value, int reset);
  hpx_status_t getRef(size_t size, void **out, int *unpin);
  bool release(void *out);
  int set(size_t size, const void *value);

  void error(hpx_status_t code) {
    std::lock_guard<LCO> _(*this);
    barrier_.signalError(code);
  }

  hpx_status_t wait(int reset) {
    return get(0, NULL, reset);
  }


  void reset() {
    std::lock_guard<LCO> _(*this);
    resetBarrier();
  }

  size_t size(size_t size) const {
    return sizeof(Reduce) + size;
  }

 public:
  /// Static action interface.
  /// @{
  static int NewHandler(void* buffer, unsigned inputs, size_t size,
                        hpx_action_t id, hpx_action_t op) {
    auto lco = new(buffer) Reduce(inputs, size, id, op);
    LCO_LOG_NEW(hpx_thread_current_target(), lco);
    return HPX_SUCCESS;
  }
  /// @}

 private:
  void resetBarrier() {
    barrier_.reset();
    remaining_ = inputs_;
    id();
  }

  void op(size_t size, const void* from) {
    if (size) {
      dbg_assert(from && size_ && op_);
      hpx_monoid_op_t f = (hpx_monoid_op_t)actions[op_].handler;
      f(value_, from, size);
    }
  }

  void id() {
    if (id_) {
      hpx_monoid_id_t f = (hpx_monoid_id_t)actions[id_].handler;
      f(value_, size_);
    }
  }

  Condition           barrier_;
  const size_t           size_;
  const hpx_action_t       id_;
  const hpx_action_t       op_;
  const unsigned       inputs_;
  volatile unsigned remaining_;
  alignas(16) char      value_[];
};

LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, New, Reduce::NewHandler, HPX_POINTER,
              HPX_UINT, HPX_SIZE_T, HPX_ACTION_T, HPX_ACTION_T);
}

Reduce::Reduce(unsigned inputs, size_t size, hpx_action_t id, hpx_action_t op)
    : LCO(LCO_REDUCE),
      barrier_(),
      size_(size),
      id_(id),
      op_(op),
      inputs_(inputs),
      remaining_(inputs)
{
  dbg_assert(!size_ || op_);
  dbg_assert(!size_ || id_);
  this->id();
}

hpx_status_t
Reduce::attach(hpx_parcel_t *p)
{
  std::lock_guard<LCO> _(*this);
  if (remaining_) {
    return barrier_.push(p);
  }

  if (auto status = barrier_.getError()) {
    // NB: should we actually send some sort of error condition?
    parcel_delete(p);
    return status;
  }

  // go ahead and send this parcel eagerly
  parcel_launch(p);
  return HPX_SUCCESS;
}

/// Update the reduction.
int
Reduce::set(size_t size, const void *from)
{
  dbg_assert(size == size_);
  dbg_assert(!size || from);
  std::lock_guard<LCO> _(*this);

  op(size, from);

  if (0 == --remaining_) {
    barrier_.signalAll();
    return 1;
  }
  else {
    dbg_assert_str(remaining_ > 0, "reduction: too many threads joined (%d).\n",
                   remaining_);
  }

  log_lco("reduce: received input %d\n", remaining_);
  return 0;
}

/// Get the value of the reduction.
hpx_status_t
Reduce::get(size_t size, void *out, int reset)
{
  dbg_assert(!size || (size == size_));
  dbg_assert(!size || out);
  std::lock_guard<LCO> _(*this);

  while (remaining_) {
    if (auto status = waitFor(barrier_)) {
      return status;
    }
  }

  // copy out the value if the caller wants it
  if (size && out) {
    memcpy(out, value_, size);
  }

  if (reset) {
    resetBarrier();
  }

  return HPX_SUCCESS;
}

hpx_status_t
Reduce::getRef(size_t size, void **out, int *unpin)
{
  dbg_assert(size == size_);
  dbg_assert(size && out);

  if (hpx_status_t status = wait(0)) {
    return status;
  }

  *out = value_;
  *unpin = 0;
  return HPX_SUCCESS;
}

bool
Reduce::release(void *out)
{
  dbg_assert(out == value_);
  return 1;
}

hpx_addr_t
hpx_lco_reduce_new(int inputs, size_t size, hpx_action_t id, hpx_action_t op)
{
  dbg_assert(inputs > 0);
  hpx_addr_t gva = HPX_NULL;
  unsigned writers(inputs);
  try {
    Reduce* lco = new(size, gva) Reduce(writers, size, id, op);
    hpx_gas_unpin(gva);
    LCO_LOG_NEW(gva, lco);
  }
  catch (const LCO::NonLocalMemory&) {
    hpx_call_sync(gva, New, nullptr, 0, &writers, &size, &id, &op);
  }
  return gva;
}

hpx_addr_t
hpx_lco_reduce_local_array_new(int n, int inputs, size_t size, hpx_action_t id,
                               hpx_action_t op)
{
  dbg_assert(inputs > 0);
  dbg_assert(n > 0);
  unsigned writers(inputs);
  size_t bsize = sizeof(Reduce) + size;
  hpx_addr_t base = lco_alloc_local(n, bsize, 0);
  if (!base) {
    throw std::bad_alloc();
  }

  hpx_addr_t bcast = hpx_lco_and_new(n);
  for (int i = 0, e = n; i < e; ++i) {
    hpx_addr_t addr = hpx_addr_add(base, i * bsize, bsize);
    dbg_check( hpx_call(addr, New, bcast, &writers, &size, &id, &op) );
  }
  hpx_lco_wait(bcast);
  hpx_lco_delete_sync(bcast);
  return base;
}
