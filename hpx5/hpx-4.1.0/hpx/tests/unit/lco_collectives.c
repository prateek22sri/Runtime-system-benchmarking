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

// Goal of this testcase is to test collectives

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hpx/hpx.h>
#include "tests.h"

typedef struct Domain {
  hpx_addr_t newdt;
  hpx_addr_t complete;
  int nDoms;
  int rank;
  int maxCycles;
  int cycle;
} Domain;

/// Initialize a double zero.
static void _initDouble_handler(double *input, const size_t bytes) {
  *input = 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, _initDouble, _initDouble_handler);

/// Update *lhs with with the max(lhs, rhs);
static void _maxDouble_handler(double *lhs, const double *rhs, size_t UNUSED) {
  *lhs = (*lhs > *rhs) ? *lhs : *rhs;
}
static HPX_ACTION(HPX_FUNCTION, 0, _maxDouble, _maxDouble_handler);

static int _initDomain_handler(Domain *domain, hpx_addr_t newdt,
                               hpx_addr_t complete, int nDoms, int maxCycles) {
  domain->newdt = newdt;
  domain->complete = complete;
  domain->nDoms = nDoms;
  domain->rank = 0;
  domain->maxCycles = maxCycles;
  domain->cycle = 0;
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _initDomain, _initDomain_handler,
                  HPX_POINTER, HPX_ADDR, HPX_ADDR, HPX_INT, HPX_INT);

static HPX_ACTION_DECL(_advanceDomain_gather);
static int _advanceDomain_gather_handler(Domain *domain, int id,
                                         unsigned long epoch) {
  if (domain->maxCycles <= domain->cycle) {
    hpx_lco_set(domain->complete, 0, NULL, HPX_NULL, HPX_NULL);
    return HPX_SUCCESS;
  }

  // Compute my gnewdt, and then start the gather
  double gnewdt = 3.14 * (domain->rank + 1) + domain->cycle;
  hpx_lco_gather_setid(domain->newdt, id, sizeof(double), &gnewdt,
                       HPX_NULL, HPX_NULL);

  // Get the gathered value, and print the debugging string.
  double newdt[domain->nDoms];
  hpx_lco_get(domain->newdt, sizeof(newdt), &newdt);

  ++domain->cycle;
  unsigned long next = epoch + 1;
  hpx_addr_t local = hpx_thread_current_target();
  return hpx_call(local, _advanceDomain_gather, HPX_NULL, &id, &next);
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _advanceDomain_gather,
                  _advanceDomain_gather_handler, HPX_POINTER, HPX_INT, HPX_ULONG);

static int lco_gather_handler(void) {
  int nDoms = 8;
  int maxCycles = 1;

  hpx_addr_t domain = hpx_gas_calloc_cyclic(nDoms, sizeof(Domain), sizeof(Domain));
  hpx_addr_t done = hpx_lco_and_new(nDoms);
  hpx_addr_t complete = hpx_lco_and_new(nDoms);

  hpx_addr_t newdt = hpx_lco_gather_new(nDoms, nDoms, sizeof(double));

  for (int i = 0, e = nDoms; i < e; ++i) {
    hpx_addr_t block = hpx_addr_add(domain, sizeof(Domain) * i, sizeof(Domain));
    hpx_call(block, _initDomain, done, &newdt, &complete, &nDoms, &maxCycles);
  }

  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);

  const unsigned long epoch = 0;
  for (int i = 0, e = nDoms; i < e; ++i) {
    hpx_addr_t block = hpx_addr_add(domain, sizeof(Domain) * i, sizeof(Domain));
    hpx_call(block, _advanceDomain_gather, HPX_NULL, &i, &epoch);
  }

  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
  hpx_lco_delete(newdt, HPX_NULL);
  hpx_gas_free(domain, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_gather, lco_gather_handler);

static HPX_ACTION_DECL(_advanceDomain_alltoall);
static int _advanceDomain_alltoall_handler(Domain *domain, unsigned long epoch) {
  if (domain->maxCycles <= domain->cycle) {
    hpx_lco_set(domain->complete, 0, NULL, HPX_NULL, HPX_NULL);
    return HPX_SUCCESS;
  }

  // Compute my gnewdt, and then start the alltoall
  int gnewdt[domain->nDoms];
  for (int k=0; k < domain->nDoms; k++) {
    gnewdt[k] = (k+1) + domain->rank * domain->nDoms;
  }

  hpx_lco_alltoall_setid(domain->newdt, domain->rank,
                         domain->nDoms * sizeof(int),
                         gnewdt, HPX_NULL, HPX_NULL);

  // Get the gathered value, and print the debugging string.
  int newdt[domain->nDoms];
  hpx_lco_alltoall_getid(domain->newdt, domain->rank, sizeof(newdt), &newdt);

  ++domain->cycle;
  unsigned long next = epoch + 1;
  hpx_addr_t local = hpx_thread_current_target();
  return hpx_call(local, _advanceDomain_alltoall, HPX_NULL, &next);
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _advanceDomain_alltoall,
                  _advanceDomain_alltoall_handler, HPX_POINTER, HPX_ULONG);

static int lco_alltoall_handler(void) {
  int nDoms = 8;
  int maxCycles = 100;

  hpx_addr_t domain = hpx_gas_calloc_cyclic(nDoms, sizeof(Domain), sizeof(Domain));
  hpx_addr_t done = hpx_lco_and_new(nDoms);
  hpx_addr_t complete = hpx_lco_and_new(nDoms);

  hpx_addr_t newdt = hpx_lco_alltoall_new(nDoms, nDoms * sizeof(int));

  for (int i = 0, e = nDoms; i < e; ++i) {
    hpx_addr_t block = hpx_addr_add(domain, sizeof(Domain) * i, sizeof(Domain));
    hpx_call(block, _initDomain, done, &newdt, &complete, &nDoms, &maxCycles);
  }

  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);

  const unsigned long epoch = 0;
  for (int i = 0, e = nDoms; i < e; ++i) {
    hpx_addr_t block = hpx_addr_add(domain, sizeof(Domain) * i, sizeof(Domain));
    hpx_call(block, _advanceDomain_alltoall, HPX_NULL, &epoch);
  }

  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
  hpx_lco_delete(newdt, HPX_NULL);
  hpx_gas_free(domain, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, lco_alltoall, lco_alltoall_handler);

TEST_MAIN({
  ADD_TEST(lco_gather, 0);
  ADD_TEST(lco_alltoall, 0);
});
