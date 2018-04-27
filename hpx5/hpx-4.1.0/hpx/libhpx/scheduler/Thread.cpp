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
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include "libhpx/process.h"
#include "libhpx/Scheduler.h"
#include <valgrind/valgrind.h>
#include <sys/mman.h>
#include <errno.h>

namespace {
using libhpx::scheduler::Thread;
}

size_t Thread::Size_;
size_t Thread::Buffer_;

Thread::Thread(hpx_parcel_t* p, Entry f)
    : sp_(nullptr),
      parcel_(p),
      next_(nullptr),
      lco_(nullptr),
      tlsId_(-1),
      continued_(false),
      masked_(false),
      canary_(CANARY_)
{
  initTransferFrame(f);
}

Thread::Thread(hpx_parcel_t* p)
    : sp_(nullptr),
      parcel_(p),
      next_(nullptr),
      lco_(nullptr),
      tlsId_(-1),
      continued_(false),
      masked_(false),
      canary_(CANARY_)
{
}

Thread::~Thread()
{
  assert(canary_ == CANARY_ && "Stack corrupted (try --hpx-dbg-mprotectstacks");
}

void
Thread::ProtectBoundaryPages(void *base, int prot) {
  if (!ProtectStacks()) {
    return;
  }

  if ((uintptr_t)base & (HPX_PAGE_SIZE - 1)) {
    dbg_error("stack must be page aligned for mprotect\n");
  }

  void *end = static_cast<char*>(base) + HPX_PAGE_SIZE + Size_;
  int e1 = mprotect(base, HPX_PAGE_SIZE, prot);
  int e2 = mprotect(end, HPX_PAGE_SIZE, prot);

  if (e1 || e2) {
    dbg_error("Mprotect error: %d (EACCES %d, EINVAL %d, ENOMEM %d)\n", errno,
              EACCES, EINVAL, ENOMEM);
  }
}

/// The operator new is responsible for allocating a thread, protecting its
/// boundary pages, if necessary, and informing valgrind that we're going to
/// context switch to its stack to suppress false positives. This requires some
/// C-style pointer manipulation.
///
/// We're going to store the valgrind stack ID *outside* of the thread structure
/// because we don't want it to be initialized by the constructor, so we use the
/// minimum address of the allocated buffer for that. When we're protecting
/// stacks that minimum is on the write-protected page, but if we're not
/// protecting stacks then we shift the returned buffer by 16 bytes (this is
/// standard allocator-style metadata functionality).
void*
Thread::operator new(size_t bytes)
{
  // Allocate an appropriately sized and aligned buffer;
  size_t align = (ProtectStacks()) ? HPX_PAGE_SIZE : 16;
  size_t n = (ProtectStacks()) ? Buffer_ : Size_;
  void* base = as_memalign(AS_REGISTERED, align, n);
  if (!base) {
    throw std::bad_alloc();
  }

  // Register the stack, storing the valgrind ID at the beginning of the
  // buffer.
  auto* id = static_cast<int*>(base);
  auto* begin = static_cast<char*>(base);
  *id = VALGRIND_STACK_REGISTER(begin, begin + n);

  // Protect the boundary pages.
  ProtectBoundaryPages(base, PROT_NONE);

  // Return the pointer into the correct part of the buffer.
  return begin + align;
}

/// The delete operator gets the pointer that was returned from operator new(),
/// and it needs to unprotect the boundary pages, and deregister the stack with
/// valgrind. The registered stack ID was stored at the base addres of the
/// allocated buffer.
void
Thread::operator delete(void* ptr)
{
  // Adjust the pointer so that it points at the actual base of the buffer.
  size_t align = (ProtectStacks()) ? HPX_PAGE_SIZE : 16;
  ptr = static_cast<char*>(ptr) - align;

  // Unprotect boundary and deregister stack.
  ProtectBoundaryPages(ptr, PROT_READ | PROT_WRITE);
  VALGRIND_STACK_DEREGISTER(*static_cast<int*>(ptr));
  as_free(AS_REGISTERED, ptr);
}

void
Thread::SetStackSize(int bytes)
{
  assert(bytes > 0);
  if (!ProtectStacks()) {
    Size_ = bytes & ~15;
    Buffer_ = bytes & ~15;
  }
  else {
    // allocate boundary pages when we want to protect the stack
    int pages = ceil_div_32(bytes, HPX_PAGE_SIZE);
    Size_ = pages * HPX_PAGE_SIZE;
    Buffer_ = Size_ + 2 * HPX_PAGE_SIZE;
  }

  if (Size_ != unsigned(bytes)) {
    log_sched("Adjusted stack size to %zu bytes\n", Size_);
  }
}

hpx_parcel_t*
Thread::generateContinue(int n, va_list* args)
{
  assert(!continued_);
  continued_ = true;
  hpx_action_t op = 0;
  hpx_addr_t target = 0;
  std::swap(parcel_->c_action, op);
  std::swap(parcel_->c_target, target);
  return action_new_parcel_va(op, target, 0, 0, n, args);
}

hpx_parcel_t*
Thread::captureContinue(const void* data, size_t bytes)
{
  assert(!continued_);
  continued_ = true;
  hpx_parcel_t *p = parcel_new(0, 0, parcel_->c_target, parcel_->c_action,
                               parcel_->pid, data, bytes);
  parcel_->c_action = 0;
  parcel_->c_target = 0;
  return p;
}

void
Thread::invokeContinue(int n, va_list* args)
{
  if (continued_) {
    return;
  }

  continued_ = true;
  if (parcel_->c_action && parcel_->c_target) {
    action_continue_va(parcel_->c_action, parcel_, n, args);
  }
  else {
    process_recover_credit(parcel_);
  }
}

void
Thread::invokeContinue()
{
  invokeContinue(0, nullptr);
}

int
Thread::getTlsId()
{
  if (tlsId_ < 0) {
    tlsId_ = here->sched->getNextTlsId();
  }
  return tlsId_;
}

bool
Thread::ProtectStacks(void)
{
#ifndef ENABLE_DEBUG
  return false;
#endif

#ifdef HAVE_HUGETLBFS
  if (here->config->dbg_mprotectstacks) {
    log_error("cannot mprotect stacks when using huge pages\n");
  }
  return false;
#endif

  dbg_assert(here && here->config);
  return here->config->dbg_mprotectstacks;
}

char*
Thread::top()
{
  // When we are not protecting stacks we used 16 bytes to store the stack ID
  // for valgrind.
  size_t shift = (ProtectStacks()) ? 0 : 16;
  return reinterpret_cast<char*>(this) + Size_ - shift;
}
