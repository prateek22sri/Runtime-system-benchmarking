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

#include <stdint.h>
#include <hpx/hpx.h>
#include "tests.h"

static const int I = 32;
static const int N = 64;

/// Allreduce initialization operation for a summation.
static void _init_handler(int *input, size_t UNUSED) {
  *input = 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, _init, _init_handler);

/// Allreduce operation for a summation.
static void _sum_handler(int *lhs, const int *rhs, size_t UNUSED) {
  *lhs += *rhs;
}
static HPX_ACTION(HPX_FUNCTION, 0, _sum, _sum_handler);

/// Handle the test broadcast.
///
/// We want to stress the allreduce by generating a bunch of parallel operations
/// on it from different parts of the system. We do this by broadcasting this
/// operation, which will spawn N instances of the @p leaf operation locally.
static int
_test_bcast_handler(hpx_addr_t allreduce, hpx_addr_t sum, hpx_action_t leaf) {
  int r;
  int row = HPX_LOCALITIES * HPX_LOCALITY_ID;
  for (int i = 0; i < N; ++i) {
    int j = row + i;
    int k = i + 1;
    // &sum is passed explicitly instead of as a continuation for the _join_leaf
    CHECK( hpx_call(HPX_HERE, leaf, HPX_NULL, &allreduce, &j, &k, &sum) );
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_bcast, _test_bcast_handler, HPX_ADDR,
                  HPX_ADDR, HPX_ACTION_T);

/// A utility that tests a certain leaf function through I iterations.
static int _test(hpx_action_t leaf) {
  int L = HPX_LOCALITIES;
  int n = N * L;
  hpx_addr_t allreduce = hpx_lco_allreduce_new(n, n, sizeof(int), _init, _sum);
  hpx_addr_t sum = hpx_lco_reduce_new(n, sizeof(int), _init, _sum);
  for (int i = 0; i < I; ++i) {
    int r;
    CHECK( hpx_bcast(_test_bcast, HPX_NULL, HPX_NULL, &allreduce, &sum, &leaf) );
    CHECK( hpx_lco_get_reset(sum, sizeof(r), &r) );
    test_assert(r == n * HPX_LOCALITIES * (N + 1) * N / 2);
  }
  hpx_lco_delete(sum, HPX_NULL);
  hpx_lco_delete(allreduce, HPX_NULL);
  return HPX_SUCCESS;
}

/// Use a set-get pair for the allreduce leaf operation.
static int
_set_get_leaf_handler(hpx_addr_t allreduce, int i, int j, hpx_addr_t sum) {
  int r;
  hpx_lco_set_lsync(allreduce, sizeof(j), &j, HPX_NULL);
  CHECK( hpx_lco_get(allreduce, sizeof(r), &r) );
  test_assert(r == HPX_LOCALITIES * N * (N + 1) / 2);
  return hpx_call_cc(sum, hpx_lco_set_action, &r, sizeof(r));
}
static HPX_ACTION(HPX_DEFAULT, 0, _set_get_leaf, _set_get_leaf_handler,
                  HPX_ADDR, HPX_INT, HPX_INT, HPX_ADDR);

/// Use a join operation in the allreduce leaf.
static int
_join_leaf_handler(hpx_addr_t allreduce, int i, int j, hpx_addr_t sum) {
  CHECK( hpx_lco_allreduce_join(allreduce, i, sizeof(j), &j, hpx_lco_set_action,
                                sum) );
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _join_leaf, _join_leaf_handler, HPX_ADDR,
                  HPX_INT, HPX_INT, HPX_ADDR);

/// Use the join_async operation in the allreduce leaf.
static int
_join_async_leaf_handler(hpx_addr_t allreduce, int i, int j, hpx_addr_t sum) {
  int r;
  hpx_addr_t f = hpx_lco_future_new(0);
  CHECK( hpx_lco_allreduce_join_async(allreduce, i, sizeof(j), &j, &r, f) );
  CHECK( hpx_lco_wait(f) );
  hpx_lco_delete(f, HPX_NULL);
  test_assert(r == HPX_LOCALITIES * N * (N + 1) / 2);
  return hpx_call_cc(sum, hpx_lco_set_action, &r, sizeof(r));
}
static HPX_ACTION(HPX_DEFAULT, 0, _join_async_leaf, _join_async_leaf_handler,
                  HPX_ADDR, HPX_INT, HPX_INT, HPX_ADDR);

/// Use the join_sync operation in the allreduce leaf.
static int
_join_sync_leaf_handler(hpx_addr_t allreduce, int i, int j, hpx_addr_t sum) {
  int r;
  CHECK( hpx_lco_allreduce_join_sync(allreduce, i, sizeof(j), &j, &r) );
  test_assert(r == HPX_LOCALITIES * N * (N + 1) / 2);
  return hpx_call_cc(sum, hpx_lco_set_action, &r, sizeof(r));
}
static HPX_ACTION(HPX_DEFAULT, 0, _join_sync_leaf, _join_sync_leaf_handler,
                  HPX_ADDR, HPX_INT, HPX_INT, HPX_ADDR);

/// Spawn the set-get test.
static int _test_allreduce_set_get_handler(void) {
  return _test(_set_get_leaf);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_allreduce_set_get,
                  _test_allreduce_set_get_handler);

/// Spawn the join test.
static int _test_allreduce_join_handler(void) {
  return _test(_join_leaf);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_allreduce_join,
                  _test_allreduce_join_handler);

/// Spawn the join async test.
static int _test_allreduce_join_async_handler(void) {
  return _test(_join_async_leaf);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_allreduce_join_async,
                  _test_allreduce_join_async_handler);

/// Spawn the join sync test.
static int _test_allreduce_join_sync_handler(void) {
  return _test(_join_sync_leaf);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_allreduce_join_sync,
                  _test_allreduce_join_sync_handler);

TEST_MAIN({
    ADD_TEST(_test_allreduce_set_get, 0);
    ADD_TEST(_test_allreduce_join_async, 0);
    ADD_TEST(_test_allreduce_join_sync, 0);
    ADD_TEST(_test_allreduce_join, 0);
  });
