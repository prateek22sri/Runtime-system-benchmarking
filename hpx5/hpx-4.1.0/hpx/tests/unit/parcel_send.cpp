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

#include "tests.h"
#include "hpx/hpx.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <atomic>

static std::atomic<int> counter;

// This testcase tests hpx_parcel_send function, which sends a parcel with
// asynchronout local completion symantics, hpx_parcel_set_cont_action - set
// the continuous action, hpx_pargel_set_cont_target - set the continuous
// address for a parcel.
static int _recv_handler(double *args, size_t n) {
  ++counter;
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _recv, _recv_handler);

static int parcel_send_handler(void) {
  printf("Testing the hpx parcel send function\n");
  int buffer[4] = {1, 100, 1000, 10000};
  int avg = 1000;

  for (int i = 0; i < 4; i++) {
    size_t size = sizeof(double) * buffer[i];
    double *buf = new double[buffer[i]];
    for (int j = 0; j < buffer[i]; j++) {
      buf[j] = rand() % 10000;
    }

    printf("%d, %d, %g: " , i, buffer[i], buf[i]);
    hpx_time_t t1 = hpx_time_now();

    // Set the lco for completing the entire loop
    hpx_addr_t completed = hpx_lco_and_new(avg);

    for(int k = 0; k < avg; k++) {
      // Set up a asynchronous parcel send
      hpx_addr_t send = hpx_lco_future_new(0);
      hpx_parcel_t *p = hpx_parcel_acquire(NULL, size);
      hpx_parcel_set_action(p, _recv);
      hpx_parcel_set_target(p, HPX_HERE);
      hpx_parcel_set_cont_action(p, hpx_lco_set_action);
      hpx_parcel_set_cont_target(p, completed);
      hpx_parcel_set_data(p, buf, size);
      hpx_parcel_send(p, send);

      // do the useless work
      double volatile d = 0.;
      for (int i = 0; i < 1000; i++) {
        d += 1./(2.*i+1.);
      }

      hpx_lco_wait(send);
      hpx_lco_delete(send, HPX_NULL);
    }

    hpx_lco_wait(completed);
    hpx_lco_delete(completed, HPX_NULL);

    double elapsed = hpx_time_elapsed_ms(t1);
    printf("Elapsed: %g\n", elapsed/avg);
    delete [] buf;
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, parcel_send, parcel_send_handler);

TEST_MAIN({
  ADD_TEST(parcel_send, 0);
});
