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

#ifndef LIBPXGL_SSSP_COMMON_H
#define LIBPXGL_SSSP_COMMON_H

#include <pxgl/pxgl.h>

typedef struct {
  adj_list_t graph;
  sssp_uint_t distance;
} _sssp_visit_vertex_args_t;

typedef struct {
  distance_t distance;
  hpx_addr_t graph;
  size_t k_current;  
} _sssp_visit_vertex_klevel_args_t;


int _kla_sssp_send_vertex(const hpx_addr_t vertex, const hpx_action_t action, const _sssp_visit_vertex_klevel_args_t * const args, const size_t len, const hpx_addr_t result, const size_t *k_current);

//typedef int (*send_vertex_t)(hpx_addr_t, hpx_action_t, const void*, size_t, hpx_addr_t);
typedef int (*send_vertex_t)(hpx_addr_t, hpx_action_t, hpx_addr_t, void*, size_t);
//typedef int (*send_vertex_t)(hpx_addr_t, hpx_action_t, hpx_addr_t, ...);
//typedef int (*kla_send_vertex_t)(hpx_addr_t, hpx_action_t, const void*, size_t, hpx_addr_t, size_t*);
typedef int (*kla_send_vertex_t)(hpx_addr_t, hpx_action_t, hpx_addr_t, void*, size_t);

bool _try_update_vertex_distance(adj_list_vertex_t *const vertex, distance_t distance);
void _send_update_to_neighbors(adj_list_vertex_t *const vertex, distance_t distance);
extern hpx_action_t _sssp_visit_vertex;
extern hpx_action_t _sssp_process_vertex;
extern sssp_kind_t _sssp_kind;
extern size_t k_level;
extern adj_list_t graph;
extern hpx_action_t _sssp_initialize_graph;
extern send_vertex_t send_vertex;
extern kla_send_vertex_t kla_send_vertex;

#endif // LIBPXGL_SSSP_COMMON_H
