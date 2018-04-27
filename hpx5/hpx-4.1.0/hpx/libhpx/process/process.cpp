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

/// @file libhpx/scheduler/process.c
#include "Bitmap.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/events.h"
#include "libhpx/locality.h"
#include "libhpx/parcel.h"
#include "libhpx/process.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <atomic>

namespace {
constexpr auto ACQUIRE = std::memory_order_acquire;
constexpr auto RELEASE = std::memory_order_release;
constexpr auto ACQ_REL = std::memory_order_acq_rel;
constexpr auto RELAXED = std::memory_order_relaxed;
using libhpx::process::Bitmap;

typedef struct {
  std::atomic<uint64_t> credit;                 // credit balance
  Bitmap                 *debt;                 // the credit that was recovered
  hpx_addr_t       termination;                 // the termination LCO
} _process_t;
}

static bool HPX_USED _is_tracked(_process_t *p) {
  return (p->termination != HPX_NULL);
}

/// Remote action to delete a process.
static void _free(_process_t *p) {
  if (!p) {
    return;
  }

  delete p->debt;

  // set the termination LCO if the process is being deleted
  // if (_is_tracked(p))
  //   hpx_lco_set(p->termination, 0, NULL, HPX_NULL, HPX_NULL);
}

/// Initialize a process.
static void _init(_process_t *p, hpx_addr_t termination) {
  p->credit.store(0, RELEASE);
  p->debt = new Bitmap();
  p->termination = termination;
}

static int _proc_call_handler(hpx_parcel_t *arg, size_t n) {
  hpx_addr_t process = hpx_thread_current_target();
  _process_t *p = NULL;
  if (!hpx_gas_try_pin(process, (void**)&p)) {
    return HPX_RESEND;
  }

  uint64_t credit = p->credit.fetch_add(1, ACQ_REL) + 1;
  hpx_gas_unpin(process);

  hpx_pid_t pid = hpx_process_getpid(process);
  hpx_parcel_t *parcel = hpx_parcel_acquire(NULL, parcel_size(arg));
  memcpy(parcel, arg, parcel_size(arg));
  hpx_parcel_set_pid(parcel, pid);
  parcel->credit = credit;
  hpx_parcel_send_sync(parcel);
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _proc_call,
                     _proc_call_handler, HPX_POINTER, HPX_SIZE_T);

static int _proc_delete_handler(_process_t *p, size_t size) {
  _free(p);
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, _proc_delete,
                     _proc_delete_handler,
                     HPX_POINTER, HPX_POINTER, HPX_SIZE_T);

static int _proc_return_credit_handler(_process_t *p, uint64_t *args, size_t size) {
  // add credit to the credit-accounting bitmap
  uint64_t debt = p->debt->addAndTest(*args);
  for (;;) {
    uint64_t credit = p->credit.load(ACQUIRE);
    if ((credit != 0) && ~(debt | ((UINT64_C(1) << (64-credit)) - 1)) == 0) {
      // log("detected quiescence...\n");
      if (!p->credit.compare_exchange_weak(credit, 0 - credit, RELEASE, RELAXED)) {
        continue;
      }
      dbg_assert(_is_tracked(p));
      hpx_lco_set(p->termination, 0, NULL, HPX_NULL, HPX_NULL);
    }
    break;
  }
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, _proc_return_credit,
                     _proc_return_credit_handler,
                     HPX_POINTER, HPX_POINTER, HPX_SIZE_T);

int process_recover_credit(hpx_parcel_t *p) {
  hpx_addr_t process = p->pid;
  if (process == HPX_NULL) {
    return HPX_SUCCESS;
  }

  if (!p->credit) {
    return HPX_SUCCESS;
  }

  hpx_parcel_t *pp = parcel_new(process, _proc_return_credit, 0, 0, 0,
                                &p->credit, sizeof(p->credit));
  if (!pp) {
    dbg_error("parcel_recover_credit failed.\n");
  }
  pp->credit = 0;

  hpx_parcel_send_sync(pp);
  return HPX_SUCCESS;
}

hpx_addr_t hpx_process_new(hpx_addr_t termination) {
  if (termination == HPX_NULL) {
    return HPX_NULL;
  }

  _process_t *p;
  hpx_addr_t process = hpx_gas_alloc_local(1, sizeof(*p), 0);
  if (!hpx_gas_try_pin(process, (void**)&p)) {
    dbg_error("Could not pin newly allocated process.\n");
  }
  _init(p, termination);
  hpx_gas_unpin(process);
  EVENT_PROCESS_NEW(process, termination);
  return process;
}

hpx_pid_t hpx_process_getpid(hpx_addr_t process) {
  return (hpx_pid_t)process;
}

int _hpx_process_call(hpx_addr_t process, hpx_addr_t addr, hpx_action_t id,
                      hpx_addr_t result, int n, ...) {
  va_list args;
  va_start(args, n);
  hpx_action_t set = hpx_lco_set_action;
  hpx_parcel_t  *p = action_new_parcel_va(id, addr, result, set, n, &args);
  va_end(args);

  if (hpx_thread_current_pid() == hpx_process_getpid(process)) {
    hpx_parcel_send_sync(p);
    return HPX_SUCCESS;
  }

  hpx_addr_t sync = hpx_lco_future_new(0);
  hpx_parcel_t *q = hpx_parcel_acquire(NULL, parcel_size(p));
  q->target = process;
  q->action = _proc_call;
  q->c_target = sync;
  q->c_action = hpx_lco_set_action;
  hpx_parcel_set_data(q, p, parcel_size(p));
  q->pid = 0;
  q->credit = 0;

  EVENT_PROCESS_CALL(process, q->pid);
  hpx_parcel_send_sync(q);

  parcel_delete(p);
  hpx_lco_wait(sync);
  hpx_lco_delete(sync, HPX_NULL);
  return HPX_SUCCESS;
}

/// Delete a process.
void hpx_process_delete(hpx_addr_t process, hpx_addr_t sync) {
  if (process == HPX_NULL) {
    return;
  }

  hpx_call_sync(process, _proc_delete, NULL, 0, NULL, 0);
  EVENT_PROCESS_DELETE(process);
  hpx_gas_free(process, sync);
}

