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

// Goal of this testcase is to test the HPX Memory Allocation
// 1. hpx_gas_alloc_local() -- Allocates the global memory.
// 3. hpx_gas_try_pin() -- Performs address translation.
// 4. hpx_gas_unpin() -- Allows an address to be remapped.

#include <stdio.h>
#include <stdlib.h>
#include <hpx/hpx.h>
#include "tests.h"

static const int N = 10;

static int gas_alloc_handler(void) {
  printf("Starting the GAS local memory allocation test\n");
  hpx_addr_t local = hpx_gas_alloc_local(1, N, 0);

  if (!local) {
    fflush(stdout);
    fprintf(stderr, "hpx_gas_alloc returned HPX_NULL\n");
    exit(EXIT_FAILURE);
  }

  if (!hpx_gas_try_pin(local, NULL)) {
    fflush(stdout);
    fprintf(stderr, "gas alloc returned non-local memory\n");
    exit(EXIT_FAILURE);
  }

  hpx_gas_free(local, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, gas_alloc, gas_alloc_handler);

static int gas_memalign_handler(void) {
  printf("Starting GAS memalign\n");
  for (int i = 4, e = 24; i < e; ++i) {
    unsigned long alignment = (UINT64_C(1) << i);
    printf("checking alignment %lu\n", alignment);
    hpx_addr_t local = hpx_gas_alloc_local(1, N, alignment);
    if (!local) {
      fflush(stdout);
      fprintf(stderr, "hpx_gas_alloc_local returned HPX_NULL\n");
    }
    void *ptr = NULL;
    if (!hpx_gas_try_pin(local, &ptr)) {
      fflush(stdout);
      fprintf(stderr, "hpx_gas_alloc_local returned non-local memory\n");
    }
    if ((uintptr_t)ptr & (alignment - 1)) {
      fflush(stdout);
      fprintf(stderr, "hpx_gas_alloc_local failed to return aligned address\n");
    }
    hpx_gas_unpin(local);
    hpx_gas_free(local, HPX_NULL);
  }

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, gas_memalign, gas_memalign_handler);

static int gas_calloc_handler(void) {
  printf("Starting GAS calloc\n");
  hpx_addr_t local = hpx_gas_calloc_local(N, sizeof(int), 0);

  if (!local) {
    fflush(stdout);
    fprintf(stderr, "hpx_gas_calloc returned HPX_NULL\n");
    exit(EXIT_FAILURE);
  }

  int *buffer = NULL;
  if (!hpx_gas_try_pin(local, (void**)&buffer)) {
    fflush(stdout);
    fprintf(stderr, "gas calloc returned non-local memory\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < N; ++i) {
    int *chunk;
    hpx_addr_t addr = hpx_addr_add(local, i * sizeof(int), sizeof(int));
    if (!hpx_gas_try_pin(addr, (void**)&chunk)) {
      fflush(stdout);
      fprintf(stderr, "gas calloc could not pin internal chunk\n");
      exit(EXIT_FAILURE);
    }

    if (chunk != buffer + i) {
      fflush(stdout);
      fprintf(stderr, "pinned chunk address does not match\n");
      exit(EXIT_FAILURE);
    }

    hpx_gas_unpin(addr);

    if (buffer[i] != 0) {
      fflush(stdout);
      fprintf(stderr, "gas calloc returned uninitialized memory\n");
      exit(EXIT_FAILURE);
    }
  }

  hpx_gas_unpin(local);
  hpx_gas_free(local, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, gas_calloc, gas_calloc_handler);

static int verify_at_handler(hpx_addr_t addr, int zero) {
  int *buffer = NULL;
  if (!hpx_gas_try_pin(addr, (void**)&buffer)) {
    fprintf(stderr, "address not located at correct locality\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < N; ++i) {
    if (zero && buffer[i] != 0) {
      fflush(stdout);
      fprintf(stderr, "gas calloc returned uninitialized memory\n");
      exit(EXIT_FAILURE);
    }
  }

  hpx_gas_unpin(addr);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_INTERRUPT, 0, verify_at, verify_at_handler, HPX_ADDR, HPX_INT);

static int gas_alloc_at_handler(void) {
  printf("Starting the GAS remote memory allocation test\n");
  int peer = (HPX_LOCALITY_ID + 1) % HPX_LOCALITIES;
  hpx_addr_t addr = hpx_gas_alloc_local_at_sync(N, sizeof(int), 0, HPX_THERE(peer));
  if (!addr) {
    fflush(stdout);
    fprintf(stderr, "failed to allocate memory at %d\n", peer);
    exit(EXIT_FAILURE);
  }

  int zero = 0;
  hpx_call_sync(HPX_THERE(peer), verify_at, NULL, 0, &addr, &zero);
  hpx_addr_t wait = hpx_lco_future_new(0);
  hpx_gas_free(addr, wait);
  hpx_lco_wait(wait);
  hpx_lco_delete(wait, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, gas_alloc_at, gas_alloc_at_handler);

static int gas_calloc_at_handler(void) {
  printf("Starting the GAS initialized remote memory allocation test\n");
  int peer = (HPX_LOCALITY_ID + 1) % HPX_LOCALITIES;
  hpx_addr_t addr = hpx_gas_calloc_local_at_sync(N, sizeof(int), 0, HPX_THERE(peer));
  if (!addr) {
    fflush(stdout);
    fprintf(stderr, "failed to allocate memory at %d\n", peer);
    exit(EXIT_FAILURE);
  }

  int zero = 1;
  hpx_call_sync(HPX_THERE(peer), verify_at, NULL, 0, &addr, &zero);
  hpx_addr_t wait = hpx_lco_future_new(0);
  hpx_gas_free(addr, wait);
  hpx_lco_wait(wait);
  hpx_lco_delete(wait, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, gas_calloc_at, gas_calloc_at_handler);

TEST_MAIN({
    ADD_TEST(gas_alloc, 0);
    ADD_TEST(gas_alloc_at, 0);
    ADD_TEST(gas_calloc, 0);
    ADD_TEST(gas_calloc_at, 0);
    ADD_TEST(gas_memalign, 0);
  });
