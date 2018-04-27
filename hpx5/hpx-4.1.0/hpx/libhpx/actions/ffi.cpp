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

#include <stdlib.h>
#include <hpx/hpx.h>
#include <libhpx/action.h>
#include <libhpx/debug.h>
#include <libhpx/parcel.h>
#include "init.h"
#include "exit.h"

static void _pack_ffi_0(const void *o, hpx_parcel_t *p, int n, va_list *args) {
}

static void _pack_ffi_n(const void *obj, hpx_parcel_t *p, int n, va_list *args)
{
  const action_t *action = static_cast<const action_t *>(obj);
  const ffi_cif *cif = static_cast<const ffi_cif *>(action->env);
  ffi_raw *buffer = static_cast<ffi_raw *>(hpx_parcel_get_data(p));

  DEBUG_IF (unsigned(n) != cif->nargs) {
    const char *key = action->key;
    dbg_error("%s requires %d arguments (%d given).\n", key, cif->nargs, n);
  }

  // copy the vaargs (pointers) to my stack array, which is what ffi
  // wants---this seems wasteful since va_args are likely *already* on my stack,
  // but that's not public information
  void *argps[n];
  for (int i = 0, e = n; i < e; ++i) {
    argps[i] = va_arg(*args, void*);
  }

  // use ffi to copy them to the buffer
  ffi_ptrarray_to_raw(const_cast<ffi_cif*>(cif), argps, buffer);
}

static void _pack_pinned_ffi_n(const void *obj, hpx_parcel_t *p, int n,
                               va_list *args) {
  const action_t *action = static_cast<const action_t *>(obj);
  const ffi_cif *cif = static_cast<const ffi_cif *>(action->env);

  DEBUG_IF (unsigned(n + 1) != cif->nargs) {
    const char *key = action->key;
    dbg_error("%s requires %d arguments (%d given).\n", key, cif->nargs, n + 1);
  }

  // Copy the vaargs (pointers) to my stack array, which is what ffi wants,
  // adding an extra "slot" for the pinned parameter.
  void *argps[++n];

  // Special case pinned actions.
  //
  // ffi actually thinks that it needs to serialize a pointer as its first
  // argument. This is the pinned pointer that we need at the call site. We
  // don't have this pointer here though, and we don't want to serialize it. No
  // matter what we do ffi_ptrarray_to_raw is going to need to process the first
  // pointer though.
  //
  // We cleverly spoof this pointer to be sizeof(void*) bytes off the front of
  // the parcel buffer. During serialization, this will cause ffi to overwrite
  // the first 8 bytes of the target buffer with the 8 bytes that were there,
  // thus no header data is changed and the rest of the payload is handled
  // properly.
  ffi_raw *buffer = reinterpret_cast<ffi_raw *>(p->buffer - sizeof(void*));
  argps[0] = buffer;

  // Copy the individual vaargs.
  for (int i = 1, e = n; i < e; ++i) {
    argps[i] = va_arg(*args, void*);
  }

  // Use ffi to copy them to the buffer.
  ffi_ptrarray_to_raw(const_cast<ffi_cif*>(cif), argps, buffer);
}

static hpx_parcel_t *_new_ffi_0(const void *obj, hpx_addr_t addr,
                                hpx_addr_t c_addr, hpx_action_t c_action,
                                int n, va_list *args) {
  const action_t *action = static_cast<const action_t *>(obj);
  hpx_action_t id = *action->id;
  hpx_pid_t pid = hpx_thread_current_pid();
  return parcel_new(addr, id, c_addr, c_action, pid, NULL, 0);
}

static hpx_parcel_t *_new_ffi_n(const void *obj, hpx_addr_t addr,
                                hpx_addr_t c_addr, hpx_action_t c_action,
                                int n, va_list *args) {
  const action_t *action = static_cast<const action_t *>(obj);
  hpx_action_t id = *action->id;
  hpx_pid_t pid = hpx_thread_current_pid();
  ffi_cif *cif = static_cast<ffi_cif *>(action->env);
  size_t bytes = ffi_raw_size(cif);
  hpx_parcel_t *p = parcel_new(addr, id, c_addr, c_action, pid, NULL, bytes);
  _pack_ffi_n(obj, p, n, args);
  return p;
}

static hpx_parcel_t *_new_pinned_ffi_n(const void *obj, hpx_addr_t addr,
                                       hpx_addr_t c_addr, hpx_action_t c_action,
                                       int n, va_list *args) {
  const action_t *action = static_cast<const action_t *>(obj);
  hpx_action_t id = *action->id;
  hpx_pid_t pid = hpx_thread_current_pid();
  // The ffi cif thinks we're going to send one more pointer than we're actually
  // going to send, so we adjust the size before parcel allocation.
  ffi_cif *cif = static_cast<ffi_cif *>(action->env);
  size_t bytes = ffi_raw_size(cif) - sizeof(void*);
  hpx_parcel_t *p = parcel_new(addr, id, c_addr, c_action, pid, NULL, bytes);
  _pack_pinned_ffi_n(obj, p, n, args);
  return p;
}

