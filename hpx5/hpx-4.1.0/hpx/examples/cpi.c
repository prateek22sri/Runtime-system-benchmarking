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

/* The ubiquitous cpi program. Compute pi using a simple quadrature rule in
   parallel */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <hpx/hpx.h>

#define INTERVALS_PER_THREAD_DEFAULT 100

static hpx_action_t _main       = 0;
static hpx_action_t _setVal     = 0;
static hpx_action_t _getVal     = 0;

double reduce_result;

/* The function to be integrated */
double f(double x) {
  double dfour = 4;
  double done = 1;
  return(dfour / (done + (x * x)));
}

/* Implementation of a simple quadrature rule */
double integrate(double left, double right, int intervals) {
  double sum = 0;
  double h = (right-left) / intervals;
  double hh = h / 2;

  /* Use the midpoint rule */
  double midpt = left + hh;
  for(int i = 0; i < intervals; i++) {
    sum += f(midpt + i * h);
  }

  return(h * sum);
}

static double myreduce(int count, double values[count]) {
  double total = 0;
  for (int i = 0; i < count; ++i)
    total += values[i];
  return total;
}

static int _getVal_action(void *args, size_t size) {
  int THREADS = HPX_LOCALITIES;
  int MYTHREAD = HPX_LOCALITY_ID;

  double mystart, myend;
  int interval = *(int*)args;

  mystart = (1.0 * MYTHREAD) / THREADS;
  myend = (1.0 * (MYTHREAD + 1)) / THREADS;

  double value = integrate(mystart, myend, interval);

  return HPX_THREAD_CONTINUE(value);
}

static int _setVal_action(void *args, size_t size) {
  reduce_result = *(double*)args;
  return HPX_SUCCESS;
}

static int _main_action(int *args, size_t size) {
  double realpi=3.141592653589793238462643;
  int THREADS = HPX_LOCALITIES;
  int MYTHREAD = HPX_LOCALITY_ID;
  int interval = *args;

  int             ns[THREADS];
  double      values[THREADS];
  void        *addrs[THREADS];
  size_t       sizes[THREADS];
  hpx_addr_t futures[THREADS];

  for (int i = 0; i < THREADS; ++i) {
    addrs[i] = &values[i];
    sizes[i] = sizeof(double);
    ns[i]    = interval;
    futures[i] = hpx_lco_future_new(sizeof(double));
    hpx_call(HPX_THERE(i), _getVal, futures[i], &ns[THREADS], sizeof(int));
  }

  hpx_lco_get_all(THREADS, futures, sizes, addrs, NULL);

  reduce_result = myreduce(THREADS, values);

  for (int i = 0; i < THREADS; ++i) {
    hpx_lco_delete(futures[i], HPX_NULL);
    futures[i] = hpx_lco_future_new(0);
    hpx_call(HPX_THERE(i), _setVal, futures[i], &reduce_result,
             sizeof(reduce_result));
  }

  hpx_lco_wait_all(THREADS, futures, NULL);

  for (int i = 0; i < THREADS; ++i)
    hpx_lco_delete(futures[i], HPX_NULL);

  if(MYTHREAD == 0) {
    printf("Approx: %20.17f fabs(reduce_result - realpi): %23.17f\n", reduce_result,
                   fabs(reduce_result - realpi));
  }

  hpx_exit(0, NULL);
}

int main(int argc, char *argv[]) {
  int interval = INTERVALS_PER_THREAD_DEFAULT;

  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _main, _main_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _setVal, _setVal_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _getVal, _getVal_action, HPX_POINTER, HPX_SIZE_T);

  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return e;
  }

  if(argc > 1) {
    interval = atoi(argv[1]);
  }

  e = hpx_run(&_main, NULL, &interval, sizeof(interval));
  hpx_finalize();
  return e;
}
