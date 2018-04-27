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

#include "libhpx/Worker.h"
#include "Condition.h"
#include "Thread.h"
#include "lco/LCO.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/events.h"
#include "libhpx/libhpx.h"
#include "libhpx/locality.h"
#include "libhpx/memory.h"
#include "libhpx/Network.h"
#include "libhpx/rebalancer.h"
#include "libhpx/Scheduler.h"
#include "libhpx/Topology.h"
#include "libhpx/system.h"
#include "libhpx/util/math.h"
#include <cstring>
#ifdef HAVE_URCU
# include <urcu-qsbr.h>
#endif

namespace {
using libhpx::self;
using libhpx::Worker;
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;
using libhpx::scheduler::Thread;
LIBHPX_ACTION(HPX_INTERRUPT, 0, StealHalf, Worker::StealHalfHandler,
              HPX_POINTER);
}

/// Storage for the thread-local worker pointer.
__thread Worker * volatile libhpx::self;

Worker::Worker(int id)
    : id_(id),
      numaNode_(here->topology->cpu_to_numa[id % here->topology->ncpus]),
      seed_(id),
      workFirst_(0),
      lastVictim_(nullptr),
      profiler_(nullptr),
      bst(nullptr),
      system_(nullptr),
      current_(nullptr),
      threads_(nullptr),
      lock_(),
      running_(),
      state_(STOP),
      workId_(0),
      queues_(),
      inbox_(),
      thread_([this]() { enter(); })
{
}

Worker::~Worker() {
  thread_.join();
  if (hpx_parcel_t* p = handleMail()) {
    parcel_delete(p);
  }

  while (hpx_parcel_t* p = popLIFO()) {
    parcel_delete(p);
  }

  while (auto* thread = threads_) {
    threads_ = thread->next;
    delete thread;
  }
}

void
Worker::pushLIFO(hpx_parcel_t* p)
{
  dbg_assert(p->target != HPX_NULL);
  dbg_assert(actions[p->action].handler != NULL);
  EVENT_SCHED_PUSH_LIFO(p->id);
#if defined(HAVE_AGAS) && defined(HAVE_REBALANCING)
  rebalancer_add_entry(p->src, here->rank, p->target, p->size);
#elif defined(ENABLE_INSTRUMENTATION)
  EVENT_GAS_ACCESS(p->src, here->rank, p->target, p->size);
#endif
  uint64_t size = queues_[workId_].push(p);
  if (workFirst_ >= 0) {
    workFirst_ = (here->config->sched_wfthreshold < size);
  }
}

hpx_parcel_t*
Worker::popLIFO()
{
  hpx_parcel_t *p = queues_[workId_].pop();
  dbg_assert(!p || p != current_);
  INST_IF (p) {
    EVENT_SCHED_POP_LIFO(p->id);
    EVENT_SCHED_WQSIZE(queues_[workId_].size());
  }
  return p;
}

hpx_parcel_t *
Worker::handleNetwork()
{
  // don't do work first scheduling in the network
  int wf = workFirst_;
  workFirst_ = -1;
  here->net->progress(0);

  hpx_parcel_t *stack = here->net->probe(0);
  workFirst_ = wf;

  while (hpx_parcel_t *p = parcel_stack_pop(&stack)) {
    pushLIFO(p);
  }

  return popLIFO();
}

hpx_parcel_t*
Worker::handleMail()
{
  hpx_parcel_t *parcels = inbox_.dequeue();
  if (!parcels) {
    return NULL;
  }

  hpx_parcel_t *prev = parcel_stack_pop(&parcels);
  do {
    hpx_parcel_t *next = NULL;
    while ((next = parcel_stack_pop(&parcels))) {
      dbg_assert(next != current_);
      EVENT_SCHED_MAIL(prev->id);
      log_sched("got mail %p\n", prev);
      pushLIFO(prev);
      prev = next;
    }
  } while ((parcels = inbox_.dequeue()));
  dbg_assert(prev);
  return prev;
}

