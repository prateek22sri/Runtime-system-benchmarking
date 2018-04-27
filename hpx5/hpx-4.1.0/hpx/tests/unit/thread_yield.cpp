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
#include "libhpx/locality.h"
#include "hpx/hpx.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cinttypes>
#include <atomic>

#define NUM_THREADS 5
#define ARRAY_SIZE 100
#define BUFFER_SIZE 128

const int DATA_SIZE = sizeof(uint64_t);
const int SET_CONT_VALUE = 1234;

// hpx_thread_yield()
struct _yield_args {
  std::atomic<size_t> *counter;
  size_t limit;
  double time_limit;
};

static int _yield_worker_handler(struct _yield_args *args, size_t n) {
  // int num =
  args->counter->fetch_add(1);

  uint64_t timeout = false;
  hpx_time_t start_time = hpx_time_now();
  while (*args->counter < args->limit) {
    if (hpx_time_elapsed_ms(start_time) > args->time_limit) {
      timeout = true;
      break;
    }
    hpx_thread_yield();
    // printf("Thread %d yielding after %f ms.\n", num, hpx_time_elapsed_ms(start_time));
  }

  // printf("Thread %d done after %f ms.\n", num, hpx_time_elapsed_ms(start_time));
  return HPX_THREAD_CONTINUE(timeout);
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _yield_worker,
                  _yield_worker_handler, HPX_POINTER, HPX_SIZE_T);

static int thread_yield_handler(void) {
  unsigned num_threads = (unsigned)hpx_get_num_threads();
  std::atomic<size_t> counter;

  struct _yield_args args = {
    .counter = &counter,
    .limit = num_threads + 1,
    .time_limit = 5000.0
  };
  hpx_addr_t *done = new hpx_addr_t [num_threads + 1];


  // now spawn num_threads + 1 num_threads
  for (int i = 0; i < num_threads + 1; i++) {
    done[i] = hpx_lco_future_new(sizeof(uint64_t));
    hpx_parcel_t *p = hpx_parcel_acquire(NULL, sizeof(args));
    hpx_parcel_set_action(p, _yield_worker);
    hpx_parcel_set_target(p, HPX_HERE);
    hpx_parcel_set_data(p, &args, sizeof(args));
    hpx_parcel_set_cont_target(p, done[i]);
    hpx_parcel_set_cont_action(p, hpx_lco_set_action);
    hpx_parcel_send(p, HPX_NULL);
  }

  // wait on all done[]s. if any are true, yield() failed
  uint64_t any_timeouts = false;
  uint64_t timeout = false;
  for (int i = 0; i < num_threads + 1; i++) {
    hpx_lco_get(done[i], sizeof(timeout), &timeout);
    any_timeouts |= timeout;
    hpx_lco_delete(done[i], HPX_NULL);
  }
  assert_msg(any_timeouts == false, "Threads did not yield.");

  delete [] done;
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, thread_yield, thread_yield_handler);

TEST_MAIN({
  ADD_TEST(thread_yield, 0);
});
