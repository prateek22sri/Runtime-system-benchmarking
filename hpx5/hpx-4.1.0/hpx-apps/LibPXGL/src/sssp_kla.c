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

#include "hpx/hpx.h"
#include "libpxgl/sssp_common.h"
#include "libpxgl/termination.h"
#include "libpxgl/sssp_delta.h"
#include "libpxgl/sssp_dc.h"
#include <stdlib.h>
#include <stdio.h>


hpx_action_t _sssp_visit_vertex_klevel = 0;

static const size_t size_t_max = (size_t) -1;

typedef struct {
  adj_list_t graph;
  size_t k_current;
} kla_visit_args_t;

typedef struct {
  distance_t distance;
  hpx_addr_t graph;
  size_t k_current;
  hpx_addr_t result;
} _sssp_klevel_process_vertex_args_t;

void _insert_neighbors_next_klevel(const int thread_no, const size_t level, adj_list_t graph, adj_list_vertex_t *vertex, distance_t distance, size_t k_current) {
  const sssp_uint_t num_edges = vertex->num_edges;
  const distance_t old_distance = distance;

  for (int i = 0; i < num_edges; ++i) {
    adj_list_edge_t *e = &vertex->edge_list[i];
    distance = old_distance + e->weight;
    const hpx_addr_t index
      = hpx_addr_add(graph, e->dest * sizeof(hpx_addr_t), _index_array_block_size);
    //printf("Inserting vertex: %zu and distance: %zu in the next level bucket\n", index, distance);
    _insert_buckets(thread_no, level, index, distance);
  }
}

void _send_update_to_klevel_neighbors(adj_list_t graph, adj_list_vertex_t *vertex, distance_t distance, size_t k_current) {
  const sssp_uint_t num_edges = vertex->num_edges;
  const distance_t old_distance = distance;

  // increase active_count
  if (_get_termination() == COUNT_TERMINATION) {
    //printf("Increasing termination counts by %zu.\n", num_edges);
    _increment_active_count(num_edges);
  }

  hpx_addr_t edges = HPX_NULL;
  if (_get_termination() == AND_LCO_TERMINATION)
    edges = hpx_lco_and_new(num_edges);

  for (int i = 0; i < num_edges; ++i) {
    adj_list_edge_t *e = &vertex->edge_list[i];
    distance = old_distance + e->weight;

    // TBD: Add a check to stop sending if a better distance comes along

    const hpx_addr_t index
      = hpx_addr_add(graph, e->dest * sizeof(hpx_addr_t), _index_array_block_size);

    const _sssp_visit_vertex_klevel_args_t visit_args = { .graph = graph, .distance = distance, .k_current = k_current };

    //assert((visit_args.k_current)<=k_level);

    //printf("Calling _sssp_visit_vertex_klevel with vertex: %zu and distance: %zu and k_current = %zu\n", index, distance, (visit_args.k_current));

    hpx_call(index, _sssp_visit_vertex_klevel, HPX_NULL, &visit_args, sizeof(visit_args));
    //kla_send_vertex(index, _sssp_visit_vertex_klevel, &visit_args, sizeof(visit_args),
    //_get_termination() == AND_LCO_TERMINATION ? edges : HPX_NULL, k_current);
  }

  //printf("Distance Action waiting on edges on (%" SSSP_UINT_PRI ", %" PRIu32 ", %" PRIu32 ")\n", target.offset, target.base_id, target.block_bytes);
  if (_get_termination() == AND_LCO_TERMINATION) {
    hpx_lco_wait(edges);
    hpx_lco_delete(edges, HPX_NULL);
  }
}


hpx_action_t _sssp_klevel_process_vertex = 0;
static int _sssp_klevel_process_vertex_action(const _sssp_visit_vertex_klevel_args_t *const args) {
  const hpx_addr_t target = hpx_thread_current_target();

  adj_list_vertex_t *vertex;
  if (!hpx_gas_try_pin(target, (void**)&vertex))
    return HPX_RESEND;

  size_t k_current = (args->k_current);
  //printf("Distance Action on %" SSSP_UINT_PRI " with distance %" SSSP_UINT_PRI "\n", target, args->distance);
  //printf("k_current = %zu, k_level = %zu\n", k_current, k_level);
  if (_try_update_vertex_distance(vertex, args->distance)) {
    k_current += 1;
    if(k_current <= k_level) {
      //printf("Sending updates to neighbours\n");
      _send_update_to_klevel_neighbors(args->graph, vertex, args->distance, k_current);
    }
    else {
      //enqueue all the neighbours in the next bucket for next klevel
      //printf("Calling action for inserting into buckets for the next level\n");
      _insert_neighbors_next_klevel(HPX_THREAD_ID, buckets->current_level + 1,args->graph, vertex, args->distance, k_current);
    }
      //if (_get_termination() == AND_LCO_TERMINATION) hpx_lco_set(result, 0, NULL, HPX_NULL, HPX_NULL);//TODO
  }

  hpx_gas_unpin(target);

  if (_get_termination() == COUNT_TERMINATION) {
    //printf("Increased finish count\n");
    _increment_finished_count();
  }

  //if (_get_termination() == AND_LCO_TERMINATION) hpx_lco_set(edges, 0, NULL, HPX_NULL, HPX_NULL);


  //printf("Distance Action finished on %" SSSP_UINT_PRI "\n", target);

  return HPX_SUCCESS;
}

