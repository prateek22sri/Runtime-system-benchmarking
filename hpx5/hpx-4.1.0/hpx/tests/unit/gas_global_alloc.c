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
// 1. hpx_gas_global_free() -- Free a global allocation.
// 2. hpx_gas_alloc_cyclic() -- Allocates the distributed
//                              global memory
#include "hpx/hpx.h"
#include "tests.h"

typedef struct Domain {
  int nDoms;
  int rank;
  int maxcycles;
} Domain;

static int _initDomain_handler(Domain *ld, int rank, int max, int n) {
  ld->rank = rank;
  ld->maxcycles = max;
  ld->nDoms = n;
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _initDomain,
           _initDomain_handler, HPX_POINTER, HPX_INT, HPX_INT, HPX_INT);

// Test code -- for global memory allocation
static int gas_global_alloc_handler(void) {
  // allocate the default argument structure on the stack

  int nDoms = 8;
  int maxCycles = 1;

  printf("Starting the GAS global memory allocation test\n");
  // allocate and start a timer
  hpx_time_t t1 = hpx_time_now();

 // output the arguments we're running with
  printf("Number of domains: %d maxCycles: %d.\n",
          nDoms, maxCycles);
  fflush(stdout);

  // Allocate the domain array
  hpx_addr_t domain = hpx_gas_alloc_cyclic(nDoms, sizeof(Domain), 0);

  // Allocate an and gate that we can wait on to detect that all of the domains
  // have completed initialization.
  hpx_addr_t done = hpx_lco_and_new(nDoms);

  // Send the initDomain action to all of the domains, in parallel.
  for (int i = 0, e = nDoms; i < e; ++i) {
    // compute the offset for this domain and send the initDomain action, with
    // the done LCO as the continuation
    hpx_addr_t block = hpx_addr_add(domain, sizeof(Domain) * i, sizeof(Domain));
    hpx_call(block, _initDomain, done, &i, &maxCycles, &nDoms);
  }

  // wait for initialization
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);

  // and free the domain
  hpx_gas_free(domain, HPX_NULL);

  printf(" Elapsed: %g\n", hpx_time_elapsed_ms(t1));
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, gas_global_alloc, gas_global_alloc_handler);

static int gas_global_alloc_block_handler(void) {
  hpx_addr_t data = hpx_gas_alloc_cyclic(1, 1024 * sizeof(char), 0);
  hpx_gas_free(data, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, gas_global_alloc_block,
                  gas_global_alloc_block_handler);

static int gas_global_calloc_block_handler(void) {
  hpx_addr_t global = hpx_gas_calloc_cyclic(1, 1024 *sizeof(char), 0);
  hpx_gas_free(global, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, gas_global_calloc_block,
                  gas_global_calloc_block_handler);

static int gas_global_mem_alloc_handler(void) {
  uint64_t size = 1024*1024*100;
  int blocks = HPX_LOCALITIES;

  hpx_addr_t local = hpx_gas_alloc_local(1, size, 0);
  hpx_gas_free(local, HPX_NULL);

  hpx_addr_t global = hpx_gas_alloc_cyclic(blocks, size, 0);
  hpx_gas_free(global, HPX_NULL);

  hpx_addr_t calloc = hpx_gas_calloc_cyclic(blocks, size, 0);
  hpx_gas_free(calloc, HPX_NULL);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, gas_global_mem_alloc,
                  gas_global_mem_alloc_handler);

TEST_MAIN({
    ADD_TEST(gas_global_alloc, 0);
    ADD_TEST(gas_global_alloc_block, 0);
    ADD_TEST(gas_global_calloc_block, 0);
    ADD_TEST(gas_global_mem_alloc, 0);
});
