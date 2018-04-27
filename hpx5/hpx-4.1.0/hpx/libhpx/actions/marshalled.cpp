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

#include <hpx/hpx.h>
#include <libhpx/action.h>
#include <libhpx/debug.h>
#include <libhpx/parcel.h>
#include "init.h"
#include "exit.h"

static void _pack_marshalled(const void *obj, hpx_parcel_t *p, int n,
                             va_list *args) {
}

static hpx_parcel_t *_new_marshalled(const void *obj, hpx_addr_t addr,
                                     hpx_addr_t c_addr, hpx_action_t c_action,
                                     int n, va_list *args) {
  dbg_assert_str(!n || args);
  dbg_assert(!n || n == 2);

  const action_t *action = static_cast<const action_t *>(obj);
  hpx_action_t id = *action->id;
  hpx_pid_t pid = hpx_thread_current_pid();
  void *data = (n) ? va_arg(*args, void*) : NULL;
  int bytes = (n) ? va_arg(*args, int) : 0;
  return parcel_new(addr, id, c_addr, c_action, pid, data, bytes);
}

static int _exec_marshalled(const void *obj, hpx_parcel_t *p) {
  const action_t *action = static_cast<const action_t *>(obj);
  hpx_action_handler_t handler = (hpx_action_handler_t)action->handler;
  void *args = hpx_parcel_get_data(p);
  return handler(args, p->size);
}

static int _exec_pinned_marshalled(const void *obj, hpx_parcel_t *p) {
  const action_t *act = static_cast<const action_t *>(obj);

  void *target;
  if (!hpx_gas_try_pin(p->target, &target)) {
    log_action("pinned action resend.\n");
    return HPX_RESEND;
  }

  void *args = hpx_parcel_get_data(p);
  int e = ((hpx_pinned_action_handler_t)act->handler)(target, args, p->size);
  hpx_gas_unpin(p->target);
  return e;
}

static void _marshalled_finish(void *act) {
  action_t *action = static_cast<action_t *>(act);
  log_action("%d: %s (%p) %s %x.\n", *action->id, action->key,
             (void*)(uintptr_t)action->handler,
             HPX_ACTION_TYPE_TO_STRING[action->type],
             action->attr);
}

static void _marshalled_fini(void *action) {
}

static const parcel_management_vtable_t _marshalled_vtable = {
  .exec_parcel = _exec_marshalled,
  .pack_parcel = _pack_marshalled,
  .new_parcel = _new_marshalled,
  .exit = exit_action
};

static const parcel_management_vtable_t _pinned_marshalled_vtable = {
  .exec_parcel = _exec_pinned_marshalled,
  .pack_parcel = _pack_marshalled,
  .new_parcel = _new_marshalled,
  .exit = exit_pinned_action
};

void action_init_marshalled(action_t *action, int n, va_list *args) {
  uint32_t pinned = action->attr & HPX_PINNED;

  // Check that the first argument type is a pointer if this is a pinned
  // action. Short circuit evaluation only consumes the first argument for
  // pinned actions.
  if (pinned && (va_arg(*args, hpx_type_t) != HPX_POINTER)) {
    dbg_error("First type of a pinned action should be HPX_POINTER\n");
  }

  // Verify the rest of the action type.
  hpx_type_t addr = va_arg(*args, hpx_type_t);
  hpx_type_t size = va_arg(*args, hpx_type_t);

  if ((addr != HPX_POINTER) ||
      (size != HPX_INT && size != HPX_UINT && size != HPX_SIZE_T)) {
    dbg_error("Marshalled action type should be HPX_POINTER, HPX_INT\n");
  }

  // Initialize the parcel class.
  if (pinned) {
    action->parcel_class = &_pinned_marshalled_vtable;
  }
  else {
    action->parcel_class = &_marshalled_vtable;
  }

  // Initialize the call class.
  action_init_call_by_parcel(action);

  // Initialize the destructor.
  action->finish = _marshalled_finish;
  action->fini = _marshalled_fini;
}
