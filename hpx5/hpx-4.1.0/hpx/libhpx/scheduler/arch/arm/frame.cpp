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
    : alignment_(),
#ifdef __VFP_FP__
      vfp_alignment_(),
      fpscr_(),
      vfpregs_(),
#endif
      regs_(),
      r5_(p),
      r4_(f),
      lr_(align_stack_trampoline),
      top_r4_(nullptr),
      top_lr_(nullptr)
  {
  }

 private:
  void     *alignment_; // keep 8-byte aligned stack
#ifdef __VFP_FP__
  void *vfp_alignment_;
  void         *fpscr_;
  void       *vfpregs_[16];
#endif
  void          *regs_[6];                      //!< r6-r11
  void            *r5_;                         //!< hold the parcel
  Thread::Entry    r4_;                         //!< hold the entry function
  void           (*lr_)(void);                  //!< return address
  void        *top_r4_;
  void       (*top_lr_)(void);
};
}

void
Thread::initTransferFrame(Entry f)
{
  void *addr = top() - sizeof(TransferFrame);
  sp_ = new(addr) TransferFrame(parcel_, f);
}

void
Thread::InitArch(Worker*)
{
}
