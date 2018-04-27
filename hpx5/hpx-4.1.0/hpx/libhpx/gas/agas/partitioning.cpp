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
#include <libhpx/debug.h>
#include <libhpx/locality.h>
#include <libhpx/memory.h>
#include <algorithm>
#include <mutex>
#include "rebalancer.h"

#ifdef HAVE_METIS
#include <metis.h>
#endif

#ifdef HAVE_PARMETIS
#include <parmetis.h>

extern void *LIBHPX_COMM;
#endif

static std::mutex _lock;

// The owner map for vertices in the graph.
typedef struct _owner_map {
  uint64_t start; //!< the starting vertex id of vertices mapped to an owner
  int      owner; //!< the partition owner
} _owner_map_t;

// A graph representing the GAS accesses.
typedef struct agas_graph {
  int nvtxs;
  int nedges;
  _owner_map_t *owner_map;
  std::vector<uint64_t> vtxs;
  std::vector<uint64_t> vtxdist;
  std::vector<uint64_t> vwgt;
  std::vector<uint64_t> vsizes;
  std::vector<uint64_t> xadj;
  std::vector<uint64_t> adjncy;
  std::vector<uint64_t> adjwgt;
  std::vector<uint64_t> *lnbrs;
  unsigned count;
} _agas_graph_t;

// Initialize a AGAS graph.
// 
// This also adds n nodes to the graph from index 0 to n. The weights
// of these nodes are initialized with INT_MAX since we don't ever
// want two locality nodes to fall into one partition. The sizes are
// presently initialized to 0.

static void _init(_agas_graph_t *g) {
  g->nvtxs  = 0;
  g->nedges = 0;
  g->count  = 0;

  const unsigned n = here->ranks;
  g->owner_map = new _owner_map_t[n];
  g->lnbrs = new std::vector<uint64_t>[n];
  for (unsigned i = 0; i < n; ++i) {
    g->vtxs.push_back(i);
  }

  g->nvtxs += n;
  g->vwgt.assign(n, INT_MAX);
  g->vsizes.assign(n, 0);
  g->xadj.assign(n+1, 0);
}

// Free the AGAS graph.
static void _free(_agas_graph_t *g) {
  delete[] g->lnbrs;
  delete[] g->owner_map;
}

// Constructor for an AGAS block graph. Note that this allocates a
// graph in the global address space and returns a global address for
// the graph.
hpx_addr_t agas_graph_new(void) {
  _agas_graph_t *g = NULL;
  hpx_addr_t graph = hpx_gas_alloc_local(1, sizeof(*g), 0);
  if (!hpx_gas_try_pin(graph, (void**)&g)) {
    dbg_error("Could not pin newly allocated process.\n");
  }
  dbg_assert(graph != HPX_NULL && g);
  _init(g);
  hpx_gas_unpin(graph);
  return graph;
}

LIBHPX_ACTION(HPX_INTERRUPT, 0, GraphDelete, agas_graph_delete, HPX_ADDR);
void agas_graph_delete(hpx_addr_t graph) {
  _agas_graph_t *g = NULL;
  if (!hpx_gas_try_pin(graph, (void**)&g)) {
    int e = hpx_call(graph, GraphDelete, HPX_NULL, &graph);
    dbg_check(e, "Could not forward agas_graph_delete\n");
    return;
  }
  _free(g);
  hpx_gas_unpin(graph);
  hpx_gas_free(graph, HPX_NULL);
}

// Deserialize the CSR representation of the BST that is pointed to by
// the @p buf buffer of size @p size bytes.
static void _deserialize_bst(uint64_t *buf, size_t size, uint64_t *nvtxs,
                             uint64_t *nedges, uint64_t **vtxs, uint64_t **vwgt,
                             uint64_t **vsizes, uint64_t **xadj,
                             uint64_t **adjncy, uint64_t **adjwgt,
                             uint64_t **lsizes, uint64_t **lnbrs) {
  *nvtxs  = *buf; buf += 1;
  *vtxs   = buf;  buf += *nvtxs;
  *vwgt   = buf;  buf += *nvtxs;
  *vsizes = buf;  buf += *nvtxs;
  *xadj   = buf;  buf += *nvtxs;

  *nedges = *buf; buf += 1;

  *adjncy = buf;  buf += *nedges;
  *adjwgt = buf;  buf += *nedges;

  const unsigned ranks = here->ranks;
  *lsizes = buf;  buf += ranks;
  for (unsigned i = 0; i < ranks; ++i) {
    lnbrs[i] = buf; buf += (*lsizes)[i];
  }
}

