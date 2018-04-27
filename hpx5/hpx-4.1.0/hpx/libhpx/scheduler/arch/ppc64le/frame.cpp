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

#include "Thread.h"
#include "../common/asm.h"
#include "libhpx/parcel.h"
#include <new>

namespace {
using libhpx::scheduler::Thread;

/// A structure describing the initial frame on a stack.
///
/// This must match the transfer.S asm file usage.
///
/// This should be managed in an asm-specific manner.
class [[ gnu::packed ]] TransferFrame
{
 public:
  TransferFrame(hpx_parcel_t*p, Thread::Entry f)
    : lr_(align_stack_trampoline),
      cr_(),
      r2_(init_r2_),
      r3_(),
      r14_(f),
      r15_(p),
      regs_{nullptr},
      VFRegs_{nullptr},
      top_r14_(nullptr),
      top_lr_(nullptr)
  {
  }

  static void
  setInitR2(const void* value) {
    init_r2_ = value;
  }

 private:
  void     (*lr_)(void);         //!< return address
  void             *cr_;         //! CR2-CR4 are non volatile registers
  const void       *r2_;         //!< TOC Pointer
  void             *r3_;
  Thread::Entry    r14_;         //!< Function
  void            *r15_;         //!> the parcel that is passed to f
  void       *regs_[16];         //!< r16-r31
  void     *VFRegs_[18];         //!< fpr14-fpr31
  void        *top_r14_;
  void (*top_lr_)(void);

  static const void* init_r2_;
};
const void* TransferFrame::init_r2_ = nullptr;
}

void
Thread::initTransferFrame(Entry f)
{
  void *addr = top() - sizeof(TransferFrame) - 32;
  sp_ = new(addr) TransferFrame(parcel_, f);
}

void
Thread::InitArch(Worker *w)
{
  // We need to extract a value to use for the table-of-contents register for
  // the thread_transfer frame that we spoof. We can do this by performing a
  // "fake" context switch using the initial thread, and capturing the value of
  // r2 relative to the stack frame for thread_transfer, which is the value that
  // gets checkpointed in the "From" parcel during the context switch.
  //
  // We borrow the system parcel/thread in order to spoof this context switch
  // and just make sure to restore its previous stack pointer before returning.
  //
  // This *doesn't actually* context switch because we carefully control the
  // stack pointer we are switching to in order not to change it during the
  // transfer function, which means these are just "normal" function calls that
  // happen to checkpoint the stuff we need in the stack frame.
  void *sp = w->current_->thread->getSp();

  // The constant here is experimentally determined and corresponds to the value
  // of r7 that we need at line 109 in transfer.S so that the instruction
  // becomes a noop.

  #if defined(__OPTIMIZED_SIZE__) || defined(__OPTIMIZE__)
     /* __OPTIMIZED_SIZE__: -Os set */
     /* __OPTIMIZE__: -O1, -O2 or -O3 set */
     w->current_->thread->setSp(&sp - 50);
  #else
     w->current_->thread->setSp(&sp - 47);
  #endif

  // This continuation captures the value of r2 that was stored on line 56 of
  // transfer.S (the "checkpointed" sp is the value of r1 from thread_transfer).
  std::function<void(hpx_parcel_t*)> c([](hpx_parcel_t *p) {
      TransferFrame::setInitR2(p->thread->getSp()[2]);
    });

  // Perform the "fake" context switch to get the correct TOC stored on this
  // stack so that the continuation above can read it.
  libhpx::Worker::ContextSwitch(w->current_, c, w);

  // Restore whatever value was in the system thread's sp previously.
  w->current_->thread->setSp(sp);
}
