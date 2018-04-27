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
#include <stdlib.h>
#include <hpx/hpx.h>
#include "tests.h"

enum { ELEMENTS = 32 };

hpx_addr_t   data = 0;
hpx_addr_t  local = 0;
hpx_addr_t remote = 0;

void fail(int i, uint64_t expected, uint64_t actual) {
  fprintf(stderr, "failed to set element %d correctly, "
          "expected %" PRIu64 ", got %" PRIu64 "\n", i, expected, actual);
  exit(EXIT_FAILURE);
}

int reset_handler(uint64_t *local) {
  memset(local, 0, ELEMENTS * sizeof(*local));
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, reset, reset_handler, HPX_POINTER);

int verify_handler(uint64_t *local, uint64_t *args, size_t n) {
  for (int i = 0, e = ELEMENTS; i < e; ++i) {
    if (local[i] != args[i]) {
      fail(i, args[i], local[i]);
    }
    local[i] = 0;
  }
  return hpx_call_cc(hpx_thread_current_target(), reset);
}
HPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, verify, verify_handler,
           HPX_POINTER, HPX_POINTER, HPX_SIZE_T);

/// Initialize the global data for a rank.
int init_handler(hpx_addr_t data) {
  size_t n = ELEMENTS * sizeof(uint64_t);
  int rank = HPX_LOCALITY_ID;
  int peer = (rank + 1) % HPX_LOCALITIES;

  data = data;
  local = hpx_addr_add(data, rank * n, n);
  remote = hpx_addr_add(data, peer * n, n);

  // test hpx_addr_add with negative offsets
  hpx_gas_ptrdiff_t off = (hpx_gas_ptrdiff_t)-rank * n;
  hpx_addr_t _local = hpx_addr_add(local, off, n);
  test_assert (_local == data);

  off = (hpx_gas_ptrdiff_t)-peer * n;
  hpx_addr_t _remote = hpx_addr_add(remote, off, n);
  test_assert (_remote == data);

  CHECK( hpx_call_sync(local, reset, NULL, 0) );
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, init, init_handler, HPX_ADDR);

int init_globals_handler(void) {
  size_t n = ELEMENTS * sizeof(uint64_t);
  hpx_addr_t data = hpx_gas_alloc_cyclic(HPX_LOCALITIES, n, 0);
  CHECK( hpx_bcast_rsync(init, &data) );
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, init_globals, init_globals_handler);

int fini_globals_handler(void) {
  hpx_gas_free(data, HPX_NULL);
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, fini_globals, fini_globals_handler);

int future_at_handler(void) {
  hpx_addr_t f = hpx_lco_future_new(0);
  return HPX_THREAD_CONTINUE(f);
}
HPX_ACTION(HPX_INTERRUPT, 0, future_at, future_at_handler);

/// Instantiate a single hpx_gas_memput test.
///
/// This will allocate lsync and rsync lcos at @p lat and @p rat, respectively,
/// and initiate and check the memput.
///
/// @param       buffer The local buffer to put from.
/// @param            n The number of bytes to put.
/// @param        block The global address to put into.
/// @param          lat The locality to allocate the lsync at.
/// @param          rat The locality to allocate the rsync at.
void test(const uint64_t *buffer, size_t n, hpx_addr_t block, hpx_addr_t lat,
          hpx_addr_t rat) {
  hpx_addr_t lsync = HPX_NULL;
  hpx_addr_t rsync = HPX_NULL;
  CHECK( hpx_call_sync(lat, future_at, &lsync, sizeof(lsync)) );
  CHECK( hpx_call_sync(rat, future_at, &rsync, sizeof(rsync)) );
  CHECK( hpx_gas_memput(block, buffer, n, lsync, rsync) );
  CHECK( hpx_lco_wait(lsync) );
  CHECK( hpx_lco_wait(rsync) );
  CHECK( hpx_call_sync(block, verify, NULL, 0, buffer, n) );
  hpx_lco_delete_sync(lsync);
  hpx_lco_delete_sync(rsync);
}

/// Instantiate a single hpx_gas_memput_lsync test.
///
/// This will allocate an lsync LCO at @p at test the lsync version.
///
/// @param       buffer The local buffer to put from.
/// @param            n The number of bytes to put.
/// @param        block The global address to put into.
/// @param           at The locality to allocate the lsync at.
void test_lsync(const uint64_t *buffer, size_t n, hpx_addr_t block,
                hpx_addr_t at) {
  hpx_addr_t lsync = HPX_NULL;
  CHECK( hpx_call_sync(at, future_at, &lsync, sizeof(lsync)) );
  CHECK( hpx_gas_memput_lsync(block, buffer, n, lsync) );
  CHECK( hpx_lco_wait(lsync) );
  CHECK( hpx_call_sync(block, verify, NULL, 0, buffer, n) );
  hpx_lco_delete_sync(lsync);
}

