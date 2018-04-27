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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "hpx/hpx.h"
#include "tests.h"

// Goal of this testcase is to use the user-defined LCO to achieve
// the “OR” gate

static void _lco_init_handler(bool *val, const size_t size,
                              int *init, size_t init_size) {
  *val = init ? *init : 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, _lco_init, _lco_init_handler);

// Update *lhs with the or gate value
static void _lco_op_handler(bool *val, const bool *new, const size_t size) {
  *val ^= *new;
}
static HPX_ACTION(HPX_FUNCTION, 0, _lco_op, _lco_op_handler);

// A predicate that "guards" the LCO.
// This has to return true as soon as soon as first one gets set to
// true.
static bool _lco_predicate_handler(bool *val, const size_t size) {
  assert(val);
  return (*val);
}
static HPX_ACTION(HPX_FUNCTION, 0, _lco_predicate, _lco_predicate_handler);

static int _lco_get_handler(void) {
  hpx_addr_t addr = hpx_thread_current_target();
  hpx_lco_wait(addr);
  hpx_lco_reset_sync(addr);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _lco_get, _lco_get_handler);

static int _lco_set_handler(int i) {
  hpx_addr_t addr = hpx_thread_current_target();
  int val = (i == 15) ? 1 : (0 == (rand() % 5));
  hpx_lco_set(addr, sizeof(val), &val, HPX_NULL, HPX_NULL);
  if (val == 0) {
    hpx_lco_error_sync(addr, HPX_LCO_ERROR);
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _lco_set, _lco_set_handler, HPX_INT);

static int lco_user_handler(void) {
  printf("Test user lco.\n");
  srand(time(NULL));
  hpx_addr_t lco;
  int init = 0;
  lco = hpx_lco_user_new(sizeof(int), _lco_init, _lco_op, _lco_predicate,
                         &init, sizeof(init));
  for (int i = 0; i < 16; ++i) {
    hpx_addr_t and = hpx_lco_and_new(2);
    hpx_call(lco, _lco_set, and, &i);
    hpx_call(lco, _lco_get, and);
    hpx_lco_wait(and);
    hpx_lco_delete(and, HPX_NULL);
    hpx_lco_reset_sync(lco);
  }
  hpx_lco_delete(lco, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_user, lco_user_handler);

TEST_MAIN({
 ADD_TEST(lco_user, 0);
});
