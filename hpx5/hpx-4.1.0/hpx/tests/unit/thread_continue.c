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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include "hpx/hpx.h"
#include "tests.h"
#include "libhpx/locality.h"

#define NUM_THREADS 5
#define ARRAY_SIZE 100
#define BUFFER_SIZE 128

const int DATA_SIZE = sizeof(uint64_t);
const int SET_CONT_VALUE = 1234;

// Finish the current thread's execution, sending value to the thread's
// continuation address (size is the size of the value and value is the value
// to be sent to the thread's continuation address.
static int _set_cont_handler(void) {
  uint64_t value = SET_CONT_VALUE;
  return hpx_thread_continue(&value, DATA_SIZE);
}
static HPX_ACTION(HPX_DEFAULT, 0, _set_cont, _set_cont_handler);

static int thread_continue_handler(void) {
  printf("Starting the Thread continue test\n");
  // Start the timer
  hpx_time_t t1 = hpx_time_now();

  hpx_addr_t *cont_fut = calloc(hpx_get_num_ranks(), sizeof(hpx_addr_t));

  for (int i = 0; i < hpx_get_num_ranks(); i++) {
    cont_fut[i] = hpx_lco_future_new(DATA_SIZE);
    hpx_parcel_t *p = hpx_parcel_acquire(NULL, 0);
    hpx_parcel_set_target(p, HPX_THERE(i));
    hpx_parcel_set_action(p, _set_cont);
    hpx_parcel_set_cont_target(p, cont_fut[i]);
    hpx_parcel_set_cont_action(p, hpx_lco_set_action);
    hpx_parcel_send(p, HPX_NULL);
    printf("Sending action with continuation to %d\n", i);
  }

  for (int i = 0; i < hpx_get_num_ranks(); i++) {
    uint64_t result;
    printf("Waiting on continuation to %d\n", i);
    hpx_lco_get(cont_fut[i], DATA_SIZE, &result);
    printf("Received continuation from %d with value %" PRIu64 "\n", i, result);
    assert(result == SET_CONT_VALUE);
    hpx_lco_delete(cont_fut[i], HPX_NULL);
  }

  free(cont_fut);

  printf(" Elapsed: %g\n", hpx_time_elapsed_ms(t1));
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, thread_continue, thread_continue_handler);

static int _interrupt_continue_handler(void) {
  uint64_t value = SET_CONT_VALUE;
  return hpx_thread_continue(&value, DATA_SIZE);
}
static HPX_ACTION(HPX_INTERRUPT, 0, _interrupt_continue,
                  _interrupt_continue_handler);

static int _test_interrupt_handler(void) {
  uint64_t val = 0;
  CHECK( hpx_call_sync(HPX_HERE, _interrupt_continue, &val, sizeof(val)) );
  test_assert(val == 1234);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_interrupt, _test_interrupt_handler);

static int _generate_continue_handler(void) {
  uint64_t value = SET_CONT_VALUE;
  hpx_parcel_t *p = hpx_thread_generate_continuation(&value, sizeof(value));
  hpx_parcel_send_sync(p);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _generate_continue,
                  _generate_continue_handler);

static int _test_generate_handler(void) {
    uint64_t val = 0;
  CHECK( hpx_call_sync(HPX_HERE, _generate_continue, &val, sizeof(val)) );
  test_assert(val == 1234);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_generate, _test_generate_handler);


TEST_MAIN({
  ADD_TEST(thread_continue, 0);
  ADD_TEST(_test_interrupt, 0);
  ADD_TEST(_test_generate, 0);
});
