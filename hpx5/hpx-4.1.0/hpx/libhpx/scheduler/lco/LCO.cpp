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

/// @file libhpx/scheduler/lco.cpp

#include "LCO.h"
#include "Thread.h"                             //<! struct ustack
#include "libhpx/action.h"
#include "libhpx/attach.h"
#include "libhpx/debug.h"
#include "libhpx/instrumentation.h"
#include "libhpx/memory.h"
#include "libhpx/Network.h"
#include "libhpx/Worker.h"
#include "libhpx/parcel.h"

namespace {
using libhpx::scheduler::LCO;
}

static constexpr short TRIGGERED_MASK = (0x2);
static constexpr short      USER_MASK = (0x4);

static LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, _lco_size,
                     LCO::SizeHandler, HPX_POINTER, HPX_SIZE_T);
static LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, _lco_get, LCO::GetHandler,
                     HPX_POINTER, HPX_INT);
static LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, _lco_wait, LCO::WaitHandler,
                     HPX_POINTER, HPX_INT);

LIBHPX_ACTION(HPX_INTERRUPT, HPX_PINNED, hpx_lco_delete_action,
              LCO::DeleteHandler, HPX_POINTER);
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, hpx_lco_set_action,
              LCO::SetHandler, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, lco_error,
              LCO::ErrorHandler, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, hpx_lco_reset_action,
              LCO::ResetHandler, HPX_POINTER);
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, lco_attach,
              LCO::AttachHandler, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);

void*
LCO::TryPin(hpx_addr_t gva)
{
  void *lva = nullptr;
  if (!hpx_gas_try_pin(gva, &lva)) {
    throw NonLocalMemory();
  }
  return lva;
}

void*
LCO::operator new(size_t, void * ptr)
{
  dbg_assert_str(ptr, "LCO placement new() requires valid address.\n");
  return ptr;
}

void*
LCO::operator new(size_t bytes, hpx_addr_t& gva)
{
  if ((gva = lco_alloc_local(1, bytes, 0))) {
    return TryPin(gva);
  }
  else {
    throw std::bad_alloc();
  }
}

void*
LCO::operator new(size_t size, size_t bytes, hpx_addr_t& gva)
{
  if ((gva = lco_alloc_local(1, size + bytes, 0))) {
    return TryPin(gva);
  }
  else {
    throw std::bad_alloc();
  }
}

void
LCO::operator delete(void* obj)
{
  dbg_error("operator delete should not be called on LCOs");
}


LCO::LCO(enum Type type) : lock_(), state_(), type_(type)
{
  trace_append(HPX_TRACE_LCO, TRACE_EVENT_LCO_INIT, this, state_);
}

/// Our infrastructure requires that the destructor run atomically with the rest
/// of the monitor interface, so the subclasses will all call lock() while they
/// run.
LCO::~LCO()
{
  unlock();
}

/// The default getRef implementation just waits for the lco and then returns
/// the address of the LCO. This is suitable for LCOs that represent only
/// control signals.
hpx_status_t
LCO::getRef(size_t size, void **out, int *unpin)
{
  dbg_assert(size && out);

  if (hpx_status_t status = wait(0)) {
    return status;
  }

  *out = this;                                  // used in release()
  *unpin = 0;                                   // don't unpin the LCO
  return HPX_SUCCESS;
}

/// The default release implementation just checks to make sure that the address
/// being released matches the address that was given out by the default getRef
/// implementation.
bool
LCO::release(void *out)
{
  if (out != this) {
    dbg_error("Releasing an LCO buffer that was never requested\n");
  }
  return true;                                     // unpin the LCO
}

/// Action LCO event handler wrappers.
///
/// These try and pin the LCO, and then forward to the local event handler
/// wrappers. If the pin fails, then the LCO isn't local, so the parcel is
/// resent.
///
/// @{
int
LCO::DeleteHandler(LCO *lco)
{
  lco->~LCO();
  hpx_addr_t target = hpx_thread_current_target();
  return hpx_call_cc(target, hpx_gas_free_action);
}

int
LCO::SetHandler(LCO *lco, void *data, size_t n)
{
  int i = lco->set(n, data);
  return HPX_THREAD_CONTINUE(i);
}

int
LCO::ErrorHandler(LCO *lco, void *args, size_t n)
{
  hpx_status_t *code = static_cast<hpx_status_t*>(args);
  lco->error(*code);
  return HPX_SUCCESS;
}

