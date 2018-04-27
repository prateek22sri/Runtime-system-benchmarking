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

#include "libhpx/Scheduler.h"
#include "Thread.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include "libhpx/Network.h"
#include <cstring>
#ifdef HAVE_APEX
#include <sys/time.h>
#endif

namespace {
using libhpx::Scheduler;
using libhpx::Worker;
using libhpx::scheduler::Thread;
LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, SetOutput,
              Scheduler::SetOutputHandler, HPX_POINTER, HPX_SIZE_T);
LIBHPX_ACTION(HPX_DEFAULT, 0, Stop, Scheduler::StopHandler);
LIBHPX_ACTION(HPX_DEFAULT, 0, TerminateSPMD, Scheduler::TerminateSPMDHandler);
}

Scheduler::Scheduler(const config_t* cfg)
    : lock_(),
      stopped_(),
      state_(STOP),
      nextTlsId_(0),
      code_(HPX_SUCCESS),
      nActive_(cfg->threads),
      spmdCount_(0),
      nWorkers_(cfg->threads),
      nTarget_(cfg->threads),
      epoch_(0),
      spmd_(0),
      nsWait_(cfg->progress_period),
      output_(nullptr),
      workers_(nWorkers_)
{
  Thread::SetStackSize(cfg->stacksize);

  // This thread can allocate even though it's not a scheduler thread.
  as_join(AS_REGISTERED);
  as_join(AS_GLOBAL);
  as_join(AS_CYCLIC);
}

Scheduler::~Scheduler()
{
  for (auto&& w : workers_) {
    if (w) {
      w->shutdown();
      delete w;
    }
  }
  as_leave();
}

int
Scheduler::start(int spmd, hpx_action_t act, void *out, int n, va_list *args)
{
  log_dflt("hpx started running %d\n", epoch_);

  // Create the worker threads for the first epoch.
  if (epoch_ == 0) {
    for (int i = 0, e = nWorkers_; i < e; ++i) {
      workers_[i] = new Worker(i);
    }
  }

  // remember the output slot
  spmd_ = spmd;
  output_ = out;

  if (spmd_ || here->rank == 0) {
    hpx_parcel_t *p = action_new_parcel_va(act, HPX_HERE, 0, 0, n, args);
    parcel_prepare(p);
    workers_[0]->pushMail(p);
  }

  // switch the state and then start all the workers
  setCode(HPX_SUCCESS);
  setState(RUN);
  for (auto&& w : workers_) {
    w->start();
  }

  // wait for someone to stop the scheduler
  {
    std::unique_lock<std::mutex> _(lock_);
    while (getState() == RUN) {
      wait(std::move(_));
    }
  }

  // stop all of the worker threads
  for (auto&& w : workers_) {
    w->stop();
  }

  // Use sched crude barrier to wait for the worker threads to stop.
  while (nActive_.load(std::memory_order_acquire)) {
  }

  // return the exit code
  DEBUG_IF (getCode() != HPX_SUCCESS && here->rank == 0) {
    log_error("hpx_run epoch exited with exit code (%d).\n", getCode());
  }
  log_dflt("hpx stopped running %d\n", epoch_);

  // clear the output slot
  spmd_ = 0;
  output_ = NULL;

  // bump the epoch
  epoch_++;
  return getCode();
}

void
Scheduler::wait(std::unique_lock<std::mutex>&& lock)
{
#ifdef HAVE_APEX
  using std::min;
  using std::max;
  int prev = nTarget_;
  nTarget_ = min(apex_get_thread_cap(), nWorkers_);
  if (prev != nTarget_) {
    log_sched("apex adjusting from %d to %d workers\n", prev, nTarget_);
  }
  for (int i = min(prev, nTarget_), e = max(prev, nTarget_); i < e; ++i) {
    dbg_assert(workers_[i]);
    if (nTarget_ < prev) {
      workers_[i]->stop();
    }
    else {
      workers_[i]->start();
    }
  }
#endif

  stopped_.wait_for(lock, nsWait_);
}

