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
#include "hpx/hpx.h"
#include "tests.h"

typedef struct {
  int    i;
  float  f;
  double d;
} _call_marshalled_args_t;

static int _init_handler(_call_marshalled_args_t *args,
                         int i, float f, double d) {
  printf("initializing to %d %f %lf\n", i, f, d);
  args->i = i;
  args->f = f;
  args->d = d;
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _init, _init_handler,
                  HPX_POINTER, HPX_INT, HPX_FLOAT, HPX_DOUBLE);

static int _verify_handler(_call_marshalled_args_t *args, int nargs,
                           void *vargs[], size_t sizes[]) {
  int *i = vargs[0];
  float *f = vargs[1];
  double *d = vargs[2];

  printf("verifying to %d=%d %f=%f %lf=%lf\n", args->i, *i,
         args->f, *f, args->d, *d);
  bool verify = (args->i == *i) && (args->f == *f) && (args->d == *d);
  int e = HPX_SUCCESS;
  if (!verify) {
    printf("ERROR: verification failed\n");
    e = HPX_ERROR;
  }
  CHECK( e );
  return e;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED | HPX_VECTORED,
                  _verify, _verify_handler, HPX_POINTER, HPX_INT,
                  HPX_POINTER, HPX_POINTER);

static int call_marshalled_handler(void) {
  printf("Testing marshalled call with variadic arguments...\n");

  int i = 42;
  float f = 3.14;
  double d = 9.99;

  hpx_addr_t buf = hpx_gas_alloc_local(1, sizeof(_call_marshalled_args_t), 0);
  if (buf == HPX_NULL) {
    return HPX_ERROR;
  }

  hpx_call_sync(buf, _init, NULL, 0, &i, &f, &d);
  hpx_call_sync(buf, _verify, NULL, 0, &i, sizeof(i), &f, sizeof(f),
                &d, sizeof(d));

  hpx_gas_free_sync(buf);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, call_marshalled, call_marshalled_handler);

TEST_MAIN({
    ADD_TEST(call_marshalled, 0);
});
