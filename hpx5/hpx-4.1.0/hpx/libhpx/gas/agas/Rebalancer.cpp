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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <libhpx/action.h>
#include <libhpx/config.h>
#include <libhpx/debug.h>
#include <libhpx/locality.h>
#include <libhpx/memory.h>
#include <libhpx/parcel.h>
#include <libhpx/rebalancer.h>
#include <libhpx/Scheduler.h>
#include <libhpx/Worker.h>
#include <unordered_map>
#include "GlobalVirtualAddress.h"
#include "BlockTranslationTable.h"
#include "BlockStatisticsTable.h"
#include "rebalancer.h"

namespace {
using libhpx::self;
using GVA = libhpx::gas::agas::GlobalVirtualAddress;
using BST = libhpx::gas::agas::HierarchicalBST;
}

// Per-locality BST.
//
// During statistics aggration, all of the thread-local BSTs are
// aggregated into a per-locality BST.
static BST *_bst = NULL;

// Add an entry to the rebalancer's (thread-local) BST table.
///
/// @param      src The "src" locality accessing the block.
/// @param      dst The "dst" locality where the block is mapped.
/// @param    block The global address of the block.
/// @param     size The block's size in bytes.
///
//// @returns An error code, or HPX_SUCCESS.
void rebalancer_add_entry(int src, int dst, hpx_addr_t block, size_t size) {
  if (here->config->gas != HPX_GAS_AGAS) {
    return;
  }

  // ignore this block if it does not have the "load-balance"
  // (HPX_GAS_ATTR_LB) attribute
  GVA gva(block);
  uint32_t attr = here->gas->getAttribute(gva);
  if (likely(!(attr & HPX_GAS_ATTR_LB))) {
    return;
  }

  // add an entry to the BST
  _bst->add(gva, src, 1, size);
}

// Initialize the AGAS-based rebalancer.
int rebalancer_init(void) {
  _bst = new BST();
  dbg_assert(_bst);

  log_gas("GAS rebalancer initialized\n");
  return HPX_SUCCESS;
}

// Finalize the AGAS-based rebalancer.
void rebalancer_finalize(void) {
  if (_bst) {
    delete _bst;
    _bst = NULL;
  }
}

// Constructs a graph at the target global address from the serialized
// parcel payload.
static int _aggregate_global_bst_handler(void *data, size_t size) {
  hpx_addr_t graph = hpx_thread_current_target();
  const hpx_parcel_t *p = hpx_thread_current_parcel();
  return agas_graph_construct(graph, data, size, p->src);
}
static LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _aggregate_global_bst,
                     _aggregate_global_bst_handler, HPX_POINTER, HPX_SIZE_T);

// Aggregate the BST on a given locality.
//
// This function collects all of the block statistics on a given
// locality and serializes it into a parcel that is sent to the @p
// graph global address. All of the local BSTs are merged into a
// single global BST in parallel, before the global BST is serialized
// into a parcel. This function steals the current continuation and
// forwards it along to the generated parcel.
static int _aggregate_bst_handler(hpx_addr_t graph) {
  hpx_parcel_t *p = _bst->toParcel();
  log_gas("Global BST size: %u bytes.\n", p->size);
  if (!p || !p->size) {
    return HPX_SUCCESS;
  }
  p->target = graph;
  p->action = _aggregate_global_bst;

  // *take* the current continuation
  hpx_parcel_t *curr = self->getCurrentParcel();
  p->c_target = curr->c_target;
  p->c_action = curr->c_action;
  curr->c_target = HPX_NULL;
  curr->c_action = HPX_ACTION_NULL;

  hpx_parcel_send(p, HPX_NULL);
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_DEFAULT, 0, _aggregate_bst, _aggregate_bst_handler,
                     HPX_ADDR);


