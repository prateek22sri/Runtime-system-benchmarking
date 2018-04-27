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

#include "pxgl/dimacs.h"
#include "libpxgl/atomics.h"

#define MODUL ((distance_t) 1 << (sizeof(distance_t) * 8 - 2))

static void dimacs_checksum_op_handler(distance_t *const output, const distance_t *const input, const size_t size) {
  *output = *output + (*input == SSSP_UINT_MAX ? 0 : *input % MODUL) % MODUL;
}

static HPX_ACTION(HPX_FUNCTION, 0, dimacs_checksum_op, dimacs_checksum_op_handler);

static void dimacs_checksum_init_handler(void *init_val, const size_t init_val_size) {
  *(distance_t*)init_val = 0;
}

static HPX_ACTION(HPX_FUNCTION, 0, dimacs_checksum_init, dimacs_checksum_init_handler);

static hpx_action_t _dimacs_send_dist = 0;
static int _dimacs_send_dist_action(const hpx_addr_t *const args) {
  const hpx_addr_t target = hpx_thread_current_target();

  adj_list_vertex_t vertex;
  adj_list_vertex_t *v;
  if (!hpx_gas_try_pin(target, (void**)&v))
    return HPX_RESEND;

  vertex = *v;
  hpx_gas_unpin(target);

  const distance_t current_d = atomic_load_explicit(&vertex.distance,
                                                    memory_order_relaxed);
  hpx_lco_set(*args, sizeof(vertex.distance), &current_d, HPX_NULL, HPX_NULL);

  return HPX_SUCCESS;
}

static hpx_action_t _dimacs_visit_vertex = 0;
static int _dimacs_visit_vertex_action(const hpx_addr_t *const args) {
  const hpx_addr_t target = hpx_thread_current_target();

  hpx_addr_t vertex;
  hpx_addr_t *v;
  if (!hpx_gas_try_pin(target, (void**)&v))
    return HPX_RESEND;

  vertex = *v;
  hpx_gas_unpin(target);

  return hpx_call(vertex, _dimacs_send_dist, HPX_NULL, args, sizeof(*args));
}

hpx_action_t dimacs_checksum = 0;
int dimacs_checksum_action(const sssp_uint_t *const num_vertices) {
  const hpx_addr_t adj_list = hpx_thread_current_target();
  hpx_addr_t checksum_lco = hpx_lco_allreduce_new(*num_vertices, 1, sizeof(distance_t), dimacs_checksum_init, dimacs_checksum_op);
  for (int i = 0; i < *num_vertices; ++i) {
    const hpx_addr_t vertex_index = hpx_addr_add(adj_list, i * sizeof(hpx_addr_t), _index_array_block_size);
    hpx_call(vertex_index, _dimacs_visit_vertex, HPX_NULL, &checksum_lco,
             sizeof(checksum_lco));
  }

  // printf("Finished with the loop\n");

  return HPX_THREAD_CONTINUE(checksum_lco);
}

static __attribute__((constructor)) void _dimacs_register_actions() {
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              dimacs_checksum, dimacs_checksum_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              _dimacs_visit_vertex, _dimacs_visit_vertex_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              _dimacs_send_dist, _dimacs_send_dist_action,
              HPX_POINTER, HPX_SIZE_T);
}
