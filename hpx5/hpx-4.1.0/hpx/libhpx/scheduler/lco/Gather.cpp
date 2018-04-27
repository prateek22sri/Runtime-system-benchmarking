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

/// Given a set of buffers distributed across processes, gather will
/// collect all of the elements to.
///

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "LCO.h"
#include "Condition.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include <cstring>
#include <mutex>

namespace {
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;

class Gather final : public LCO {
 public:
  Gather(unsigned writers, unsigned readers, size_t size);

  ~Gather() {
    lock();                                     // reset in ~LCO()
  }

  hpx_status_t get(size_t size, void *value, int reset);
  hpx_status_t attach(hpx_parcel_t *p);
  hpx_status_t setId(unsigned offset, size_t size, const void* buffer);

  int set(size_t size, const void *value) {
    // @todo: is this even an LCO in this case?
    dbg_error("Gather LCO does not support get\n");
  }

  void error(hpx_status_t code) {
    std::lock_guard<LCO> _(*this);
    cvar_.signalError(code);
  }

  hpx_status_t getRef(size_t size, void **out, int *unpin) {
    *out = registered_malloc(size);             // clone the buffer because
    *unpin = 1;                                 // it is reset between rounds
    return get(size, *out, 0);
  }

  bool release(void *out) {
    registered_free(out);                       // free the clone
    return 0;
  }

  hpx_status_t wait(int reset) {
    return get(0, NULL, reset);
  }

  void reset() {
    std::lock_guard<LCO> _(*this);
    cvar_.reset();
  }

  size_t size(size_t bytes) const {
    return sizeof(Gather) + writers_ * bytes;
  }

 public:
  /// Static action interface.
  /// @{
  static int NewHandler(void* buffer, unsigned writers, unsigned readers,
                        size_t size) {
    auto lco = new(buffer) Gather(writers, readers, size);
    LCO_LOG_NEW(hpx_thread_current_target(), lco);
    return HPX_SUCCESS;
  }

  struct SetIdArgs {
    unsigned offset;
    char     buffer[];
  };
  static int SetIdHandler(Gather& lco, const SetIdArgs& args, size_t n);
  /// @}

 private:
  Condition           cvar_;
  const unsigned   writers_;
  const unsigned   readers_;
  volatile unsigned wcount_;                    // volatile because read in
  volatile unsigned rcount_;                    // while() loops
  char               value_[];
};

LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, New, Gather::NewHandler, HPX_POINTER,
              HPX_UINT, HPX_UINT, HPX_SIZE_T);
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, SetId,
              Gather::SetIdHandler, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);
}

Gather::Gather(unsigned writers, unsigned readers, size_t size)
    : LCO(LCO_GATHER),
      cvar_(),
      writers_(writers),
      readers_(readers),
      wcount_(),
      rcount_(readers)
{
  memset(value_, 0, size);
}

hpx_status_t
Gather::attach(hpx_parcel_t *p)
{
  std::lock_guard<LCO> _(*this);
  // Pick attach to mean "set" for gather. We have to wait for gathering to
  // complete before sending the parcel.
  if (wcount_ == writers_) {
    return cvar_.push(p);
  }

  // If the gather has an error, then return that error without sending the
  // parcel.
  if (auto status = cvar_.getError()) {
    return status;
  }

  // Go ahead and send this parcel eagerly.
  hpx_parcel_send(p, HPX_NULL);
  return HPX_SUCCESS;
}

/// Get the value of the gather LCO. This operation will wait if the
/// writers have not finished gathering.
hpx_status_t
Gather::get(size_t size, void *out, int reset)
{
  std::lock_guard<LCO> _(*this);

  // Wait until we're reading, and watch for errors.
  while (wcount_ < writers_) {
    if (auto status = waitFor(cvar_)) {
      return status;
    }
  }

  // We're in a reading phase, and if the user wants the data, copy it out.
  if (size && out) {
    memcpy(out, value_, size);
  }

  // Update the count, if I'm the last reader to arrive, switch the mode and
  // release all of the other readers, otherwise wait for the phase to change
  // back to gathering---this blocking behavior prevents gets from one "epoch"
  // to satisfy earlier READING epochs.
  if (0 == --rcount_) {
    wcount_ = 0;
    cvar_.signalAll();
  }

  return HPX_SUCCESS;
}

