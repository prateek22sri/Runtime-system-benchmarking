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

/// @file libhpx/scheduler/allreduce.c
/// @brief Defines the all-reduction LCO.

#include "LCO.h"
#include "Condition.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include "libhpx/Worker.h"
#include <cstring>
#include <mutex>

namespace {
using libhpx::self;
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;

class AllReduce final : public LCO {
  static constexpr int REDUCING = 0;
  static constexpr int READING = 1;

 public:
  AllReduce(size_t writers, size_t readers, size_t size,
            hpx_action_t id, hpx_action_t op);

  ~AllReduce() {
    // lock(TRACE_EVENT_LCO_DELETE);
    lock();
  }

  hpx_status_t attach(hpx_parcel_t *p);

  int set(size_t size, const void *value) {
    //lock(TRACE_EVENT_LCO_SET);
    std::lock_guard<LCO> _(*this);
    return setInner(size, value);
  }

  void error(hpx_status_t code) {
    std::lock_guard<LCO> _(*this);
    epoch_.signalError(code);
  }

  hpx_status_t get(size_t size, void *value, int reset) {
    // lock(TRACE_EVENT_LCO_WAIT);
    std::lock_guard<LCO> _(*this);
    return getInner(size, value, reset);
  }

  hpx_status_t getRef(size_t size, void **out, int *unpin) {
    // We can't retain a pointer to their buffer due to implicit reset.
    *out = registered_malloc(size);
    *unpin = 1;
    return get(size, *out, 0);
  }

  bool release(void *out) {
    // We know that allreduce buffers were always copies.
    registered_free(out);
    return false;
  }

  hpx_status_t wait(int reset) {
    return get(0, NULL, reset);
  }

  void reset() {
    // lock(TRACE_EVENT_LCO_RESET);
    std::lock_guard<LCO> _(*this);
    epoch_.reset();
  }

  size_t size(size_t bytes) const {
    return sizeof(AllReduce) + bytes;
  }

  int join(size_t size, const void* value, void* out) {
    std::lock_guard<LCO> _(*this);
    setInner(size, value);
    return getInner(size, out, 0);
  }

 public:
  /// Static action interface.
  /// @{
  static int NewHandler(void* buffer, size_t writers, size_t readers,
                        size_t size, hpx_action_t id, hpx_action_t op) {
    auto lco = new(buffer) AllReduce(writers, readers, size, id, op);
    LCO_LOG_NEW((uintptr_t)self->getCurrentParcel(), lco);
    return HPX_SUCCESS;
  }

  static int JoinHandler(AllReduce* lco, const void* data, size_t n);

  struct JoinAsyncArgs {
    void *out;
    hpx_addr_t done;
    char data[];
  };
  static int JoinRequestHandler(AllReduce *lco, JoinAsyncArgs *args, size_t n);
  static int JoinReplyHandler(JoinAsyncArgs *args, size_t n);
  /// @}

 private:
  /// Helper function that applies the stored operation.
  void op(size_t size, const void* from) {
    if (size) {
      dbg_assert(from && op_);
      hpx_monoid_op_t f = (hpx_monoid_op_t)actions[op_].handler;
      f(value_, from, size);
    }
  }

  /// Helper function that resets the stored value.
  void id(size_t size) {
    if (id_) {
      hpx_monoid_id_t f = (hpx_monoid_id_t)actions[id_].handler;
      f(value_, size);
    }
  }

  /// Handle the set operation without acquiring the lock.
  ///
  /// @param       size The number of bytes in @p value.
  /// @param      value The input value for the set operation.
  ///
  /// @returns          HPX_SUCCESS
  int setInner(size_t size, const void *value);

  /// Handle the get operation without acquiring the lock.
  ///
  /// @param       size The number of bytes in @p value.
  /// @param[out] value The value returned.
  /// @param      reset A flag indicating if we should reset the LCO.
  ///
  /// @returns          HPX_SUCCESS
  hpx_status_t getInner(size_t size, void *value, int reset);

