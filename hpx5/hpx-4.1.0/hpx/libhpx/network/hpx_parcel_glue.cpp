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

#include <string.h>
#include <hpx/hpx.h>
#include <libhpx/action.h>
#include <libhpx/config.h>
#include <libhpx/debug.h>
#include <libhpx/parcel.h>

hpx_parcel_t *hpx_parcel_acquire(const void *buffer, size_t bytes) {
  hpx_addr_t target = HPX_HERE;
  hpx_pid_t pid = hpx_thread_current_pid();
  return parcel_new(target, 0, 0, 0, pid, buffer, bytes);
}

hpx_status_t hpx_parcel_send_sync(hpx_parcel_t *p) {
  parcel_launch(p);
  return HPX_SUCCESS;
}

// Register a thread entry point to do an asynchronous send. It's important that
// this is a DEFAULT action so that we don't try to work-first it.
static LIBHPX_ACTION(HPX_DEFAULT, 0, _send_async, hpx_parcel_send_sync, HPX_POINTER);

hpx_status_t hpx_parcel_send(hpx_parcel_t *p, hpx_addr_t lsync) {
  parcel_state_t state = parcel_get_state(p);
  if (p->size < LIBHPX_SMALL_THRESHOLD || parcel_serialized(state)) {
    parcel_launch(p);
    hpx_lco_error(lsync, HPX_SUCCESS, HPX_NULL);
    return HPX_SUCCESS;
  }
  else {
    return hpx_call(HPX_HERE, _send_async, lsync, &p);
  }
}

hpx_status_t hpx_parcel_send_through_sync(hpx_parcel_t *p, hpx_addr_t gate) {
  dbg_assert(p->target != HPX_NULL);
  parcel_launch_through(p, gate);
  parcel_delete(p);
  return HPX_SUCCESS;
}

// Register a thread entry point to do an asynchronous send-through. It's
// important that this is a DEFAULT action so that we don't try to work-first
// it.
static LIBHPX_ACTION(HPX_DEFAULT, 0, _send_through_async,
                     hpx_parcel_send_through_sync, HPX_POINTER, HPX_ADDR);

hpx_status_t hpx_parcel_send_through(hpx_parcel_t *p, hpx_addr_t gate,
                                     hpx_addr_t lsync) {
  // If the parcel is small, regardless of if its been serialized already, we
  // want to do the send through eagerly, otherwise we want to spawn a thread to
  // do it.
  if (parcel_size(p) < LIBHPX_SMALL_THRESHOLD) {
    parcel_launch_through(p, gate);
    parcel_delete(p);
    hpx_lco_error(lsync, HPX_SUCCESS, HPX_NULL);
    return HPX_SUCCESS;
  }
  else {
    return hpx_call(HPX_HERE, _send_through_async, lsync, &p, &gate);
  }
}

void hpx_parcel_retain(hpx_parcel_t *p) {
  parcel_retain(p);
}

void hpx_parcel_release(hpx_parcel_t *p) {
  parcel_release(p);
  parcel_delete(p);
}

void hpx_parcel_set_action(hpx_parcel_t *p, hpx_action_t action) {
  p->action = action;
}

void hpx_parcel_set_target(hpx_parcel_t *p, hpx_addr_t addr) {
  p->target = addr;
}

void hpx_parcel_set_cont_action(hpx_parcel_t *p, hpx_action_t action) {
  p->c_action = action;
}

void hpx_parcel_set_cont_target(hpx_parcel_t *p, hpx_addr_t cont) {
  p->c_target = cont;
}

void hpx_parcel_set_data(hpx_parcel_t *p, const void *data, size_t size) {
  if (size) {
    void *to = hpx_parcel_get_data(p);
    memmove(to, data, size);
  }
}

void _hpx_parcel_set_args(hpx_parcel_t *p, int n, ...) {
  va_list vargs;
  va_start(vargs, n);
  action_pack_parcel_va(p->action, p, n, &vargs);
  va_end(vargs);
}

void hpx_parcel_set_pid(hpx_parcel_t *p, const hpx_pid_t pid) {
  p->pid = pid;
}

hpx_action_t hpx_parcel_get_action(const hpx_parcel_t *p) {
  return p->action;
}

hpx_addr_t hpx_parcel_get_target(const hpx_parcel_t *p) {
  return p->target;
}

hpx_action_t hpx_parcel_get_cont_action(const hpx_parcel_t *p) {
  return p->c_action;
}

hpx_addr_t hpx_parcel_get_cont_target(const hpx_parcel_t *p) {
  return p->c_target;
}

void *hpx_parcel_get_data(hpx_parcel_t *p) {
  if (p->size == 0) {
    return NULL;
  }

  parcel_state_t state = parcel_get_state(p);
  if (parcel_serialized(state)) {
    return (void*)&p->buffer;
  }

  // Copy out the pointer stored in the bufferm but don't violate strict
  // aliasing.
  void *buffer = NULL;
  memcpy(&buffer, &p->buffer, sizeof(buffer));
  return buffer;
}

size_t hpx_parcel_get_size(const hpx_parcel_t *p) {
  return p->size;
}

hpx_pid_t hpx_parcel_get_pid(const hpx_parcel_t *p) {
  return p->pid;
}
