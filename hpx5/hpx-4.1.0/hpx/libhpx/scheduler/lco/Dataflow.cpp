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

/// @file libhpx/scheduler/dataflow.c
/// @brief A dataflow LCO.
#include "LCO.h"
#include "Condition.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "hpx/hpx.h"
#include <mutex>

namespace {
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;

class Dataflow final : public LCO {
 public:
  Dataflow() : LCO(LCO_DATAFLOW), cvar_() {
  }

  ~Dataflow() {
    lock();                                     // released in ~LCO()
  }

  hpx_status_t get(size_t size, void *value, int reset);
  hpx_status_t attach(hpx_parcel_t *p);

  int set(size_t size, const void *value) {
    // @todo: why doesn't this do anything at all?
    std::lock_guard<LCO> _(*this);
    return HPX_SUCCESS;
  }

  void error(hpx_status_t code) {
    std::lock_guard<LCO> _(*this);
    cvar_.signalError(code);
  }

  hpx_status_t wait(int reset) {
    return get(0, NULL, reset);
  }

  void reset() {
    std::lock_guard<LCO> _(*this);
    resetCondition();
  }

  size_t size(size_t) const {
    return sizeof(Dataflow);
  }

 public:
  /// Static action interface.
  /// @{
  static int NewHandler(void* buffer) {
    auto lco = new(buffer) Dataflow();
    return HPX_THREAD_CONTINUE(lco);
  }
  /// @}

 private:
  void resetCondition() {
    log_lco("resetting dataflow LCO %p\n", this);
    resetTriggered();
    cvar_.reset();
  }

  Condition cvar_;
};

/// Class that manages the dataflow run operation.
class RunOp {
 public:
  static hpx_parcel_t* Create(hpx_addr_t lco, hpx_action_t action,
                              hpx_addr_t out, int n, va_list& args);

  static int Handler(const RunOp& run, size_t) {
    return run();
  }

 private:
  void init(hpx_action_t action, int n, va_list& args);
  int operator()() const;

  size_t n_;
  hpx_action_t action_;
  struct Record {
    size_t     size;
    hpx_addr_t addr;
  } data_[];
};

LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, New, Dataflow::NewHandler, HPX_POINTER);
LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, Run, RunOp::Handler, HPX_POINTER, HPX_SIZE_T);
}

hpx_status_t
Dataflow::attach(hpx_parcel_t *p) {
  std::lock_guard<LCO> _(*this);
  if (hpx_status_t status = cvar_.getError()) {
    return status;
  }

  if (getTriggered()) {
    return hpx_parcel_send(p, HPX_NULL);
  }

  return cvar_.push(p);
}

/// Invoke a get operation on the dataflow LCO.
hpx_status_t
Dataflow::get(size_t size, void *out, int reset)
{
  std::lock_guard<LCO> _(*this);
  if (getTriggered()) {
    return cvar_.getError();
  }

  if (hpx_status_t status = waitFor(cvar_)) {
    return status;
  }

  if (reset) {
    resetCondition();
  }

  return HPX_SUCCESS;
}

void
RunOp::init(hpx_action_t action, int n, va_list& args)
{
  action_ = action;
  n_ = n;
  for (int i = 0, e = n_; i < e; ++i) {
    data_[i].addr = va_arg(args, hpx_addr_t);
    data_[i].size = va_arg(args, size_t);
  }
}

int
RunOp::operator()() const
{
  std::unique_ptr<hpx_addr_t[]> inputs(new hpx_addr_t[n_]);
  std::unique_ptr<size_t[]> sizes(new size_t[n_]);
  std::unique_ptr<void*[]> values(new void*[n_]);

  for (unsigned i = 0; i < n_; ++i) {
    sizes[i] = data_[i].size;
    inputs[i] = data_[i].addr;
    values[i] = new char[sizes[i]];
  }

  // fetch all of the inputs and run the handler
  hpx_action_handler_t handler = hpx_action_get_handler(action_);
  dbg_check(hpx_lco_get_all(n_, &inputs[0], &sizes[0], &values[0], nullptr));
  return handler(&values[0], n_);
}

hpx_parcel_t*
RunOp::Create(hpx_addr_t lco, hpx_action_t action, hpx_addr_t out, int n,
              va_list& vargs)
{
  dbg_assert(n > 0);
  dbg_assert(~(n & 1));
  size_t size = sizeof(RunOp) + n / 2 * sizeof(Record);
  hpx_parcel_t *p = action_new_parcel(Run,                // action
                                      out,                // target
                                      out,                // continuation target
                                      hpx_lco_set_action, // continuation action
                                      2,                  // number of args
                                      nullptr,            // marshalled buffer
                                      size);              // buffer size

  auto& args = *static_cast<RunOp*>(hpx_parcel_get_data(p));
  args.init(action, n / 2, vargs);
  return p;
}

hpx_addr_t
hpx_lco_dataflow_new(void)
{
  hpx_addr_t gva = HPX_NULL;
  Dataflow* lco = nullptr;
  try {
    lco = new(gva) Dataflow();
    hpx_gas_unpin(gva);
  }
  catch (const LCO::NonLocalMemory&) {
    hpx_call_sync(gva, New, &lco, sizeof(lco));
  }
  LCO_LOG_NEW(gva, lco);
  return gva;
}

int
_hpx_lco_dataflow_add(hpx_addr_t lco, hpx_action_t action, hpx_addr_t out,
                      int n, ...)
{
  va_list vargs;
  va_start(vargs, n);
  hpx_parcel_t* p = RunOp::Create(lco, action, out, n, vargs);
  va_end(vargs);
  hpx_parcel_send(p, HPX_NULL);
  return HPX_SUCCESS;
}