int
LCO::ResetHandler(LCO *lco)
{
  lco->reset();
  return HPX_SUCCESS;
}

int
LCO::SizeHandler(const LCO *lco, int arg)
{
  return lco->size(arg);
}

int
LCO::GetHandler(LCO *lco, int n)
{
  dbg_assert(n > 0);

  // Use the getref handler, no need to worry about pinning here because we
  // _release() in the continuation.
  void *buffer;
  int ignore_unpin;
  hpx_status_t status = lco->getRef(n, &buffer, &ignore_unpin);
  if (status != HPX_SUCCESS) {
    return status;
  }
  int e = hpx_thread_continue(buffer, n);
  lco->release(buffer);
  return e;
}

int
LCO::WaitHandler(LCO *lco, int reset)
{
  return lco->wait(reset);
}

int
LCO::AttachHandler(LCO *lco, hpx_parcel_t *p, size_t size)
{
  hpx_parcel_t *parent = self->getCurrentParcel();
  dbg_assert(hpx_parcel_get_data(parent) == p);
  log_lco("pinning %p, nesting %p\n", (void*)parent, (void*)p);
  parcel_pin(parent);
  parcel_nest(p);
  return lco->attach(p);
}

void
LCO::lock(hpx_parcel_t* p)
{
  lock_.lock();
  log_lco("%p acquired lco %p\n", p, this);
  p->thread->enterLCO(this);
}

void
LCO::unlock(hpx_parcel_t* p)
{
  p->thread->leaveLCO(this);
  log_lco("%p released lco %p\n", p, this);
  lock_.unlock();
}

void
LCO::lock()
{
  lock(self->getCurrentParcel());
}

void
LCO::unlock()
{
  unlock(self->getCurrentParcel());
}

short
LCO::setTriggered()
{
  trace_append(HPX_TRACE_LCO, TRACE_EVENT_LCO_TRIGGER, this, state_);
  auto state = state_;
  state_ = state | TRIGGERED_MASK;
  return (state & TRIGGERED_MASK);
}

void
LCO::resetTriggered()
{
  state_ &= ~TRIGGERED_MASK;
}

short
LCO::getTriggered() const
{
  return (state_ & TRIGGERED_MASK);
}

void
LCO::setUser()
{
  state_ |= USER_MASK;
}

short
LCO::getUser() const
{
  return (state_ & USER_MASK);
}

hpx_status_t
LCO::waitFor(Condition& cond)
{
  return self->wait(*this, cond);
}

void
hpx_lco_delete(hpx_addr_t target, hpx_addr_t rsync)
{
  LCO *lco = NULL;
  if (!hpx_gas_try_pin(target, (void**)&lco)) {
    int e = hpx_call(target, hpx_lco_delete_action, rsync);
    dbg_check(e, "Could not forward lco_delete\n");
  }
  else {
    log_lco("deleting lco %" PRIu64 " (%p)\n", target, (void*)lco);
    lco->~LCO();
    hpx_gas_unpin(target);
    hpx_gas_free(target, HPX_NULL);
    hpx_lco_error(rsync, HPX_SUCCESS, HPX_NULL);
  }
}

void
hpx_lco_delete_sync(hpx_addr_t target)
{
  hpx_addr_t sync = hpx_lco_future_new(0);
  hpx_lco_delete(target, sync);
  hpx_lco_wait(sync);
  hpx_lco_delete(sync, HPX_NULL);
}

void
hpx_lco_error(hpx_addr_t target, hpx_status_t code, hpx_addr_t rsync)
{
  if (code == HPX_SUCCESS) {
    hpx_lco_set(target, 0, NULL, HPX_NULL, rsync);
    return;
  }

  if (target == HPX_NULL) {
    return;
  }

  LCO *lco = NULL;
  if (hpx_gas_try_pin(target, (void**)&lco)) {
    lco->error(code);
    hpx_gas_unpin(target);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
    return;
  }

  size_t size = sizeof(code);
  int e = hpx_call_async(target, lco_error, HPX_NULL, rsync, &code, size);
  dbg_check(e, "Could not forward lco_error\n");
}

void
hpx_lco_error_sync(hpx_addr_t addr, hpx_status_t code)
{
  hpx_addr_t sync = hpx_lco_future_new(0);
  hpx_lco_error(addr, code, sync);
  hpx_lco_wait(sync);
  hpx_lco_delete(sync, HPX_NULL);
}

