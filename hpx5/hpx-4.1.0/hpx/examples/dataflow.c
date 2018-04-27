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

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "hpx/hpx.h"

// This examples computes the following set of expressions by creating
// a dataflow network for it.
//
// c = a + b;
// d = b + c;
// e = c * d;
//
// The purpose of this example is to demonstrate the use of dataflow
// LCO in HPX.

// Initial data:
double _a = 'a';
double _b = 'b';

static int _add_handler(double *inputs[], int n) {
  double output = 0.0;
  for (int i = 0; i < n; ++i) {
    output += *inputs[i];
  }
  return HPX_THREAD_CONTINUE(output);
}
static HPX_ACTION(HPX_DEFAULT, 0, _add, _add_handler, HPX_POINTER, HPX_INT);

static int _mul_handler(double *inputs[], int n) {
  double output = 1.0;
  for (int i = 0; i < n; ++i) {
    output *= *inputs[i];
  }
  return HPX_THREAD_CONTINUE(output);
}
static HPX_ACTION(HPX_DEFAULT, 0, _mul, _mul_handler, HPX_POINTER, HPX_INT);

static int _main_handler(void) {
  double _e = 0.0;
  printf("e="); fflush(stdout);
  hpx_time_t now = hpx_time_now();

  // create the dataflow graph nodes using LCOs.
  hpx_addr_t a = hpx_lco_future_new(sizeof(_a));
  hpx_addr_t b = hpx_lco_future_new(sizeof(_b));
  hpx_addr_t c = hpx_lco_future_new(sizeof(double));
  hpx_addr_t d = hpx_lco_future_new(sizeof(double));
  hpx_addr_t e = hpx_lco_future_new(sizeof(_e));

  // populate the dataflow LCO:
  hpx_addr_t df = hpx_lco_dataflow_new();
  hpx_lco_dataflow_add(df, _add, c,
                       a, sizeof(double), b, sizeof(double));
  hpx_lco_dataflow_add(df, _add, d,
                       b, sizeof(double), c, sizeof(double));
  hpx_lco_dataflow_add(df, _mul, e,
                       c, sizeof(double), d, sizeof(double));

  // set the inputs:
  hpx_lco_set(a, sizeof(_a), &_a, HPX_NULL, HPX_NULL);
  hpx_lco_set(b, sizeof(_b), &_b, HPX_NULL, HPX_NULL);
  hpx_lco_get(e, sizeof(_e), &_e);

  hpx_lco_delete_sync(a);
  hpx_lco_delete_sync(b);
  hpx_lco_delete_sync(c);
  hpx_lco_delete_sync(d);
  hpx_lco_delete_sync(e);
  hpx_lco_delete_sync(df);

  double elapsed = hpx_time_elapsed_ms(now)/1e3;

  printf("%lf\n", _e);
  printf("seconds: %.7f\n", elapsed);
  printf("localities: %d\n", HPX_LOCALITIES);
  printf("threads/locality: %d\n", HPX_THREADS);
  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _main, _main_handler);

int main(int argc, char *argv[]) {
  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return e;
  }

  // run the main action
  e = hpx_run(&_main, NULL);
  hpx_finalize();
  return e;
}

