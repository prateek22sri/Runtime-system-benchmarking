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

#include <stdint.h>
#include <hpx/hpx.h>
#include "tests.h"

static const int I = 32;
static const int N = 16;

#define BSIZE (N * sizeof(element_t))

/// Allreduce initialization operation for a summation.
static void _init_handler(int *input, size_t UNUSED) {
  *input = 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, _init, _init_handler);

/// Allreduce operation for a summation.
static void _sum_handler(int *lhs, const int *rhs, size_t UNUSED) {
  *lhs += *rhs;
}
static HPX_ACTION(HPX_FUNCTION, 0, _sum, _sum_handler);

typedef struct {
  hpx_addr_t f;
  int32_t   id;
} element_t;

static int _subscribe_handler(element_t *block, hpx_addr_t allreduce) {
  for (int i = 0; i < N; ++i) {
    block[i].f = hpx_lco_future_new(sizeof(int32_t));
    block[i].id = hpx_process_collective_allreduce_subscribe(allreduce,
                                                             hpx_lco_set_action,
                                                             block[i].f);
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _subscribe, _subscribe_handler,
                  HPX_POINTER, HPX_ADDR);

static int _unsubscribe_handler(element_t *block, hpx_addr_t allreduce) {
  for (int i = 0; i < N; ++i) {
    hpx_process_collective_allreduce_unsubscribe(allreduce, block[i].id);
    hpx_lco_delete_sync(block[i].f);
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _unsubscribe, _unsubscribe_handler,
                  HPX_POINTER, HPX_ADDR);

static int _reduce_handler(element_t *element, hpx_addr_t allreduce, int i) {
  hpx_process_collective_allreduce_join(allreduce, element->id, sizeof(i), &i);
  hpx_lco_get_reset(element->f, sizeof(i), &i);
  return HPX_THREAD_CONTINUE(i);
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _reduce, _reduce_handler,
                  HPX_POINTER, HPX_ADDR, HPX_INT);

static int _reduce_block_handler(hpx_addr_t allreduce) {
  hpx_addr_t block = hpx_thread_current_target();
  hpx_addr_t reduce = hpx_lco_reduce_new(N, sizeof(int), _init, _sum);
  for (int i = 0; i < N; ++i) {
    hpx_addr_t element = hpx_addr_add(block, i * sizeof(element_t), BSIZE);
    hpx_call(element, _reduce, reduce, &allreduce, &i);
  }
  int result;
  hpx_lco_get(reduce, sizeof(result), &result);
  hpx_lco_delete_sync(reduce);
  return HPX_THREAD_CONTINUE(result);
}
static HPX_ACTION(HPX_DEFAULT, 0, _reduce_block, _reduce_block_handler,
                  HPX_ADDR);

static int _test_handler(void) {
  hpx_addr_t base = hpx_gas_alloc_cyclic(HPX_LOCALITIES, BSIZE, 0);
  hpx_addr_t allreduce = hpx_process_collective_allreduce_new(sizeof(int),
                                                              _init, _sum);

  hpx_addr_t and = hpx_lco_and_new(HPX_LOCALITIES);
  for (int i = 0, e =  HPX_LOCALITIES; i < e; ++i) {
    hpx_addr_t block = hpx_addr_add(base, i * BSIZE, BSIZE);
    hpx_call(block, _subscribe, and, &allreduce);
  }
  hpx_lco_wait_reset(and);
  hpx_process_collective_allreduce_subscribe_finalize(allreduce);

  hpx_addr_t reduce = hpx_lco_reduce_new(HPX_LOCALITIES, sizeof(int), _init,
                                         _sum);
  int my_rank = hpx_get_my_rank();
  for (int i = 0, e = I; i < e; ++i) {
    for (int j = 0, e = HPX_LOCALITIES; j < e; ++j) {
      if(j != my_rank){	    
        hpx_addr_t block = hpx_addr_add(base, j * BSIZE, BSIZE);
        hpx_call(block, _reduce_block, reduce, &allreduce);
      }
    }

    hpx_addr_t block = hpx_addr_add(base, my_rank * BSIZE, BSIZE);
    hpx_call(block, _reduce_block, reduce, &allreduce);
    
    int result;
    hpx_lco_get_reset(reduce, sizeof(result), &result);

    // each locality contributes N * (N -1) / 2 to the total, so each leaf
    int leaves = HPX_LOCALITIES * N;
    int total = (N * (N - 1) / 2) * HPX_LOCALITIES;
    int expected = leaves * total;
    test_assert(result == expected);
    if(i % 2000 == 0)
      printf("expected = %d  result = %d  i = [%d] \n", expected, result, i);

  }
  hpx_lco_delete_sync(reduce);

  for (int i = 0, e =  HPX_LOCALITIES; i < e; ++i) {
    hpx_addr_t block = hpx_addr_add(base, i * BSIZE, BSIZE);
    hpx_call(block, _unsubscribe, and, &allreduce);
  }
  hpx_lco_wait_reset(and);
  hpx_lco_delete(and, HPX_NULL);
  hpx_process_collective_allreduce_delete(allreduce);
  hpx_gas_free(base, HPX_NULL);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _test, _test_handler);

TEST_MAIN({
    ADD_TEST(_test, 0);
  });
