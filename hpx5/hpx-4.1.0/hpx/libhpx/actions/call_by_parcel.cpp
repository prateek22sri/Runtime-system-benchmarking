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

#include <libhpx/action.h>
#include <libhpx/debug.h>
#include <libhpx/parcel.h>
#include "init.h"

static int _call_by_parcel_async(const void *o, hpx_addr_t addr,
                                 hpx_addr_t lsync, hpx_action_t lop,
                                 hpx_addr_t rsync, hpx_action_t rop,
                                 int n, va_list *args) {
  dbg_assert(lop == hpx_lco_set_action);
  const action_t *a = static_cast<const action_t *>(o);
  hpx_parcel_t *p = a->parcel_class->new_parcel(a, addr, rsync, rop, n, args);
  hpx_parcel_send(p, lsync);
  return HPX_SUCCESS;
}

static int _call_by_parcel_lsync(const void *o, hpx_addr_t addr,
                                 hpx_addr_t rsync, hpx_action_t rop, int n,
                                 va_list *args) {
  const action_t *a = static_cast<const action_t *>(o);
  hpx_parcel_t *p = a->parcel_class->new_parcel(a, addr, rsync, rop, n, args);
  parcel_launch(p);
  return HPX_SUCCESS;
}

static int _call_by_parcel_rsync(const void *o, hpx_addr_t addr, void *rout,
                                 size_t rbytes, int n, va_list *args) {
  const action_t *a = static_cast<const action_t *>(o);
  hpx_addr_t rsync = hpx_lco_future_new(rbytes);
  hpx_action_t rop = hpx_lco_set_action;
  hpx_parcel_t *p = a->parcel_class->new_parcel(a, addr, rsync, rop, n, args);
  parcel_launch(p);
  int e = hpx_lco_get(rsync, rbytes, rout);
  hpx_lco_delete_sync(rsync);
  return e;
}

static int _call_by_parcel_when_async(const void *o, hpx_addr_t addr,
                                      hpx_addr_t gate, hpx_addr_t lsync,
                                      hpx_action_t lop, hpx_addr_t rsync,
                                      hpx_action_t rop, int n, va_list *args) {
  dbg_assert(lop == hpx_lco_set_action);
  dbg_assert(gate);

  const action_t *a = static_cast<const action_t *>(o);
  hpx_parcel_t *p = a->parcel_class->new_parcel(a, addr, rsync, rop, n, args);
  return hpx_parcel_send_through(p, gate, lsync);
}

static int _call_by_parcel_when_lsync(const void *o, hpx_addr_t addr,
                                      hpx_addr_t gate, hpx_addr_t rsync,
                                      hpx_action_t rop, int n, va_list *args) {
  dbg_assert(gate);
  const action_t *a = static_cast<const action_t *>(o);
  hpx_parcel_t *p = a->parcel_class->new_parcel(a, addr, rsync, rop, n, args);
  return hpx_parcel_send_through_sync(p, gate);
}

static int _call_by_parcel_when_rsync(const void *o, hpx_addr_t addr,
                                      hpx_addr_t gate, void *rout,
                                      size_t rbytes, int n, va_list *args) {
  dbg_assert(gate);

  const action_t *a = static_cast<const action_t *>(o);
  hpx_addr_t rsync = hpx_lco_future_new(rbytes);
  hpx_action_t rop = hpx_lco_set_action;
  hpx_parcel_t *p = a->parcel_class->new_parcel(a, addr, rsync, rop, n, args);
  int e = hpx_parcel_send_through(p, gate, HPX_NULL);
  if (e == HPX_SUCCESS) {
    e = hpx_lco_get(rsync, rbytes, rout);
  }
  hpx_lco_delete_sync(rsync);
  return e;
}

static int _call_by_parcel_continue(const void *o, hpx_parcel_t *p, int n,
                                    va_list *args) {
  const action_t *a = static_cast<const action_t *>(o);
  hpx_addr_t addr = p->c_target;
  hpx_parcel_t *c = a->parcel_class->new_parcel(a, addr, 0, 0, n, args);
  c->credit = p->credit;
  p->credit = 0;
  parcel_launch(c);
  return HPX_SUCCESS;
}

static const calling_convention_vtable_t _call_by_parcel = {
  .async = _call_by_parcel_async,
  .lsync = _call_by_parcel_lsync,
  .rsync = _call_by_parcel_rsync,
  .when_async = _call_by_parcel_when_async,
  .when_lsync = _call_by_parcel_when_lsync,
  .when_rsync = _call_by_parcel_when_rsync,
  .cont = _call_by_parcel_continue
};

void action_init_call_by_parcel(action_t *action) {
  action->call_class = &_call_by_parcel;
}
