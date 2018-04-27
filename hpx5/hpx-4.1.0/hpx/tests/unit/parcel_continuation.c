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
#include <unistd.h>
#include "hpx/hpx.h"
#include "tests.h"

// Testcase to test the parcel continuation
static int _get_cont_value_handler(uint64_t *args, size_t n) {
  hpx_addr_t local = hpx_thread_current_target();
  uint64_t *value;
  if (!hpx_gas_try_pin(local, (void**)&value))
    return HPX_RESEND;

  memcpy(value, args, n);
  //printf("Value =  %"PRIu64"\n", *value);

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _get_cont_value,
                  _get_cont_value_handler, HPX_POINTER, HPX_SIZE_T);

static int parcel_get_continuation_handler(void) {
  printf("Testing parcel contination target and action\n");

  hpx_time_t t1 = hpx_time_now();

  hpx_addr_t addr = hpx_gas_alloc_cyclic(1, sizeof(uint64_t), sizeof(uint64_t));

  hpx_addr_t done = hpx_lco_and_new(1);
  hpx_parcel_t *p = hpx_parcel_acquire(NULL, sizeof(uint64_t));

  // Get access to the data, and fill it with the necessary data.
  uint64_t *result = hpx_parcel_get_data(p);
  *result = 1234;

  // Set the target address and action for the parcel
  hpx_parcel_set_target(p, addr);
  hpx_parcel_set_action(p, _get_cont_value);

  // Set the continuation target and action for the parcel
  hpx_parcel_set_cont_target(p, done);
  hpx_parcel_set_cont_action(p, hpx_lco_set_action);

  hpx_action_t get_act = hpx_parcel_get_cont_action(p);
  assert_msg(get_act == hpx_lco_set_action,
                "Error in getting cont action");

  assert(hpx_parcel_get_cont_target(p) == done);

  // Send the parcel
  hpx_parcel_send(p, HPX_NULL);

  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);
  hpx_gas_free(addr, HPX_NULL);

  printf("Elapsed: %g\n", hpx_time_elapsed_ms(t1));
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, parcel_get_continuation,
                  parcel_get_continuation_handler);

TEST_MAIN({
  ADD_TEST(parcel_get_continuation, 0);
});