// Move blocks in bulk to their new owners.
static int
_bulk_move_handler(int n, void *args[], size_t sizes[]) {
  uint64_t      *vtxs = static_cast<uint64_t*>(args[0]);
  uint64_t *partition = static_cast<uint64_t*>(args[1]);

  size_t bytes = sizes[0];
  uint64_t count = bytes/sizeof(uint64_t);

  hpx_addr_t done = hpx_lco_and_new(count);
  for (unsigned i = 0; i < count; ++i) {
    unsigned new_owner = partition[i];
    if (new_owner != here->rank) {
      log_gas("move block 0x%lx from %d to %d\n", vtxs[i], here->rank, new_owner);
      hpx_gas_move(vtxs[i], HPX_THERE(new_owner), done);
    } else {
      hpx_lco_set(done, 0, NULL, HPX_NULL, HPX_NULL);
    }
  }
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_VECTORED, _bulk_move,
                     _bulk_move_handler, HPX_INT, HPX_POINTER, HPX_POINTER);

// Start rebalancing the blocks.
// This can be called by any locality in the system.
static int _rebalance_sync(uint64_t *partition, hpx_addr_t graph, void *g) {
  // get the vertex array
  uint64_t *vtxs = NULL;
  size_t nvtxs = agas_graph_get_vtxs(g, &vtxs);
  if (nvtxs > 0 && partition) {
    // rebalance blocks in each partition
    hpx_addr_t done = hpx_lco_and_new(here->ranks);
    for (unsigned i = 0; i < here->ranks; ++i) {
      int start, end, owner;
      agas_graph_get_owner_entry(g, i, &start, &end, &owner);
      size_t bytes = (end-start)*sizeof(uint64_t);
      if (!bytes) {
        hpx_lco_set(done, 0, NULL, HPX_NULL, HPX_NULL);
        continue;
      }
      hpx_call(HPX_THERE(owner), _bulk_move, done,
               vtxs+start, bytes, partition+start, bytes);
    }
    hpx_lco_wait(done);
    hpx_lco_delete(done, HPX_NULL);
  }

  hpx_gas_unpin(graph);
  free(partition);
  agas_graph_delete(graph);
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_DEFAULT, 0, _rebalance, _rebalance_sync,
                     HPX_POINTER, HPX_ADDR, HPX_POINTER);

// Start partitioning the aggregated BST.
//
static int _partition_sync(hpx_addr_t msync) {
  hpx_addr_t graph = hpx_thread_current_target();
  void *g = NULL;
  if (!hpx_gas_try_pin(graph, (void**)&g)) {
    return HPX_RESEND;
  }

  uint64_t *partition = NULL;
  size_t nvtxs = agas_graph_partition(g, here->ranks, &partition);
  log_gas("Finished partitioning block graph (%ld vertices)\n", nvtxs);
  return hpx_call(HPX_HERE, _rebalance, msync, &partition, &graph, &g);
}
static LIBHPX_ACTION(HPX_DEFAULT, 0, _partition, _partition_sync, HPX_ADDR);

// Aggregate the global BSTs.
//
static int _aggregate_sync(hpx_addr_t psync, hpx_addr_t msync) {
  log_gas("Starting GAS rebalancing\n");

  hpx_addr_t graph = agas_graph_new();
  // first, aggregate the "block" graph locally
  hpx_bcast_rsync(_aggregate_bst, &graph);
  log_gas("Block graph aggregated on locality %d\n", HPX_LOCALITY_ID);
  return hpx_call(graph, _partition, psync, &msync);
}
static LIBHPX_ACTION(HPX_DEFAULT, 0, _aggregate, _aggregate_sync, HPX_ADDR,
                     HPX_ADDR);

// Start rebalancing.
//
// The LCO @p sync can be used for completion notification. This can
// be called by any locality in the system.
int rebalancer_start(hpx_addr_t async, hpx_addr_t psync, hpx_addr_t msync) {
  if (here->config->gas != HPX_GAS_AGAS) {
    hpx_lco_set(async, 0, NULL, HPX_NULL, HPX_NULL);
    hpx_lco_set(psync, 0, NULL, HPX_NULL, HPX_NULL);
    hpx_lco_set(msync, 0, NULL, HPX_NULL, HPX_NULL);
    return 0;
  }

  return hpx_call(HPX_HERE, _aggregate, async, &psync, &msync);
}
