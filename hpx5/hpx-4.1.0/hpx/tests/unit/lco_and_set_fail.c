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

static int _and_set_handler(hpx_addr_t lco) {
  hpx_lco_set(lco, 0, NULL, HPX_NULL, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _and_set, _and_set_handler, HPX_ADDR);

static int _and_wait_handler(hpx_addr_t lco) {
  hpx_lco_wait(lco);
  hpx_lco_delete_sync(lco);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _and_wait, _and_wait_handler, HPX_ADDR);

static int lco_and_set_fail_handler(void) {
  hpx_addr_t lco = hpx_lco_and_new(8);
  for (int i = 0; i < 16; ++i) {
    hpx_call_sync(HPX_HERE, _and_set, NULL, 0, &lco);
    if (i == 7) {
      hpx_call_sync(HPX_HERE, _and_wait, NULL, 0, &lco);
    }
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_and_set_fail, lco_and_set_fail_handler);

TEST_MAIN({
 ADD_TEST(lco_and_set_fail);
});
