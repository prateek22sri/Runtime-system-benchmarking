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
#include <libhpx/libhpx.h>
#include "hpx/hpx.h"
#include "tests.h"

/// @file tests/unit/gas_move.c
///
/// This is a simple AGAS test: We allocate two futures cyclically,
/// one on the root locality and the other on a remote locality. After
/// a move operation, we verify if the remote future has moved to the
/// local calling locality.

static int get_rank_handler(void) {
  int rank = HPX_LOCALITY_ID;
  return HPX_THREAD_CONTINUE(rank);
}
static HPX_ACTION(HPX_DEFAULT, 0, get_rank, get_rank_handler);

static int gas_move_handler(void) {
  if (HPX_LOCALITIES < 2) {
    return HPX_SUCCESS;
  }
  printf("root locality: %d, thread: %d.\n", HPX_LOCALITY_ID, HPX_THREAD_ID);
  hpx_addr_t base = hpx_lco_future_array_new(2, sizeof(int), 1);
  hpx_addr_t other = hpx_lco_future_array_at(base, 1, sizeof(int), 1);

  int my_rank = 0;
  int other_rank = 0;
  hpx_call_sync(other, get_rank, &other_rank, sizeof(other_rank));
  printf("target locality's ID (before move): %d\n", other_rank);

  if (other_rank == HPX_LOCALITY_ID) {
    printf("AGAS test: failed.\n");
    hpx_abort();
  }

  hpx_addr_t done = hpx_lco_future_new(0);
  // move address to our locality.
  printf("initiating AGAS move from (0x%lx) to (0x%lx).\n", (long)other, (long)HPX_HERE);
  hpx_gas_move(other, HPX_HERE, done);
  if (hpx_lco_wait(done) != HPX_SUCCESS) {
    printf("error in hpx_move().\n");
    hpx_abort();
  }

  hpx_lco_delete(done, HPX_NULL);

  hpx_call_sync(other, get_rank, &my_rank, sizeof(my_rank));
  printf("target locality's rank (after move): %d\n", my_rank);

  const libhpx_config_t *cfg = libhpx_get_config();
  if (((my_rank == HPX_LOCALITY_ID) && cfg->gas == HPX_GAS_AGAS) ||
      ((my_rank == other_rank) && cfg->gas == HPX_GAS_PGAS)) {
    printf("AGAS test: passed.\n");
  } else {
    printf("AGAS test: failed.\n");
    hpx_abort();
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, gas_move, gas_move_handler);

TEST_MAIN({
    ADD_TEST(gas_move, 0);
  });