// Local set id function.
hpx_status_t
Gather::setId(unsigned offset, size_t size, const void* buffer)
{
  std::lock_guard<LCO> _(*this);
  // wait until we're gathering
  while (wcount_ == writers_) {
    if (auto status = waitFor(cvar_)) {
      return status;
    }
  }

  // copy in our chunk of the data
  assert(size && buffer);
  char* dest = (char*)value_ + (offset * size);
  memcpy(dest, buffer, size);

  // if we're the last one to arrive, switch the phase and signal readers
  if (++wcount_ == writers_) {
    rcount_ = readers_;
    cvar_.signalAll();
  }

  return HPX_SUCCESS;
}

int
Gather::SetIdHandler(Gather& lco, const SetIdArgs& args, size_t n)
{
  return lco.setId(args.offset, n - sizeof(args), args.buffer);
}

hpx_status_t
hpx_lco_gather_setid(hpx_addr_t gather, unsigned id, int size,
                     const void *value, hpx_addr_t lsync, hpx_addr_t rsync)
{
  Gather* lco = nullptr;
  if (hpx_gas_try_pin(gather, (void**)&lco)) {
    auto status = lco->setId(id, size, value);
    hpx_gas_unpin(gather);
    hpx_lco_set(lsync, 0, NULL, HPX_NULL, HPX_NULL);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
    return status;
  }

  size_t bytes = sizeof(Gather::SetIdArgs) + size;
  hpx_parcel_t *p = action_new_parcel(SetId,              // action
                                      gather,             // target
                                      rsync,              // continuation target
                                      hpx_lco_set_action, // continuation action
                                      2,                  // number of args
                                      nullptr,            // buffer
                                      bytes);             // bytes

  void* buffer = hpx_parcel_get_data(p);
  auto& args = *static_cast<Gather::SetIdArgs*>(buffer);
  args.offset = id;
  memcpy(args.buffer, value, size);
  hpx_parcel_send(p, lsync);
  return HPX_SUCCESS;
}

hpx_addr_t
hpx_lco_gather_new(size_t inputs, size_t outputs, size_t size)
{
  dbg_assert(inputs < UINT_MAX);
  dbg_assert(outputs < UINT_MAX);
  hpx_addr_t gva = HPX_NULL;
  unsigned writers(inputs);
  unsigned readers(outputs);
  size_t bytes(writers * size);
  try {
    Gather* lco = new(bytes, gva) Gather(writers, readers, bytes);
    hpx_gas_unpin(gva);
    LCO_LOG_NEW(gva, lco);
  }
  catch (const LCO::NonLocalMemory&) {
    hpx_call_sync(gva, New, nullptr, 0, &writers, &readers, &bytes);
  }
  return gva;
}

hpx_addr_t
hpx_lco_gather_local_array_new(int count, size_t inputs, size_t outputs,
                               size_t size)
{
  dbg_assert(count > 0);
  dbg_assert(inputs < UINT_MAX);
  dbg_assert(outputs < UINT_MAX);
  unsigned n(count);
  unsigned writers(inputs);
  unsigned readers(outputs);
  size_t bytes = writers * size;
  size_t bsize = sizeof(Gather) + bytes;
  hpx_addr_t base = lco_alloc_local(n, bsize, 0);
  if (!base) {
    throw std::bad_alloc();
  }

  hpx_addr_t bcast = hpx_lco_and_new(n);
  for (int i = 0, e = n; i < e; ++i) {
    hpx_addr_t addr = hpx_addr_add(base, i * bsize, bsize);
    dbg_check( hpx_call(addr, New, bcast, &writers, &readers, &bytes) );
  }
  hpx_lco_wait(bcast);
  hpx_lco_delete_sync(bcast);
  return base;

}
