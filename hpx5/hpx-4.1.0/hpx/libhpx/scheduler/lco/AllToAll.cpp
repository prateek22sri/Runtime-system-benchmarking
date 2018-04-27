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

// AlltoAll is an extention of allgather to the case where each process sends
// distinct data to each of the receivers. The jth block sent from process i
// is received by process j and is placed in the ith block of recvbuf.
// (Complete exchange).

//           sendbuff
//           ####################################
//           #      #      #      #      #      #
//         0 #  A0  #  A1  #  A2  #  A3  #  A4  #
//           #      #      #      #      #      #
//           ####################################
//      T    #      #      #      #      #      #
//         1 #  B0  #  B1  #  B2  #  B3  #  B4  #
//      a    #      #      #      #      #      #
//           ####################################
//      s    #      #      #      #      #      #
//         2 #  C0  #  C1  #  C2  #  C3  #  C4  #       BEFORE
//      k    #      #      #      #      #      #
//           ####################################
//      s    #      #      #      #      #      #
//         3 #  D0  #  D1  #  D2  #  D3  #  D4  #
//           #      #      #      #      #      #
//           ####################################
//           #      #      #      #      #      #
//         4 #  E0  #  E1  #  E2  #  E3  #  E4  #
//           #      #      #      #      #      #
//           ####################################

//             <---------- recvbuff ---------->
//           ####################################
//           #      #      #      #      #      #
//         0 #  A0  #  B0  #  C0  #  D0  #  E0  #
//           #      #      #      #      #      #
//           ####################################
//      T    #      #      #      #      #      #
//         1 #  A1  #  B1  #  C1  #  D1  #  E1  #
//      a    #      #      #      #      #      #
//           ####################################
//      s    #      #      #      #      #      #
//         2 #  A2  #  B2  #  C2  #  D2  #  E2  #       AFTER
//      k    #      #      #      #      #      #
//           ####################################
//      s    #      #      #      #      #      #
//         3 #  A3  #  B3  #  C3  #  D3  #  E3  #
//           #      #      #      #      #      #
//           ####################################
//           #      #      #      #      #      #
//         4 #  A4  #  B4  #  C4  #  D4  #  E4  #
//           #      #      #      #      #      #
//           ####################################

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "LCO.h"
#include "Condition.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include "libhpx/Worker.h"
#include <mutex>
#include <cstring>

namespace {
using libhpx::self;
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;

class AllToAll final : public LCO {
  static constexpr int GATHERING = 0;
  static constexpr int READING = 1;

 public:
  AllToAll(unsigned inputs);

  ~AllToAll() {
    lock();                                     // released in ~LCO()
  }

  /// Set the buffer for a specific offset.
  hpx_status_t setId(unsigned offset, size_t size, const void* buffer);

  /// Get the offset for a specific address.
  hpx_status_t getId(unsigned offset, size_t size, void *out);

  hpx_status_t attach(hpx_parcel_t *p);

  int set(size_t size, const void *value) {
    // @todo: why is this an LCO?
    dbg_error("can't call set on an alltoall LCO.\n");
  }

  void error(hpx_status_t code) {
      std::lock_guard<LCO> _(*this);
      wait_.signalError(code);
  }

  hpx_status_t get(size_t size, void *value, int reset) {
    // @todo: why is this an LCO?
    dbg_error("can't call get on an alltoall LCO.\n");
  }

  hpx_status_t getRef(size_t size, void **out, int *unpin) {
    // @todo: why is this an LCO?
    dbg_error("can't call get on an alltoall LCO.\n");
  }

  bool release(void *out) {
    // @todo: why is this an LCO?
    dbg_error("can't call get on an alltoall LCO.\n");
  }

  hpx_status_t wait(int reset) {
    return getId(0, 0, nullptr);
  }

  void reset() {
      std::lock_guard<LCO> _(*this);
      wait_.reset();
  }

  size_t size(size_t bytes) const {
    return sizeof(AllToAll) + inputs_ * bytes;
  }

 public:
  /// Static action interface.
  /// @{
  static int NewHandler(void* buffer, unsigned inputs) {
    auto lco = new(buffer) AllToAll(inputs);
    LCO_LOG_NEW(hpx_thread_current_target(), lco);
    return HPX_SUCCESS;
  }

  static int GetIdHandler(AllToAll& lco, unsigned offset, size_t size);
  struct SetIdArgs {
    unsigned offset;
    char buffer[];
  };
  static int SetIdHandler(AllToAll& lco, const SetIdArgs& args, size_t n);
  /// @}

 private:
  Condition         wait_;
  const unsigned  inputs_;
  unsigned         count_;
  volatile int     phase_;
  alignas(16) char value_[];
};

LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, New, AllToAll::NewHandler, HPX_POINTER,
              HPX_UINT);
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, GetId, AllToAll::GetIdHandler,
              HPX_POINTER, HPX_UINT, HPX_SIZE_T);
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, SetId,
              AllToAll::SetIdHandler, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);
}

AllToAll::AllToAll(unsigned inputs)
    : LCO(LCO_ALLTOALL),
      wait_(),
      inputs_(inputs),
      count_(inputs),
      phase_(GATHERING),
      value_()
{
}

hpx_status_t
AllToAll::attach(hpx_parcel_t *p)
{
  std::lock_guard<LCO> _(*this);
  if (phase_ != GATHERING) {
    if (auto status = wait_.push(p)) {
      return status;
    }
  }

  if (auto status = wait_.getError()) {
    return status;
  }

  // Go ahead and send this parcel eagerly.
  parcel_launch(p);
  return HPX_SUCCESS;
}