void
Worker::bind(hpx_parcel_t *p)
{
  dbg_assert(!p->thread);

  Thread* thread;
  if (void* buffer = threads_) {
    threads_ = threads_->next;
    thread = new(buffer) Thread(p, ExecuteUserThread);
  }
  else {
    thread = new Thread(p, ExecuteUserThread);
  }

  if (Thread* old = parcel_set_thread(p, thread)) {
    dbg_error("Replaced stack %p with %p in %p: cthis usually means two workers "
              "are trying to start a lightweight thread at the same time.\n",
              old, thread, p);
  }
}

void
Worker::unbind(hpx_parcel_t* p)
{
  if (Thread* thread = parcel_set_thread(p, nullptr)) {
    thread->~Thread();
    threads_ = new(thread) FreelistNode(threads_);
  }
  else {
    return;
  }

  const auto limit = here->config->sched_stackcachelimit;
  if (limit < 0 || threads_->depth < limit) {
    return;
  }

  for (auto i = 0, e = util::ceil_div(limit, 2); i < e; ++i) {
    auto* thread = threads_->next;
    delete threads_;
    threads_ = thread;
  }
  assert(!threads_ || threads_->depth == util::ceil_div(limit, 2));
}

void
Worker::schedule(Continuation& f)
{
  EVENT_SCHED_BEGIN();
  if (state_ != RUN) {
    transfer(system_, f);
  }
  else if (hpx_parcel_t *p = handleMail()) {
    transfer(p, f);
  }
  else if (hpx_parcel_t *p = popLIFO()) {
    transfer(p, f);
  }
  else {
    transfer(system_, f);
  }
  EVENT_SCHED_END(0, 0);
}

void
Worker::enter()
{
  EVENT_SCHED_BEGIN();
  dbg_assert(here && here->config && here->gas && here->net);

  if(Scheduler::begin_callback != nullptr) {
      Scheduler::begin_callback();
  }

  self = this;

  // Ensure that all of the threads have joined the address spaces.
  as_join(AS_REGISTERED);
  as_join(AS_GLOBAL);
  as_join(AS_CYCLIC);

#ifdef HAVE_URCU
  // Make ourself visible to urcu.
  rcu_register_thread();
#endif

#ifdef HAVE_APEX
  // let APEX know there is a new thread
  apex_register_thread("HPX WORKER THREAD");
#endif

  // affinitize the worker thread
  libhpx_thread_affinity_t policy = here->config->thread_affinity;
  int status = system_set_worker_affinity(id_, policy);
  if (status != LIBHPX_OK) {
    log_dflt("WARNING: running with no worker thread affinity. "
             "This MAY result in diminished performance.\n");
  }

  // allocate a parcel and a stack header for the system stack
  hpx_parcel_t system;
  parcel_init(0, 0, 0, 0, 0, nullptr, 0, &system);
  Thread thread(&system);
  parcel_set_thread(&system, &thread);

  system_ = &system;
  current_ = &system;

  // At this point (once we have a "current_" pointer we can do any
  // architecture-specific initialization necessary, up to and including calling
  // ContextSwitch.
  Thread::InitArch(this);

  // Hang out here until we're shut down.
  while (state_ != SHUTDOWN) {
    run();                                   // returns when state_ != RUN
    sleep();                                 // returns when state_ != STOP
  }

  system_ = NULL;
  current_ = NULL;

#ifdef HAVE_APEX
  // finish whatever the last thing we were doing was
  if (profiler_) {
    apex_stop(profiler_);
  }
  // let APEX know the thread is exiting
  apex_exit_thread();
#endif

#ifdef HAVE_URCU
  // leave the urcu domain
  rcu_unregister_thread();
#endif

  // leave the global address space
  as_leave();

  EVENT_SCHED_END(0, 0);
}

