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
#include "asm.h"
#include "libhpx/parcel.h"
#include <new>

namespace {
using libhpx::scheduler::Thread;

/// A structure describing the initial frame on a stack.
///
/// This must match the transfer.S asm file usage.
///
/// This should be managed in an asm-specific manner, but we are just worried
/// about x86-64 at the moment.
class [[ gnu::packed ]] TransferFrame {
 public:
  TransferFrame(hpx_parcel_t* p, Thread::Entry f, uint32_t mxcsr,
                uint16_t fpucw)
  : mxcsr_(mxcsr),
    fpucw_(fpucw),
    padding_(),
    r15_(),
    r14_(),
    r13_(),
    r12_(p),
    rbx_(f),
    rbp_(&rip_),
    rip_(align_stack_trampoline),
    top_rbp_(),
    top_rip_()
  {
  }

 private:
  uint32_t          mxcsr_;                    //!< 7
  uint16_t          fpucw_;                    //!< 7.5
  uint16_t        padding_;                    //!< 7.75 has to match transfer.S
  void               *r15_;                    //!< 6
  void               *r14_;                    //!< 5
  void               *r13_;                    //!< 4
  const hpx_parcel_t* r12_;                    //!< 3
  void              (*rbx_)(hpx_parcel_t*);    //!< 2
  void               *rbp_;                    //!< 1
  void              (*rip_)(void);             //!< 0
  void           *top_rbp_;
  void          (*top_rip_)(void);
};
}

static inline uint32_t GetMXCSR() {
  uint32_t mxcsr;
  get_mxcsr(&mxcsr);
  return mxcsr;
}

static inline uint16_t GetFPUCW() {
  uint16_t fpucw;
  get_fpucw(&fpucw);
  return fpucw;
}

void
Thread::initTransferFrame(Entry f)
{
  static const uint32_t mxcsr = GetMXCSR();
  static const uint16_t fpucw = GetFPUCW();

  void *addr = top() - sizeof(TransferFrame);
  sp_ = new(addr) TransferFrame(parcel_, f, mxcsr, fpucw);
}

void
Thread::InitArch(Worker *w)
{
}
