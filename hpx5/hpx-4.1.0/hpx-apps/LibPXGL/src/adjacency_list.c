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

#include <stdatomic.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "hpx/hpx.h"
#include "libpxgl/termination.h"
#include "libpxgl/adjacency_list.h"
#include "libpxgl/atomics.h"

#define PXGL_ADJ_SYNC_ARG(orig, lco_name, arg_name) \
typedef struct { \
  orig; \
  hpx_addr_t lco_name; \
} arg_name##_args_t;

typedef uint32_t count_t;

hpx_addr_t count_array;
hpx_addr_t index_array;

sssp_uint_t _count_array_block_size = 0;
sssp_uint_t _index_array_block_size = 0;

static hpx_action_t _set_count_array_bsize = HPX_ACTION_INVALID;
static int _set_count_array_bsize_action(const uint32_t *arg) {
  _count_array_block_size = *arg;
  return HPX_SUCCESS;
}

static hpx_action_t _set_count_array = HPX_ACTION_INVALID;
static int _set_count_array_action(const hpx_addr_t *arg) {
  count_array = *arg;
  return HPX_SUCCESS;
}

static hpx_action_t _set_index_array_bsize = HPX_ACTION_INVALID;
static int _set_index_array_bsize_action(const uint32_t *arg) {
  _index_array_block_size = *arg;
  return HPX_SUCCESS;
}

static hpx_action_t _set_index_array = HPX_ACTION_INVALID;
static int _set_index_array_action(const hpx_addr_t *arg) {
  index_array = *arg;
  return HPX_SUCCESS;
}

// Action to increment count in the count array
static hpx_action_t _increment_count = HPX_ACTION_INVALID;
static int _increment_count_action(const void *const args) {
  const hpx_addr_t target = hpx_thread_current_target();

  //printf("_increment_count with %zu.\n", target);

  _Atomic count_t * const count;
  if (!hpx_gas_try_pin(target, (void**)&count))
    return HPX_RESEND;

  atomic_fetch_add_explicit(count, 1, memory_order_relaxed);

  _increment_finished_count();

  hpx_gas_unpin(target);
  return HPX_SUCCESS;
}


// Action to count an edge
static hpx_action_t _count_edge = HPX_ACTION_INVALID;
static int _count_edge_action(const void * const args) {
  const hpx_addr_t target = hpx_thread_current_target();

  // printf("_count_edge with %zu.\n", target);

  const edge_list_edge_t * const edge;
  if (!hpx_gas_try_pin(target, (void**)&edge))
    return HPX_RESEND;

  // if(*count_array != 272629760) printf("Huston, we have a problem: %zu.\n", *count_array);

  const hpx_addr_t count = hpx_addr_add(count_array, edge->source * sizeof(count_t),
                                        _count_array_block_size);
  hpx_gas_unpin(target);

  return hpx_call(count, _increment_count, HPX_NULL, NULL, 0);
}


// Initialize vertex adjacency list size and distance = inf
static hpx_action_t _init_vertex = HPX_ACTION_INVALID;
static int _init_vertex_action(const hpx_addr_t * const vertices_sync) {
   const hpx_addr_t target = hpx_thread_current_target();

   adj_list_vertex_t *vertex;
   if (!hpx_gas_try_pin(target, (void**)&vertex))
     return HPX_RESEND;

   vertex->num_edges = 0;
   vertex->distance = SSSP_UINT_MAX;

   _increment_finished_count();

   hpx_gas_unpin(target);
   return HPX_SUCCESS;
}

static hpx_action_t _alloc_vertex = HPX_ACTION_INVALID;
static int _alloc_vertex_action(const count_t * const count) {
  const hpx_addr_t target = hpx_thread_current_target();

  hpx_addr_t *index;
  if (!hpx_gas_try_pin(target, (void**)&index))
    return HPX_RESEND;

  // We first pin the index and then allocate to get the blocks on the
  // same locality (at least initially)
  hpx_addr_t vertex = hpx_gas_alloc_local(1, sizeof(adj_list_vertex_t) + (*count * sizeof(adj_list_edge_t)), 0);

  *index = vertex;
  hpx_gas_unpin(target);

  return hpx_call(vertex, _init_vertex, HPX_NULL, NULL, 0);
}


