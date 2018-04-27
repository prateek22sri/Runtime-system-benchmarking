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

// hpx_thread_get_tls_id() Generates a consecutive new ID for a thread
// The first time this is called in a leightweight thread, it assigns
// the next available ID. Each time it's called after that it returns
// the same ID.
static int _assignID_handler(void) {
  hpx_thread_get_tls_id();
  hpx_thread_get_tls_id();
 // int tid = hpx_thread_get_tls_id();
 // int consecutiveID = hpx_thread_get_tls_id();
  //printf("First time generated ID: %d, consecutive new ID:  %d\n", tid,
  //                                     consecutiveID);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _assignID, _assignID_handler);

static int thread_gettlsid_handler(void) {
  printf("Starting the Threads ID generation test\n");
  // Start the timer
  hpx_time_t t1 = hpx_time_now();

  hpx_addr_t done = hpx_lco_and_new(NUM_THREADS);

  // HPX Threads are spawned as a result of hpx_parcel_send() / hpx_parcel_
  // sync().
  for (int t = 0; t < NUM_THREADS; t++) {
    hpx_parcel_t *p = hpx_parcel_acquire(NULL, 0);

    // Set the target address and action for the parcel
    hpx_parcel_set_target(p, HPX_THERE(t % hpx_get_num_ranks()));
    hpx_parcel_set_action(p, _assignID);

    // Set the continuation target and action for parcel
    hpx_parcel_set_cont_target(p, done);
    hpx_parcel_set_cont_action(p, hpx_lco_set_action);

    // and send the parcel, this spawns the HPX thread
    hpx_parcel_send(p, HPX_NULL);
  }

  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);

  printf(" Elapsed: %g\n", hpx_time_elapsed_ms(t1));
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, thread_gettlsid, thread_gettlsid_handler);

TEST_MAIN({
  ADD_TEST(thread_gettlsid, 0);
});
