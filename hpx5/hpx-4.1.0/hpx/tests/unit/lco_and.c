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

static int _and_set_handler(void) {
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _and_set, _and_set_handler);

static int lco_and_handler(void) {
  printf("Test hpx_lco_and\n");
  hpx_addr_t lco = hpx_lco_and_new(8);
  for (int i = 0; i < 8; ++i) {
    hpx_call(HPX_HERE, _and_set, lco);
  }
  hpx_lco_wait(lco);
  hpx_lco_delete(lco, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_and, lco_and_handler);

static int _and_set_num_handler(hpx_addr_t lco) {
  hpx_lco_and_set_num(lco, 4, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _and_set_num, _and_set_num_handler, HPX_ADDR);

static int lco_and_num_handler(void) {
  printf("Test hpx_lco_and_set_num\n");
  hpx_addr_t lco = hpx_lco_and_new(8);
  for (int i = 0; i < 4; ++i) {
    hpx_call(HPX_HERE, _and_set, lco);
  }
  hpx_call(HPX_HERE, _and_set_num, HPX_NULL, &lco);
  hpx_lco_wait(lco);
  hpx_lco_delete(lco, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_and_num, lco_and_num_handler);

TEST_MAIN({
 ADD_TEST(lco_and, 0);
 ADD_TEST(lco_and_num, 0);
});