// Action to allocate an adjacency list entry: a vertex structure that
// holds the list of edges.
static hpx_action_t _alloc_adj_list_entry = HPX_ACTION_INVALID;
static int _alloc_adj_list_entry_action(const void * const args)
{
  const hpx_addr_t target = hpx_thread_current_target();

  _Atomic count_t * local;
  if (!hpx_gas_try_pin(target, (void**)&local)) {
    return HPX_RESEND;
  }

  const count_t count = atomic_load_explicit(local, memory_order_acquire);

  hpx_gas_unpin(target);

  const int64_t offset = hpx_addr_sub(target, count_array, _count_array_block_size) / sizeof(count_t);
  const hpx_addr_t index = hpx_addr_add(index_array, offset * sizeof(hpx_addr_t), _index_array_block_size);
  return hpx_call(index, _alloc_vertex, HPX_NULL, &count, sizeof(count));
}

static hpx_action_t _put_edge = HPX_ACTION_INVALID;
static int _put_edge_action(const adj_list_edge_t *edge)
{
  const hpx_addr_t target = hpx_thread_current_target();

  adj_list_vertex_t *vertex;
  if (!hpx_gas_try_pin(target, (void**)&vertex))
    return HPX_RESEND;

  // Increment the size of the vertex
  sssp_uint_t num_edges = atomic_fetch_add_explicit(&vertex->num_edges, 1, 
                                                    memory_order_relaxed);
  vertex->edge_list[num_edges] = *edge;

  _increment_finished_count();

  hpx_gas_unpin(target);
  return HPX_SUCCESS;
}


PXGL_ADJ_SYNC_ARG(adj_list_edge_t edge, edges_sync, _put_edge_index)
static hpx_action_t _put_edge_index = HPX_ACTION_INVALID;
static int _put_edge_index_action(const adj_list_edge_t *edge)
{
  const hpx_addr_t target = hpx_thread_current_target();

  hpx_addr_t vertex;
  hpx_addr_t *v;
  if (!hpx_gas_try_pin(target, (void**)&v))
    return HPX_RESEND;

  vertex = *v;
  hpx_gas_unpin(target);

  return hpx_call(vertex, _put_edge, HPX_NULL, edge, sizeof(*edge));
}

static hpx_action_t _insert_edge = HPX_ACTION_INVALID;
static int _insert_edge_action(const void * const args)
{
  const hpx_addr_t target = hpx_thread_current_target();

  edge_list_edge_t *edge;
  if (!hpx_gas_try_pin(target, (void**)&edge))
    return HPX_RESEND;

  const sssp_uint_t source = edge->source;

  adj_list_edge_t e;
  e.dest = edge->dest;
  e.weight = edge->weight;

  hpx_gas_unpin(target);

 // Then get the appropriate index array position to retrieve the
 // address of the vertex in the adjacency list
  const hpx_addr_t index
    = hpx_addr_add(index_array, source * sizeof(hpx_addr_t), _index_array_block_size);

  // Insert the edge into the index array at the right index. Since we
  // call this action synchronously, we can safely send the stack
  // pointer out.
  return hpx_call(index, _put_edge_index, HPX_NULL, &e, sizeof(e));
}


static hpx_action_t _print_edge = HPX_ACTION_INVALID;
static int _print_edge_action(int *i)
{
  const hpx_addr_t target = hpx_thread_current_target();

  edge_list_edge_t *edge;
  if (!hpx_gas_try_pin(target, (void**)&edge))
    return HPX_RESEND;

  hpx_gas_unpin(target);
  printf("%d %" SSSP_UINT_PRI " %" SSSP_UINT_PRI " %" SSSP_UINT_PRI ".\n", *i, edge->source, edge->dest, edge->weight);
  return HPX_SUCCESS;
}


