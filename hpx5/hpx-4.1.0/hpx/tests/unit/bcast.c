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

#include "hpx/hpx.h"
#include "tests.h"

static int _bcast_untyped_handler(void) {
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _bcast_untyped, _bcast_untyped_handler);

static hpx_action_t _bcast_typed;
static int _bcast_typed_handler(int i, float f, char c) {
  printf("Typed action: %d %f %c!\n", i, f, c);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _bcast_typed, _bcast_typed_handler,
                  HPX_INT, HPX_FLOAT, HPX_CHAR);

static int bcast_handler(void) {
  printf("Test hpx_bcast (untyped)\n");
  hpx_addr_t lco = hpx_lco_future_new(0);
  int e = hpx_bcast(_bcast_untyped, lco, HPX_NULL);
  assert(e == HPX_SUCCESS);
  hpx_lco_wait(lco);
  hpx_lco_delete(lco, HPX_NULL);

  printf("Test hpx_bcast_lsync (untyped)\n");
  e = hpx_bcast_lsync(_bcast_untyped, HPX_NULL);
  assert(e == HPX_SUCCESS);

  int i = 42;
  float f = 1.0;
  char c = 'a';

  printf("Test hpx_bcast (typed)\n");
  lco = hpx_lco_future_new(0);
  e = hpx_bcast(_bcast_typed, lco, HPX_NULL, &i, &f, &c);
  assert(e == HPX_SUCCESS);
  e = hpx_lco_wait(lco);
  assert(e == HPX_SUCCESS);
  hpx_lco_delete(lco, HPX_NULL);

  printf("Test hpx_bcast_lsync (typed)\n");
  e = hpx_bcast_lsync(_bcast_typed, HPX_NULL, &i, &f, &c);
  assert(e == HPX_SUCCESS);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, bcast, bcast_handler);

TEST_MAIN({
    ADD_TEST(bcast, 0);
});
