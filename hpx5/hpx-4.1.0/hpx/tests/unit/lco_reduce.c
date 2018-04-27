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
#include <math.h>
#include <time.h>
#include "tests.h"

/// Initialize a double zero.
static void _initDouble_handler(double *input, const size_t bytes) {
  *input = 0.0;
}
static HPX_ACTION(HPX_FUNCTION, 0, _initDouble, _initDouble_handler);

static void _addDouble_handler(double *lhs, const double *rhs, size_t UNUSED) {
  *lhs += *rhs;
}
static HPX_ACTION(HPX_FUNCTION, 0, _addDouble, _addDouble_handler);

static int _reduce_handler(double data) {
  return HPX_THREAD_CONTINUE(data);
}
static HPX_ACTION(HPX_DEFAULT, 0, _reduce, _reduce_handler, HPX_DOUBLE);

static int lco_reduce_handler(void) {
  static const double data = 3141592653.58979;
  static const int nDoms = 91;
  static const int cycles = 10;
  hpx_addr_t domain = hpx_gas_calloc_cyclic(nDoms, sizeof(int), 0);
  hpx_addr_t newdt = hpx_lco_reduce_new(nDoms, sizeof(double), _initDouble,
                                        _addDouble);

  for (int i = 0, e = cycles; i < e; ++i) {
    printf("reducing iteration %d \n", i);
    hpx_addr_t and = hpx_lco_and_new(nDoms);
    for (int j = 0, e = nDoms; j < e; ++j) {
      hpx_addr_t block = hpx_addr_add(domain, sizeof(int) * j, sizeof(int));
      hpx_call_async(block, _reduce, and, newdt, &data);
    }
    hpx_lco_wait(and);
    hpx_lco_delete(and, HPX_NULL);

    // Get the gathered value, and print the debugging string.
    double ans;
    hpx_lco_get(newdt, sizeof(ans), &ans);
    double compval = nDoms * data;
    if (fabs((compval - ans)/compval) > 0.001) { // works if not near zero
      fprintf(stderr, "expected %f, got %f (delta = %f)\n", nDoms * data, ans,
          fabs(nDoms * data - ans));
      exit(EXIT_FAILURE);
    }
    hpx_lco_reset_sync(newdt);
  }

  hpx_lco_delete(newdt, HPX_NULL);
  hpx_gas_free(domain, HPX_NULL);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_reduce, lco_reduce_handler);

static int lco_reduce_getRef_handler(void) {
  static const double data = 3141592.65358979;
  static const int nDoms = 91;
  static const int cycles = 10;
  double tmp;

  hpx_addr_t domain = hpx_gas_calloc_cyclic(nDoms, sizeof(int), 0);
  hpx_addr_t newdt = hpx_lco_reduce_new(nDoms, sizeof(double), _initDouble,
                                        _addDouble);

  for (int i = 0, e = cycles; i < e; ++i) {
    printf("reducing iteration %d \n", i);
    hpx_addr_t and = hpx_lco_and_new(nDoms);
    for (int j = 0, e = nDoms; j < e; ++j) {
      hpx_addr_t block = hpx_addr_add(domain, sizeof(int) * j, sizeof(int));
      tmp = j*data;
      hpx_call_async(block, _reduce, and, newdt, &tmp);
    }
    hpx_lco_wait(and);
    hpx_lco_delete(and, HPX_NULL);

    // Get the gathered value, and print the debugging string.
    double *ans = NULL;
    hpx_lco_getref(newdt, sizeof(*ans), (void **)&ans);
    double compval = nDoms * ((nDoms-1)/2) * data;
    if (fabs((compval - *ans)/compval) > 0.001) { // works if not near zero
      fprintf(stderr, "expected %f, got %f (delta = %f)\n",
              nDoms * ((nDoms-1)/2) * data, *ans,
              nDoms * ((nDoms-1)/2) * data - *ans);
      exit(EXIT_FAILURE);
    }
    hpx_lco_release(newdt, ans);
    hpx_lco_reset_sync(newdt);
  }

  hpx_lco_delete(newdt, HPX_NULL);
  hpx_gas_free(domain, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_reduce_getRef, lco_reduce_getRef_handler);

struct _par_reduce_args {
  hpx_addr_t rlco;
  double *nums;
};

static int _par_reduce(const int i, void *args) {
  struct _par_reduce_args *a = args;
  hpx_lco_set(a->rlco, sizeof(double), &a->nums[i], HPX_NULL, HPX_NULL);
  return 0;
}

static int lco_par_reduce_handler(void) {
  #define ITERS 42
  double nums[ITERS];

  srand((unsigned)time(NULL));
  for (int i = 0, e = ITERS; i < e; ++i) {
    nums[i] = ((double)rand()/(double)RAND_MAX);
  }

  double val = 0.0;
  for (int i = 0, e = ITERS; i < e; ++i) {
    val += nums[i];
  }

  hpx_addr_t rlco = hpx_lco_reduce_new(ITERS, sizeof(double), _initDouble,
                                       _addDouble);
  struct _par_reduce_args args = {
    .rlco = rlco,
    .nums = nums
  };

  // perform parallel reduction
  hpx_par_for_sync(_par_reduce, 0, ITERS, &args);

  // get the gathered value
  double ans;
  hpx_lco_get(rlco, sizeof(ans), &ans);
  printf("expected %f, got %f (delta = %f)\n", val, ans, fabs(val - ans));
  // works if not near zero
  if (fabs((val - ans)/val) > 0.001) {
    exit(EXIT_FAILURE);
  }

  hpx_lco_delete(rlco, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_par_reduce, lco_par_reduce_handler);

TEST_MAIN({
  ADD_TEST(lco_reduce, 0);
  ADD_TEST(lco_reduce_getRef, 0);
  ADD_TEST(lco_par_reduce, 0);
});
