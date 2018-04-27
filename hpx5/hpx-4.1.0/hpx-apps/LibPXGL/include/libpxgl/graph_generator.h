// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================


/* Use, modification and distribution is subject to the Boost Software     */
/* License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at */
/* http://www.boost.org/LICENSE_1_0.txt)                                   */
/*                                                                         */
/*  Authors: Jeremiah Willcock                                             */
/*           Andrew Lumsdaine                                              */

#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

//#include "user_settings.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include "pxgl/edge_list.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef GENERATOR_USE_PACKED_EDGE_TYPE

typedef struct packed_edge {
  uint32_t v0_low;
  uint32_t v1_low;
  uint32_t high; /* v1 in high half, v0 in low half */
} packed_edge;

static inline int64_t get_v0_from_edge(const packed_edge* p) {
  return (p->v0_low | ((int64_t)((int16_t)(p->high & 0xFFFF)) << 32));
}

static inline int64_t get_v1_from_edge(const packed_edge* p) {
  return (p->v1_low | ((int64_t)((int16_t)(p->high >> 16)) << 32));
}

static inline void write_edge(packed_edge* p, int64_t v0, int64_t v1) {
  p->v0_low = (uint32_t)v0;
  p->v1_low = (uint32_t)v1;
  p->high = (uint32_t)(((v0 >> 32) & 0xFFFF) | (((v1 >> 32) & 0xFFFF) << 16));
}

#else

typedef struct packed_edge {
  int64_t v0;
  int64_t v1;
} packed_edge;

static inline int64_t get_v0_from_edge(const packed_edge* p) {
  return p->v0;
}

static inline int64_t get_v1_from_edge(const packed_edge* p) {
  return p->v1;
}

static inline void write_edge(packed_edge* p, int64_t v0, int64_t v1) {
  p->v0 = v0;
  p->v1 = v1;
}

#endif

/* Generate a range of edges (from start_edge to end_edge of the total graph),
 * writing into elements [0, end_edge - start_edge) of the edges array.  This
 * code is parallel on OpenMP and XMT; it must be used with
 * separately-implemented SPMD parallelism for MPI. */
void generate_kronecker_range(
       const uint_fast32_t seed[5] /* All values in [0, 2^31 - 1) */,
       int logN /* In base 2 */,
       sssp_uint_t start_edge, sssp_uint_t end_edge /* Indices (in [0, M)) for the edges to generate */,
       const graph500_edge_list_local_args_t * const args);

void make_one_edge(int64_t nverts, int level, int lgN, mrg_state* st, const graph500_edge_list_local_args_t * const args/*packed_edge* result*/, uint64_t val0, uint64_t val1, sssp_uint_t edge_index);

#ifdef __cplusplus
}
#endif

#endif /* GRAPH_GENERATOR_H */