// Construct a new AGAS graph from a deserialized buffer @p buf of
// size @p size bytes.
int agas_graph_construct(hpx_addr_t graph, void *input, size_t size,
                         int owner) {
  uint64_t nvtxs;
  uint64_t nedges;
  uint64_t *vtxs;
  uint64_t *vwgt;
  uint64_t *vsizes;
  uint64_t *xadj;
  uint64_t *adjncy;
  uint64_t *adjwgt;
  uint64_t *lsizes;

  int n = here->ranks;
  uint64_t *lnbrs[n];
  uint64_t *buf = static_cast<uint64_t*>(input);
  _deserialize_bst(buf, size, &nvtxs, &nedges, &vtxs, &vwgt, &vsizes,
                   &xadj, &adjncy, &adjwgt, &lsizes, lnbrs);

  _agas_graph_t *g = NULL;
  if (!hpx_gas_try_pin(graph, (void**)&g)) {
    dbg_error("Could not pin the graph. Did it move?\n");
    return HPX_ERROR;
  }

  _lock.lock();

  // add owner map entry
  _owner_map_t *map = &g->owner_map[g->count++];
  map->start = g->nvtxs;
  map->owner = owner;

  uint64_t *gxadj = static_cast<uint64_t*>(g->xadj.data());
  for (int i = 0; i < n; ++i) {
    gxadj[i+1] += lsizes[i];
    for (unsigned j = 0; j < lsizes[i]; ++j) {
      lnbrs[i][j] += g->nvtxs;
    }

    std::copy(lnbrs[i], lnbrs[i]+lsizes[i], std::back_inserter(g->lnbrs[i]));
  }

  g->nvtxs  += nvtxs;
  g->nedges += nedges;

  std::copy(vtxs, vtxs+nvtxs, std::back_inserter(g->vtxs));
  std::copy(vwgt, vwgt+nvtxs, std::back_inserter(g->vwgt));
  std::copy(vsizes, vsizes+nvtxs, std::back_inserter(g->vsizes));
  std::copy(xadj, xadj+nvtxs, std::back_inserter(g->xadj));
  std::copy(adjncy, adjncy+nedges, std::back_inserter(g->adjncy));
  std::copy(adjwgt, adjwgt+nedges, std::back_inserter(g->adjwgt));

  _lock.unlock();
  hpx_gas_unpin(graph);

  return HPX_SUCCESS;
}

static void _dump_agas_graph(_agas_graph_t *g) {
#if defined(ENABLE_INSTRUMENTATION) && defined(ENABLE_DEBUG)
  char filename[256];
  snprintf(filename, 256, "rebalancer_%d.graph", HPX_LOCALITY_ID);
  FILE *file = fopen(filename, "w");
  if (!file) {
    log_error("failed to open action id file %s\n", filename);
  }

  uint64_t *vwgt   = static_cast<uint64_t*>(g->vwgt.data());
  uint64_t *vsizes = static_cast<uint64_t*>(g->vsizes.data());
  uint64_t *xadj   = static_cast<uint64_t*>(g->xadj.data());
  uint64_t *adjncy = static_cast<uint64_t*>(g->adjncy.data());

  fprintf(file, "%d %d 110\n", g->nvtxs, g->nedges);
  for (int i = 0; i < g->nvtxs; ++i) {
    fprintf(file, "%lu %lu ", vsizes[i], vwgt[i]);
    for (int j = xadj[i]; j < xadj[i+1]; ++j) {
      fprintf(file, "%lu ", adjncy[i]);
    }
  }
  fprintf(file, "\n");

  int e = fclose(file);
  if (e) {
    log_error("failed to dump the AGAS graph\n");
  }
#endif
}

int _update_partition_id(int i, uint64_t *partition) {
  int id = partition[i];
  partition[i] = partition[id];
  return HPX_SUCCESS;
}