void
Worker::sleep()
{
  std::unique_lock<std::mutex> _(lock_);
  while (state_ == STOP) {
    while (hpx_parcel_t *p = queues_[1 - workId_].pop()) {
      pushLIFO(p);
    }

    if (hpx_parcel_t *p = handleMail()) {
      pushLIFO(p);
    }

    // go back to sleep
    here->sched->subActive();
    running_.wait(_);
    here->sched->addActive();
  }
}

void
Worker::checkpoint(hpx_parcel_t *p, Continuation& f, void *sp)
{
  if(Scheduler::before_transfer_callback != nullptr) {
      Scheduler::before_transfer_callback();
  }

  current_->thread->setSp(sp);
  std::swap(current_, p);
  f(p);

#ifdef HAVE_URCU
  rcu_quiescent_state();
#endif

  if(Scheduler::after_transfer_callback != nullptr) {
      Scheduler::after_transfer_callback();
  }
}

void
Worker::Checkpoint(hpx_parcel_t* p, Continuation& f, Worker* w, void *sp)
{
  w->checkpoint(p, f, sp);
}

void
Worker::transfer(hpx_parcel_t *p, Continuation& f)
{
  dbg_assert(p != current_);

  if (p->thread == nullptr) {
    bind(p);
  }

  if (unlikely(current_->thread->getMasked())) {
    sigset_t set;
    dbg_check(pthread_sigmask(SIG_SETMASK, &here->mask, &set));
    ContextSwitch(p, f, this);
    dbg_check(pthread_sigmask(SIG_SETMASK, &set, NULL));
  }
  else {
    ContextSwitch(p, f, this);
  }
}

void
Worker::run()
{
  std::function<void(hpx_parcel_t*)> null([](hpx_parcel_t*){});
  while (state_ ==  RUN) {
    if (hpx_parcel_t *p = handleMail()) {
      transfer(p, null);
    }
    else if (hpx_parcel_t *p = popLIFO()) {
      transfer(p, null);
    }
    else if (hpx_parcel_t *p = handleEpoch()) {
      transfer(p, null);
    }
    else if (hpx_parcel_t *p = handleNetwork()) {
      transfer(p, null);
    }
    else if (hpx_parcel_t *p = handleSteal()) {
      transfer(p, null);
    }
    else {
#ifdef HAVE_URCU
      rcu_quiescent_state();
#endif
    }
  }
}

void
Worker::spawn(hpx_parcel_t* p)
{
  dbg_assert(p);
  dbg_assert(actions[p->action].handler != NULL);

  // If the target has affinity then send the parcel to that worker.
  int affinity = here->gas->getAffinity(p->target);
  if (0 <= affinity && affinity != id_) {
    here->sched->getWorker(affinity)->pushMail(p);
    return;
  }

  // If we're not running then push the parcel and return. This prevents an
  // infinite spawn from inhibiting termination.
  if (state_ != RUN) {
    pushLIFO(p);
    return;
  }

  // If we're not in work-first mode, then push the parcel for later.
  if (workFirst_ < 1) {
    pushLIFO(p);
    return;
  }

  // If we're holding a lock then we have to push the spawn for later
  // processing, or we could end up causing a deadlock.
  if (current_->thread->inLCO()) {
    pushLIFO(p);
    return;
  }

  // If we are currently running an interrupt, then we can't work-first since we
  // don't have our own stack to suspend.
  if (action_is_interrupt(current_->action)) {
    pushLIFO(p);
    return;
  }

  // Process p work-first. If we're running the system thread then we need to
  // prevent it from being stolen, which we can do by using the NULL
  // continuation.
  EVENT_THREAD_SUSPEND(current_);
  if (current_ == system_) {
    transfer(p, [](hpx_parcel_t*) {});
  }
  else {
    transfer(p, [this](hpx_parcel_t* p) { pushLIFO(p); });
  }
  self->EVENT_THREAD_RESUME(current_);          // re-read self
}

int
Worker::StealHalfHandler(Worker* src)
{
  if (hpx_parcel_t* half = self->stealHalf()) {
    src->pushMail(half);
  }
  return HPX_SUCCESS;
}

