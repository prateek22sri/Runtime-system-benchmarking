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
#include "config.h"
#endif

/// ----------------------------------------------------------------------------
/// @file libhpx/scheduler/condition.c
/// ----------------------------------------------------------------------------

#include "Condition.h"
#include "Thread.h"
#include "libhpx/debug.h"
#include "libhpx/parcel.h"
#include "libhpx/Worker.h"
#include <cstdint>

namespace {
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;
}

static constexpr int CODE_OFFSET = ((sizeof(uintptr_t) / 2) * 8);
static constexpr uintptr_t ERROR_MASK = 0x1;

// static check to make sure the error code is related to the cvar size in the
// way that we expect---if this fails we're probably on a 32-bit platform, and
// we need a different error code size
static_assert(sizeof(Condition) - (2 * sizeof(hpx_status_t)) + 1 > 0,
             "Condition instances have unexpected size.");

Condition::Condition()
    : top_(nullptr)
{
}

Condition::~Condition()
{
  DEBUG_IF(!hasError() && !empty()) {
    dbg_error("Destroying a condition that has waiting threads.\n");
  }
}

uintptr_t
Condition::hasError() const
{
  return (uintptr_t)top_ & ERROR_MASK;
}

hpx_parcel_t *
Condition::setError(hpx_status_t code)
{
  if (hasError()) {
    return nullptr;
  }

  hpx_parcel_t *top = top_;
  top_ = (hpx_parcel_t*)((((uintptr_t)code) << CODE_OFFSET) | ERROR_MASK);
  return top;
}

hpx_status_t
Condition::getError() const
{
  if (hasError()) {
    return (hpx_status_t)((uintptr_t)(top_) >> CODE_OFFSET);
  }
  else {
    return HPX_SUCCESS;
  }
}

void
Condition::clearError()
{
  if (hasError()) {
    top_ = nullptr;
  }
}

hpx_status_t
Condition::push(hpx_parcel_t *parcel)
{
  if (hasError()) {
    return getError();
  }

  parcel->next = top_;
  top_ = parcel;
  return HPX_SUCCESS;
}

hpx_parcel_t *
Condition::pop()
{
  if (hasError()) {
    return nullptr;
  }

  hpx_parcel_t *top = top_;
  if (top) {
    top_ = top->next;
    top->next = nullptr;
  }
  return top;
}

hpx_parcel_t *
Condition::popAll()
{
  if (hasError()) {
    return nullptr;
  }

  hpx_parcel_t *top = top_;
  top_ = nullptr;
  return top;
}

void
Condition::reset()
{
  DEBUG_IF(!hasError() && !empty()) {
    dbg_error("Resetting a condition that has waiting threads.\n");
  }
  top_ = nullptr;
}

bool
Condition::empty() const
{
  return (top_ == nullptr);
}

void
Condition::signal()
{
  parcel_launch_all(pop());
}

void
Condition::signalAll()
{
  parcel_launch_all(popAll());
}

void
Condition::signalError(hpx_status_t code)
{
  parcel_launch_all(setError(code));
}
