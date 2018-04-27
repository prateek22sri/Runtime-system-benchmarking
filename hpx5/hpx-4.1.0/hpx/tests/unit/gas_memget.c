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
#include "hpx/hpx.h"
#include "tests.h"

// Size of the data we're transferring.
enum { ELEMENTS = 32 };

hpx_addr_t   data = 0;
hpx_addr_t  local = 0;
hpx_addr_t remote = 0;

void fail(int i, uint64_t expected, uint64_t actual) {
  fprintf(stderr, "failed to set element %d correctly, "
          "expected %" PRIu64 ", got %" PRIu64 "\n", i, expected, actual);
  exit(EXIT_FAILURE);
}

void verify(uint64_t *buffer) {
  for (int i = 0, e = ELEMENTS; i < e; ++i) {
    if (buffer[i] != i) {
      fail(i, i, buffer[i]);
    }
    buffer[i] = 0;
  }
}

/// Allocate a 0-sized future at the target locality.
int future_at_handler(void) {
  hpx_addr_t f = hpx_lco_future_new(0);
  return HPX_THREAD_CONTINUE(f);
}
HPX_ACTION(HPX_INTERRUPT, 0, future_at, future_at_handler);

/// Initialize the global data for a rank.
int init_handler(hpx_addr_t d) {
  size_t n = ELEMENTS * sizeof(uint64_t);
  int rank = HPX_LOCALITY_ID;
  int peer = (rank + 1) % HPX_LOCALITIES;

  data = d;
  local = hpx_addr_add(data, rank * n, n);
  remote = hpx_addr_add(data, peer * n, n);

  uint64_t *buffer;
  test_assert( hpx_gas_try_pin(local, (void**)&buffer) );
  printf("initializing %"PRIu64" (%p at %d)\n", local, (void*)buffer, rank);
  for (int i = 0; i < ELEMENTS; ++i) {
    buffer[i] = i;
  }
  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, init, init_handler, HPX_ADDR);

int init_globals_handler(void) {
  size_t n = ELEMENTS * sizeof(uint64_t);
  hpx_addr_t data = hpx_gas_alloc_cyclic(HPX_LOCALITIES, n, 0);
  test_assert_msg(data != HPX_NULL, "failed to allocate data\n");

  CHECK( hpx_bcast_rsync(init, &data) );
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, init_globals, init_globals_handler);

int fini_globals_handler(void) {
  hpx_gas_free_sync(data);
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, fini_globals, fini_globals_handler);

/// Test a particular memget instantiation.
///
/// @param       buffer The local buffer to get into.
/// @param            n The number of bytes to get.
/// @param        block The global address to get from.
/// @param           at The locality to allocate the lsync at.
void test(void *buffer, size_t n, hpx_addr_t block, hpx_addr_t at) {
  hpx_addr_t lsync = HPX_NULL;
  CHECK( hpx_call_sync(at, future_at, &lsync, sizeof(lsync)) );
  CHECK( hpx_gas_memget(buffer, block, n, lsync) );
  CHECK( hpx_lco_wait(lsync) );
  verify(buffer);
  hpx_lco_delete_sync(lsync);
}

/// Test the gas_memget operation.
///
/// This will test memget using lsync LCOs at three different localities, the
/// source of the memget, the target of the memget, and some third locality.
///
/// @param       buffer The local buffer to get into.
/// @param            n The number of bytes to get.
/// @param        block The global address to get from.
void test_memget(void *buffer, size_t n, hpx_addr_t block) {
  test_assert(buffer != NULL);
  test_assert(block != HPX_NULL);
  test(buffer, n, block, HPX_HERE);
  test(buffer, n, block, remote);
  unsigned here = (unsigned)HPX_LOCALITY_ID;
  unsigned there = (here - 1) % HPX_LOCALITIES;
  test(buffer, n, block, HPX_THERE(there));
}

/// Test the gas_memget_sync operation.
///
/// @param       buffer The local buffer to get into.
/// @param            n The number of bytes to get.
/// @param        block The global address to get from.
void test_memget_sync(void *buffer, size_t n, hpx_addr_t block) {
  test_assert(buffer != NULL);
  test_assert(block != HPX_NULL);
  CHECK( hpx_gas_memget_sync(buffer, remote, n) );
  verify(buffer);
}

/// Run all the tests for a particular buffer.
///
/// This will run memget and memget_sync from a local and remote block.
///
/// @param       buffer The local buffer to get into.
/// @param            n The number of bytes to get.
void test_all(void *buffer, size_t n) {
  printf("Testing gas_memget_sync from a local block\n");
  test_memget_sync(buffer, n, local);
  printf("Testing gas_memget_sync from a remote block\n");
  test_memget_sync(buffer, n, remote);
  printf("Testing gas_memget from a local block\n");
  test_memget(buffer, n, local);
  printf("Testing gas_memget from a remote block\n");
  test_memget(buffer, n, remote);
}

