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

// Goal of this testcase is to test the HPX LCO Semaphores
// 1. hpx_lco_sema_new -- Create new semaphore
// 2. hpx_lco_sema_p -- Standard semaphore P (Wait) operation.
// 3. hpx_lco_sema_v -- Standard semaphore V (Signal) operation.
#include "hpx/hpx.h"
#include "tests.h"

hpx_addr_t mutex;
int counter; /* shared variable */

static int _sema_handler(uint32_t *args, size_t n) {
  uint32_t x = *args;
  printf("Thread %d: Waiting to enter critical region...\n", x);

  // Standard semaphore P (wait) operation.
  // Attempts to decrement the count in the semaphore; block
  // if the count is 0
  hpx_lco_sema_p(mutex);

  /* START CRITICAL REGION */
  printf("Thread %d: Now in critical region...\n", x);
  printf("Thread %d: Counter Value: %d\n", x, counter);
  printf("Thread %d: Incrementing Counter...\n", x);
  counter++;
  printf("Thread %d: New Counter Value: %d\n", x, counter);
  printf("Thread %d: Exiting critical region...\n", x);
  /* END CRITICAL REGION */

  // Standard semaphore V (Signal) operation.
  // increments the count in the semaphore, signaling the LCO if the
  // increment transitions from 0 to 1. This is always async.
  hpx_lco_sema_v(mutex, HPX_NULL);       /* up semaphore */

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _sema,
                  _sema_handler, HPX_POINTER, HPX_SIZE_T);

// Test code -- for HPX LCO Semaphores
static int lco_sema_handler(void) {
  hpx_addr_t peers[] = {HPX_HERE, HPX_HERE};
  uint32_t i[] = {0, 1};
  size_t sizes[] = {sizeof(uint32_t), sizeof(uint32_t)};
  uint32_t array[] = {0, 0};
  void *addrs[] = {&array[0], &array[1]};

  hpx_addr_t futures[] = {
    hpx_lco_future_new(sizeof(uint32_t)),
    hpx_lco_future_new(sizeof(uint32_t))
  };

  printf("Starting the HPX LCO Semaphore test\n");
  hpx_time_t t1 = hpx_time_now();

  // create a new semaphore
  // Returns the global address
  // initial value this semaphore would be created with
  mutex = hpx_lco_sema_new(1);

  hpx_call(peers[0], _sema, futures[0], &i[0], sizeof(uint32_t));
  hpx_call(peers[1], _sema, futures[1], &i[1], sizeof(uint32_t));

  hpx_lco_get_all(2, futures, sizes, addrs, NULL);

  hpx_lco_delete(futures[0], HPX_NULL);
  hpx_lco_delete(futures[1], HPX_NULL);

  printf(" Elapsed: %g\n", hpx_time_elapsed_ms(t1));
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_sema, lco_sema_handler);

TEST_MAIN({
  ADD_TEST(lco_sema, 0);
});
