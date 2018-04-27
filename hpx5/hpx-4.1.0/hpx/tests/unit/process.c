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

#include <stdlib.h>
#include "hpx/hpx.h"
#include "tests.h"

static HPX_ACTION_DECL(_spawn1);
static int _spawn1_handler(hpx_addr_t sync) {
  hpx_addr_t zero = HPX_NULL;
  if (sync != HPX_NULL) {
    hpx_lco_wait(sync);
  }

  int spawns = rand()%2;
  if (spawns) {
    for (int i = 0; i < spawns; ++i) {
      hpx_call(HPX_HERE, _spawn1, HPX_NULL, &zero);
    }
  }

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _spawn1, _spawn1_handler, HPX_ADDR);

static HPX_ACTION_DECL(_spawn2);
static int _spawn2_handler(hpx_addr_t sync) {
  hpx_addr_t zero = HPX_NULL;
  int spawns = rand()%2;
  if (spawns) {
    for (int i = 0; i < spawns; ++i) {
      hpx_call(HPX_HERE, _spawn2, HPX_NULL, &zero);
    }
  }

  if (sync != HPX_NULL) {
    hpx_lco_set(sync, 0, NULL, HPX_NULL, HPX_NULL);
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _spawn2, _spawn2_handler, HPX_ADDR);

static int process_handler(void) {
  printf("Test hpx_lco_process\n");

  hpx_addr_t psync = hpx_lco_future_new(0);
  hpx_addr_t sync = hpx_lco_future_new(0);
  hpx_addr_t proc = hpx_process_new(psync);
  hpx_process_call(proc, HPX_HERE, _spawn1, HPX_NULL, &sync);
  hpx_process_call(proc, HPX_HERE, _spawn2, HPX_NULL, &sync);
  hpx_lco_wait(psync);
  hpx_lco_delete(psync, HPX_NULL);
  hpx_lco_delete(sync, HPX_NULL);
  hpx_process_delete(proc, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, process, process_handler);

TEST_MAIN({
 ADD_TEST(process, 0);
});