void
hpx_lco_reset(hpx_addr_t addr, hpx_addr_t rsync)
{
  if (addr == HPX_NULL) {
    return;
  }

  LCO *lco = NULL;
  if (hpx_gas_try_pin(addr, (void**)&lco)) {
    lco->reset();
    hpx_gas_unpin(addr);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
    return;
  }

  int e = hpx_call(addr, hpx_lco_reset_action, rsync);
  dbg_check(e, "Could not forward lco_reset\n");
}

void
hpx_lco_reset_sync(hpx_addr_t addr)
{
  hpx_addr_t sync = hpx_lco_future_new(0);
  hpx_lco_reset(addr, sync);
  hpx_lco_wait(sync);
  hpx_lco_delete(sync, HPX_NULL);
}

void
hpx_lco_set_with_continuation(hpx_addr_t target,
                              size_t size, const void *value,
                              hpx_addr_t lsync,
                              hpx_addr_t raddr, hpx_action_t rop)
{
  if (target == HPX_NULL) {
    if (lsync != HPX_NULL) {
      hpx_lco_set_with_continuation(lsync, 0, NULL, HPX_NULL, HPX_NULL,
                                    HPX_ACTION_NULL);
    }
    if (raddr != HPX_NULL) {
      int zero = 0;
      hpx_call(raddr, rop, HPX_NULL, &zero, sizeof(zero));
    }
    return;
  }

  LCO *lco = NULL;
  if (hpx_gas_try_pin(target, (void**)&lco)) {
    int set = lco->set(size, value);
    hpx_gas_unpin(target);
    if (lsync != HPX_NULL) {
      hpx_lco_set_with_continuation(lsync, 0, NULL, HPX_NULL, HPX_NULL,
                                    HPX_ACTION_NULL);
    }
    if (raddr != HPX_NULL) {
      hpx_call(raddr, rop, HPX_NULL, &set, sizeof(set));
    }
    return;
  }

  hpx_parcel_t *p = hpx_parcel_acquire(value, size);
  p->target = target;
  p->action = hpx_lco_set_action;
  p->c_target = raddr;
  p->c_action = rop;

  int e = hpx_parcel_send(p, lsync);
  dbg_check(e, "Could not forward lco_set\n");
}

void
hpx_lco_set(hpx_addr_t target, size_t size, const void *value,
            hpx_addr_t lsync, hpx_addr_t rsync)
{
  hpx_lco_set_with_continuation(target, size, value, lsync, rsync,
                                hpx_lco_set_action);
}

void
hpx_lco_set_lsync(hpx_addr_t target, size_t size, const void *value,
                  hpx_addr_t rsync)
{
  if (target == HPX_NULL) {
    if (rsync) {
      int zero = 0;
      hpx_call(rsync, hpx_lco_set_action, HPX_NULL, &zero, sizeof(zero));
    }
    return;
  }

  LCO* lco = NULL;
  if (hpx_gas_try_pin(target, (void**)&lco)) {
    int set = lco->set(size, value);
    hpx_gas_unpin(target);
    if (rsync) {
      hpx_call(rsync, hpx_lco_set_action, HPX_NULL, &set, sizeof(set));
    }
    return;
  }

  hpx_addr_t lsync = hpx_lco_future_new(0);
  hpx_lco_set(target, size, value, lsync, rsync);
  hpx_lco_wait(lsync);
  hpx_lco_delete(lsync, HPX_NULL);
}

int
hpx_lco_set_rsync(hpx_addr_t target, size_t size, const void *value)
{
  if (target == HPX_NULL) {
    return 0;
  }

  LCO* lco = NULL;
  if (hpx_gas_try_pin(target, (void**)&lco)) {
    int set = lco->set(size, value);
    hpx_gas_unpin(target);
    return set;
  }

  int set = 0;
  hpx_addr_t rsync = hpx_lco_future_new(4);
  hpx_lco_set(target, size, value, HPX_NULL, rsync);
  hpx_lco_get(rsync, sizeof(set), &set);
  hpx_lco_delete(rsync, HPX_NULL);
  return set;
}