void
Scheduler::setOutput(size_t bytes, const void* value)
{
  if (!bytes) return;
  if (!output_) return;
  std::lock_guard<std::mutex> _(lock_);
  memcpy(output_, value, bytes);
}

void
Scheduler::stop(uint64_t code)
{
  std::lock_guard<std::mutex> _(lock_);
  dbg_assert(code < UINT64_MAX);
  setCode(int(code));
  setState(Scheduler::STOP);
  stopped_.notify_all();
}

void
Scheduler::terminateSPMD()
{
  if (++spmdCount_ != here->ranks) return;
  spmdCount_ = 0;

  hpx_addr_t sync = hpx_lco_and_new(here->ranks - 1);
  for (auto i = 0u, e = here->ranks; i < e; ++i) {
    if (i == here->rank) continue;

    hpx_parcel_t *p = action_new_parcel(Stop, // action
                                        HPX_THERE(i),          // target
                                        0,      // continuation target
                                        0,      // continuation action
                                        0);     // number of args
    hpx_parcel_t *q = action_new_parcel(hpx_lco_set_action, // action
                                        sync,               // target
                                        0,      // continuation target
                                        0,      // continuation action
                                        0);     // number of args

    parcel_prepare(p);
    parcel_prepare(q);
    dbg_check( here->net->send(p, q) );
  }
  dbg_check( hpx_lco_wait(sync) );
  hpx_lco_delete_sync(sync);
  stop(HPX_SUCCESS);
}

/// This is deceptively complex when we have synchronous network progress (i.e.,
/// when the scheduler is responsible for calling network progress from the
/// schedule loop) because we can't stop the scheduler until we are sure that
/// the signal has made it out. We use the network_send operation manually here
/// because it allows us to wait for the `ssync` event (this event means that
/// we're guaranteed that we don't need to keep progressing locally for the send
/// to be seen remotely).
///
/// Don't perform the local shutdown until we're sure all the remote shutdowns
/// have gotten out, otherwise we might not progress the network enough.
void
Scheduler::exitDiffuse(size_t size, const void *out)
{
  hpx_addr_t sync = hpx_lco_and_new(here->ranks - 1);
  for (auto i = 0u, e = here->ranks; i < e; ++i) {
    if (i == here->rank) continue;

    hpx_parcel_t *p = action_new_parcel(SetOutput,    // action
                                        HPX_THERE(i), // target
                                        HPX_THERE(i), // continuation target
                                        Stop,         // continuation action
                                        2,            // number of args
                                        out,          // arg 0
                                        size);        // the 1
    hpx_parcel_t *q = action_new_parcel(hpx_lco_set_action, // action
                                        sync,               // target
                                        0,    // continuation target
                                        0,    // continuation action
                                        0);   // number of args

    parcel_prepare(p);
    parcel_prepare(q);
    dbg_check( here->net->send(p, q) );
  }
  dbg_check( hpx_lco_wait(sync) );
  hpx_lco_delete_sync(sync);

  setOutput(size, out);
  stop(HPX_SUCCESS);
}

void
Scheduler::exitSPMD(size_t size, const void *out)
{
  setOutput(size, out);
  hpx_call(HPX_THERE(0), TerminateSPMD, HPX_NULL);
}

void
Scheduler::exit(size_t size, const void *out)
{
  if (spmd_) {
    exitSPMD(size, out);
  }
  else {
    exitDiffuse(size, out);
  }
  hpx_thread_exit(HPX_SUCCESS);
}

int
Scheduler::SetOutputHandler(const void *out, size_t bytes)
{
  here->sched->setOutput(bytes, out);
  return HPX_SUCCESS;
}

int
Scheduler::StopHandler(void)
{
  here->sched->stop(HPX_SUCCESS);
  return HPX_SUCCESS;
}


int
Scheduler::TerminateSPMDHandler()
{
  here->sched->terminateSPMD();
  return HPX_SUCCESS;
}

CallbackType Scheduler::begin_callback = nullptr;
CallbackType Scheduler::before_transfer_callback = nullptr;
CallbackType Scheduler::after_transfer_callback = nullptr;