/// Test memget to a stack location.
int memget_stack_handler(void) {
  printf("Testing hpx memget to a stack location\n");
  uint64_t buffer[ELEMENTS] = {0};
  test_all(buffer, sizeof(buffer));
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, memget_stack, memget_stack_handler);

/// Test memget to a registered location.
int memget_registered_handler(void) {
  printf("Testing hpx memget to a registered address\n");
  size_t n = ELEMENTS * sizeof(uint64_t);
  uint64_t *buffer = hpx_malloc_registered(n);
  test_all(buffer, n);
  hpx_free_registered(buffer);
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, memget_registered, memget_registered_handler);

/// Test memget to a static location.
int memget_static_handler(void) {
  printf("Testing hpx memget to a static address\n");
  static uint64_t buffer[ELEMENTS] = {0};
  test_all(buffer, sizeof(buffer));
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, memget_static, memget_static_handler);

/// Test memget to a malloced location.
int memget_malloc_handler(void) {
  printf("Testing hpx memget to a malloced address\n");
  size_t n = ELEMENTS * sizeof(uint64_t);
  uint64_t *buffer = malloc(n);
  test_all(buffer, n);
  free(buffer);
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, memget_malloc, memget_malloc_handler);

hpx_addr_t global_addr = 0;
hpx_addr_t barrier = 0;

int _putget_action(void *UNUSED, size_t size) {  
  int comm_rank = hpx_get_my_rank();
  int comm_size = hpx_get_num_ranks();

  int num_neighbors = 0;
  int *recv_length = 0;

  if (comm_rank % 2 == 0) {
    num_neighbors = 1;  
    recv_length = malloc(num_neighbors*sizeof(int));
    recv_length[0] = 256;
  } else {
    num_neighbors = 2;  
    recv_length = malloc(num_neighbors*sizeof(int));
    recv_length[0] = 256;
    recv_length[1] = 256;
  }

  // put the data in the global address space
  hpx_addr_t destb = hpx_addr_add(global_addr,
              comm_rank*comm_size*sizeof(int),comm_size*sizeof(int)); 

  hpx_gas_memput_rsync(destb,recv_length,num_neighbors*sizeof(int));

  hpx_lco_set(barrier, 0, NULL, HPX_NULL, HPX_NULL);
  free(recv_length);

  // wait for the memputs to finish
  hpx_lco_wait(barrier);

  // once everyone has finished these operations, get what we need
  int *buf = calloc(comm_size, comm_size*sizeof(int));
  for (int i = 0; i < comm_size; ++i) {
  hpx_addr_t destc = hpx_addr_add(global_addr,
              i*comm_size*sizeof(int),comm_size*sizeof(int));

    hpx_gas_memget_sync(buf+(i*comm_size),destc, comm_size*sizeof(int));
  }

  int sum = 0;
  int expected = 0;
  for (int i = 0; i < comm_size*comm_size; ++i) {
    if (i < comm_size) {
      expected += (((i%2 == 1)+1) * 256);
    }
    sum += buf[i];
  }

  free(buf);
  printf("sum = %d expected = %d\n", sum, expected);
  test_assert(sum == expected);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _putget, _putget_action, HPX_POINTER, HPX_SIZE_T);

int _bcast_addr_action(hpx_addr_t _global_addr, hpx_addr_t _barrier) {
  global_addr = _global_addr;
  barrier = _barrier;
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _bcast_addr, _bcast_addr_action, HPX_ADDR, HPX_ADDR);

/// Test memget of more than 1 block.
int memget_blocks_handler(void) {
  printf("Testing hpx memget of more than 1 blocks\n");
  int comm_size = hpx_get_num_ranks();
  hpx_addr_t global_addr = hpx_gas_calloc_cyclic(comm_size,comm_size*sizeof(int), 0);
  hpx_addr_t barrier = hpx_lco_and_new(comm_size);

  hpx_addr_t complete = hpx_lco_and_new(comm_size);
  for (int i=0;i<comm_size;i++) {
    hpx_call(HPX_THERE(i),_bcast_addr,complete,&global_addr,&barrier);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  complete = hpx_lco_and_new(comm_size);
  for (int i=0;i<comm_size;i++) {
    hpx_call(HPX_THERE(i),_putget,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
  hpx_lco_delete(barrier, HPX_NULL);
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, memget_blocks, memget_blocks_handler);

TEST_MAIN({
    ADD_TEST(init_globals, 0);
    ADD_TEST(memget_stack, 0);
    ADD_TEST(memget_stack, 1 % HPX_LOCALITIES);
    ADD_TEST(memget_registered, 0);
    ADD_TEST(memget_registered, 1 % HPX_LOCALITIES);
    ADD_TEST(memget_static, 0);
    ADD_TEST(memget_static, 1 % HPX_LOCALITIES);
    ADD_TEST(memget_malloc, 0);
    ADD_TEST(memget_malloc, 1 % HPX_LOCALITIES);
    ADD_TEST(memget_blocks, 0);
    ADD_TEST(fini_globals, 0);
  });
