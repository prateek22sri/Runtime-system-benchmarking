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

#include <inttypes.h>
#include <hpx/hpx.h>
#include "tests.h"

#define N UINT32_C(8192)
static const int ONES[N] = {1};
static const int ZEROS[N] = {0};
static int _set[N];
static int _get[N];

static void _zero(int *buffer) {
  memset(buffer, 0, sizeof(int) * N);
}

static int _cmp(const int *lhs, const int *rhs) {
  for (int i = 0; i < N; ++i) {
    if (lhs[i] != rhs[i]) {
      return 0;
    }
  }
  return 1;
}

static void _verify_get(void) {
  if (!_cmp(_get, ONES)) {
    exit(-1);
  }
}

static void _cpy(int *lhs, const int *rhs) {
  memcpy(lhs, rhs, sizeof(int) * N);
}

static void _reset(int *set, int *get, hpx_addr_t sync, hpx_addr_t lco) {
  if (set) {
    _zero(set);
  }
  if (get) {
    _zero(get);
  }
  if (sync) {
    hpx_lco_reset_sync(sync);
  }
  if (lco) {
    hpx_lco_reset_sync(lco);
    hpx_lco_set_rsync(lco, sizeof(ZEROS), ZEROS);
    hpx_lco_reset_sync(lco);
  }
}

static int _new_future_at_handler(void) {
  hpx_addr_t future = hpx_lco_future_new(sizeof(ONES));
  return HPX_THREAD_CONTINUE(future);
}
static HPX_ACTION(HPX_DEFAULT, 0, _new_future_at, _new_future_at_handler);

static int _test_set_handler(hpx_addr_t lco) {
  hpx_addr_t sync = hpx_lco_future_new(4);

  printf("\ttesting async ... ");
  {
    hpx_lco_set(lco, sizeof(ONES), ONES, HPX_NULL, HPX_NULL);
    hpx_lco_get(lco, sizeof(_get), _get);
    _verify_get();
    _reset(NULL, _get, sync, lco);
  }
  printf("ok\n");

  printf("\ttesting lsync ... ");
  {
    _cpy(_set, ONES);
    hpx_lco_set(lco, sizeof(ONES), ONES, sync, HPX_NULL);
    hpx_lco_wait(sync);
    _cpy(_set, ZEROS);
    hpx_lco_get(lco, sizeof(_get), &_get);
    _verify_get();
    _reset(_set, _get, sync, lco);
  }
  printf("ok\n");

  printf("\ttesting rsync ... ");
  {
    _cpy(_set, ONES);
    hpx_lco_set(lco, sizeof(ONES), ONES, HPX_NULL, sync);
    hpx_lco_wait(sync);
    _cpy(_set, ZEROS);
    hpx_lco_get(lco, sizeof(_get), &_get);
    _verify_get();
    _reset(_set, _get, sync, lco);
  }
  printf("ok\n");

  return hpx_call_cc(sync, hpx_lco_delete_action);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_set, _test_set_handler, HPX_ADDR);

static int _test_set_lsync_handler(hpx_addr_t lco) {
  hpx_addr_t rsync = hpx_lco_future_new(4);

  printf("\ttesting ... ");
  {
    _cpy(_set, ONES);
    hpx_lco_set_lsync(lco, sizeof(ONES), ONES, HPX_NULL);
    _cpy(_set, ZEROS);
    hpx_lco_get(lco, sizeof(_get), &_get);
    _verify_get();
    _reset(_set, _get, HPX_NULL, lco);
  }
  printf("ok\n");

  printf("\ttesting rsync ... ");
  {
    _cpy(_set, ONES);
    hpx_lco_set_lsync(lco, sizeof(ONES), ONES, rsync);
    _cpy(_set, ZEROS);
    hpx_lco_wait(rsync);
    hpx_lco_get(lco, sizeof(_get), &_get);
    _verify_get();
    _reset(_set, _get, rsync, lco);
  }
  printf("ok\n");

  return hpx_call_cc(rsync, hpx_lco_delete_action);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_set_lsync, _test_set_lsync_handler, HPX_ADDR);

static int _test_set_rsync_handler(hpx_addr_t lco) {
  printf("\ttesting ... ");
  _cpy(_set, ONES);
  hpx_lco_set_rsync(lco, sizeof(ONES), ONES);
  _cpy(_set, ZEROS);
  hpx_lco_get(lco, sizeof(_get), &_get);
  _verify_get();
  _reset(_set, _get, HPX_NULL, lco);
  printf("ok\n");
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_set_rsync, _test_set_rsync_handler, HPX_ADDR);

static int _test_set_local_handler(void) {
  hpx_addr_t lco;
  hpx_call_sync(HPX_HERE, _new_future_at, &lco, sizeof(lco));
  hpx_call_sync(HPX_HERE, _test_set, NULL, 0, &lco);
  return hpx_call_cc(lco, hpx_lco_delete_action);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_set_local, _test_set_local_handler);

