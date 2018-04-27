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

#include "hpx/hpx.h"
#include "tests.h"

static HPX_ACTION_DECL(_test_array);
static HPX_ACTION_DECL(_test_struct);

#define lengthof(array) sizeof(array) / sizeof(array[0])

static const struct num {
  int array[10];
} fib = {
  .array = {
    1,
    1,
    2,
    3,
    5,
    8,
    13,
    21,
    34,
    55 }
};

static int _test_fib(struct num n) {
  assert(&n != &fib);
  for (int i = 0, e = lengthof(n.array); i < e; ++i) {
    printf("n[%d] = %d\n", i, n.array[i]);
    assert(n.array[i] == fib.array[i]);
  }
  return HPX_SUCCESS;
}

static const struct point {
  int n;
  double x;
  double y;
  double z;
} p = {
  .n = 999,
  .x = 3.1415,
  .y = 2.7184,
  .z = 0.110001
};

static HPX_ACTION_DECL(_test_point);
static int _test_nxyz(struct point point) {
  assert(p.n == point.n);
  assert(p.x == point.x);
  assert(p.y == point.y);
  assert(p.z == point.z);
  return HPX_SUCCESS;
}

static int test_datatype_handler(void) {
  printf("Test hpx array types\n");
  struct num n = fib;
  hpx_call_sync(HPX_HERE, _test_array, NULL, 0, &n);

  printf("Test hpx struct types 1\n");
  struct num m = fib;
  hpx_call_sync(HPX_HERE, _test_struct, NULL, 0, &m);

  printf("Test hpx struct types 2\n");
  struct point point = p;
  hpx_call_sync(HPX_HERE, _test_point, NULL, 0, &point);
  
  hpx_exit(0, NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, test_datatype, test_datatype_handler);

int main(int argc, char *argv[]) {
  if (hpx_init(&argc, &argv)) {
    fprintf(stderr, "failed to initialize HPX.\n");
    return 1;
  }

  hpx_type_t array_type;
  hpx_array_type_create(&array_type, HPX_INT, 10);
  assert(array_type);
  HPX_REGISTER_TYPED_ACTION(DEFAULT, _test_fib, &_test_array, array_type);

  hpx_type_t struct_type;
  hpx_struct_type_create(&struct_type, HPX_INT, HPX_INT, HPX_INT, HPX_INT,
                         HPX_INT, HPX_INT, HPX_INT, HPX_INT, HPX_INT, HPX_INT);
  assert(struct_type);
  HPX_REGISTER_TYPED_ACTION(DEFAULT, _test_fib, &_test_struct, struct_type);

  hpx_type_t point_struct_type;
  hpx_struct_type_create(&point_struct_type, HPX_INT, HPX_DOUBLE, HPX_DOUBLE,
                         HPX_DOUBLE);
  assert(point_struct_type);
  HPX_REGISTER_TYPED_ACTION(DEFAULT, _test_nxyz, &_test_point, point_struct_type);

  int e = hpx_run(&test_datatype, NULL);

  hpx_type_destroy(point_struct_type);
  hpx_type_destroy(struct_type);
  hpx_type_destroy(array_type);
  return e;
}