int _sssp_visit_vertex_klevel_action(const _sssp_visit_vertex_klevel_args_t *const args) {
  const hpx_addr_t target = hpx_thread_current_target();
  //assert( (args->k_current) <= k_level);
  //printf("-->visit_vertex at %zu with distance %" SSSP_UINT_PRI"\n", target, args->distance);
  //printf("k current inside visit_klevel_action = %zu\n", *(args->k_current));
  hpx_addr_t vertex;
  hpx_addr_t *v;
  if (!hpx_gas_try_pin(target, (void**)&v))
    return HPX_RESEND;

  vertex = *v;
  hpx_gas_unpin(target);

  const _sssp_visit_vertex_klevel_args_t sssp_args = { .graph = args->graph, .distance = args->distance, .k_current = args->k_current  };

  if (_get_termination() == AND_LCO_TERMINATION) {
    return hpx_call_sync(vertex, _sssp_process_vertex, NULL, 0, &sssp_args, sizeof(sssp_args));
  } else {
    //printf("Calling sssp kla process vertex\n");
    //assert( (sssp_args.k_current) <= k_level);
    return hpx_call(vertex, _sssp_process_vertex, HPX_NULL, &sssp_args, sizeof(sssp_args));
  }
}

typedef struct {
  buffer_t *buffer;
  hpx_addr_t graph;
  size_t k_current;
} _visit_all_in_buffer_klevel_args_t;

hpx_action_t _visit_all_in_klevel_buffer = 0;
static int _visit_all_in_klevel_buffer_action(const _visit_all_in_buffer_klevel_args_t * const args) {
  // We rely on a guaranteed affinity rather than just a suggestion
  //  hpx_thread_set_affinity(args->thread_id);
  //  assert(args->thread_id == HPX_THREAD_ID);
  // buffer_t * const buffer = buckets->buckets[args->thread_id][buckets->current_level];

  while(!empty(args->buffer)) {
    //printf("popping in visit_all. buffer: %zu, bucket [%d][%zu]\n.", args->buffer, HPX_THREAD_ID, buckets->current_level);
    buffer_node_t node = pop(args->buffer);
    //assert((args->k_current) <= k_level);
    const _sssp_visit_vertex_klevel_args_t sssp_args = { .graph = args->graph, .distance = node.distance, .k_current = args->k_current  };
    //printf("Calling sssp visit_vertex with vertex %zu and distance %" SSSP_UINT_PRI " (node.distance is: %zu in zu, and %" SSSP_UINT_PRI " in sssp_uint_pri\n", node.vertex, sssp_args.distance, node.distance, node.distance);
    hpx_call(node.vertex, _sssp_visit_vertex_klevel, HPX_NULL, &sssp_args, sizeof(sssp_args));
  }
  delete_buffer(args->buffer);
  //  buckets->buckets[args->thread_id][buckets->current_level] = NULL;
  return HPX_SUCCESS;
}

hpx_action_t _visit_all_in_current_klevel = 0;
static int _visit_all_in_current_klevel_action(const kla_visit_args_t * const args) {
  for(size_t i = 0; i < HPX_THREADS; ++i) {
    if(buckets->num_buckets[i] > buckets->current_level && buckets->buckets[i][buckets->current_level] != NULL) {
      //assert((args->k_current) <= k_level);
      const _visit_all_in_buffer_klevel_args_t visit_args = { .buffer = buckets->buckets[i][buckets->current_level], .graph = args->graph, .k_current = args->k_current };
      //printf("Calling visit all in k level buffer action.\n");
      hpx_call(HPX_HERE, _visit_all_in_klevel_buffer, HPX_NULL, &visit_args, sizeof(visit_args));
    }
  }
  return HPX_SUCCESS;
}


int _kla_sssp_send_vertex(const hpx_addr_t vertex, const hpx_action_t action, const _sssp_visit_vertex_klevel_args_t * const args, const size_t len, const hpx_addr_t result, const size_t *k_current) {
  // printf("delta_send_vertex, vertex: %zu, distance: %" SSSP_UINT_PRI "\n", vertex, args->distance);
  /* if(_get_level(args->distance) > buckets->current_level) { */
  /*   _insert_buckets(HPX_THREAD_ID, _get_level(args->distance), vertex, args->distance); */
  /*   if (_get_termination() == COUNT_TERMINATION) _increment_finished_count(); */
  /*   if (_get_termination() == AND_LCO_TERMINATION) hpx_lco_set(result, 0, NULL, HPX_NULL, HPX_NULL); */
  /* } else { */
  //printf("Inside kla_sssp_send_vertex with k current = %zu\n", (args->k_current));
  hpx_call(vertex, _sssp_visit_vertex_klevel, HPX_NULL, args, sizeof(*args));
    // printf("after delta_sssp_send vertex: %zu, distance: %" SSSP_UINT_PRI "\n", vertex, args->distance);
    //}
  return HPX_SUCCESS;
}