#ifdef HAVE_METIS
static size_t _metis_partition(_agas_graph_t *g, idx_t nparts,
                               uint64_t **partition) {
  idx_t options[METIS_NOPTIONS];
  METIS_SetDefaultOptions(options);
  options[METIS_OPTION_NUMBERING] = 0;
  options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_CUT;
  options[METIS_OPTION_CTYPE] = METIS_CTYPE_RM;

  idx_t ncon   = 1;
  idx_t objval = 0;

  idx_t nvtxs = g->nvtxs;
  *partition = calloc(nvtxs, sizeof(uint64_t));
  dbg_assert(partition);

  idx_t *vwgt   = static_cast<idx_t*>(g->vwgt.data());
  idx_t *vsizes = static_cast<idx_t*>(g->vsizes.data());
  idx_t *xadj   = static_cast<idx_t*>(g->xadj.data());
  idx_t *adjncy = static_cast<idx_t*>(g->adjncy.data());

  int e = METIS_PartGraphRecursive(&nvtxs, &ncon, xadj, adjncy, vwgt, vsizes,
            NULL, &nparts, NULL, NULL, options, &objval, (idx_t*)*partition);
  if (e != METIS_OK) {
    return 0;
  }

  hpx_par_for_sync(_update_partition_id, HPX_LOCALITIES, nvtxs, *partition);
  return g->nvtxs;
}
#endif


#ifdef HAVE_PARMETIS
static size_t _parmetis_partition(_agas_graph_t *g, idx_t nparts,
                                  uint64_t **partition) {
  idx_t options[1] = { 0 };

  idx_t ncon    = 1;
  idx_t wgtflag = 2;
  idx_t numflag = 0;
  idx_t edgecut = 0;

  idx_t nvtxs = g->nvtxs;
  *partition = calloc(nvtxs, sizeof(uint64_t));
  dbg_assert(partition);

  idx_t *vtxdist = static_cast<idx_t*>(g->vtxdist.data());
  idx_t *vwgt    = static_cast<idx_t*>(g->vwgt.data());
  idx_t *xadj    = static_cast<idx_t*>(g->xadj.data());
  idx_t *adjncy  = static_cast<idx_t*>(g->adjncy.data());

  int e = ParMETIS_V3_PartKway(vtxdist, xadj, adjncy, vwgt, NULL, &wgtflag,
                               &numflag, &ncon, &nparts, NULL, NULL, options,
                               &edgecut, (idx_t*)*partition, &LIBHPX_COMM);
  if (e != METIS_OK) {
    return 0;
  }
  return g->nvtxs;
}
#endif

// Perform any post-processing operations on the graph. In particular,
// we merge the locality nbrs array with the adjacency array to get
// the final adjacency array. We also fix the xadj array to reflect
// the correct positions into the adjacency array.
static void _postprocess_graph(_agas_graph_t *g) {
  // aggregate the locality nbrs array
  
  for (unsigned i = here->ranks; i > 0; --i) {
    g->adjncy.insert(g->adjncy.begin(), g->lnbrs[i].begin(), g->lnbrs[i].end());
  }
  g->nedges = g->adjncy.size();

  // fix the adj array
  uint64_t *xadj = static_cast<uint64_t*>(g->xadj.data());
  for (int i = 1; i < g->nvtxs+1; ++i) {
    xadj[i] += xadj[i-1];
  }
}

// Partition an AGAS graph into @p nparts number of partitions. The
// partitions are returned in the @p partition array which includes
// the indices of the nodes and their partition id.
size_t agas_graph_partition(void *graph, int nparts, uint64_t **partition) {
  _agas_graph_t *g = static_cast<_agas_graph_t*>(graph);
  _postprocess_graph(g);
  _dump_agas_graph(g);
#ifdef HAVE_METIS
  return _metis_partition(g, nparts, partition);
#elif HAVE_PARMETIS
  return _parmetis_partition(g, nparts, partition);
#else
  log_dflt("No partitioner found.\n");
#endif
  return 0;
}

// Get tht number of vertices/nodes in the AGAS graph.
size_t agas_graph_get_vtxs(void *graph, uint64_t **vtxs) {
  _agas_graph_t *g = (_agas_graph_t*)graph;
  dbg_assert(g);
  *vtxs = static_cast<uint64_t*>(g->vtxs.data());
  return g->nvtxs;
}

// Get the owner entries associated with the owner map of the AGAS
// graph.
void agas_graph_get_owner_entry(void *graph, unsigned id, int *start,
                               int *end, int *owner) {
  _agas_graph_t *g = (_agas_graph_t*)graph;
  dbg_assert(id >= 0 && id < here->ranks);
  _owner_map_t entry = g->owner_map[id];
  *owner = entry.owner;

  if (id == g->count - 1) {
    *start = entry.start;
    *end = g->nvtxs - 1;
  } else if (id >= g->count) {
    *start = 0;
    *end = 0;
  } else {
    *start = entry.start;
    _owner_map_t next_entry = g->owner_map[id+1];
    *end = next_entry.start - 1;
  }
}

