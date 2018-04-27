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

#include <stdio.h>

#include "pxgl/gteps.h"
#include "libpxgl/atomics.h"

#define MODUL ((distance_t) 1 << 62)

static void gteps_calculate_op_handler(size_t *const output, const size_t *const input, const size_t size) {
  *output = *output + *input ;
}

static HPX_ACTION(HPX_FUNCTION, 0, gteps_calculate_op, gteps_calculate_op_handler);

static void gteps_calculate_init_handler(void *init_val, const size_t init_val_size) {
  *(distance_t*)init_val = 0;
}

static HPX_ACTION(HPX_FUNCTION, 0, gteps_calculate_init, gteps_calculate_init_handler);

static hpx_action_t _gteps_send_dist = HPX_ACTION_INVALID;
static int _gteps_send_dist_action(const hpx_addr_t *const args) {
  const hpx_addr_t target = hpx_thread_current_target();

  adj_list_vertex_t vertex;
  adj_list_vertex_t *v;
  if (!hpx_gas_try_pin(target, (void**)&v))
    return HPX_RESEND;

  vertex = *v;
  hpx_gas_unpin(target);

  if(vertex.distance==SSSP_UINT_MAX){
    const size_t edge_traversed = 0;
    hpx_lco_set(*args, sizeof(edge_traversed), &edge_traversed, HPX_NULL, HPX_NULL);
  }
  else {
    sssp_uint_t local = atomic_load_explicit(&vertex.num_edges, memory_order_acquire);
    hpx_lco_set(*args, sizeof(vertex.num_edges), &local, HPX_NULL, HPX_NULL);
  }

  return HPX_SUCCESS;
}

static hpx_action_t _gteps_visit_vertex = HPX_ACTION_INVALID;
static int _gteps_visit_vertex_action(const hpx_addr_t *const args) {
  const hpx_addr_t target = hpx_thread_current_target();

  hpx_addr_t vertex;
  hpx_addr_t *v;
  if (!hpx_gas_try_pin(target, (void**)&v))
    return HPX_RESEND;

  vertex = *v;
  hpx_gas_unpin(target);

  return hpx_call(vertex, _gteps_send_dist, HPX_NULL, args, sizeof(*args));
}

hpx_action_t gteps_calculate = HPX_ACTION_INVALID;
int gteps_calculate_action(const sssp_uint_t *const num_vertices) {
  const hpx_addr_t adj_list = hpx_thread_current_target();
  hpx_addr_t calculate_lco = hpx_lco_allreduce_new(*num_vertices, 1, sizeof(size_t), gteps_calculate_init, gteps_calculate_op);

  for (int i = 0; i < *num_vertices; ++i) {
    const hpx_addr_t vertex_index = hpx_addr_add(adj_list, i * sizeof(hpx_addr_t), _index_array_block_size);
    hpx_call(vertex_index, _gteps_visit_vertex, HPX_NULL, &calculate_lco,
             sizeof(calculate_lco));
  }

  // printf("Finished with the loop\n");

  return HPX_THREAD_CONTINUE(calculate_lco);
}

static __attribute__((constructor)) void _gteps_register_actions() {
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              gteps_calculate, gteps_calculate_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              _gteps_visit_vertex, _gteps_visit_vertex_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              _gteps_send_dist, _gteps_send_dist_action,
              HPX_POINTER, HPX_SIZE_T);
}