hpx_parcel_t*
Worker::stealFrom(Worker* victim) {
  hpx_parcel_t *p = victim->queues_[victim->workId_].steal();
  lastVictim_ = (p) ? victim : nullptr;
  EVENT_SCHED_STEAL((p) ? p->id : 0, victim->getId());
  return p;
}

hpx_parcel_t*
Worker::stealRandom()
{
  int n = here->sched->getNWorkers();
  int id;
  do {
    id = rand(n);
  } while (id == id_);
  return stealFrom(here->sched->getWorker(id));
}

hpx_parcel_t*
Worker::stealRandomNode()
{
  int   n = here->topology->cpus_per_node;
  int cpu = rand(n);
  int  id = here->topology->numa_to_cpus[numaNode_][cpu];
  while (id == id_) {
    cpu = rand(n);
    id = here->topology->numa_to_cpus[numaNode_][cpu];
  }
  return stealFrom(here->sched->getWorker(id));
}

hpx_parcel_t*
Worker::stealHalf()
{
  int qsize = queues_[workId_].size();
  if (qsize < MAGIC_STEAL_HALF_THRESHOLD) {
    return nullptr;
  }

  hpx_parcel_t *parcels = nullptr;
  for (int i = 0, e = qsize / 2; i < e; ++i) {
    hpx_parcel_t *p = popLIFO();
    if (!p) {
      break;
    }
    if (p->action == StealHalf) {
      parcel_delete(p);
      continue;
    }
    parcel_stack_push(&parcels, p);
  }
  return parcels;
}

/// Hierarchical work-stealing policy.
///
/// This policy is only applicable if the worker threads are
/// pinned. This policy works as follows:
///
/// 1. try to steal from the last succesful victim in
///    the same numa domain.
/// 2. if failed, try to steal randomly from the same numa domain.
/// 3. if failed, repeat step 2.
/// 4. if failed, try to steal half randomly from across the numa domain.
/// 5. if failed, go idle.
///
hpx_parcel_t*
Worker::stealHierarchical()
{
  // disable hierarchical stealing if the worker threads are not
  // bound, or if the system is not hierarchical.
  if (here->config->thread_affinity == HPX_THREAD_AFFINITY_NONE) {
    return stealRandom();
  }

  if (here->topology->numa_to_cpus == NULL) {
    return stealRandom();
  }

  dbg_assert(numaNode_ >= 0);

  // step 1
  if (lastVictim_) {
    if (hpx_parcel_t* p = stealFrom(lastVictim_)) {
      return p;
    }
  }

  // step 2
  if (hpx_parcel_t* p = stealRandomNode()) {
    return p;
  }

  // step 3
  if (hpx_parcel_t* p = stealRandomNode()) {
    return p;
  }

  // step 4
  int nn = numaNode_;
  while (nn == numaNode_) {
    nn = rand(here->topology->nnodes);
  }

  int        idx = rand(here->topology->cpus_per_node);
  int        cpu = here->topology->numa_to_cpus[nn][idx];
  Worker* victim = here->sched->getWorker(cpu);
  Worker*    src = this;
  hpx_parcel_t* p = action_new_parcel(StealHalf, // action
                                      HPX_HERE,  // target
                                      0,         // c_action
                                      0,         // c_taget
                                      1,         // n args
                                      &src);     // reply
  parcel_prepare(p);
  victim->pushMail(p);
  return NULL;
}

hpx_parcel_t*
Worker::handleSteal()
{
  if (here->sched->getNWorkers() == 1) {
    return NULL;
  }

  libhpx_sched_policy_t policy = here->config->sched_policy;
  switch (policy) {
    default:
      log_dflt("invalid scheduling policy, defaulting to random..");
    case HPX_SCHED_POLICY_DEFAULT:
    case HPX_SCHED_POLICY_RANDOM:
     return stealRandom();
    case HPX_SCHED_POLICY_HIER:
     return stealHierarchical();
  }
}

