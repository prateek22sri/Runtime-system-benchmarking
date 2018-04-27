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

// hpx_thread_current_cont_action gets the continuation action for the current
// thread
static int _thread_current_cont_target_handler(void) {
  hpx_action_t c_action = hpx_thread_current_cont_action();
  hpx_addr_t c_target = hpx_thread_current_cont_target();
  hpx_call(c_target, c_action, HPX_NULL, NULL, 0);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _thread_current_cont_target,
                  _thread_current_cont_target_handler);

static int thread_cont_action_handler(void) {
  printf("Starting the Thread continue target and action test\n");
  // Start the timer
  hpx_time_t t1 = hpx_time_now();

  hpx_addr_t *cont_and = calloc(hpx_get_num_ranks(), sizeof(hpx_addr_t));

  for (int i = 0; i < hpx_get_num_ranks(); i++) {
    cont_and[i] = hpx_lco_and_new(2);
    hpx_parcel_t *p = hpx_parcel_acquire(NULL, DATA_SIZE);
    hpx_parcel_set_target(p, HPX_THERE(i));
    hpx_parcel_set_action(p, _thread_current_cont_target);
    hpx_parcel_set_cont_target(p, cont_and[i]);
    hpx_parcel_set_cont_action(p, hpx_lco_set_action);
    hpx_parcel_send_sync(p);
    printf("Started index %d.\n", i);
  }

  for (int i = 0; i < hpx_get_num_ranks(); i++) {
    hpx_lco_wait(cont_and[i]);
    printf("Received continuation from %d\n",i);
    hpx_lco_delete(cont_and[i], HPX_NULL);
  }

  free(cont_and);

  printf(" Elapsed: %g\n", hpx_time_elapsed_ms(t1));
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, thread_cont_action, thread_cont_action_handler);

TEST_MAIN({
  ADD_TEST(thread_cont_action, 0);
});