hpx_action_t adj_list_from_edge_list = HPX_ACTION_INVALID;
int adj_list_from_edge_list_action(const edge_list_t * const el) {

  hpx_addr_t allocs_sync_lco = hpx_lco_and_new(4);

  // Start allocating the index array.
  const uint64_t no_blocks = HPX_LOCALITIES * 128 ;
  _index_array_block_size = ((el->num_vertices + no_blocks - 1) / no_blocks) * sizeof(hpx_addr_t);
  // Array is allocated while the broadst of the block size is performed
  index_array = hpx_gas_alloc_cyclic(no_blocks, _index_array_block_size, 0);
  // printf("Index array block size is %" SSSP_UINT_PRI ".\n", _index_array_block_size);
  hpx_bcast(_set_index_array_bsize, HPX_NULL, allocs_sync_lco,
        &_index_array_block_size, sizeof(_index_array_block_size));
  hpx_bcast(_set_index_array, HPX_NULL, allocs_sync_lco, &index_array,
        sizeof(index_array));

  // Start allocating the count array for creating an edge histogram
  _count_array_block_size = ((el->num_vertices + no_blocks - 1) / no_blocks) * sizeof(count_t);
  // Array is allocated while the broadcast of the block size is performed
  count_array = hpx_gas_calloc_cyclic(no_blocks, _count_array_block_size, 0);
  // printf("count_array hpx address is %zu.\n", count_array);
  hpx_bcast(_set_count_array_bsize, HPX_NULL, allocs_sync_lco,
        &_count_array_block_size, sizeof(_count_array_block_size));
  hpx_bcast(_set_count_array, HPX_NULL, allocs_sync_lco, &count_array,
        sizeof(count_array));

  hpx_lco_wait(allocs_sync_lco);
  hpx_lco_delete(allocs_sync_lco, HPX_NULL);

  printf("Count the number of edges per source vertex\n");
  hpx_time_t now = hpx_time_now();

  _initialize_termination();

  // printf("edge_list is %" PRIxPTR " with source %" SSSP_UINT_PRI ", target %" SSSP_UINT_PRI ", and weight %" SSSP_UINT_PRI ".\n", edge_list, edge_list->source, edge_list->dest, edge_list->weight);
  //#include <sys/mman.h>
  //mprotect(edge_list, el->edge_list_bsize * 24, PROT_READ);

  // Count the number of edges per source vertex
  //for (int i = 0; i < el->num_edges; ++i) {
    //hpx_addr_t edge = hpx_addr_add(el->edge_list, i * sizeof(edge_list_edge_t), el->edge_list_bsize);
   //hpx_call(edge, _count_edge, HPX_NULL, &count_array, sizeof(count_array));
   //}
  hpx_count_range_call(_count_edge, el->edge_list, el->num_edges, sizeof(edge_list_edge_t), el->edge_list_bsize, 0, NULL);
  double elapsed = hpx_time_elapsed_ms(now)/1e3;
  printf("Time elapsed in the loop: %f\n", elapsed);
  now = hpx_time_now();
  hpx_addr_t edges_sync = hpx_lco_and_new(2);
  _increment_active_count(el->num_edges);
  _detect_termination(edges_sync, edges_sync);
  hpx_lco_wait(edges_sync);
  elapsed = hpx_time_elapsed_ms(now)/1e3;
  printf("Time elapsed waiting for completion: %f\n", elapsed);
  hpx_lco_delete(edges_sync, HPX_NULL);

  _initialize_termination();

  printf("Allocate the adjacency list according to the count of edges per vertex\n");
  // Allocate the adjacency list according to the count of edges per vertex
  now = hpx_time_now();
  hpx_count_range_call(_alloc_adj_list_entry, count_array, el->num_vertices, sizeof(count_t), _count_array_block_size, 0, NULL);
  elapsed = hpx_time_elapsed_ms(now)/1e3;
  printf("Time elapsed in the loop: %f\n", elapsed);
  now = hpx_time_now();
  edges_sync = hpx_lco_and_new(2);
  _increment_active_count(el->num_vertices);
  _detect_termination(edges_sync, edges_sync);
  elapsed = hpx_time_elapsed_ms(now)/1e3;
  printf("Time elapsed waiting for completion: %f\n", elapsed);
  hpx_lco_delete(edges_sync, HPX_NULL);

  printf("Convert edges to adjacencies\n");
  // For each edge in the edge list, we add it as an adjacency to a
  // vertex
  now = hpx_time_now();

  _initialize_termination();

  hpx_count_range_call(_insert_edge, el->edge_list, el->num_edges, sizeof(edge_list_edge_t), el->edge_list_bsize, 0, NULL);
  elapsed = hpx_time_elapsed_ms(now)/1e3;
  printf("Time elapsed in the loop: %f\n", elapsed);
  now = hpx_time_now();
  edges_sync = hpx_lco_and_new(2);
  _increment_active_count(el->num_edges);
  _detect_termination(edges_sync, edges_sync);
  hpx_lco_wait(edges_sync);
  elapsed = hpx_time_elapsed_ms(now)/1e3;
  printf("Time elapsed waiting for completion: %f\n", elapsed);
  hpx_lco_delete(edges_sync, HPX_NULL);

  return HPX_THREAD_CONTINUE(index_array);
}


