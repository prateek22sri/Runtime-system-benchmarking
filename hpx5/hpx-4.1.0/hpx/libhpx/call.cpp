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

/// @file libhpx/call.c
/// @brief Implement the hpx/call.h header.

#include "libhpx/action.h"
#include "libhpx/parcel.h"
#include "libhpx/Worker.h"
#include "hpx/hpx.h"
#include <cstring>
#include <cstdarg>

namespace {
using libhpx::self;
}

/// A RPC call with a user-specified continuation action.
int
_hpx_call_with_continuation(hpx_addr_t addr, hpx_action_t action,
                            hpx_addr_t c_target, hpx_action_t c_action,
                            int n, ...)
{
  va_list args;
  va_start(args, n);
  int e = action_call_lsync_va(action, addr, c_target, c_action, n, &args);
  va_end(args);
  return e;
}

int
_hpx_call_async(hpx_addr_t addr, hpx_action_t id, hpx_addr_t lsync,
                hpx_addr_t rsync, int n, ...)
{
  va_list args;
  va_start(args, n);
  hpx_action_t op = hpx_lco_set_action;
  int e = action_call_async_va(id, addr, lsync, op, rsync, op, n, &args);
  va_end(args);
  return e;
}

/// Encapsulates an asynchronous remote-procedure-call.
int
_hpx_call(hpx_addr_t addr, hpx_action_t id, hpx_addr_t result, int n, ...)
{
  va_list args;
  va_start(args, n);
  hpx_action_t rop = hpx_lco_set_action;
  int e = action_call_lsync_va(id, addr, result, rop, n, &args);
  va_end(args);
  return e;
}

int
_hpx_call_sync(hpx_addr_t addr, hpx_action_t id, void *out, size_t olen, int n,
               ...)
{
  va_list args;
  va_start(args, n);
  int e = action_call_rsync_va(id, addr, out, olen, n, &args);
  va_end(args);
  return e;
}

int
_hpx_call_cc(hpx_addr_t addr, hpx_action_t id, int n, ...)
{
  va_list args;
  va_start(args, n);
  hpx_parcel_t *p = self->getCurrentParcel();
  hpx_addr_t rsync = p->c_target;
  hpx_action_t rop = p->c_action;
  int e = action_call_lsync_va(id, addr, rsync, rop, n, &args);
  va_end(args);

  if (e == HPX_SUCCESS) {
    p->c_target = HPX_NULL;
    p->c_action = HPX_NULL;
    e = hpx_thread_continue(NULL, 0);
  }

  return e;
}

int
_hpx_call_when(hpx_addr_t gate, hpx_addr_t addr, hpx_action_t id,
               hpx_addr_t result, int n, ...)
{
  va_list args;
  va_start(args, n);
  hpx_action_t rop = hpx_lco_set_action;
  int e = action_when_lsync_va(id, addr, gate, result, rop, n, &args);
  va_end(args);
  return e;
}

int
_hpx_call_when_sync(hpx_addr_t gate, hpx_addr_t addr, hpx_action_t id,
                    void *out, size_t olen, int n, ...)
{
  va_list args;
  va_start(args, n);
  int e = action_when_rsync_va(id, addr, gate, out, olen, n, &args);
  va_end(args);
  return e;
}

/// hpx_call_when with a user-specified continuation action.
int
_hpx_call_when_with_continuation(hpx_addr_t gate, hpx_addr_t addr,
                                 hpx_action_t id, hpx_addr_t rsync,
                                 hpx_action_t rop, int n, ...)
{
  va_list args;
  va_start(args, n);
  int e = action_when_lsync_va(id, addr, gate, rsync, rop, n, &args);
  va_end(args);
  return e;
}

int
_hpx_call_when_cc(hpx_addr_t gate, hpx_addr_t addr, hpx_action_t id, int n,
                  ...)
{
  va_list args;
  va_start(args, n);
  hpx_parcel_t *p = self->getCurrentParcel();
  hpx_addr_t rsync = p->c_target;
  hpx_action_t rop = p->c_action;
  p->c_target = HPX_NULL;
  p->c_action = HPX_NULL;

  int e = action_when_lsync_va(id, addr, gate, rsync, rop, n, &args);
  va_end(args);

  if (e == HPX_SUCCESS) {
    e = hpx_thread_continue(NULL, 0);
  }

  return e;
}
