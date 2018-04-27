// ==================================================================-*- C++ -*-
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

#ifndef LIBHPX_WORKER_H
#define LIBHPX_WORKER_H

#include "libhpx/Network.h"
#include "libhpx/util/Aligned.h"
#include "libhpx/util/ChaseLevDeque.h"
#include "libhpx/util/TwoLockQueue.h"
#include "hpx/hpx.h"
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <functional>

#if defined(__APPLE__)
# define STRINGIFY(S) #S
# define SYMBOL(S) STRINGIFY(_##S)
#else
# define SYMBOL(S) #S
#endif

namespace libhpx {

namespace scheduler {
class Condition;
class LCO;
class Thread;
}

class Worker : public libhpx::util::Aligned<HPX_CACHELINE_SIZE>
{
  static constexpr int MAGIC_STEAL_HALF_THRESHOLD = 6;

  enum State {
    SHUTDOWN,
    RUN,
    STOP
  };
  using Thread = libhpx::scheduler::Thread;

 public:
  using Continuation = std::function<void(hpx_parcel_t*)>;
  using Mailbox = libhpx::util::TwoLockQueue<hpx_parcel_t*>;
  using Deque = libhpx::util::ChaseLevDeque<hpx_parcel_t*>;

  /// Event handlers.
  ///
  /// These event handlers are used during instrumentation to keep track of
  /// events that are occurring on specific threads. When we're not using
  /// instrumentation they're just defined as empty inline functions
  ///
  /// @{
#ifdef ENABLE_INSTRUMENTATION
  void EVENT_THREAD_RUN(struct hpx_parcel *p);
  void EVENT_THREAD_END(struct hpx_parcel *p);
  void EVENT_THREAD_SUSPEND(struct hpx_parcel *p);
  void EVENT_THREAD_RESUME(struct hpx_parcel *p);
#else
  void EVENT_THREAD_RUN(struct hpx_parcel *p) {}
  void EVENT_THREAD_END(struct hpx_parcel *p) {}
  void EVENT_THREAD_SUSPEND(struct hpx_parcel *p) {}
  void EVENT_THREAD_RESUME(struct hpx_parcel *p) {}
#endif
  /// @}

  /// Initialize a worker structure.
  ///
  /// This initializes a worker.
  ///
  /// @param      sched The scheduler associated with this worker.
  /// @param         id The worker's id.
  Worker(int id);

  /// Finalize a worker structure.
  ///
  /// This will cleanup any queues and free any stacks and parcels associated with
  /// the worker. This should only be called once *all* of the workers have been
  /// joined so that an _in-flight_ mail message doesn't get missed.
  ~Worker();

  /// Spawn a new lightweight thread.
  ///
  /// This is unsynchronized and only safe when self == this.
  void spawn(hpx_parcel_t* p);

  /// Yield the current user-level thread.
  ///
  /// This triggers a scheduling event, and possibly selects a new user-level
  /// thread to run. If a new thread is selected, this moves the thread into the
  /// next local epoch, and also makes the thread available to be stolen within
  /// the locality.
  void yield();

  /// Suspend the execution of the current thread.
  ///
  /// This suspends the execution of the current lightweight thread. It must be
  /// explicitly resumed in the future. The continuation @p f(p, @p env) is run
  /// synchronously after the thread has been suspended but before a new thread
  /// is run. This allows the lightweight thread to perform "safe"
  /// synchronization where @p f will trigger a resume operation on the current
  /// thread, and we don't want to miss that signal or possibly have our thread
  /// stolen before it is checkpointed. The runtime passes the parcel we
  /// transferred away from to @p as the first parameter.
  ///
  /// This will find a new thread to execute, and will effect a context
  /// switch. It is not possible to immediately switch back to the current
  /// thread, even if @p f makes it runnable, because thread selection is
  /// performed prior to the execution of @p f, and the current thread is not a
  /// valid option at that point.
  ///
  /// The continuation @p f, and the environment @p env, can be on the stack.
  ///
  /// @param            f A continuation to run after the context switch.
  /// @param          env The environment to pass to the continuation @p f.
  void suspend(void (*f)(hpx_parcel_t *, void*), void *env);

  /// Wait for a condition.
  ///
  /// This suspends execution of the current user level thread until the condition
  /// is signaled. The calling thread must hold the lock. This releases the lock
  /// during the wait, but reacquires it before the user-level thread returns.
  ///
  /// scheduler_wait() will call _schedule() and transfer away from the calling
  /// thread.
  ///
  /// @param          lco The LCO that is executing.
  /// @param         cond The condition to wait for.
  ///
  /// @returns            LIBHPX_OK or an error
  hpx_status_t wait(scheduler::LCO& lco, scheduler::Condition& cond);

  /// Create a random integer bounded by @p mod.
  ///
  /// @todo: now that we are using C++11 we should switch to standard random
  /// number generation.
  ///
  /// @param        mod The bound for the random number.
  ///
  /// @returns          A random number in the range [0, @p mod).
  int rand(int mod) {
    return rand_r(&seed_) % mod;
  }

