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

#ifndef LIBHPX_ACTIONS_INIT_H
#define LIBHPX_ACTIONS_INIT_H

#include "libhpx/action.h"
#include "hpx/hpx.h"

void action_init(action_t *action, int n, va_list *args);

void action_init_marshalled(action_t *action, int n, va_list *args);
void action_init_ffi(action_t *action, int n, va_list *args);
void action_init_vectored(action_t *action, int n, va_list *args);

void action_init_call_by_parcel(action_t *action);

#endif // LIBHPX_ACTIONS_INIT_H