static hpx_action_t _init_vertex_distance = HPX_ACTION_INVALID;
static int _init_vertex_distance_action(void *arg) {
   const hpx_addr_t target = hpx_thread_current_target();

   adj_list_vertex_t *vertex;
   if (!hpx_gas_try_pin(target, (void**)&vertex))
     return HPX_RESEND;

   vertex->distance = SSSP_UINT_MAX;

   hpx_gas_unpin(target);
   return HPX_SUCCESS;
}


hpx_action_t _reset_vertex = HPX_ACTION_INVALID;
static int _reset_vertex_action(void *args) {
  const hpx_addr_t target = hpx_thread_current_target();
  hpx_addr_t vertex;
  hpx_addr_t *v;
  if (!hpx_gas_try_pin(target, (void**)&v))
    return HPX_RESEND;

  vertex = *v;
  hpx_gas_unpin(target);
  return hpx_call_sync(vertex, _init_vertex_distance, NULL, 0, NULL, 0);
}


int reset_adj_list(adj_list_t adj_list, edge_list_t *el) {
  hpx_addr_t vertices = hpx_lco_and_new(el->num_vertices);
  for (int i = 0; i < el->num_vertices; ++i) {
    hpx_addr_t index = hpx_addr_add(index_array, i * sizeof(hpx_addr_t), _index_array_block_size);
    hpx_call(index, _reset_vertex, vertices, NULL, 0);
  }
  hpx_lco_wait(vertices);
  hpx_lco_delete(vertices, HPX_NULL);
  return HPX_SUCCESS;
}


hpx_action_t free_adj_list = HPX_ACTION_INVALID;
int free_adj_list_action(void *arg) {
  // Should free vertices here.
  hpx_gas_free(count_array, HPX_NULL);
  hpx_gas_free(index_array, HPX_NULL);
  return HPX_SUCCESS;
}


static __attribute__((constructor)) void _adj_list_register_actions() {
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED, adj_list_from_edge_list,
              adj_list_from_edge_list_action,
                      HPX_POINTER, HPX_SIZE_T,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              free_adj_list, free_adj_list_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _increment_count, _increment_count_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _set_count_array_bsize, _set_count_array_bsize_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _set_index_array_bsize, _set_index_array_bsize_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _set_count_array, _set_count_array_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _set_index_array, _set_index_array_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _count_edge, _count_edge_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _init_vertex, _init_vertex_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _init_vertex_distance, _init_vertex_distance_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _reset_vertex, _reset_vertex_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _alloc_vertex, _alloc_vertex_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _alloc_adj_list_entry, _alloc_adj_list_entry_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _print_edge, _print_edge_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _insert_edge, _insert_edge_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _put_edge_index, _put_edge_index_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _put_edge, _put_edge_action,
              HPX_POINTER, HPX_SIZE_T);
}