void
Worker::ExecuteUserThread(hpx_parcel_t *p)
{
  Worker* w = self;
  w->EVENT_THREAD_RUN(p);
  EVENT_SCHED_END(0, 0);
  int status = HPX_SUCCESS;
  try {
    status = action_exec_parcel(p->action, p);
  } catch (const int &nonLocal) {
    status = nonLocal;
  }

  // NB: No EVENT_SCHED_BEGIN here. All code paths from this point will reach
  //     _schedule_nb in worker.c and that will begin scheduling
  //     again. Effectively we consider continuation generation as user-level
  //     work.
  switch (status) {
   case HPX_RESEND:
    w = self;
    w->EVENT_THREAD_END(p);
    EVENT_PARCEL_RESEND(w->current_->id, w->current_->action,
                        w->current_->size, w->current_->src);
    w->schedule([w](hpx_parcel_t* p) {
        dbg_assert(w == self);
        w->unbind(p);
        parcel_launch(p);
      });
    unreachable();

   case HPX_ABANDON:                            // like RESEND without relaunch
    w = self;
    w->EVENT_THREAD_END(p);
    // EVENT_PARCEL_STOP(w->current_->id, w->current_->action,
    //                   w->current_->size, w->current_->src);
    w->schedule([w](hpx_parcel_t* p) {
        dbg_assert(w == self);
        w->unbind(p);
      });
    unreachable();

   case HPX_SUCCESS:
    p->thread->invokeContinue();
    w = self;
    w->EVENT_THREAD_END(p);
    w->schedule([w](hpx_parcel_t* p) {
        dbg_assert(w == self);
        w->unbind(p);
        parcel_delete(p);
      });
    unreachable();

   case HPX_LCO_ERROR:
    // rewrite to lco_error and continue the error status
    p->c_action = lco_error;
    _hpx_thread_continue(2, &status, sizeof(status));
    w = self;
    w->EVENT_THREAD_END(p);
    w->schedule([w](hpx_parcel_t* p) {
        dbg_assert(w == self);
        w->unbind(p);
        parcel_delete(p);
      });
    unreachable();

   case HPX_ERROR:
   default:
    dbg_error("thread produced unexpected error %s.\n", hpx_strerror(status));
  }
}

void
Worker::yield()
{
  dbg_assert(action_is_default(current_->action));
  EVENT_SCHED_YIELD();
  EVENT_THREAD_SUSPEND(current_);
  schedule([this](hpx_parcel_t* p) {
      pushYield(p);
    });

  // `this` is volatile across the scheduler call but we can't actually indicate
  // that, so re-read self here
  self->EVENT_THREAD_RESUME(current_);
}

void
Worker::suspend(void (*f)(hpx_parcel_t *, void*), void *env)
{
  hpx_parcel_t* p = current_;
  log_sched("suspending %p in %s\n", p, actions[p->action].key);
  EVENT_THREAD_SUSPEND(p);
  schedule(std::bind(f, std::placeholders::_1, env));

  // `this` is volatile across the scheduler call but we can't actually indicate
  // that, so re-read self here
  self->EVENT_THREAD_RESUME(p);
  log_sched("resuming %p in %s\n", p, actions[p->action].key);
}

hpx_status_t
Worker::wait(LCO& lco, Condition& cond)
{
  hpx_parcel_t* p = current_;
  // we had better be holding a lock here
  dbg_assert(p->thread->inLCO());

  if (hpx_status_t status = cond.push(p)) {
    return status;
  }

  EVENT_THREAD_SUSPEND(p);
  schedule([&lco](hpx_parcel_t* p) {
      lco.unlock(p);
    });

  // `this` is volatile across schedule
  self->EVENT_THREAD_RESUME(p);
  lco.lock(p);
  return cond.getError();
}

Worker::FreelistNode::FreelistNode(FreelistNode* n)
    : next(n),
      depth((n) ? n->depth + 1 : 1)
{
}

void
Worker::FreelistNode::operator delete(void* obj)
{
  Thread::operator delete(obj);
}