static int _test_set_lsync_local_handler(void) {
  hpx_addr_t lco;
  hpx_call_sync(HPX_HERE, _new_future_at, &lco, sizeof(lco));
  hpx_call_sync(HPX_HERE, _test_set_lsync, NULL, 0, &lco);
  return hpx_call_cc(lco, hpx_lco_delete_action);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_set_lsync_local, _test_set_lsync_local_handler);

static int _test_set_rsync_local_handler(void) {
  hpx_addr_t lco;
  hpx_call_sync(HPX_HERE, _new_future_at, &lco, sizeof(lco));
  hpx_call_sync(HPX_HERE, _test_set_rsync, NULL, 0, &lco);
  return hpx_call_cc(lco, hpx_lco_delete_action);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_set_rsync_local, _test_set_rsync_local_handler);

static int _test_set_remote_handler(void) {
  hpx_addr_t peer = HPX_THERE((HPX_LOCALITY_ID + 1) % HPX_LOCALITIES);
  hpx_addr_t lco;
  hpx_call_sync(peer, _new_future_at, &lco, sizeof(lco));
  hpx_call_sync(peer, _test_set, NULL, 0, &lco);
  return hpx_call_cc(lco, hpx_lco_delete_action);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_set_remote, _test_set_remote_handler);

static int _test_set_lsync_remote_handler(void) {
  hpx_addr_t peer = HPX_THERE((HPX_LOCALITY_ID + 1) % HPX_LOCALITIES);
  hpx_addr_t lco;
  hpx_call_sync(peer, _new_future_at, &lco, sizeof(lco));
  hpx_call_sync(peer, _test_set_lsync, NULL, 0, &lco);
  return hpx_call_cc(lco, hpx_lco_delete_action);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_set_lsync_remote, _test_set_lsync_remote_handler);

static int _test_set_rsync_remote_handler(void) {
  hpx_addr_t peer = HPX_THERE((HPX_LOCALITY_ID + 1) % HPX_LOCALITIES);
  hpx_addr_t lco;
  hpx_call_sync(peer, _new_future_at, &lco, sizeof(lco));
  hpx_call_sync(peer, _test_set_rsync, NULL, 0, &lco);
  return hpx_call_cc(lco, hpx_lco_delete_action);
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_set_rsync_remote, _test_set_rsync_remote_handler);

// Testcase to test hpx_lco_wait_all function.
static int _init_block_handler(uint32_t *args, size_t n) {
  hpx_addr_t target = hpx_thread_current_target();
  uint32_t *buffer = NULL;
  printf("Called at %d.\n", HPX_LOCALITY_ID);
  if (!hpx_gas_try_pin(target, (void**)&buffer))
    return HPX_RESEND;

  uint32_t block_size = args[0];
  for (int i = 0; i < block_size; i++){
    // Initialixe the buffer
    buffer[i] = 1234;
    printf("Initializing the buffer for locality ID %d: %d\n",
           HPX_LOCALITY_ID, buffer[i]);
  }
  hpx_gas_unpin(target);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _init_block,
                  _init_block_handler, HPX_POINTER, HPX_SIZE_T);

static int _init_memory_handler(uint32_t *args, size_t n) {
  hpx_addr_t local = hpx_thread_current_target();
  hpx_call_sync(local, _init_block, NULL, 0, args, sizeof(*args));
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _init_memory,
                  _init_memory_handler, HPX_POINTER, HPX_SIZE_T);

static int lco_waitall_handler(void) {
  int size = HPX_LOCALITIES;
  int block_size = 1;
  int ranks = hpx_get_num_ranks();

  printf("Starting the HPX LCO Wait all test\n");
  printf("localities: %d\n", size);

  // Start the timer
  hpx_time_t t1 = hpx_time_now();

  uint32_t blocks = size;
  uint32_t block_bytes = block_size * sizeof(uint32_t);

  printf("Number of blocks and bytes per block = %d, %d\n", blocks, block_bytes);
  printf("Ranks and blocks per rank = %d, %d\n", ranks, blocks / ranks);
  hpx_addr_t addr = hpx_gas_alloc_cyclic(blocks, sizeof(uint32_t), 0);

  uint32_t args[2] = {
    block_size,
    (blocks / ranks)
  };

  int rem = blocks % ranks;
  hpx_addr_t done[2] = {
    hpx_lco_and_new(ranks),
    hpx_lco_and_new(rem)
  };

  for (int i = 0; i < ranks; i++) {
    hpx_addr_t there = hpx_addr_add(addr, i * block_bytes, sizeof(uint32_t));
    hpx_call(there, _init_memory, done[0], args, sizeof(args));
  }

  for (int i = 0; i < rem; i++) {
    hpx_gas_ptrdiff_t off = args[1] * ranks + i * block_bytes;
    hpx_addr_t block = hpx_addr_add(addr, off, block_bytes);
    hpx_call(block, _init_memory, done[1], args, sizeof(args));
  }

  // Blocks the thread until all of the LCO's have been set.
  hpx_lco_wait_all(2, done, NULL);
  hpx_lco_delete_all(2, done, HPX_NULL);
  hpx_gas_free(addr, HPX_NULL);

  printf(" Elapsed: %g\n", hpx_time_elapsed_ms(t1));
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_waitall, lco_waitall_handler);

