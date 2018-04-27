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

#include "init.h"
#include "libhpx/debug.h"
#include "libhpx/locality.h"
#include "libhpx/padding.h"
#include "hpx/hpx.h"
#include <sys/mman.h>
#include <cstdlib>
#include <cstring>

/// A static action table.
///
/// We currently need to be able to register actions before we call hpx_init()
/// because we use constructors inside of libhpx to do action registration. We
/// expose this action table to be used for that purpose.
static int _n = 1;
HPX_ALIGNED(HPX_PAGE_SIZE) action_t actions[LIBHPX_ACTION_MAX];

static void HPX_CONSTRUCTOR _init_null_handler(void) {
  actions[0].handler = NULL;
  actions[0].id = NULL;
  actions[0].key = "";
  actions[0].type = HPX_DEFAULT;
  actions[0].attr = UINT32_C(0);
  actions[0].env = NULL;
}

int action_table_size(void) {
  return _n;
}

#ifdef ENABLE_DEBUG
void CHECK_ACTION(hpx_action_t id) {
  if (id == HPX_ACTION_INVALID) {
    dbg_error("action registration is not complete");
  }
  else if (id >= _n) {
    dbg_error("action id, %d, out of bounds [0,%u)\n", id, _n);
  }
}
#endif

/// Compare two entries by their keys.
///
/// This is used to sort the action table during finalization, so that we can
/// uniformly assign ids to actions independent of which region in the local
/// address space the functions are loaded into.
///
/// @param          lhs A pointer to the left-hand entry.
/// @param          rhs A pointer tot he right-hand entry.
///
/// @return             The lexicographic comparison of the entries' keys.
static int _cmp_keys(const void *lhs, const void *rhs) {
  const action_t *el = static_cast<const action_t *>(lhs);
  const action_t *er = static_cast<const action_t *>(rhs);

  // if either the left or right entry's id is NULL, that means it is
  // our reserved action (user-registered actions can never have the
  // ID as NULL), and we always want it to be "less" than any other
  // registered action.
  if (el->id == NULL) {
    return -1;
  } else if (er->id == NULL) {
    return 1;
  } else {
    return strcmp(el->key, er->key);
  }
}

/// Sort the actions in an action table by their key.
static void _sort_entries(void) {
  qsort(&actions, _n, sizeof(action_t), _cmp_keys);
}

/// Assign all of the entry ids in the table.
static void _assign_ids(void) {
  for (int i = 1, e = _n; i < e; ++i) {
    *actions[i].id = i;
  }
}

void action_registration_finalize(void) {
  _sort_entries();
  _assign_ids();

  for (int i = 1, e = _n; i < e; ++i) {
    actions[i].finish(&actions[i]);
  }

  // this is a sanity check to ensure that the reserved "null" action
  // is still at index 0.
  dbg_assert(actions[0].id == NULL);

#ifndef HAVE_HUGETLBFS
  size_t bytes = _n * sizeof(actions[0]);
  bytes += _BYTES(HPX_PAGE_SIZE, bytes);
  int e1 = mprotect(&actions, bytes, PROT_READ);
  if (e1) {
    log_error("could not protect the action table\n");
  }
#endif
}

void action_table_finalize(void) {
  for (int i = 1, e = _n; i < e; ++i) {
    actions[i].fini(&actions[i]);
  }
}

/// Insert an action into the table.
///
/// @param         type The type of this action.
/// @param         attr The attributes associated with this action.
/// @param          key The unique key for this action; read in _sort_entries().
/// @param           id The address of the user's id; written in _assign_ids().
/// @param       system A flag indicating if this is a system or user action.
/// @param            n The number of args.
/// @param         args The args.
static void _register_action_va(hpx_action_type_t type, uint32_t attr,
                                const char *key, hpx_action_t *id,
                                int system, int n, va_list *args) {
  dbg_assert(id);
  dbg_assert(n > 0);
  *id = HPX_ACTION_INVALID;

  if (system) {
    attr |= HPX_INTERNAL;
  }

  action_t *back = &actions[_n++];
  if (unsigned(_n) >= LIBHPX_ACTION_MAX) {
    dbg_error("action table overflow\n");
  }

  // consume the first argument here---it's currently always a handler
  back->handler = reinterpret_cast<handler_t>(va_arg(*args, void*));
  n--;

  back->id = id;
  back->key = key;
  back->type = type;
  back->attr = attr;
  back->env = NULL;
  action_init(back, n, args);
}

void libhpx_register_action(hpx_action_type_t type, uint32_t attr,
                            const char *key, hpx_action_t *id, unsigned n, ...)
{
  va_list args;
  va_start(args, n);
  _register_action_va(type, attr, key, id, 1, n, &args);
  va_end(args);
}

int hpx_register_action(hpx_action_type_t type, uint32_t attr, const char *key,
                        hpx_action_t *id, unsigned n, ...) {
  va_list args;
  va_start(args, n);
  _register_action_va(type, attr, key, id, 0, n, &args);
  va_end(args);
  return HPX_SUCCESS;
}
