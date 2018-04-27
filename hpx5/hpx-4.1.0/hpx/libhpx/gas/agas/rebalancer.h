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

#ifndef LIBHPX_GAS_AGAS_REBALANCER_H
#define LIBHPX_GAS_AGAS_REBALANCER_H

#include <hpx/hpx.h>

// AGAS Graph Partitioning API
hpx_addr_t agas_graph_new(void);
void agas_graph_delete(hpx_addr_t graph);
int agas_graph_construct(hpx_addr_t graph, void* input, size_t size,
                         int owner);
size_t agas_graph_partition(void *g, int nparts, uint64_t **partition);
size_t agas_graph_get_vtxs(void *graph, uint64_t **vtxs);
void agas_graph_get_owner_entry(void *graph, unsigned id, int *start,
                                int *end, int *owner);

#endif // LIBHPX_GAS_AGAS_REBALANCER_H