/// Test the gas_memput operation.
///
/// @param       buffer The local buffer to put from.
/// @param            n The number of bytes to put.
/// @param        block The global address to put into.
void test_memput(const uint64_t *buffer, size_t n, hpx_addr_t block) {
  test_assert(buffer != NULL);
  test_assert(block != HPX_NULL);
  test(buffer, n, block, HPX_HERE, HPX_HERE);
  test(buffer, n, block, block, block);
  test(buffer, n, block, HPX_HERE, block);
  test(buffer, n, block, block, HPX_HERE);

  unsigned here = HPX_LOCALITY_ID;
  unsigned up = (here + 1) % HPX_LOCALITIES;
  unsigned down = (here - 1) % HPX_LOCALITIES;

  test(buffer, n, block, HPX_THERE(up), HPX_THERE(down));
  test(buffer, n, block, HPX_THERE(down), HPX_THERE(up));
}

/// Test the gas_memput_lsync operation.
///
/// @param       buffer The local buffer to put from.
/// @param            n The number of bytes to put.
/// @param        block The global address to put into.
void test_memput_lsync(const uint64_t *buffer, size_t n, hpx_addr_t block) {
  test_assert(buffer != NULL);
  test_assert(block != HPX_NULL);
  test_lsync(buffer, n, block, HPX_HERE);
  test_lsync(buffer, n, block, block);

  unsigned here = HPX_LOCALITY_ID;
  unsigned up = (here + 1) % HPX_LOCALITIES;
  unsigned down = (here - 1) % HPX_LOCALITIES;

  test_lsync(buffer, n, block, HPX_THERE(up));
  test_lsync(buffer, n, block, HPX_THERE(down));
}

/// Test the gas_memput_rsync operation.
///
/// @param       buffer The local buffer to put from.
/// @param            n The number of bytes to put.
/// @param        block The global address to put into.
void test_memput_rsync(const uint64_t *buffer, size_t n, hpx_addr_t block) {
  CHECK( hpx_gas_memput_rsync(block, buffer, n) );
  CHECK( hpx_call_sync(block, verify, NULL, 0, buffer, n) );
}

/// Run all the tests for a particular buffer configuration.
///
/// This will run memput, memput_lsync, and memput_rsync to a local and remote
/// buffer.
///
/// @param       buffer The local buffer to put from.
/// @param            n The number of bytes to put.
void test_all(const uint64_t *buffer, size_t n) {
  printf("Testing hpx_gas_memput_rsync to a local block\n");
  test_memput_rsync(buffer, n, local);
  printf("Testing hpx_gas_memput_rsync to a remote block\n");
  test_memput_rsync(buffer, n, remote);
  printf("Testing hpx_gas_memput_lsync to a local block\n");
  test_memput_lsync(buffer, n, local);
  printf("Testing hpx_gas_memput_lsync to a remote block\n");
  test_memput_lsync(buffer, n, remote);
  printf("Testing hpx_gas_memput to a local block\n");
  test_memput(buffer, n, local);
  printf("Testing hpx_gas_memput to a remote block\n");
  test_memput(buffer, n, remote);
}

/// Set up the local block with a well-defined pattern so that we can verify
/// puts.
void prepare(uint64_t *buffer) {
  test_assert(buffer != NULL);
  for (int i = 0, e = ELEMENTS; i < e; ++i) {
    buffer[i] = i;
  }
}

/// Test memput from stack locations.
int memput_stack_handler(void) {
  printf("Testing gas memput from a stack location\n");
  uint64_t buffer[ELEMENTS];
  prepare(buffer);
  test_all(buffer, sizeof(buffer));
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, memput_stack, memput_stack_handler);

/// Testing memput from a static location.
int memput_static_handler(void) {
  printf("Testing gas memput from a static location\n");
  static uint64_t buffer[ELEMENTS] = {0};
  prepare(buffer);
  test_all(buffer, sizeof(buffer));
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, memput_static, memput_static_handler);

/// Testing memput from a registered buffer.
int memput_registered_handler(void) {
  printf("Testing gas memput from a registered location\n");
  size_t n = ELEMENTS * sizeof(uint64_t);
  uint64_t *buffer = hpx_malloc_registered(n);
  prepare(buffer);
  test_all(buffer, n);
  hpx_free_registered(buffer);
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, memput_registered, memput_registered_handler);

/// Testing memput from a malloced location.
int memput_malloc_handler(void) {
  printf("Testing gas memput from a malloced location\n");
  size_t n = ELEMENTS * sizeof(uint64_t);
  uint64_t *buffer = malloc(n);
  prepare(buffer);
  test_all(buffer, n);
  free(buffer);
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, memput_malloc, memput_malloc_handler);

TEST_MAIN({
    ADD_TEST(init_globals, 0);
    ADD_TEST(memput_stack, 0);
    ADD_TEST(memput_registered, 0);
    ADD_TEST(memput_registered, 1 % HPX_LOCALITIES);
    ADD_TEST(memput_malloc, 0);
    ADD_TEST(memput_malloc, 1 % HPX_LOCALITIES);
    ADD_TEST(memput_static, 0);
    ADD_TEST(memput_static, 1 % HPX_LOCALITIES);
    ADD_TEST(fini_globals, 0);
  });
