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

hpx_action_handler_t hpx_action_get_handler(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *entry = &actions[id];
  return (hpx_action_handler_t)entry->handler;
}
