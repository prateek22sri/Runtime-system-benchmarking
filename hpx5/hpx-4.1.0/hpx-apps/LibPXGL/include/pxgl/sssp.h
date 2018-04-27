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
// ===========================================================================

#include "defs.h"

#ifndef PXGL_SSSP_H
#define PXGL_SSSP_H

typedef struct {
  adj_list_t graph;
  vertex_t source;
  hpx_addr_t termination_lco;
  size_t delta;
  size_t k_level;
} call_sssp_args_t;

typedef enum {_CHAOTIC_SSSP_KIND, _DC_SSSP_KIND, _DC1_SSSP_KIND, KLEVEL_SSSP_KIND } sssp_kind_t;
typedef enum {_GRAPH500, _DIMACS} graph_generator_type_t;

extern hpx_action_t call_sssp;
extern hpx_action_t sssp_run_delta_stepping;
extern hpx_action_t call_delta_sssp;
extern hpx_action_t sssp_run_kla;
extern hpx_action_t call_kla_sssp;
extern hpx_action_t set_klevel;
extern hpx_action_t sssp_init_dc;
typedef struct {
  size_t num_pq;
  size_t yield_count;
  size_t freq;
  size_t queue_threshold;
  size_t num_elem;
} sssp_init_dc_args_t;
extern hpx_action_t initialize_sssp_kind;

#endif // PXGL_SSSP_H
