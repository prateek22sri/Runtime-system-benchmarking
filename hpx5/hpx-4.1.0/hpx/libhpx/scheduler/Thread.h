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

#ifndef LIBHPX_THREAD_H
#define LIBHPX_THREAD_H

/// @file thread.h
/// @brief Defines the lightweight thread stack structure and interface for user
///        level threads.
#include "libhpx/parcel.h"
#include "libhpx/Worker.h"
#include <functional>

namespace libhpx {
namespace scheduler {
class LCO;
class Thread {
 public:
  using Entry = void (*)(hpx_parcel_t*);

  /// Create a thread.
  ///
  /// The thread can be transferred to using thread_transfer() in order to start
  /// execution. The @p entry function will be run as a result of the initial
  /// transfer.
  ///
  /// @param       parcel The parcel that is generating this thread.
  /// @param            f The entry function for the thread.
  Thread(hpx_parcel_t* p, Entry f);

  /// Create a thread.
  ///
  /// This constructor creates a thread without creating a transfer frame. It
  /// can be used for stack allocation of threads.
  Thread(hpx_parcel_t* p);

  /// Destroy a thread.
  ~Thread();

  static void* operator new(size_t bytes);
  static void* operator new(size_t bytes, void* addr) { return addr; }
  static void operator delete(void* ptr);

  void setSp(void *sp) {
    sp_ = sp;
  }

  const void** getSp() const {
    return reinterpret_cast<const void**>(sp_);
  }

  intptr_t canAlloca(size_t bytes) {
    return (char*)&bytes - stack_ - bytes;
  }

  void setMasked() {
    masked_ = true;
  }

  bool getMasked() const {
    return masked_;
  }

  Thread* getNext() {
    return next_;
  }

  int getTlsId();

  void setNext(Thread* thread) {
    assert(!next_);
    next_ = thread;
  }

  void enterLCO(const LCO* lco) {
    assert(lco_ == nullptr);
    lco_ = lco;
  }

  void leaveLCO(const LCO* lco) {
    assert(lco_ == lco);
    lco_ = nullptr;
  }

  bool inLCO() const {
    return (lco_ != nullptr);
  }

  /// Generate a parcel for the thread's continuation without sending it.
  hpx_parcel_t* generateContinue(int n, va_list* args);

  /// Capture the current continuation as a new parcel without sending it.
  hpx_parcel_t* captureContinue(const void* args, size_t bytes);

  /// Invoking a thread's continuation.
  ///
  /// @param       thread The thread.
  /// @param            n The number of arguments to continue.
  /// @param         args The arguments we are continuing.
  void invokeContinue(int n, va_list* args);
  void invokeContinue();

  /// Sets the protection for the bottom and top pages (uses Size_).
  ///
  /// @param         base The base address.
  /// @param         prot The new permissions.
  static void ProtectBoundaryPages(void* base, int prot);

  /// Sets the size of a stack.
  ///
  /// All of the stacks in the system need to have the same size.
  static void SetStackSize(int bytes);

  /// Do any architecture-specific initialization for the worker.
  static void InitArch(Worker*);

 private:
  /// Get the top address in the stack.
  char* top();

  /// Architecture-specific transfer frame initialization.
  ///
  /// Each architecture will provide its own functionality for initialing a
  /// stack stack frame, typically as an asm file. The result is a thread that we
  /// can start running through  thread_transfer.
  ///
  /// @param            f The initial function to run after the transfer.
  ///
  /// @return             The stack address to use during the first transfer.
  void initTransferFrame(Thread::Entry f);

  /// Determine if we're supposed to be protecting the stack.
  static bool ProtectStacks(void);

 private:
  static constexpr unsigned CANARY_ = 0xA55AA55A;
  static size_t Size_;                          //!< The size of stacks.
  static size_t Buffer_;                        //!< The size of buffers.

  void* sp_;                     //!< checkpointed stack pointer
  hpx_parcel_t* parcel_;         //!< the progenitor parcel
  Thread* next_;                 //!< intrusive list for freelist and Conditions
  const LCO* lco_;               //!< which LCO is running
  int tlsId_;                    //!< backs tls
  bool continued_;               //!< the continuation flag
  bool masked_;                  //!< should we checkpoint sigmask
  const unsigned canary_;        //!< a bitpattern we can check for overflow
  char stack_[];
};
} // namespace scheduler
} // namespace libhpx

#endif  // LIBHPX_THREAD_H
