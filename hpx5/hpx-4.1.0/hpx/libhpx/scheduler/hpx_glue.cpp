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

#include "hpx/hpx.h"
#include "Thread.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/parcel.h"
#include "libhpx/Scheduler.h"
#include "libhpx/Worker.h"
#include <signal.h>

namespace {
using libhpx::self;
using libhpx::Worker;
using libhpx::scheduler::Thread;
}

hpx_parcel_t *
_hpx_thread_generate_continuation(int n, ...)
{
  va_list args;
  va_start(args, n);
  hpx_parcel_t *p = self->getCurrentParcel();
  hpx_parcel_t* c = p->thread->generateContinue(n, &args);
  va_end(args);
  return c;
}

hpx_parcel_t *
hpx_thread_capture_continuation(const void* args, size_t bytes)
{
  hpx_parcel_t *p = self->getCurrentParcel();
  return p->thread->captureContinue(args, bytes);
}

void
hpx_exit(size_t size, const void *out)
{
  assert(here && self);
  here->sched->exit(size, out);
}

void
hpx_thread_yield(void)
{
  self->yield();
}

int
hpx_get_my_thread_id(void)
{
  assert(self && "hpx not active on current pthread");
  return self->getId();
}


const hpx_parcel_t*
hpx_thread_current_parcel(void)
{
  assert(self && "hpx not active on current pthread");
  return self->getCurrentParcel();
}

hpx_addr_t
hpx_thread_current_target(void)
{
  assert(self && "hpx not active on current pthread");
  return self->getCurrentParcel()->target;
}

hpx_addr_t
hpx_thread_current_cont_target(void)
{
  assert(self && "hpx not active on current pthread");
  return self->getCurrentParcel()->c_target;
}

hpx_action_t
hpx_thread_current_action(void)
{
  assert(self && "hpx not active on current pthread");
  return self->getCurrentParcel()->action;
}

hpx_action_t
hpx_thread_current_cont_action(void)
{
  assert(self && "hpx not active on current pthread");
  return self->getCurrentParcel()->c_action;
}

hpx_pid_t
hpx_thread_current_pid(void)
{
  if (auto w = self) return w->getCurrentParcel()->pid;
  else return 0;
}

uint32_t
hpx_thread_current_credit(void)
{
  assert(self && "hpx not active on current pthread");
  return self->getCurrentParcel()->credit;
}

int
hpx_is_active(void)
{
  assert(here && "hpx not yet initialized");
  return (here->sched->getState() == Scheduler::RUN);
}

void
hpx_thread_exit(int status)
{
  throw status;
}

int
hpx_thread_get_tls_id(void)
{
  assert(self && "hpx not active on current pthread");
  Thread *thread = self->getCurrentParcel()->thread;
  return thread->getTlsId();
}

intptr_t
hpx_thread_can_alloca(size_t bytes)
{
  assert(self && "hpx not active on current pthread");
  // The local "p" variable gets a stack location that we use as a proxy for the
  // current stack address, and we know that the ustack->stack address is the
  // lowest address in the stack.
  hpx_parcel_t* p = self->getCurrentParcel();
  return p->thread->canAlloca(bytes);
}

int
hpx_thread_sigmask(int how, int mask)
{
  assert(self && "hpx not active on current pthread");
  Worker *w = self;
  hpx_parcel_t *p = w->getCurrentParcel();
  p->thread->setMasked();

  sigset_t set;
  sigemptyset(&set);
  if (mask & HPX_SIGSEGV) {
    sigaddset(&set, SIGSEGV);
  }

  if (mask & HPX_SIGABRT) {
    sigaddset(&set, SIGABRT);
  }

  if (mask & HPX_SIGFPE) {
    sigaddset(&set, SIGFPE);
  }

  if (mask & HPX_SIGILL) {
    sigaddset(&set, SIGILL);
  }

  if (mask & HPX_SIGBUS) {
    sigaddset(&set, SIGBUS);
  }

  if (mask & HPX_SIGIOT) {
    sigaddset(&set, SIGIOT);
  }

  if (mask & HPX_SIGSYS) {
    sigaddset(&set, SIGSYS);
  }

  if (mask & HPX_SIGTRAP) {
    sigaddset(&set, SIGTRAP);
  }

  mask = HPX_SIGNONE;

  if (how == HPX_SIG_BLOCK) {
    how = SIG_BLOCK;
  }

  if (how == HPX_SIG_UNBLOCK) {
    how = SIG_UNBLOCK;
  }

  if (how == HPX_SIG_SET) {
    how = SIG_SETMASK;
  }

  dbg_check(pthread_sigmask(how, &set, &set));

  if (sigismember(&set, SIGSEGV)) {
    mask |= HPX_SIGSEGV;
  }

  if (sigismember(&set, SIGABRT)) {
    mask |= HPX_SIGABRT;
  }

  if (sigismember(&set, SIGFPE)) {
    mask |= HPX_SIGFPE;
  }

  if (sigismember(&set, SIGILL)) {
    mask |= HPX_SIGILL;
  }

  if (sigismember(&set, SIGBUS)) {
    mask |= HPX_SIGBUS;
  }

  if (sigismember(&set, SIGIOT)) {
    mask |= HPX_SIGIOT;
  }

  if (sigismember(&set, SIGSYS)) {
    mask |= HPX_SIGSYS;
  }

  if (sigismember(&set, SIGTRAP)) {
    mask |= HPX_SIGTRAP;
  }

  return mask;
}

int
_hpx_thread_continue(int n, ...)
{
  Worker *w = self;
  hpx_parcel_t *p = w->getCurrentParcel();

  va_list args;
  va_start(args, n);
  p->thread->invokeContinue(n, &args);
  va_end(args);

  return HPX_SUCCESS;
}

int
libhpx_register_begin_callback(CallbackType callback)
{
    Scheduler::begin_callback = callback;
    return HPX_SUCCESS;
}

int
libhpx_register_before_transfer_callback(CallbackType callback)
{
    Scheduler::before_transfer_callback = callback;
    return HPX_SUCCESS;
}

int
libhpx_register_after_transfer_callback(CallbackType callback)
{
    Scheduler::after_transfer_callback = callback;
    return HPX_SUCCESS;
}