hpx_action_t call_kla_sssp = 0;
int call_kla_sssp_action(const call_sssp_args_t *const args) {
  // printf("KLA sssp called.\n");
  size_t k_current = 1;
  kla_visit_args_t kla_visit_args;
  const hpx_addr_t bcast_lco = hpx_lco_future_new(0);
  const hpx_addr_t initialize_graph_lco = hpx_lco_future_new(0);
  hpx_bcast(_init_buckets, HPX_NULL, bcast_lco, &args->delta, sizeof(args->delta));
  hpx_bcast(_sssp_initialize_graph, HPX_NULL, initialize_graph_lco, &args->graph,
            sizeof(args->graph));
  hpx_lco_wait(initialize_graph_lco);
  hpx_lco_wait(bcast_lco);
  hpx_lco_delete(initialize_graph_lco, HPX_NULL);
  hpx_lco_delete(bcast_lco, HPX_NULL);
  // printf("Init bucket done\n");
  const hpx_addr_t index
    = hpx_addr_add(args->graph, args->source * sizeof(hpx_addr_t), _index_array_block_size);
  // printf("Inserting index %zu into buckets. Source is %zu.\n", index, args->source);
  _insert_buckets(HPX_THREAD_ID, 0, index, 0);
  while(true) {
    //printf("Inside while loop\n");
    // The following code is similar to sssp_common.  The 2 should be unified somehow.
    k_current = 1;
    // DC is only supported with count termination now.
    //assert(_sssp_kind != DC_SSSP_KIND || _get_termination() == COUNT_TERMINATION);

    // Initialize counts and increment the active count for the first vertex.
    // We expect that the termination global variable is set correctly
    // on this locality.  This is true because call_sssp_action is called
    // with HPX_HERE and termination is set before the call by program flags.

    const hpx_addr_t termination_lco = hpx_lco_future_new(0);

    if (_get_termination() == COUNT_TERMINATION) {
      //printf("Working with the assumption that termination is based on count  termination\n");
      hpx_addr_t internal_termination_lco = hpx_lco_future_new(0);

      hpx_bcast_rsync(_initialize_termination_detection, NULL, 0);

      hpx_bcast_rsync(_delta_sssp_increase_active_counts, NULL, 0);

      kla_visit_args.graph = args->graph;
      kla_visit_args.k_current = k_current;
      // printf("Visiting all in level %zu.\n", buckets->current_level);
      hpx_bcast(_visit_all_in_current_klevel, HPX_NULL, HPX_NULL, &kla_visit_args, sizeof(kla_visit_args));
      // printf("starting termination detection\n");
      _detect_termination(termination_lco, internal_termination_lco);
    } else {
      // process termination should be easy to add
      fprintf(stderr, "sssp: invalid termination mode.\n");
      hpx_abort();
    }
    // Finish level
    hpx_lco_wait(termination_lco);
    hpx_lco_delete(termination_lco, HPX_NULL);

    //printf("Finding next level.\n");
    _find_next_level();
    //printf("After calling finding next level of buckets, next bucket level is:%zu\n", buckets->current_level);
    if (buckets->current_level == size_t_max) {
      //printf("before deleting buckets bucket current level %zu\n", buckets->current_level);
      hpx_bcast_rsync(_delete_buckets, NULL, 0);
      break;
      /* hpx_addr_t delete_termination_lco = hpx_lco_and_new(HPX_LOCALITIES); */
      /* hpx_bcast(_delete_buckets, HPX_NULL, &delete_termination_lco, sizeof(delete_termination_lco)); */
      /* hpx_lco_wait(delete_termination_lco); */
      /* hpx_lco_set(args->termination_lco, 0, NULL, HPX_NULL, HPX_NULL); */
      /* hpx_lco_delete(delete_termination_lco, HPX_NULL); */
      /* break; */
    }
  }
  hpx_lco_set(args->termination_lco, 0, NULL, HPX_NULL, HPX_NULL);
  return HPX_SUCCESS;
}

static HPX_CONSTRUCTOR void _sssp_kla_register_actions() {
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
		      call_kla_sssp, call_kla_sssp_action ,
		      HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
		       _visit_all_in_klevel_buffer, _visit_all_in_klevel_buffer_action,
		      HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
		       _visit_all_in_current_klevel, _visit_all_in_current_klevel_action,
		      HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
		      _sssp_klevel_process_vertex, _sssp_klevel_process_vertex_action,
		      HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
		      _sssp_visit_vertex_klevel, _sssp_visit_vertex_klevel_action,
		      HPX_POINTER, HPX_SIZE_T);
}
