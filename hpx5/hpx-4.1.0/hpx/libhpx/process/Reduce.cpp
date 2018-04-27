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

#include "Reduce.h"
#include "libhpx/debug.h"
#include "libhpx/locality.h"
#include "libhpx/padding.h"
#include "libhpx/Scheduler.h"
#include "libhpx/Worker.h"
#include <stdlib.h>

namespace {
using libhpx::process::Reduce;
using libhpx::util::PadToCacheline;

constexpr auto ACQUIRE = std::memory_order_acquire;
constexpr auto RELEASE = std::memory_order_release;
constexpr auto ACQ_REL = std::memory_order_acq_rel;
}

void*
Reduce::operator new(size_t n, size_t bytes)
{
  dbg_assert(n == sizeof(Reduce));

  int workers = here->sched->getNWorkers();
  size_t padded = PadToCacheline(bytes) + bytes;
  size_t size = n + padded * workers;

  void* ptr;
  if (posix_memalign(&ptr, HPX_CACHELINE_SIZE, size)) {
    throw std::bad_alloc();
  }
  return ptr;
}

void
Reduce::operator delete(void* ptr)
{
  if (ptr) {
    free(ptr);
  }
}

Reduce::Reduce(size_t bytes, hpx_monoid_id_t id, hpx_monoid_op_t op)
    : bytes_(bytes),
      padded_(PadToCacheline(bytes) + bytes),
      id_(id),
      op_(op),
      slots_(here->sched->getNWorkers()),
      i_(),
      n_()
{
  for (int i = 0, e = slots_; i < e; ++i) {
    id_(values_ + i * padded_, bytes_);
  }
}

Reduce::~Reduce()
{
}

bool
Reduce::add()
{
  dbg_assert(i_.load(ACQUIRE) == n_);
  i_.store(++n_, RELEASE);
  return (n_ == 1);
}

bool
Reduce::remove()
{
  dbg_assert(i_.load(ACQUIRE) == n_);
  i_.store(--n_, RELEASE);
  return (n_ == 0);
}

bool
Reduce::join(const void *in)
{
  int id = libhpx::self->getId();
  op_(values_ + id * padded_, in, bytes_);
  return (i_.fetch_sub(1, ACQ_REL) - 1 == 0);
}

void
Reduce::reset(void *out)
{
  id_(out, bytes_);
  for (int i = 0, e = slots_; i < e; ++i) {
    char* slot = values_ + i * padded_;
    op_(out, slot, bytes_);
    id_(slot, bytes_);
  }
  i_.store(n_, RELEASE);
}