  Condition        epoch_;
  const size_t   readers_;
  const size_t   writers_;
  const hpx_action_t  id_;
  const hpx_action_t  op_;
  size_t           count_;
  volatile int     phase_;
  alignas(16) char value_[];
};

LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, New, AllReduce::NewHandler,
              HPX_POINTER, HPX_SIZE_T, HPX_SIZE_T, HPX_SIZE_T, HPX_ACTION_T,
              HPX_ACTION_T);

LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_PINNED, Join,
              AllReduce::JoinHandler, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);

LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_PINNED, JoinRequest,
              AllReduce::JoinRequestHandler, HPX_POINTER, HPX_POINTER,
              HPX_SIZE_T);

LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, JoinReply,
              AllReduce::JoinReplyHandler, HPX_POINTER, HPX_SIZE_T);
}

int
AllReduce::setInner(size_t size, const void *value)
{
  // wait until we're reducing (rather than reading) and then perform the
  // operation
  while (phase_ != REDUCING) {
    if (waitFor(epoch_)) {
      // no way to push an error back to the user here
      dbg_error("Error detected in AllReduce\n");
      unreachable();
    }
  }

  op(size, value);

  // if we were the last one to arrive then its our responsibility to switch the
  // phase
  if (0 == --count_) {
    phase_ = READING;
    epoch_.signalAll();
    return 1;
  }

  return 0;
}

hpx_status_t
AllReduce::getInner(size_t size, void *out, int reset)
{
  dbg_assert(!size || out);

  // wait until we're reading
  while (phase_ != READING) {
    if (auto status = waitFor(epoch_)) {
      return status;
    }
  }

  // copy out the value if the caller wants it
  if (size) {
    memcpy(out, value_, size);
  }

  // update the count, if I'm the last reader to arrive, switch the mode and
  // release all of the other readers, otherwise wait for the phase to change
  // back to reducing---this blocking behavior prevents gets from one "epoch"
  // to satisfy earlier READING epochs because sets don't block
  if (readers_ == ++count_) {
    count_ = writers_;
    phase_ = REDUCING;
    id(size);
    epoch_.signalAll();
    return HPX_SUCCESS;
  }

  while (phase_ == READING) {
    if (auto status = waitFor(epoch_)) {
      return status;
    }
  }

  return HPX_SUCCESS;
}

hpx_status_t
AllReduce::attach(hpx_parcel_t *p)
{
  // lock(TRACE_EVENT_LCO_ATTACH_PARCEL);
  std::lock_guard<LCO> _(*this);

  // Pick attach to mean "set" for allreduce. We have to wait for reducing to
  // complete before sending the parcel.
  if (phase_ != REDUCING) {
    return epoch_.push(p);
  }

  // If the allreduce has an error, then return that error without sending the
  // parcel.
  if (auto status = epoch_.getError()) {
    return status;
  }

  // Go ahead and send this parcel eagerly.
  parcel_launch(p);
  return HPX_SUCCESS;
}


AllReduce::AllReduce(size_t writers, size_t readers, size_t size,
                     hpx_action_t id, hpx_action_t op)
    : LCO(LCO_ALLREDUCE),
      epoch_(),
      readers_(readers),
      writers_(writers),
      id_(id),
      op_(op),
      count_(writers),
      phase_(REDUCING)
{
  if (size) {
    assert(id);
    assert(op);
    this->id(size);
  }
}

hpx_addr_t
hpx_lco_allreduce_new(size_t inputs, size_t outputs, size_t size,
                      hpx_action_t id, hpx_action_t op)
{
  hpx_addr_t gva = HPX_NULL;
  try {
    AllReduce* lva = new(size, gva) AllReduce(inputs, outputs, size, id, op);
    hpx_gas_unpin(gva);
    LCO_LOG_NEW(gva, lva);
  }
  catch (const LCO::NonLocalMemory&) {
    hpx_call_sync(gva, New, nullptr, 0, &inputs, &outputs, &size, &id, &op);
  }
  return gva;
}

