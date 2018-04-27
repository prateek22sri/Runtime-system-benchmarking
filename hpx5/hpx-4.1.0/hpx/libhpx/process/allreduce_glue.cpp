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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "Allreduce.h"
#include "libhpx/debug.h"
#include "libhpx/events.h"
#include "libhpx/instrumentation.h"
#include "libhpx/locality.h"

namespace {
using libhpx::process::Allreduce;
constexpr size_t BSIZE = sizeof(Allreduce);
}

hpx_addr_t
hpx_process_collective_allreduce_new(size_t bytes, hpx_action_t reset,
                                     hpx_action_t op)
{
  // allocate and initialize a root node
  hpx_addr_t root = hpx_gas_alloc_local(1, BSIZE, 0);
  dbg_assert(root);
  hpx_addr_t null = HPX_NULL;
  dbg_check( hpx_call_sync(root, Allreduce::Init, nullptr, 0, &bytes, &null,
                           &reset, &op) );

  // allocate an array of local elements for the process
  int n = here->ranks;
  hpx_addr_t base = hpx_gas_alloc_cyclic(n, BSIZE, 0);
  dbg_assert(base);

  // initialize the array to point to the root as their parent (fat tree)
  hpx_addr_t done = hpx_lco_and_new(n);
  dbg_check( hpx_gas_bcast_with_continuation(Allreduce::Init, base, n, 0, BSIZE,
                                             hpx_lco_set_action, done, &bytes,
                                             &root, &reset, &op) );
  hpx_lco_wait(done);
  hpx_lco_delete_sync(done);

  // return the array
  EVENT_COLLECTIVE_NEW(base);
  return base;
}

void
hpx_process_collective_allreduce_delete(hpx_addr_t allreduce)
{
  Allreduce *r;
  hpx_addr_t proxy = hpx_addr_add(allreduce, here->rank * BSIZE, BSIZE);
  if (!hpx_gas_try_pin(proxy, reinterpret_cast<void**>(&r))) {
    dbg_error("could not pin local element for an allreduce\n");
  }
  hpx_addr_t root = r->getParent();
  hpx_gas_unpin(proxy);

  int n = here->ranks;
  hpx_addr_t done = hpx_lco_and_new(n + 1);
  dbg_check( hpx_gas_bcast_with_continuation(Allreduce::Fini, allreduce, n, 0,
                                             BSIZE, hpx_lco_set_action, done) );
  dbg_check( hpx_call(root, Allreduce::Fini, done) );
  hpx_lco_wait(done);
  hpx_lco_delete_sync(done);

  hpx_gas_free_sync(root);
  hpx_gas_free_sync(allreduce);
}

int32_t
hpx_process_collective_allreduce_subscribe(hpx_addr_t allreduce,
                                           hpx_action_t c_action,
                                           hpx_addr_t c_target)
{
  int id;
  hpx_addr_t leaf = hpx_addr_add(allreduce, here->rank * BSIZE, BSIZE);
  dbg_check( hpx_call_sync(leaf, Allreduce::Add, &id, sizeof(id), &c_action,
                           &c_target) );
  EVENT_COLLECTIVE_SUBSCRIBE(allreduce, c_action, c_target, id, here->rank);
  return id;
}

int hpx_process_collective_allreduce_subscribe_finalize(hpx_addr_t allreduce) {
  if (here->config->coll_network) {
    Allreduce *r = NULL;
    hpx_addr_t leaf = hpx_addr_add(allreduce, here->rank * BSIZE, BSIZE);
    if (!hpx_gas_try_pin(leaf, reinterpret_cast<void**>(&r))) {
      dbg_error("could not pin local element for an allreduce\n");
    }
    hpx_addr_t root = r->getParent();
    hpx_gas_unpin(leaf);
    dbg_check(hpx_call_sync(root, Allreduce::BCastComm, NULL, 0, &allreduce,
                            sizeof(hpx_addr_t)));
  }
  return HPX_SUCCESS;
}

void hpx_process_collective_allreduce_unsubscribe(hpx_addr_t allreduce,
                                                  int32_t id) {
  hpx_addr_t leaf = hpx_addr_add(allreduce, here->rank * BSIZE, BSIZE);
  dbg_check( hpx_call_sync(leaf, Allreduce::Remove, NULL, 0, &id) );
  EVENT_COLLECTIVE_UNSUBSCRIBE(allreduce, id, here->rank);
}

int hpx_process_collective_allreduce_join(hpx_addr_t allreduce,
                                          int32_t id, size_t bytes,
                                          const void *in) {
  hpx_addr_t proxy = hpx_addr_add(allreduce, here->rank * BSIZE, BSIZE);
  Allreduce *r = NULL;
  if (!hpx_gas_try_pin(proxy, reinterpret_cast<void**>(&r))) {
    dbg_error("could not pin local element for an allreduce\n");
  }
  r->reduce(in);
  hpx_gas_unpin(proxy);
  EVENT_COLLECTIVE_JOIN(allreduce, proxy, bytes, id, here->rank);
  return HPX_SUCCESS;
}