hpx_status_t
hpx_lco_wait(hpx_addr_t target)
{
  LCO *lco;
  if (hpx_gas_try_pin(target, (void**)&lco)) {
    hpx_status_t status = lco->wait(0);
    hpx_gas_unpin(target);
    return status;
  }

  int zero = 0;
  return hpx_call_sync(target, _lco_wait, NULL, 0, &zero);
}

hpx_status_t
hpx_lco_wait_reset(hpx_addr_t target)
{
  LCO *lco;
  if (hpx_gas_try_pin(target, (void**)&lco)) {
    hpx_status_t status = lco->wait(1);
    hpx_gas_unpin(target);
    return status;
  }

  int one = 0;
  return hpx_call_sync(target, _lco_wait, NULL, 0, &one);
}

size_t
hpx_lco_size(hpx_addr_t target, size_t size)
{
  LCO *lco = nullptr;
  if (hpx_gas_try_pin(target, (void**)&lco)) {
    size_t bytes = lco->size(size);
    hpx_gas_unpin(target);
    return bytes;
  }
  return hpx_call_sync(target, _lco_size, &size, size, &size);
}

hpx_status_t
hpx_lco_get(hpx_addr_t target, size_t size, void *value)
{
  if (size == 0) {
    return hpx_lco_wait(target);
  }

  dbg_assert(value);
  LCO *lco = nullptr;
  if (!hpx_gas_try_pin(target, (void**)&lco)) {
    return here->net->get(target, size, value, 0);
  }

  hpx_status_t status = lco->get(size, value, 0);
  hpx_gas_unpin(target);
  return status;
}

hpx_status_t
hpx_lco_get_reset(hpx_addr_t target, size_t size, void *value)
{
  if (size == 0) {
    return hpx_lco_wait_reset(target);
  }

  dbg_assert(value);
  LCO *lco = nullptr;
  if (!hpx_gas_try_pin(target, (void**)&lco)) {
    return here->net->get(target, size, value, 1);
  }

  hpx_status_t status = lco->get(size, value, 1);
  hpx_gas_unpin(target);
  return status;
}

hpx_status_t
hpx_lco_getref(hpx_addr_t target, size_t size, void **out)
{
  if (size == 0) {
    return hpx_lco_wait(target);
  }

  dbg_assert(out);
  LCO *lco = nullptr;
  if (!hpx_gas_try_pin(target, (void**)&lco)) {
    *out = registered_malloc(size);
    dbg_assert(*out);
    return hpx_lco_get(target, size, *out);
  }

  int unpin = 0;
  hpx_status_t e = lco->getRef(size, out, &unpin);
  if (unpin) {
    hpx_gas_unpin(target);
  }
  return e;
}

void
hpx_lco_release(hpx_addr_t target, void *out)
{
  dbg_assert(out);
  LCO *lco = nullptr;
  if (!hpx_gas_try_pin(target, (void**)&lco)) {
    // guaranteed to be a registered temporary buffer if the LCO was non-local
    registered_free(out);
  }
  else if (lco->release(out)) {
    // release tells us if we left the LCO pinned in getref
    hpx_gas_unpin(target);
  }
  hpx_gas_unpin(target);
}

int
hpx_lco_wait_all(int n, hpx_addr_t lcos[], hpx_status_t statuses[])
{
  dbg_assert(n > 0);

  // Will partition the lcos up into local and remote LCOs. We waste some
  // space here, since, for each lco in lcos, we either have a local mapping or
  // a remote address. We don't use the stack because we can't control how big
  // @p n gets.
  LCO **locals = new LCO*[n]();
  dbg_assert_str(locals, "failed to allocate array for %d elements", n);
  hpx_addr_t *remotes = new hpx_addr_t[n]();
  dbg_assert_str(remotes, "failed to allocate array for %d elements", n);

  int zero = 0;

  // Try and translate (and pin) all of the lcos, for any of the lcos that
  // aren't local, allocate a proxy future and initiate the remote wait. This
  // two-phase approach achieves some parallelism.
  for (int i = 0; i < n; ++i) {
    // We neither issue a remote proxy for HPX_NULL, nor wait locally on
    // HPX_NULL. We manually set the status output for these elements to
    // indicate success.
    if (lcos[i] == HPX_NULL) {
      locals[i] = NULL;
      remotes[i] = HPX_NULL;
    }
    else if (hpx_gas_try_pin(lcos[i], (void**)&locals[i])) {
      remotes[i] = HPX_NULL;
    }
    else {
      locals[i] = NULL;
      remotes[i] = hpx_lco_future_new(0);
      hpx_call_async(lcos[i], _lco_wait, HPX_NULL, remotes[i], &zero);
    }
  }

  // Wait on all of the lcos sequentially. If the lco is local (i.e., we have a
  // local translation for it) we use the local get operation, otherwise we wait
  // for the completion of the remote proxy.
  int errors = 0;
  for (int i = 0; i < n; ++i) {
    hpx_status_t status = HPX_SUCCESS;
    if (locals[i] != NULL) {
      status = locals[i]->wait(0);
      hpx_gas_unpin(lcos[i]);
    }
    else if (remotes[i] != HPX_NULL) {
      status = hpx_lco_wait(remotes[i]);
      hpx_lco_delete(remotes[i], HPX_NULL);
    }
    else {
      status = HPX_SUCCESS;
    }

    if (status != HPX_SUCCESS) {
      ++errors;
    }

    if (statuses) {
      statuses[i] = status;
    }
  }

  delete [] remotes;
  delete [] locals;
  return errors;
}

