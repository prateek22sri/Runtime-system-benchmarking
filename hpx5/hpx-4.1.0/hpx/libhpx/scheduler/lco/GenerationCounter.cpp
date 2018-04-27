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

/// @file libhpx/scheduler/sema.c
/// @brief Implements the semaphore LCO.

#include "LCO.h"
#include "Condition.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include <mutex>
#include <cstring>

namespace {
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;

struct GenerationCounter final : public LCO {
 public:
  GenerationCounter(unsigned ninplace);
  ~GenerationCounter();

  /// Wait for the specified generation.
  hpx_status_t waitForGeneration(unsigned long i);

  int set(size_t size, const void *value) {
    std::lock_guard<LCO> _(*this);
    unsigned long i = ++gen_;
    oflow_.signalAll();
    if (ninplace_) {
      conditionAt(i % ninplace_).signalAll();
    }
    return 1;
  }

  void error(hpx_status_t code) {
    std::lock_guard<LCO> _(*this);
    for (unsigned i = 0, e = ninplace_; i < e; ++i) {
      conditionAt(i).signalError(code);
    }
    oflow_.signalError(code);
  }

  hpx_status_t get(size_t size, void *out, int reset) {
    // @note: No lock here... just a single volatile read is good enough.
    dbg_assert(!size || out);
    if (size) {
      unsigned long i = gen_;                   // volatile read
      memcpy(out, &i, size);
    }
    return HPX_SUCCESS;
  }

  hpx_status_t wait(int reset) {
    std::lock_guard<LCO> _(*this);
    return waitFor(oflow_);
  }

  hpx_status_t attach(hpx_parcel_t *p) {
    std::lock_guard<LCO> _(*this);
    return oflow_.push(p);
  }

  void reset() {
    std::lock_guard<LCO> _(*this);
    for (unsigned i = 0, e = ninplace_; i < e; ++i) {
      conditionAt(i).reset();
    }
    oflow_.reset();
  }

  size_t size(size_t size) const {
    return sizeof(GenerationCounter) + ninplace_ * sizeof(Condition);
  }

 public:
  /// Static action interface.
  /// @{
  static int WaitForGenerationHandler(GenerationCounter& lco, unsigned long i) {
    return lco.waitForGeneration(i);
  }

  static int NewHandler(void* buffer, unsigned ninplace) {
    auto lco = new(buffer) GenerationCounter(ninplace);
    return HPX_THREAD_CONTINUE(lco);
  }
  /// @}

 private:
  Condition& conditionAt(size_t i) {
    return *reinterpret_cast<Condition*>(inplace_ + i * sizeof(Condition));
  }

  Condition            oflow_;
  volatile unsigned long gen_;
  const unsigned    ninplace_;
  char               inplace_[];
};

LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, New, GenerationCounter::NewHandler,
              HPX_POINTER, HPX_UINT);
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, WaitForGeneration,
              GenerationCounter::WaitForGenerationHandler, HPX_POINTER,
              HPX_ULONG);
}

GenerationCounter::GenerationCounter(unsigned ninplace)
    : LCO(LCO_GENCOUNT),
      gen_(),
      ninplace_(ninplace),
      inplace_()
{
  for (unsigned i = 0, e = ninplace; i < e; ++i) {
    new(inplace_ + i * sizeof(Condition)) Condition();
  }
}

GenerationCounter::~GenerationCounter()
{
  lock();
  for (unsigned i = 0, e = ninplace_; i < e; ++i) {
    conditionAt(i).~Condition();
  }
}

hpx_status_t
GenerationCounter::waitForGeneration(unsigned long i)
{
  std::lock_guard<LCO> _(*this);
  while (gen_ < i) {
    bool inplace = i < (gen_ + ninplace_);
    Condition& cond = (inplace) ? conditionAt(i % ninplace_) : oflow_;
    if (auto status = waitFor(cond)) {
      return status;
    }
  }
  return oflow_.getError();
}

hpx_addr_t
hpx_lco_gencount_new(unsigned long ninplace)
{
  hpx_addr_t gva = HPX_NULL;
  GenerationCounter* lco = nullptr;
  try {
    lco = new(ninplace * sizeof(Condition), gva) GenerationCounter(ninplace);
    hpx_gas_unpin(gva);
  }
  catch (const LCO::NonLocalMemory&) {
    hpx_call_sync(gva, New, &lco, sizeof(Condition), &ninplace);
  }
  LCO_LOG_NEW(gva, lco);
  return gva;
}

void
hpx_lco_gencount_inc(hpx_addr_t gencnt, hpx_addr_t rsync)
{
  hpx_lco_set(gencnt, 0, NULL, HPX_NULL, rsync);
}


hpx_status_t
hpx_lco_gencount_wait(hpx_addr_t gva, unsigned long i)
{
  GenerationCounter *lva = nullptr;
  if (hpx_gas_try_pin(gva, (void**)&lva)) {
    hpx_status_t status = lva->waitForGeneration(i);
    hpx_gas_unpin(gva);
    return status;
  }
  return hpx_call_sync(gva, WaitForGeneration, NULL, 0, &i);
}
