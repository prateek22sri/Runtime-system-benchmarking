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

static int _is_error(hpx_status_t s) {
  if (s != HPX_SUCCESS) {
    fprintf(stderr, "HPX operation returned error %s", hpx_strerror(s));
  }
  return (s != HPX_SUCCESS);
}

static int _is_hpxnull(hpx_addr_t addr) {
  assert(addr != HPX_NULL);
  return (addr == HPX_NULL);
}

static int _is_null(void *addr) {
  assert(addr != NULL);
  return (addr == NULL);
}

/// Store a value to an integer in memory.
static int _store_int_handler(int *addr, int val) {
  *addr = val;
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _store_int,
                  _store_int_handler, HPX_POINTER, HPX_INT);

/// Load a value from an integer in memory.
static int _load_int_handler(int *addr) {
  return HPX_THREAD_CONTINUE(*addr);
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _load_int,
                  _load_int_handler, HPX_POINTER);

/// Increment an integer in memory.
///
/// This is slightly more complicated than an increment. We send along the value
/// that we expect to see, to make sure our cascade is running in the correct
/// order.
static int _parcelsendthrough_increment_handler(int *val, int arg) {
  // don't need synchronization since this is done in a sequential cascade
  int i = val[0]++;
  printf("expected %d, got %d\n", i, arg);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _parcelsendthrough_increment,
                  _parcelsendthrough_increment_handler, HPX_POINTER, HPX_INT);

/// This test sets up a simple cascade of parcels in a cyclic array of
/// futures. Each parcel waits for the future at i, then executes the
/// _parcelSendThrough_increment on val, with the integer i, and
/// then triggers the future at i + 1.
hpx_addr_t _cascade(hpx_addr_t done, hpx_addr_t val, const int n) {
  // allocate the cascade array
  hpx_addr_t gates = hpx_lco_future_array_new(n, 0, 1);
  if (_is_hpxnull(gates)) {
    goto unwind0;
  }

  // set up the prefix of the cascade
  for (int i = 0, e = n; i < e; ++i) {
    hpx_parcel_t *p = hpx_parcel_acquire(NULL, sizeof(int));
    if (_is_null(p)) {
      goto unwind1;
    }

    // set up the initial action we want to run
    hpx_parcel_set_target(p, val);
    hpx_parcel_set_action(p, _parcelsendthrough_increment);
    hpx_parcel_set_args(p, &i);

    // set up the continuation (trigger the next lco, or the done lco if we're
    // done)
    if (i < n - 1) {
      hpx_addr_t next = hpx_lco_future_array_at(gates, i + 1, 0, 1);
      hpx_parcel_set_cont_target(p, next);
    }
    else {
      hpx_parcel_set_cont_target(p, done);
    }
    hpx_parcel_set_cont_action(p, hpx_lco_set_action);

    // send the parcel through the current gate
    hpx_addr_t gate = hpx_lco_future_array_at(gates, i, 0, 1);
    if (_is_hpxnull(gate)) {
      goto unwind1;
    }

    if (_is_error(hpx_parcel_send_through_sync(p, gate))) {
      goto unwind1;
    }
  }

  return gates;

 unwind1:
  hpx_lco_delete(gates, HPX_NULL);
 unwind0:
  return HPX_NULL;
}

/// Test the parcel_send_though functionality.
///
/// The send-through operation is designed so that LCOs can buffer sent parcels
/// until their condition triggers. This permits a style of thread-less
/// synchronization that can aid in dataflow programming.
///
static int parcel_send_through_handler(void) {
  const int n = 2 * HPX_LOCALITIES;

  printf("Testing parcel sends through LCOs\n");
  fflush(stdout);

  hpx_time_t t1 = hpx_time_now();

  // allocate a future to signal the completion of the cascade
  hpx_addr_t done = hpx_lco_future_new(0);
  if (_is_hpxnull(done)) {
    goto unwind0;
  }

  // allocate and initialize a shared integer for the cascade to update
  hpx_addr_t val = hpx_gas_alloc_local(1, sizeof(int), 0);
  if (_is_hpxnull(val)) {
    goto unwind1;
  }

  const int zero = 0;
  if (_is_error(hpx_call_sync(val, _store_int, NULL, 0, &zero))) {
    goto unwind2;
  }

  // allocate and initialize the cascade
  hpx_addr_t gates = _cascade(done, val, n);
  if (gates == HPX_NULL) {
    goto unwind2;
  }

  // start the cascade by setting the first future
  if (_is_error(hpx_call(gates, hpx_lco_set_action, HPX_NULL, NULL, 0))) {
    goto unwind2;
  }

  // wait for the cascade to finish
  if (_is_error(hpx_lco_wait(done))) {
    goto unwind3;
  }

  // check the final value to make sure everything actually worked
  int fin;
  if (_is_error(hpx_call_sync(val, _load_int, &fin, sizeof(fin)))) {
    goto unwind3;
  }

  if (fin != n) {
    fprintf(stderr, "expected final value %d, got %d", n, fin);
    return HPX_ERROR;
  }

 unwind3:
  hpx_lco_delete(gates, HPX_NULL);
 unwind2:
  hpx_gas_free(val, HPX_NULL);
 unwind1:
  hpx_lco_delete(done, HPX_NULL);
 unwind0:
  printf("Elapsed: %g\n", hpx_time_elapsed_ms(t1));
  fflush(stdout);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, parcel_send_through, parcel_send_through_handler);

TEST_MAIN({
  ADD_TEST(parcel_send_through, 0);
});
