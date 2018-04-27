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

/// Testing the hpx_gas_{set,clear}_affinity functionality doesn't really make
/// very much sense, in that the call makes no guarantees about behavior. A
/// no-op is a valid implementation for these functions.

#include <hpx/hpx.h>
#include "tests.h"

static int _op_handler(int expected) {
  int current = hpx_get_my_thread_id();
  if (current != expected) {
    printf("running on %d, expected %d: hpx_gas_set_affinity() could be "
           "unimplemented.\n", current, expected);
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_ATTR_NONE, _op, _op_handler, HPX_INT);

static int _test_handler(void) {
  hpx_addr_t block = hpx_gas_alloc_local(1, 8, 0);

  for (int i = 0, e = hpx_get_num_threads(); i < e; ++i) {
    hpx_gas_set_affinity(block, i);
    CHECK( hpx_call_sync(block, _op, NULL, 0, &i) );
    hpx_gas_clear_affinity(block);
  }

  hpx_gas_free_sync(block);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_ATTR_NONE, _test, _test_handler);

TEST_MAIN({
    ADD_TEST(_test, 0);
  });