hpx_status_t
AllToAll::getId(unsigned offset, size_t size, void *out)
{
  std::lock_guard<LCO> _(*this);

  // wait until we're reading, and watch for errors
  while (phase_ != READING) {
    if (auto status = waitFor(wait_)) {
      return status;
    }
  }

  // We're in the reading phase, if the user wants data copy it out.
  if (size && out) {
    memcpy(out, value_ + (offset * size), size);
  }

  // Update the count, if I'm the last reader to arrive, switch the mode and
  // release all of the other readers, otherwise wait for the phase to change
  // back to gathering---this blocking behavior prevents gets from one "epoch"
  // to satisfy earlier READING epochs.
  if (++count_ == inputs_) {
    phase_ = GATHERING;
    wait_.signalAll();
  }
  else {
    while (phase_ == READING) {
      if (auto status = waitFor(wait_)) {
        return status;
      }
    }
  }

  return HPX_SUCCESS;
}

// Local set id function.
hpx_status_t
AllToAll::setId(unsigned offset, size_t size, const void* buffer)
{
  dbg_assert(size && buffer);
  std::lock_guard<LCO> _(*this);

  // wait until we're gathering
  while (phase_ != GATHERING) {
    if (auto status = waitFor(wait_)) {
      return status;
    }
  }

  unsigned elementSize = size / inputs_;
  unsigned columnOffset = offset * elementSize;

  for (int i = 0, e = inputs_; i < e; ++i) {
    unsigned rowOffset = i * size;
    unsigned destOffset = rowOffset + columnOffset;
    unsigned sourceOffset = i * elementSize;
    memcpy(value_ + destOffset, (char *)buffer + sourceOffset, elementSize);
  }

  // if we're the last one to arrive, switch the phase and signal readers
  if (--count_ == 0) {
    phase_ = READING;
    wait_.signalAll();
  }
  return HPX_SUCCESS;
}

int
AllToAll::GetIdHandler(AllToAll& lco, unsigned offset, size_t size)
{
  // Allocate a parcel that targeting our continuation with enough space for the
  // reduced value, and use its data buffer to get the value---this prevents a
  // copy or two. This "steals" the current continuation.
  auto*     p = self->getCurrentParcel();
  auto target = p->c_target;
  auto action = p->c_action;
  auto    pid = p->pid;
  auto*     c = parcel_new(target, action, 0, 0, pid, nullptr, size);
  dbg_assert(c);
  p->c_target = 0;
  p->c_action = 0;
  int rc = lco.getId(offset, size, hpx_parcel_get_data(c));
  parcel_launch_error(c, rc);
  return HPX_SUCCESS;
}

int
AllToAll::SetIdHandler(AllToAll& lco, const SetIdArgs& args, size_t n)
{
  return lco.setId(args.offset, n - sizeof(args), args.buffer);
}

hpx_status_t
hpx_lco_alltoall_getid(hpx_addr_t gva, unsigned id, int size, void *value)
{
  dbg_assert(size >= 0);
  AllToAll *lva = nullptr;
  if (hpx_gas_try_pin(gva, (void**)&lva)) {
    hpx_status_t status = lva->getId(id, size, value);
    hpx_gas_unpin(gva);
    return status;
  }
  size_t n(size);
  return hpx_call_sync(gva, GetId, value, size, &id, &n);
}

hpx_status_t
hpx_lco_alltoall_setid(hpx_addr_t gva, unsigned id, int size, const void *value,
                       hpx_addr_t lsync, hpx_addr_t rsync)
{
  dbg_assert(size > 0);
  AllToAll* lva = nullptr;
  if (hpx_gas_try_pin(gva, (void**)&lva)) {
    hpx_status_t status = lva->setId(id, size, value);
    hpx_gas_unpin(gva);
    hpx_lco_set(lsync, 0, NULL, HPX_NULL, HPX_NULL);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
    return status;
  }

  size_t bytes = sizeof(AllToAll::SetIdArgs) + size;
  hpx_parcel_t *p = action_new_parcel(SetId,              // action
                                      gva,                // target
                                      rsync,              // continuation target
                                      hpx_lco_set_action, // continuation action
                                      2,                  // nargs
                                      nullptr,            // buffer
                                      bytes);             // size

  auto& args = *static_cast<AllToAll::SetIdArgs*>(hpx_parcel_get_data(p));
  args.offset = id;
  memcpy(args.buffer, value, size);
  hpx_parcel_send(p, lsync);
  return HPX_SUCCESS;
}

hpx_addr_t
hpx_lco_alltoall_new(size_t inputs, size_t size)
{
  dbg_assert(inputs < UINT_MAX);
  hpx_addr_t gva = HPX_NULL;
  unsigned in(inputs);
  try {
    size_t bytes = in * size;
    AllToAll* lco = new(bytes, gva) AllToAll(in);
    hpx_gas_unpin(gva);
    LCO_LOG_NEW(gva, lco);
  }
  catch (const LCO::NonLocalMemory&) {
    dbg_check( hpx_call_sync(gva, New, nullptr, 0, &in) );
  }
  return gva;
}

hpx_addr_t
hpx_lco_alltoall_local_array_new(int n, size_t inputs, size_t size)
{
  dbg_assert(inputs < UINT_MAX);
  unsigned in(inputs);
  size_t bytes = in * size;
  size_t bsize = sizeof(AllToAll) + bytes;
  hpx_addr_t base = lco_alloc_local(n, bsize, 0);
  if (!base) {
    throw std::bad_alloc();
  }

  hpx_addr_t bcast = hpx_lco_and_new(n);
  for (int i = 0, e = n; i < e; ++i) {
    hpx_addr_t addr = hpx_addr_add(base, i * bsize, bsize);
    dbg_check( hpx_call(addr, New, bcast, &in) );
  }
  hpx_lco_wait(bcast);
  hpx_lco_delete_sync(bcast);
  return base;
}

