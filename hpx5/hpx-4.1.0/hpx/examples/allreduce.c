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

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hpx/hpx.h"

static int value;

static hpx_action_t _set_value;
static hpx_action_t _get_value;
static hpx_action_t allreduce;

static int _sum(int count, int values[count]) {
  int total = 0;
  for (int i = 0; i < count; ++i) {
    total += values[i];
  }
  return total;
}

static int _get_value_handler(void) {
  return HPX_THREAD_CONTINUE(value);
}

static int _set_value_handler(int args) {
  value = args;
  printf("At rank %d received value %d\n", HPX_LOCALITY_ID, value);
  return HPX_SUCCESS;
}

static int action_allreduce(void) {
  int num_ranks = HPX_LOCALITIES;
  assert(HPX_LOCALITY_ID == 0);

  int         values[num_ranks];
  void        *addrs[num_ranks];
  size_t       sizes[num_ranks];
  hpx_addr_t futures[num_ranks];

  for (int i = 0; i < num_ranks; ++i) {
    addrs[i] = &values[i];
    sizes[i] = sizeof(int);
    futures[i] = hpx_lco_future_new(sizeof(int));
    hpx_call(HPX_THERE(i), _get_value, futures[i]);
  }

  hpx_lco_get_all(num_ranks, futures, sizes, addrs, NULL);

  value = _sum(num_ranks, values);

  for (int i = 0; i < num_ranks; ++i) {
    hpx_lco_delete(futures[i], HPX_NULL);
    futures[i] = hpx_lco_future_new(0);
    hpx_call(HPX_THERE(i), _set_value, futures[i], &value);
  }

  hpx_lco_wait_all(num_ranks, futures, NULL);

  for (int i = 0; i < num_ranks; ++i) {
    hpx_lco_delete(futures[i], HPX_NULL);
  }

  hpx_exit(0, NULL);
}

static void _init_int_handler(int *input, const size_t size) {
  assert(sizeof(int) == size);
  *input = 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, _init_int, _init_int_handler);

static void _sum_int_handler(int *output, const int *input, const size_t size) {
  assert(sizeof(int) == size);
  *output += *input;
}
static HPX_ACTION(HPX_FUNCTION, 0, _sum_int, _sum_int_handler);

static int do_allreduce_handler(hpx_addr_t rlco, int value, int iters,
                                hpx_addr_t barrier) {

  hpx_addr_t lco = hpx_lco_future_new(sizeof(int));
  int32_t id = hpx_process_collective_allreduce_subscribe(rlco,
                                                          hpx_lco_set_action,
                                                          lco);
  hpx_lco_set(barrier, 0, NULL, HPX_NULL, HPX_NULL);
  hpx_lco_wait(barrier);

  for (int i = 0; i < iters; i++) {
    hpx_process_collective_allreduce_join(rlco, id, sizeof(value), &value);
    hpx_lco_get_reset(lco, sizeof(value), &value);
    printf("rank %d: iter %d: reduced value %d\n", HPX_LOCALITY_ID, i, value);
  }

  hpx_process_collective_allreduce_unsubscribe(rlco, id);
  hpx_lco_delete(lco, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, do_allreduce, do_allreduce_handler,
                  HPX_ADDR, HPX_INT, HPX_INT, HPX_ADDR);


static int proc_allreduce_handler(int value) {
  hpx_addr_t lco = hpx_process_collective_allreduce_new(sizeof(int),
                                                        _init_int,
                                                        _sum_int);

  int iters = 10;
  hpx_addr_t barrier = hpx_lco_and_new(HPX_LOCALITIES);
  hpx_bcast_rsync(do_allreduce, &lco, &value, &iters, &barrier);

  hpx_lco_delete_sync(barrier);
  hpx_process_collective_allreduce_delete(lco);
  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, proc_allreduce, proc_allreduce_handler,
                  HPX_INT);

int main(int argc, char** argv) {
  // register actions
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _set_value, _set_value_handler, HPX_INT);
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _get_value, _get_value_handler);
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, allreduce, action_allreduce);

  int e = hpx_init(&argc, &argv);
  if (e != 0) {
    printf("Error %d in hpx_init!\n", e);
    exit(EXIT_FAILURE);
  }

  // Initialize the values that we want to reduce
  value = HPX_LOCALITY_ID;

  e = hpx_run(&allreduce, NULL);
  if (e != HPX_SUCCESS) {
    printf("Error %d in hpx_run(allreduce)!\n", e);
    exit(EXIT_FAILURE);
  }

  e = hpx_run(&proc_allreduce, NULL, &value);
  hpx_finalize();
  return e;
}
