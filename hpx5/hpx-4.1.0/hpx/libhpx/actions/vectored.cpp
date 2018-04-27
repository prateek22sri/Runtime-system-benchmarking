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
#include <libhpx/debug.h>
#include <libhpx/parcel.h>
#include <libhpx/padding.h>
#include <libhpx/percolation.h>
#include "init.h"
#include "exit.h"

static void _pack_vectored(const void *obj, hpx_parcel_t *p, int n,
                           va_list *vargs) {
  n >>= 1;
  void *buffer = hpx_parcel_get_data(p);
  *(int*)buffer = n;
  size_t *sizes = (size_t*)((char*)buffer + sizeof(int));
  char *args = (char*)sizes + (sizeof(size_t) * n);

  size_t bytes = ALIGN(args - (char*)buffer, 8);
  for (int i = 0; i < n; ++i) {
    void *arg = va_arg(*vargs, void*);
    size_t size = va_arg(*vargs, int);
    sizes[i] = size;
    memcpy(args+bytes, arg, size);
    bytes += size + ALIGN(size, 8);
  }
}

static hpx_parcel_t *_new_vectored(const void *obj, hpx_addr_t addr,
                                   hpx_addr_t c_addr, hpx_action_t c_action,
                                   int n, va_list *args) {
  dbg_assert(n && args);
  dbg_assert_str(!(n & 1), "Vectored actions require even arg count: %d\n", n);

  const action_t *action = static_cast<const action_t *>(obj);
  hpx_action_t id = *action->id;
  hpx_pid_t pid = hpx_thread_current_pid();

  // For vectored arguments we want to read through the argument pairs and
  // accumuluate the total number of bytes that we'll need to allocate.  We need
  // `int` bytes for the number of tuples, then `n/2` `size_t` bytes for the
  // size array, then padding to align the first buffer, and then 8-byte aligned
  // sizes for each buffer.
  int ntuples = n >> 1;
  size_t bytes = sizeof(int) + ntuples * sizeof(size_t);

  // We will need to iterate the va_args again, so we make a copy, and position
  // its starting location on the first size. Then we just go through the list,
  // checking every other list element for the next size.
  va_list temp;
  va_copy(temp, *args);
  va_arg(temp, void*);
  for (int i = 0, e = ntuples; i < e; ++i, va_arg(temp, void*)) {
    bytes += ALIGN(bytes, 8);
    bytes += va_arg(temp, int);
  }
  va_end(temp);

  hpx_parcel_t *p = parcel_new(addr, id, c_addr, c_action, pid, NULL, bytes);
  _pack_vectored(obj, p, n, args);
  return p;
}

static int _exec_pinned_vectored(const void *obj, hpx_parcel_t *p) {
  const action_t *act = static_cast<const action_t *>(obj);

  void *target;
  if (!hpx_gas_try_pin(p->target, &target)) {
    log_action("pinned action resend.\n");
    return HPX_RESEND;
  }

  void *args = hpx_parcel_get_data(p);
  int nargs = *(int*)args;
  size_t *sizes = (size_t*)((char*)args + sizeof(int));
  void *argsp[nargs];
  char *vargs = (char*)sizes + (nargs * sizeof(size_t));
  argsp[0] = (char*)vargs + ALIGN((char*)vargs - (char*)args, 8);

  for (int i = 0, e = nargs - 1; i < e; ++i) {
    argsp[i + 1] = (char*)argsp[i] + sizes[i] + ALIGN(sizes[i], 8);
  }

  typedef hpx_pinned_vectored_action_handler_t handler_t;
  int e = ((handler_t)act->handler)(target, nargs, argsp, sizes);
  hpx_gas_unpin(p->target);
  return e;
}

static int _exec_vectored(const void *obj, hpx_parcel_t *p) {
  const action_t *action = static_cast<const action_t *>(obj);
  void *args = hpx_parcel_get_data(p);
  int nargs = *(int*)args;
  size_t *sizes = (size_t*)((char*)args + sizeof(int));
  void *argsp[nargs];
  char *vargs = (char*)sizes + (nargs * sizeof(size_t));
  argsp[0] = (char*)vargs + ALIGN((char*)vargs - (char*)args, 8);

  for (int i = 0, e = nargs - 1; i < e; ++i) {
    argsp[i + 1] = (char*)argsp[i] + sizes[i] + ALIGN(sizes[i], 8);
  }

  return ((hpx_vectored_action_handler_t )action->handler)(nargs, argsp, sizes);
}

static void _vectored_fini(void *action) {
}

static void _opencl_fini(void *act) {
  action_t *action = static_cast<action_t *>(act);
  percolation_destroy(here->percolation, action->env);
}

static void _vectored_finish(void *act) {
  action_t *action = static_cast<action_t *>(act);
  dbg_assert(here);
  dbg_assert(here->percolation);

  // Special-case for opencl actions.
  if (action->type == HPX_OPENCL) {
    action->env = percolation_prepare(here->percolation, action->key,
                                      (const char*)action->handler);
    dbg_assert_str(action->env, "failed to prepare percolation kernel: %s\n",
                   action->key);
    action->handler = (handler_t)percolation_execute_handler;
    action->fini = _opencl_fini;
  }

  log_action("%d: %s (%p) %s %x.\n", *action->id, action->key,
             (void*)(uintptr_t)action->handler,
             HPX_ACTION_TYPE_TO_STRING[action->type],
             action->attr);
}

static const parcel_management_vtable_t _vectored_vtable = {
  .exec_parcel = _exec_vectored,
  .pack_parcel = _pack_vectored,
  .new_parcel = _new_vectored,
  .exit = exit_action
};

static const parcel_management_vtable_t _pinned_vectored_vtable = {
  .exec_parcel = _exec_pinned_vectored,
  .pack_parcel = _pack_vectored,
  .new_parcel = _new_vectored,
  .exit = exit_pinned_action
};

void action_init_vectored(action_t *action, int n, va_list *vargs) {
  uint32_t pinned = action->attr & HPX_PINNED;

  // Check that the first argument type is a pointer if this is a pinned
  // action. Short circuit evaluation only consumes the first argument for
  // pinned actions.
  if (pinned && (va_arg(*vargs, hpx_type_t) != HPX_POINTER)) {
    dbg_error("First type of a pinned action should be HPX_POINTER\n");
  }

  // Verify that the rest of the type matches.
  hpx_type_t count = va_arg(*vargs, hpx_type_t);
  hpx_type_t args = va_arg(*vargs, hpx_type_t);
  hpx_type_t sizes = va_arg(*vargs, hpx_type_t);

  if ((count != HPX_INT && count != HPX_UINT && count != HPX_SIZE_T) ||
      (args != HPX_POINTER) ||
      (sizes != HPX_POINTER)) {
    dbg_error("Vectored registration type failure\n");
  }

  // Initialize the parcel class.
  if (pinned) {
    action->parcel_class = &_pinned_vectored_vtable;
  }
  else {
    action->parcel_class = &_vectored_vtable;
  }

  // Initialize the call class.
  action_init_call_by_parcel(action);

  // Initialize the destructor.
  action->finish = _vectored_finish;
  action->fini = _vectored_fini;
}