static HPX_ACTION_DECL(_getAll);
// Testcase to test hpx_lco_get_all function
static int _getAll_handler(uint32_t *args, size_t size) {
  uint32_t n = *args;
  if (n < 2) {
    return HPX_THREAD_CONTINUE(n);
  }

  hpx_addr_t peers[] = {
    HPX_HERE,
    HPX_HERE
  };

  uint32_t ns[] = {
    n - 1,
    n - 2
  };

  hpx_addr_t futures[] =  {
    hpx_lco_future_new(sizeof(uint32_t)),
    hpx_lco_future_new(sizeof(uint32_t))
  };

  uint32_t ssn[] = {
    0,
    0
  };

  void *addrs[] = {
    &ssn[0],
    &ssn[1]
  };

  size_t sizes[] = {
    sizeof(uint32_t),
    sizeof(uint32_t)
  };

  hpx_call(peers[0], _getAll, futures[0], &ns[0], sizeof(uint32_t));
  hpx_call(peers[1], _getAll, futures[1], &ns[1], sizeof(uint32_t));

  hpx_lco_get_all(2, futures, sizes, addrs, NULL);

  hpx_lco_wait(futures[0]);
  hpx_lco_wait(futures[1]);

  hpx_addr_t wait = hpx_lco_future_new(0);
  hpx_lco_delete_all(2, futures, wait);
  hpx_lco_wait(wait);
  hpx_lco_delete(wait, HPX_NULL);

  uint32_t sn = ssn[0] * ssn[0] + ssn[1] * ssn[1];

  return HPX_THREAD_CONTINUE(sn);
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _getAll,
                  _getAll_handler, HPX_POINTER, HPX_SIZE_T);

static int lco_getall_handler(void) {
  uint32_t n, ssn;
  printf("Starting the HPX LCO get all test\n");
  for (uint32_t i = 0; i < 6; i++) {
    ssn = 0;
    n = i + 1;
    hpx_time_t t1 = hpx_time_now();
    printf("Square series for (%d): ", n);

    hpx_call_sync(HPX_HERE, _getAll, &ssn, sizeof(ssn), &n, sizeof(n));
    printf("%d", ssn);
    printf(" Elapsed: %.7f\n", hpx_time_elapsed_ms(t1)/1e3);
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_getall, lco_getall_handler);

// Testcase to test hpx_lco_error function
static int _errorset_handler(void *args, size_t n) {
  hpx_addr_t addr = *(hpx_addr_t*)args;
  // Propagate an error to an LCO
  hpx_lco_error(addr, HPX_ERROR, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _errorset,
                  _errorset_handler, HPX_POINTER, HPX_SIZE_T);

static int lco_error_handler(void) {
  printf("Starting the HPX LCO get all test\n");
  hpx_time_t t1 = hpx_time_now();
  hpx_addr_t lco = hpx_lco_future_new(0);
  hpx_addr_t done = hpx_lco_future_new(0);
  hpx_call(HPX_HERE, _errorset, done, &lco, sizeof(lco));
  hpx_status_t status = hpx_lco_wait(lco);
  printf("status == %d\n", status);
  assert(status == HPX_ERROR);
  hpx_lco_wait(done);

  hpx_lco_delete(lco, HPX_NULL);
  hpx_lco_delete(done, HPX_NULL);

  printf(" Elapsed: %.7f\n", hpx_time_elapsed_ms(t1)/1e3);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_error, lco_error_handler);

TEST_MAIN({
    ADD_TEST(_test_set_local, 0);
    ADD_TEST(_test_set_local, 1 % HPX_LOCALITIES);
    ADD_TEST(_test_set_lsync_local, 0);
    ADD_TEST(_test_set_lsync_local, 1 % HPX_LOCALITIES);
    ADD_TEST(_test_set_rsync_local, 0);
    ADD_TEST(_test_set_rsync_local, 1 % HPX_LOCALITIES);
    ADD_TEST(_test_set_remote, 0);
    ADD_TEST(_test_set_remote, 1 % HPX_LOCALITIES);
    ADD_TEST(_test_set_lsync_remote, 0);
    ADD_TEST(_test_set_lsync_remote, 1 % HPX_LOCALITIES);
    ADD_TEST(_test_set_rsync_remote, 0);
    ADD_TEST(_test_set_rsync_remote, 1 % HPX_LOCALITIES);

    ADD_TEST(lco_waitall, 0);
    ADD_TEST(lco_getall, 0);
    ADD_TEST(lco_error, 0);
});
