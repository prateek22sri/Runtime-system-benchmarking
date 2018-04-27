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
#include <cinttypes>

void action_init(action_t *action, int n, va_list *args) {
  uint32_t attr = action->attr & (HPX_MARSHALLED | HPX_VECTORED);
  switch (attr) {
   case (HPX_ATTR_NONE):
    action_init_ffi(action, n, args);
    return;
   case (HPX_MARSHALLED):
    action_init_marshalled(action, n, args);
    return;
   case (HPX_MARSHALLED | HPX_VECTORED):
    action_init_vectored(action, n, args);
    return;
  }
  dbg_error("Could not initialize action for attr %" PRIu32 "\n", attr);
}