  int getId() const {
    return id_;
  }

  hpx_parcel_t* getCurrentParcel() const {
    return current_;
  }

  /// Stop processing lightweight threads.
  ///
  /// This will cause the worker to drop into its sleep() loop the next time a
  /// context switch happens, where it will wait for the start() operation.
  void stop() {
    std::lock_guard<std::mutex> _(lock_);
    state_ = STOP;
  }

  /// Start processing lightweight threads.
  ///
  /// This will notify the worker that it should start running lightweight
  /// threads.
  void start() {
    std::lock_guard<std::mutex> _(lock_);
    state_ = RUN;
    running_.notify_all();
  }

  void shutdown() {
    std::lock_guard<std::mutex> _(lock_);
    state_ = SHUTDOWN;
    running_.notify_all();
  }

  void pushMail(hpx_parcel_t* p) {
    inbox_.enqueue(p);
  }

  void pushYield(hpx_parcel_t* p) {
    queues_[1 - workId_].push(p);
  }

  /// The non-blocking schedule operation.
  ///
  /// This will schedule new work relatively quickly, in order to avoid delaying
  /// the execution of the user's continuation. If there is no local work we can
  /// find quickly we'll transfer back to the main pthread stack and go through an
  /// extended transfer time.
  ///
  /// @param          f The continuation function.
  void schedule(Continuation& f);

  template <typename Lambda>
  void schedule(Lambda&& l) {
    Continuation f(std::forward<Lambda>(l));
    schedule(f);
  }

  /// Asynchronous entry point for the steal-half operation.
  ///
  /// @todo This is public because we register it with the LIBHPX_ACTION
  ///       macro. We should increase its protection if possible.
  ///
  /// @param        src The source of the steal request.
  static int StealHalfHandler(Worker* src);

 private:
  /// This node structure is used to freelist threads.
  struct FreelistNode {
    /// Push a new freelist node onto a stack.
    FreelistNode(FreelistNode* n);

    /// This will just forward to the Thread::operator delete().
    static void operator delete(void* obj);

    FreelistNode* next;
    const int depth;
  };

  /// Process a mail queue.
  ///
  /// This processes all of the parcels in the mailbox of the worker, moving them
  /// into the work queue of the designated worker. It will return a parcel if
  /// there was one.
  ///
  /// @returns          A parcel from the mailbox if there is one.
  hpx_parcel_t* handleMail();

  /// Handle anything we need to do between epochs.
  hpx_parcel_t* handleEpoch() {
    workId_ = 1 - workId_;
    return nullptr;
  }

  /// Handle the network.
  ///
  /// This will return a parcel from the network if it finds any. It will also
  /// refill the local work queue.
  ///
  /// @returns          A parcel from the network if there is one.
  hpx_parcel_t* handleNetwork();

  hpx_parcel_t* handleSteal();

  /// Pop the next available parcel from our lifo work queue.
  hpx_parcel_t* popLIFO();

  /// Push a parcel into the lifo queue.
  void pushLIFO(hpx_parcel_t *p);

  /// All of the steal functionality.
  ///
  /// @todo We should extract stealing policies into a policy class that is
  ///       initialized based on the runtime configuration.
  /// @{
  hpx_parcel_t* stealFrom(Worker* victim);
  hpx_parcel_t* stealHalf();
  hpx_parcel_t* stealRandom();
  hpx_parcel_t* stealRandomNode();
  hpx_parcel_t* stealHierarchical();
  /// @}

  /// The main entry point for the worker thread.
  void enter();

  /// The primary schedule loop.
  ///
  /// This will continue to try and schedule lightweight threads while the
  /// worker's state is SCHED_RUN.
  void run();

  /// The sleep loop.
  ///
  /// This will continue to sleep the scheduler until the worker's state is no
  /// longer SCHED_STOP.
  void sleep();

  /// Try to bind a stack to the parcel.
  ///
  /// This uses the worker's stack caching infrastructure to find a stack, or
  /// allocates a new stack if necessary. The newly created thread is runnable,
  /// and can be thread_transfer()ed to in the same way as any other lightweight
  /// thread can be. Calling bind() on a parcel that already has a stack
  /// (i.e., a thread) is permissible and has no effect.
  ///
  /// @param          p The parcel to which we are binding.
  void bind(hpx_parcel_t* p);

  /// This returns the parcel's stack to the stack cache.
  ///
  /// This will push the parcel's stack onto the local worker's freelist, and
  /// possibly trigger a freelist flush if there are too many parcels cached
  /// locally.
  ///
  /// @param          p The parcel that has the stack that we are freeing.
  void unbind(hpx_parcel_t* p);

  /// The basic checkpoint continuation used by the worker.
  ///
  /// This transfer continuation updates the current_ parcel to record that we
  /// are now running @p p, checkpoints the previous stack pointer in the
  /// previous stack, and then runs the continuation encapsulated in @p f.
  ///
  /// @param          p The parcel we transferred to.
  /// @param          f A function to call as a continuation.
  /// @param         sp The stack pointer we transferred from.
  void checkpoint(hpx_parcel_t* p, Continuation& f, void *sp);