static int _exec_ffi_n(const void *obj, hpx_parcel_t *p) {
  const action_t *action = static_cast<const action_t *>(obj);
  char ffiret[8];               // https://github.com/atgreen/libffi/issues/35
  int *ret = (int*)&ffiret[0];
  ffi_raw *args = static_cast<ffi_raw *>(hpx_parcel_get_data(p));
  ffi_cif *cif = static_cast<ffi_cif*>(action->env);
  ffi_raw_call(cif, action->handler, ret, args);
  return *ret;
}

static int _exec_pinned_ffi_n(const void *obj, hpx_parcel_t *p) {
  void *target;
  if (!hpx_gas_try_pin(p->target, &target)) {
    log_action("pinned action resend.\n");
    return HPX_RESEND;
  }

  const action_t *action = static_cast<const action_t *>(obj);

  // We need to pack a void* array from the parcel data buffer using the ffi cif
  // information. ffi thinks that we have one more parameter then we sent in the
  // parcel though, the pinned pointer. We roll-back the pointer we give to ffi
  // to deal with this---it reads garbage into the first element but we then
  // overwrite that with the pointer to the pinned target, so we get the right
  // value in the end.
  ffi_cif *cif = static_cast<ffi_cif *>(action->env);
  ffi_raw *args = reinterpret_cast<ffi_raw *>(p->buffer - sizeof(void*));
  void *avalue[cif->nargs];
  ffi_raw_to_ptrarray(cif, args, avalue);
  avalue[0] = &target;
  char ffiret[8];               // https://github.com/atgreen/libffi/issues/35
  int *ret = (int*)&ffiret[0];
  ffi_call(cif, action->handler, ret, avalue);
  hpx_gas_unpin(p->target);
  return *ret;
}

static void _ffi_finish(void *act) {
  action_t *action = static_cast<action_t *>(act);
  log_action("%d: %s (%p) %s %x.\n", *action->id, action->key,
             (void*)(uintptr_t)action->handler,
             HPX_ACTION_TYPE_TO_STRING[action->type],
             action->attr);
}

static void _ffi_fini(void *act) {
  action_t *action = static_cast<action_t *>(act);
  ffi_cif *cif = static_cast<ffi_cif *>(action->env);
  free(cif->arg_types);
  free(action->env);
}

static const parcel_management_vtable_t _ffi_0_vtable = {
  .exec_parcel = _exec_ffi_n,
  .pack_parcel = _pack_ffi_0,
  .new_parcel = _new_ffi_0,
  .exit = exit_action
};

static const parcel_management_vtable_t _pinned_ffi_0_vtable = {
  .exec_parcel = _exec_pinned_ffi_n,
  .pack_parcel = _pack_ffi_0,
  .new_parcel = _new_ffi_0,
  .exit = exit_pinned_action
};

static const parcel_management_vtable_t _ffi_n_vtable = {
  .exec_parcel = _exec_ffi_n,
  .pack_parcel = _pack_ffi_n,
  .new_parcel = _new_ffi_n,
  .exit = exit_action
};

static const parcel_management_vtable_t _pinned_ffi_n_vtable = {
  .exec_parcel = _exec_pinned_ffi_n,
  .pack_parcel = _pack_pinned_ffi_n,
  .new_parcel = _new_pinned_ffi_n,
  .exit = exit_pinned_action
};

void action_init_ffi(action_t *action, int n, va_list *vargs) {
  // Translate the argument types into a stack allocated buffer suitable for use
  // with ffi.
  hpx_type_t *args = static_cast<hpx_type_t *>(calloc(n, sizeof(args[0])));
  for (int i = 0; i < n; ++i) {
    args[i] = va_arg(*vargs, hpx_type_t);
  }

  // Check to make sure that pinned actions start with a pointer type.
  uint32_t pinned = action->attr & HPX_PINNED;
  if (pinned && (args[0] != HPX_POINTER)) {
    dbg_error("First type of a pinned action should be HPX_POINTER\n");
  }

  // Allocate and initialize an ffi_cif, which is the structure that ffi uses to
  // encode calling conventions.
  action->env = calloc(1, sizeof(ffi_cif));
  dbg_assert(action->env);

  ffi_cif *cif = static_cast<ffi_cif *>(action->env);
  ffi_status s = ffi_prep_cif(cif, FFI_DEFAULT_ABI, n, HPX_INT, args);
  if (s != FFI_OK) {
    dbg_error("failed to process type information for action id %d.\n",
              *action->id);
  }

  // Initialize the parcel class.
  if (pinned && cif->nargs > 1) {
    action->parcel_class = &_pinned_ffi_n_vtable;
  }
  else if (pinned) {
    action->parcel_class = &_pinned_ffi_0_vtable;
  }
  else if (cif->nargs) {
    action->parcel_class = &_ffi_n_vtable;
  }
  else {
    action->parcel_class = &_ffi_0_vtable;
  }

  // Initialize the action class.
  action_init_call_by_parcel(action);

  // Initialize the destructor.
  action->fini = _ffi_fini;
  action->finish = _ffi_finish;
}
