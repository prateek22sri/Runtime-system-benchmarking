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

// Goal of this testcase is to test the HPX LCO Generation counter
//
// The generation counter is simply a counter where threads can wait on specific
// values of the counter. In order to test this functionality we will
// concurrently spawn 100 generations worth of waiting threads from around the
// system, while at the same time setting the counter.
//
// At the

#include <hpx/hpx.h>
#include "tests.h"

static const int DEPTH = 100;
static const int NINPLACE = 9;


// A call_cc cleanup handler to print out a message.
static void _done(void* message) {
  printf("done %s\n", (char*)message);
}

// Testcase for gencount LCO.
static int _increment_handler(void) {
  hpx_addr_t counter = hpx_thread_current_target();
  hpx_lco_gencount_inc(counter, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _increment, _increment_handler);

static int _wait_handler(int i) {
  hpx_addr_t counter = hpx_thread_current_target();
  return hpx_lco_gencount_wait(counter, i);
}
static HPX_ACTION(HPX_DEFAULT, 0, _wait, _wait_handler, HPX_INT);

static int _seed_handler(int n, hpx_addr_t counter, hpx_addr_t done) {
  for (int i = 0; i < n; ++i) {
    hpx_xcall(counter, _wait, done, i);
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _seed, _seed_handler, HPX_INT, HPX_ADDR,
                  HPX_ADDR);

static int _set_handler(int n, hpx_addr_t counter, hpx_addr_t done) {
  assert((HPX_LOCALITIES*(n/HPX_LOCALITIES))+(n%HPX_LOCALITIES)==n);

  for (int i = 0, e = n / HPX_LOCALITIES; i < e; ++i) {
    if (i & 1) {
      hpx_call(counter, _increment, done);
    }
    else {
      hpx_call(counter, hpx_lco_set_action, done, NULL, 0);
    }
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _set, _set_handler, HPX_INT, HPX_ADDR,
                  HPX_ADDR);

static int _single_wait(int inplace) {
  int CORRECTED_DEPTH = HPX_LOCALITIES*(DEPTH/HPX_LOCALITIES);  
  hpx_addr_t counter = hpx_lco_gencount_new(inplace);
  hpx_addr_t and = hpx_lco_and_new(1);
  hpx_addr_t done = hpx_lco_and_new(CORRECTED_DEPTH);
  int end = CORRECTED_DEPTH - 1;
  hpx_xcall(counter, _wait, and, end);
  hpx_bcast(_set, HPX_NULL, HPX_NULL, &CORRECTED_DEPTH, &counter, &done);
  hpx_lco_wait(and);
  hpx_lco_wait(done);

  hpx_addr_t cleanup = hpx_lco_and_new(3);
  hpx_lco_delete(and, cleanup);
  hpx_lco_delete(counter, cleanup);
  hpx_lco_delete(done, cleanup);
  int e = hpx_call_cc(cleanup, hpx_lco_delete_action);
  _done("");
  return e;
}

static int _multi_wait(int inplace) {
  int CORRECTED_DEPTH = HPX_LOCALITIES*(DEPTH/HPX_LOCALITIES); 
  // allocate the counter
  hpx_addr_t counter = hpx_lco_gencount_new(inplace);

  // make sure we don't delete anything until all of the sets are done
  hpx_addr_t sets = hpx_lco_and_new(CORRECTED_DEPTH);

  // we will seed the counter with DEPTH * LOCALITIES waiting threads
  hpx_addr_t done = hpx_lco_and_new(CORRECTED_DEPTH * HPX_LOCALITIES);

  // broadcast the seed---lots of asynchronous stuff here
  hpx_bcast(_seed, HPX_NULL, HPX_NULL, &CORRECTED_DEPTH, &counter, &done);

  // broadcast the sets---lots of asynchronous stuff here too
  hpx_bcast(_set, HPX_NULL, HPX_NULL, &CORRECTED_DEPTH, &counter, &sets);

  // all of the seeds should wake up and signal done
  hpx_lco_wait(done);
  hpx_lco_wait(sets);

  hpx_addr_t cleanup = hpx_lco_and_new(3);
  hpx_lco_delete(done, cleanup);
  hpx_lco_delete(counter, cleanup);
  hpx_lco_delete(sets, cleanup);
  int e = hpx_call_cc(cleanup, hpx_lco_delete_action);
  _done("");
  return e;
}

static int _single_wait_0_handler(void) {
  printf("Starting _single_wait_0\n");
  return _single_wait(0);
}
static HPX_ACTION(HPX_DEFAULT, 0, _single_wait_0, _single_wait_0_handler);

static int _single_wait_N_handler(void) {
  printf("Starting _single_wait_N\n");
  return _single_wait(NINPLACE);
}
static HPX_ACTION(HPX_DEFAULT, 0, _single_wait_N, _single_wait_N_handler);

static int _multi_wait_0_handler(void) {
  printf("Starting _multi_wait_0\n");
  return _multi_wait(0);
}
static HPX_ACTION(HPX_DEFAULT, 0, _multi_wait_0, _multi_wait_0_handler);

static int _multi_wait_N_handler(void) {
  printf("Starting _multi_wait_N\n");
  return _multi_wait(NINPLACE);
}
static HPX_ACTION(HPX_DEFAULT, 0, _multi_wait_N, _multi_wait_N_handler);

TEST_MAIN({
    ADD_TEST(_single_wait_0, 0);
    ADD_TEST(_single_wait_N, 0);
    ADD_TEST(_multi_wait_0, 0);
    ADD_TEST(_multi_wait_N, 0);
});
