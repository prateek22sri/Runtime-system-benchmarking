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

#include "hpx/hpx.h"
#include "tests.h"

static const int elts_per_block = 4;
static const size_t bsize = 4 * sizeof(float);

static int _initialize_handler(float *element, float initializer) {
  for (int i = 0; i < elts_per_block; ++i) {
    element[i] = initializer;
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _initialize, _initialize_handler,
                  HPX_POINTER, HPX_FLOAT);

static int _multiply_handler(float *multiplicand, float multiplier) {
  for (int i = 0; i < elts_per_block; ++i) {
    multiplicand[i] *= multiplier;
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _multiply, _multiply_handler,
                  HPX_POINTER, HPX_FLOAT);

static int _verify_handler(float *element, float expected) {
  for (int i = 0; i < elts_per_block; ++i) {
    if (element[i] != expected) {
      hpx_abort();
    }
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _verify, _verify_handler,
                  HPX_POINTER, HPX_FLOAT);

static int map_handler(void) {
  int blocks = HPX_LOCALITIES;
  hpx_addr_t array = hpx_gas_calloc_cyclic(blocks, bsize, 0);
  test_assert(array != HPX_NULL);

  float initializer = 4.0;
  printf("Testing hpx_gas_bcast_sync...\n");
  int e = hpx_gas_bcast_sync(_initialize, array, blocks, 0, bsize,
                             &initializer);
  test_assert(e == HPX_SUCCESS);
  printf("Testing hpx_gas_bcast_with_continuation...\n");
  float multiplier = 5.0;
  hpx_addr_t lco = hpx_lco_and_new(blocks);
  e = hpx_gas_bcast_with_continuation(_multiply, array, blocks, 0, bsize,
                                      hpx_lco_set_action, lco, &multiplier);
  test_assert(e == HPX_SUCCESS);
  e = hpx_lco_wait(lco);
  test_assert(e == HPX_SUCCESS);
  hpx_lco_delete(lco, HPX_NULL);

  printf("Verifying results...\n");
  float expected = initializer * multiplier;
  hpx_gas_bcast_sync(_verify, array, blocks, 0, bsize, &expected);
  hpx_gas_free(array, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, map, map_handler);

TEST_MAIN({
    ADD_TEST(map, 0);
});
