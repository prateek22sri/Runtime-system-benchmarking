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

#ifndef LIBHPX_SCHEDULER_H
#define LIBHPX_SCHEDULER_H

#include "libhpx/Worker.h"
#include "libhpx/util/Aligned.h"
#include "libhpx/libhpx.h"
#include <atomic>
#include <vector>

namespace libhpx {
/// The scheduler class.
///
/// The scheduler class represents the shared-memory state of the entire
/// scheduling process. It serves as a collection of native worker threads, and
/// a network port, and allows them to communicate with each other and the
/// network.
///
/// It is possible to have multiple scheduler instances active within the same
/// memory space---though it is unclear why we would need or want that at this
/// time---and it is theoretically possible to move workers between schedulers
/// by updating the worker's scheduler pointer and the scheduler's worker
/// table, though all of the functionality that is required to make this work is
/// not implemented.
class Scheduler : public libhpx::util::Aligned<HPX_CACHELINE_SIZE> {
 public:
  enum State {
    SHUTDOWN,
    STOP,
    RUN,
  };

  /// Allocate and initialize a scheduler.
  Scheduler(const config_t* cfg);

  /// Finalize and free a scheduler.
  ///
  /// The scheduler must already have been shutdown with
  /// scheduler_shutdown(). Shutting down a scheduler that is active results in
  /// undefined behavior.
  ~Scheduler();

  /// Restart the scheduler.
  ///
  /// This resumes all of the low-level scheduler threads that were
  /// suspended at the end of the previous execution of hpx_run. It will block
  /// until the run epoch is terminated, at which point it will return the
  /// status.
  ///
  /// @param        sched The scheduler to restart.
  /// @param         spmd True if every locality should run the startup action.
  /// @param          act The startup action id.
  /// @param[out]     out Local output slot, if any.
  /// @param            n The number of arguments
  /// @param         args The arguments to the action.
  int start(int spmd, hpx_action_t act, void *out, int n, va_list *args);


  /// Stop scheduling lightweight threads, and return @p code from the
  /// scheduler_stop operation.
  ///
  /// This is exposed in the public scheduler interface because it is used from
  /// the locality_stop handler.
  ///
  /// @todo: If the scheduler was in the global address space then we could skip
  ///        the indirection through the locality, but we don't do that now.
  ///
  /// @param         code The code to return.
  void stop(uint64_t code);

  /// Suspend the scheduler cooperatively.
  ///
  /// External interface to support hpx_exit().
  ///
  /// @param        bytes The number of bytes of output data.
  /// @param          out The output data.
  [[ noreturn ]]
  void exit(size_t bytes, const void *out);

  /// Set the output for the top level process.
  ///
  /// This external interface supports the locality_set_output handler.
  ///
  /// @todo: If the scheduler exposed the top-level process we would use process
  ///        infrastructure for this operation.
  ///
  /// @param        bytes The number of bytes of output data.
  /// @param          out The output data.
  void setOutput(size_t bytes, const void* out);

  int getNextTlsId() {
    return nextTlsId_.fetch_add(1, std::memory_order_acq_rel);
  }

  void addActive() {
    nActive_ += 1;
  }

  void subActive() {
    nActive_ -= 1;
  }

  int getCode() const {
    return code_.load(std::memory_order_relaxed);
  }

  void setCode(int code) {
    code_.store(code, std::memory_order_relaxed);
  }

  State getState() const {
    return state_.load(std::memory_order_acquire);
  }

  void setState(State state) {
    state_.store(state, std::memory_order_release);
  }

  int getNWorkers() const {
    return nWorkers_;
  }

  std::vector<libhpx::Worker*>& getWorkers() {
    return workers_;
  }

  libhpx::Worker* getWorker(int i) {
    assert(0 <= i && i < nWorkers_ && workers_[i]);
    return workers_[i];
  }

  static int SetOutputHandler(const void* value, size_t bytes);
  static int StopHandler();
  static int TerminateSPMDHandler();

  // Thread event callbacks
  static CallbackType begin_callback;
  static CallbackType before_transfer_callback;
  static CallbackType after_transfer_callback;

 private:
  /// This blocks the calling thread until the worker threads shutdown.
  void wait(std::unique_lock<std::mutex>&&);

  /// This only happens at rank 0 and accumulates all of the SPMD termination
  /// messages.
  void terminateSPMD();

  /// Exit a diffuse epoch.
  ///
  /// This is called from the context of a lightweight thread, and needs to
  /// broadcast the stop signal along with the output value.
  ///
  /// @param         size The number of bytes of output data.
  /// @param          out The output data.
  void exitDiffuse(size_t size, const void* out);

  /// Exit a spmd epoch.
  void exitSPMD(size_t size, const void* out);

  std::mutex                      lock_;     //!< lock for running condition
  std::condition_variable      stopped_;     //!< the running condition
  std::atomic<State>             state_;     //!< the run state
  std::atomic<int>           nextTlsId_;     //!< lightweight thread ids
  std::atomic<int>                code_;     //!< the exit code
  std::atomic<int>             nActive_;     //!< active number of workers
  std::atomic<unsigned>      spmdCount_;     //!< barrier count for spmd
  const int                   nWorkers_;     //!< total number of workers
  int                          nTarget_;     //!< target number of workers
  int                            epoch_;     //!< current scheduler epoch
  int                             spmd_;     //!< 1 if the current epoch is spmd
  std::chrono::nanoseconds      nsWait_;     //!< nanoseconds to wait in start()
  void                         *output_;     //!< the output slot
  std::vector<libhpx::Worker*> workers_;     //!< array of worker data
};
} // namespace libhpx
#endif // LIBHPX_SCHEDULER_H
