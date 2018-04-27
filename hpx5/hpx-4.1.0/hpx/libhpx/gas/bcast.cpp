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

#include <alloca.h>
#include <hpx/hpx.h>
#include <libhpx/action.h>
#include <libhpx/debug.h>
#include <libhpx/locality.h>
#include <libhpx/memory.h>
#include <libhpx/parcel.h>

static
int _va_gas_bcast_cont(hpx_action_t act, hpx_addr_t base, int n,
                       size_t offset, size_t bsize, hpx_action_t rop,
                       hpx_addr_t rsync, int nargs, va_list *vargs)
{
  hpx_addr_t done = hpx_lco_and_new(n);
  hpx_action_t set = hpx_lco_set_action;
  for (int i = 0; i < n; ++i) {
    va_list temp;
    va_copy(temp, *vargs);
    hpx_gas_ptrdiff_t off = i * bsize + offset;
    hpx_addr_t addr = hpx_addr_add(base, off, bsize);
    int e = action_call_async_va(act, addr, done, set, rsync, rop, nargs, &temp);
    dbg_check(e, "failed to call action\n");
    va_end(temp);
  }
  int e = hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);
  return e;
}

int
_hpx_gas_bcast_with_continuation(hpx_action_t action, hpx_addr_t base, int n,
                                 size_t offset, size_t bsize, hpx_action_t rop,
                                 hpx_addr_t raddr, int nargs, ...)
{
  va_list vargs;
  va_start(vargs, nargs);
  int e = _va_gas_bcast_cont(action, base, n, offset, bsize, rop, raddr, nargs,
                             &vargs);
  dbg_check(e, "failed _hpx_gas_bcast_with_continuation\n");
  va_end(vargs);
  return e;
}

int
_hpx_gas_bcast_sync(hpx_action_t action, hpx_addr_t base, int n,
                    size_t offset, size_t bsize, int nargs, ...)
{
  hpx_addr_t sync = hpx_lco_and_new(n);
  if (sync == HPX_NULL) {
    log_error("could not allocate an LCO.\n");
    return HPX_ENOMEM;
  }

  va_list vargs;
  va_start(vargs, nargs);
  int e = _va_gas_bcast_cont(action, base, n, offset, bsize, hpx_lco_set_action,
                             sync, nargs, &vargs);
  va_end(vargs);

  if (HPX_SUCCESS != hpx_lco_wait(sync)) {
    dbg_error("failed map\n");
  }

  hpx_lco_delete(sync, HPX_NULL);
  return e;
}
