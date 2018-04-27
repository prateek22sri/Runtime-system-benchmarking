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
#include "tests.h"

static int
_new_future_handler(void) {
  hpx_addr_t f = hpx_lco_future_new(sizeof(int));
  return hpx_thread_continue(&f, sizeof(f));
}
static HPX_ACTION(HPX_TASK, 0, _new_future, _new_future_handler);

static int
_lco_get_remote_handler(void) {
  int rank = (HPX_LOCALITY_ID + 1) % HPX_LOCALITIES;
  hpx_addr_t there = HPX_THERE(rank);
  hpx_addr_t lco;
  int e = hpx_call_sync(there, _new_future, &lco, sizeof(lco));
  assert(e == HPX_SUCCESS);
  int i = 42;
  e = hpx_call(lco, hpx_lco_set_action, HPX_NULL, &i, sizeof(i));
  assert(e == HPX_SUCCESS);
  i = 0;
  e = hpx_lco_get(lco, sizeof(i), &i);
  assert(e == HPX_SUCCESS);
  assert(i = 42);
  return hpx_call_cc(lco, hpx_lco_delete_action);
}
static HPX_ACTION(HPX_DEFAULT, 0, _lco_get_remote, _lco_get_remote_handler);

TEST_MAIN({
    ADD_TEST(_lco_get_remote, 0);
  });