hpx_addr_t
hpx_lco_allreduce_local_array_new(int n, size_t participants, size_t readers,
                                  size_t size, hpx_action_t id, hpx_action_t op)
{
  size_t bsize = sizeof(AllReduce) + size;
  hpx_addr_t base = lco_alloc_local(n, bsize, 0);
  if (!base) {
    throw std::bad_alloc();
  }

  hpx_addr_t bcast = hpx_lco_and_new(n);
  for (int i = 0, e = n; i < e; ++i) {
    hpx_addr_t addr = hpx_addr_add(base, i * bsize, bsize);
    dbg_check( hpx_call(addr, New, bcast, &participants, &readers, &size, &id,
                        &op) );
  }
  hpx_lco_wait(bcast);
  hpx_lco_delete_sync(bcast);
  return base;
}

int
AllReduce::JoinHandler(AllReduce *lco, const void *data, size_t n)
{
  // Allocate a parcel that targeting our continuation with enough space for the
  // reduced value, and use its data buffer to join---this prevents a copy or
  // two. This "steals" the current continuation.
  auto*     p = self->getCurrentParcel();
  auto target = p->c_target;
  auto action = p->c_action;
  auto    pid = p->pid;
  auto*     c = parcel_new(target, action, 0, 0, pid, nullptr, n);
  dbg_assert(c);
  p->c_target = 0;
  p->c_action = 0;
  int rc = lco->join(n, data, hpx_parcel_get_data(c));
  parcel_launch_error(c, rc);
  return HPX_SUCCESS;
}

hpx_status_t
hpx_lco_allreduce_join(hpx_addr_t lco, int id, size_t n, const void *value,
                       hpx_action_t cont, hpx_addr_t at) {
  dbg_assert(cont && at);

  // This interface is *asynchronous* so we can just blast out a
  // call-with-continuation using the arguments, even if the allreduce is
  // local.
  return hpx_call_with_continuation(lco, Join, at, cont, value, n);
}
/// @}

/// Synchronous allreduce join interface.
hpx_status_t
hpx_lco_allreduce_join_sync(hpx_addr_t lco, int id, size_t n, const void *value,
                            void *out)
{
  AllReduce *allreduce = nullptr;
  if (hpx_gas_try_pin(lco, (void**)&allreduce)) {
    int rc = allreduce->join(n, value, out);
    hpx_gas_unpin(lco);
    return rc;
  }
  return hpx_call_sync(lco, Join, out, n, value, n);
}

/// The request handler uses the join member to do the join, and then continues
/// the reduced value along with the header information.
int
AllReduce::JoinRequestHandler(AllReduce *lco, JoinAsyncArgs *args, size_t n)
{
  size_t bytes = n - sizeof(*args);
  if (int rc = lco->join(bytes, &args->data, &args->data)) {
    return rc;
  }
  else {
    return hpx_thread_continue(args, n);
  }
}

/// The join reply handler copies the data as specified in the arguments, and
/// then signals the done LCO explicitly.
int
AllReduce::JoinReplyHandler(JoinAsyncArgs *args, size_t n)
{
  if (size_t bytes = n - sizeof(*args)) {
    memcpy(args->out, args->data, bytes);
  }
  hpx_lco_error(args->done, HPX_SUCCESS, HPX_NULL);
  return HPX_SUCCESS;
}

hpx_status_t
hpx_lco_allreduce_join_async(hpx_addr_t lco, int id, size_t n,
                             const void *value, void *out, hpx_addr_t done)
{
  AllReduce *allreduce = nullptr;
  if (hpx_gas_try_pin(lco, (void**)&allreduce)) {
    int rc = allreduce->join(n, value, out);
    hpx_lco_error(done, rc, HPX_NULL);
    hpx_gas_unpin(lco);
    return rc;
  }

  // avoid extra copy by allocating and sending a parcel directly
  auto bytes = sizeof(AllReduce::JoinAsyncArgs) + n;
  auto pid = self->getCurrentParcel()->pid;
  auto *p = parcel_new(lco, JoinRequest, HPX_HERE, JoinReply, pid, nullptr,
                       bytes);

  void *buffer = hpx_parcel_get_data(p);
  auto args = static_cast<AllReduce::JoinAsyncArgs*>(buffer);
  args->out = out;
  args->done = done;
  memcpy(args->data, value, n);
  parcel_launch(p);
  return HPX_SUCCESS;
}