int
hpx_lco_get_all(int n, hpx_addr_t lcos[], size_t sizes[], void *values[],
                hpx_status_t statuses[])
{
  dbg_assert(n > 0);

  // Will partition the lcos up into local and remote LCOs. We waste some
  // space here, since, for each lco in lcos, we either have a local mapping or
  // a remote address. We don't use the stack because we can't control how big
  // @p n gets.
  LCO **locals = new LCO*[n]();
  dbg_assert_str(locals, "failed to allocate array for %d elements", n);
  hpx_addr_t *remotes = new hpx_addr_t[n]();
  dbg_assert_str(remotes, "failed to allocate array for %d elements", n);

  // Try and translate (and pin) all of the lcos, for any of the lcos that
  // aren't local, allocate a proxy future and initiate the remote get. This
  // two-phase approach achieves some parallelism.
  for (int i = 0; i < n; ++i) {
    if (lcos[i] == HPX_NULL) {
      locals[i] = NULL;
      remotes[i] = HPX_NULL;
    }
    else if (hpx_gas_try_pin(lcos[i], (void**)&locals[i])) {
      remotes[i] = HPX_NULL;
    }
    else {
      locals[i] = NULL;
      remotes[i] = hpx_lco_future_new(sizes[i]);
      hpx_call_async(lcos[i], _lco_get, HPX_NULL, remotes[i], &sizes[i],
                     sizeof(sizes[i]));
    }
  }

  // Wait on all of the lcos sequentially. If the lco is local (i.e., we have a
  // local translation for it) we use the local get operation, otherwise we wait
  // for the completion of the remote proxy.
  int errors = 0;
  for (int i = 0; i < n; ++i) {
    hpx_status_t status = HPX_SUCCESS;
    if (locals[i] != NULL) {
      status = locals[i]->get(sizes[i], values[i], 0);
      hpx_gas_unpin(lcos[i]);
    }
    else if (remotes[i] != HPX_NULL) {
      status = hpx_lco_get(remotes[i], sizes[i], values[i]);
      hpx_lco_delete(remotes[i], HPX_NULL);
    }
    else {
      status = HPX_SUCCESS;
    }

    if (status != HPX_SUCCESS) {
      ++errors;
    }

    if (statuses) {
      statuses[i] = status;
    }
  }

  delete [] remotes;
  delete [] locals;
  return errors;
}

int
hpx_lco_delete_all(int n, hpx_addr_t *lcos, hpx_addr_t rsync)
{
  hpx_addr_t cand = HPX_NULL;
  if (rsync) {
    cand = hpx_lco_and_new(n);
    int e;
    e = hpx_call_when_with_continuation(cand, rsync, hpx_lco_set_action, cand,
                                        hpx_lco_delete_action, NULL, 0);
    dbg_check(e, "failed to enqueue delete action\n");
  }

  for (int i = 0, e = n; i < e; ++i) {
    if (lcos[i]) {
      hpx_lco_delete(lcos[i], cand);
    }
  }

  return HPX_SUCCESS;
}

// Generic array indexer API.
hpx_addr_t
hpx_lco_array_at(hpx_addr_t array, int i, size_t size)
{
  size_t lco_bytes = hpx_lco_size(array, size);
  return hpx_addr_add(array, i * lco_bytes, lco_bytes);
}