  /// Used inside of the thread transfer ASM in order to call the
  /// Worker::checkpoint member.
  ///
  /// We use a static member because it is called from asm and I don't know how
  /// to call a C++ member function from asm code. I force a specific asm label
  /// so that I don't have to deal a mangled name. This just forwards to
  /// w->checkpoint(p, f, sp), but it is outlined to make sure it is included in
  /// the binary.
  ///
  /// @param          p The parcel we transferred to.
  /// @param          f A function to call as a continuation.
  /// @param          w The worker structure.
  /// @param         sp The stack pointer we transferred from.
  static void Checkpoint(hpx_parcel_t* p, Continuation& f, Worker* w, void *sp)
    asm(SYMBOL(worker_checkpoint));

  /// This performs the context switch.
  ///
  /// This will checkpoint the current thread's state, and then switch stacks to
  /// the stack associated with p, and then call the continuation @p f. This is
  /// an architecture-specific asm function. It is static because I do not want
  /// to know how all of the different architecture ABIs deal with member
  /// function calls (i.e., which register `this` is in, vs. the arguments).
  ///
  /// I give this an explicit asm label so that I do not need to understand C++
  /// name mangling.
  ///
  /// @param          p The parcel we transferred to.
  /// @param          f A function to call as a continuation.
  /// @param          w The worker structure.
  static void ContextSwitch(hpx_parcel_t* p, Continuation& f, Worker* w)
    asm(SYMBOL(thread_transfer));

  /// The thread transfer call.
  ///
  /// This will handle any transfer-time details (e.g., reset the signal mask)
  /// and then forward to the context switch operation.
  ///
  /// @param          p The parcel to transfer to.
  /// @param          f The checkpoint continuation.
  void transfer(hpx_parcel_t* p, Continuation& f);

  /// A convenience wrapper for the transfer() operation.
  ///
  /// This version of transfer allows the continuation to be specified as any
  /// lambda function that is compatible with the Continuation type.
  ///
  /// @param          p The parcel to transfer to.
  /// @param     lambda A lambda function to run as a continuation.
  template <typename Lambda>
  void transfer(hpx_parcel_t* p, Lambda&& lambda) {
    Continuation f(std::forward<Lambda>(lambda));
    transfer(p, f);
  }

  /// The main entry point for worker threads.
  ///
  /// Just used through the pthread interface during create to bounce to the
  /// worker's run member.
  ///
  /// @param     worker The initialized worker structure for this thread.
  /// @returns          nullptr
  static void* Enter(void *worker) {
    static_cast<Worker*>(worker)->enter();
    return nullptr;
  }

  /// The entry function that the worker uses to start a lightweight thread.
  ///
  /// This is the function that sits at the outermost stack frame for a
  /// lightweight thread, and deals with dispatching the parcel's action and
  /// handling the action's return value.
  ///
  /// It is not a member function because the lightweight thread can context
  /// switch and thus the `this` pointer can change asynchronously. I (a) don't
  /// know how to indicate that the `this` pointer is volatile and (b) even if I
  /// knew how I don't really want to reload `this` every time I need it.
  ///
  /// It does not return.
  ///
  /// @param          p The parcel to execute.
  [[ noreturn ]]
  static void ExecuteUserThread(hpx_parcel_t *p);

 private:
  const int                    id_;             //!< this worker's id
  const int              numaNode_;             //!< this worker's numa node
  unsigned                   seed_;             //!< my random seed
  int                   workFirst_;             //!< this worker's mode
  Worker*              lastVictim_;             //!< last successful victim
  void                  *profiler_;             //!< reference to the profiler
 public:
  void                        *bst;            //!< the block statistics table
 private:
  hpx_parcel_t            *system_;             //!< this worker's native parcel
  hpx_parcel_t           *current_;             //!< current thread
  FreelistNode           *threads_;             //!< freelisted threads
  alignas(HPX_CACHELINE_SIZE)
  std::mutex                 lock_;             //!< state lock
  std::condition_variable running_;             //!< local condition for sleep
  std::atomic<State>        state_;             //!< what state are we in
  std::atomic<int>         workId_;             //!< which queue are we using
  Deque                    queues_[2];          //!< work and yield queues
  Mailbox                   inbox_;             //!< mail sent to me
  std::thread              thread_;             //!< this worker's native thread

  // Allow the thread class to call ContextSwitch directly.
  friend class libhpx::scheduler::Thread;
};


/// NB: The use of volatile in the following declaration may not achieve the
/// desired effect on some compilers/architectures because the address
/// of self may be cached. See
/// http://stackoverflow.com/questions/25673787/making-thread-local-variables-fully-volatile#
/// for more details.
extern __thread Worker * volatile self;

} // namespace libhpx

#endif // LIBHPX_WORKER_H
