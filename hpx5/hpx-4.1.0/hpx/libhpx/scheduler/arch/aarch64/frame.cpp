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
  TransferFrame(hpx_parcel_t* p, Thread::Entry f)
    : x19_(f),
      x20_(p),
      regs_(),
      x29_(),
      x30_(align_stack_trampoline),
      vfp_alignment_(),
      fpscr_(),
      vfpregs_(),
      top_x19_(nullptr),
      top_x30_(nullptr)
  {
  }

 private:
  Thread::Entry   x19_;        //!< used to hold f
  void           *x20_;        //!< the parcel that is passed to f
  void          *regs_[8];     //!< x21-x28
  void           *x29_;        //!< The frame pointer
  void          (*x30_)(void); //!< return address - set to align_stack_trampoline
  void *vfp_alignment_;
  void         *fpscr_;
  void       *vfpregs_[8];
  void       *top_x19_;
  void      (*top_x30_)(void);
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
