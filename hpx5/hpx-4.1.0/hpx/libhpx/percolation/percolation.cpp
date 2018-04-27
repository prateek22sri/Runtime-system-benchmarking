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

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <hpx/hpx.h>
#include <libhpx/action.h>
#include <libhpx/config.h>
#include <libhpx/debug.h>
#include <libhpx/libhpx.h>
#include <libhpx/locality.h>
#include <libhpx/parcel.h>
#include <libhpx/percolation.h>


static const char *_dummy_id(void) {
  return "dummy";
}

static void *_dummy_prepare(const void *obj, const char *key, const char *kern)
{
  return NULL;
}

static void _dummy_deallocate(void* obj) {
}

static int _dummy_execute(const void *obj, void *o, int n, void **v, size_t *s)
{
  return 0;
}

static void _dummy_destroy(const void *obj, void *o) {
}

static percolation_t _dummy = {
  .id = _dummy_id,
  .deallocate = _dummy_deallocate,
  .prepare = _dummy_prepare,
  .execute = _dummy_execute,
  .destroy = _dummy_destroy
};

percolation_t *percolation_new(void) {
#ifdef HAVE_OPENCL
  return percolation_new_opencl();
#endif
  return &_dummy;
}

int percolation_execute_handler(int nargs, void *vargs[], size_t sizes[]) {
  const hpx_parcel_t *p = hpx_thread_current_parcel();

  percolation_t *percolation = static_cast<percolation_t*>(here->percolation);
  dbg_assert(percolation);

  CHECK_ACTION(p->action);
  void *obj = actions[p->action].env;
  if (!obj) {
    return HPX_ERROR;
  }

  int e = percolation_execute(percolation, obj, nargs, vargs, sizes);
  return e;
}
LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_VECTORED,
              percolation_execute_action, percolation_execute_handler,
              HPX_INT, HPX_POINTER, HPX_POINTER);
