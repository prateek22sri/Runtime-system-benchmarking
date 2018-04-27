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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdatomic.h>
#include <inttypes.h>

#include "hpx/hpx.h"
#include "libpxgl/adjacency_list.h"
#include "libpxgl/sssp_dc.h"
#include "libpxgl/sssp_common.h"
#include "libpxgl/unlocked_pqueue_wrapper.h"
#include "libpxgl/sssp_queue.h"
#include "libpxgl/termination.h"
#include "libpxgl/atomics.h"

/// SSSP Distributed Control

static sssp_queue_t **sssp_queues;
static size_t *processed_counts;
static size_t *yield_counts;
static size_t *last_sizes;
static size_t *empty_factors;
static size_t num_elem;
static size_t freq;
static size_t yield;
static size_t yield_size_threshold;
static size_t handle_queues_count;

static HPX_ACTION_DECL(_sssp_delete_queues);
static HPX_ACTION_DECL(_sssp_init_queues_dc1);
static HPX_ACTION_DECL(_handle_queue);
static HPX_ACTION_DECL(_sssp_dc1_process_vertex);
static HPX_ACTION_DECL(sssp_init_dc1);

/* static void _set_affinity(const int thread_id) { */
/*   while (HPX_THREAD_ID != thread_id) */
/*     hpx_thread_set_affinity(thread_id); */
/* } */

static bool _maybe_process_one(int thread_id) {
  hpx_addr_t v;
  distance_t d;
  adj_list_vertex_t *vertex;
  sssp_queue_t *q = sssp_queues[HPX_THREAD_ID];
  if (sssp_queue_pop(q, &v, &d)) {
    // if (rand() % 5000 == 1) printf("Queue size at pop %zu on thread %d.\n", qsize, thread_id);
    // printf("Trying to pop one vertex from the priority queue in the handle queue action\n");
    // We are not handling AGAS currently and assume the vertex is on the same locality.
    // For AGAS we could just send an action.
    if (!hpx_gas_try_pin(v, (void**)&vertex)) hpx_abort();
    // Using relaxed semantics may be incorrect in the long run
      const distance_t current_d = atomic_load_explicit(&vertex->distance,
                                   memory_order_relaxed);
    if (current_d == d) {
      _send_update_to_neighbors(vertex, d);
    } else {
      /* stale */
    }
    _increment_finished_count();
    hpx_gas_unpin(v);
    return true;
  } else {
    return false;
  }
}

static int _sssp_dc1_process_vertex_handler(const distance_t *const distance) {
  const hpx_addr_t target = hpx_thread_current_target();
  const int thread_id = HPX_THREAD_ID;
  // if(rand() % 4 == 1)
  //hpx_thread_set_affinity(rand() % HPX_THREADS);

  adj_list_vertex_t *vertex;
  if (!hpx_gas_try_pin(target, (void**)&vertex))
    return HPX_RESEND;

  // printf("Distance Action on %" SSSP_UINT_PRI "\n", target);

  if (_try_update_vertex_distance(vertex, *distance)) {
    if(yield_counts[thread_id] == 0) {
      sssp_queue_t *queue = sssp_queues[thread_id];
      sssp_queue_push(queue, target, *distance);
      if(++processed_counts[thread_id] == freq) {
    processed_counts[thread_id] = 0;
    _maybe_process_one(thread_id);
    const size_t qsize = sssp_queue_size(queue);
    if (qsize < yield_size_threshold) yield_counts[thread_id] = yield;
      }
    } else {
      // if (rand() % 5000 == 1) printf("Yield count  %zu on thread %d.\n", yield_counts[thread_id], thread_id);
      yield_counts[thread_id]--;
      _send_update_to_neighbors(vertex, *distance);
      _increment_finished_count();
    }
  } else if (_get_termination() == COUNT_TERMINATION) {
    _increment_finished_count();
  } else {
    // We only support count termination with DC at this time.
    hpx_abort();
  }

  hpx_gas_unpin(target);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_TASK, HPX_MARSHALLED | HPX_COALESCED, _sssp_dc1_process_vertex,
          _sssp_dc1_process_vertex_handler, HPX_POINTER, HPX_SIZE_T);

static int sssp_init_dc1_handler(sssp_init_dc_args_t *args) {
  freq = args->freq;
  yield = args->yield_count;
  // printf("freq: %zu\n", freq);
  num_elem = args->num_elem;
  // printf("num_elem: %zu\n", num_elem);
  handle_queues_count = 0;//HPX_THREADS / 2;
  handle_queues_count++;
  yield_size_threshold = args->queue_threshold;

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_INTERRUPT, HPX_MARSHALLED | HPX_COALESCED, sssp_init_dc1,
          sssp_init_dc1_handler, HPX_POINTER, HPX_SIZE_T);


/* void _sssp_init_dc_function(size_t num_pq, size_t freq) { */
/*   hpx_addr_t init_lco = hpx_lco_future_new(0); */
/*   const sssp_init_dc_args_t init_args = { .num_pq = num_pq, .freq = freq }; */
/*   hpx_bcast(sssp_init_dc, init_lco, &init_args, sizeof(init_args)); */
/*   hpx_lco_wait(init_lco); */
/*   hpx_lco_delete(init_lco, HPX_NULL); */
/* } */

typedef struct {
  hpx_addr_t termination_lco;
  sssp_queue_t **queues;
} _sssp_delete_queues_args_t;
static int _sssp_delete_queues_handler(const _sssp_delete_queues_args_t * const args) {
  hpx_lco_wait(args->termination_lco);

  // printf("Delete queues wait succeeded at %zu. Deleting queues.\n", args->termination_lco);

  for(unsigned int i = 0; i < HPX_THREADS; ++i) {
    // Land on the specific scheduler thread to block handle_queue, and then delete the queue and set it to NULL.
    // _set_affinity(i);
    sssp_queue_free(args->queues[i]);
    args->queues[i] = NULL;
  }

  // We do not delete queues so that handle_queue actions can die.  A better way to do this would be to run a separate termination detection for handle_queues from a given algorithm run and delete queues when all handle_queues die.
  // free(args->queues);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED, _sssp_delete_queues,
       _sssp_delete_queues_handler, HPX_POINTER, HPX_SIZE_T);

static int _sssp_init_queues_dc1_handler(const hpx_addr_t * const termination_lco) {
  // printf("DC1 _sssp_init_queues\n");
  // printf("num_pq in init queues: %zu\n", num_pq);
  // printf("sssp_queues before malloc: %" PRIxPTR "\n", (uintptr_t)sssp_queues);
  sssp_queues = calloc(HPX_THREADS, sizeof(sssp_queue_t));
  processed_counts = calloc(HPX_THREADS, sizeof(sssp_queue_t));
  yield_counts = calloc(HPX_THREADS, sizeof(sssp_queue_t));
  last_sizes = calloc(HPX_THREADS, sizeof(sssp_queue_t));
  empty_factors = calloc(HPX_THREADS, sizeof(sssp_queue_t));
  // printf("sssp_queues after malloc: %" PRIxPTR "\n", (uintptr_t)sssp_queues);
  for(unsigned int i = 0; i < HPX_THREADS; ++i) {
    sssp_queues[i] = sssp_queue_create(num_elem);
    empty_factors[i] = 1;
  }
  const _sssp_delete_queues_args_t delete_args = { .termination_lco = *termination_lco, .queues = sssp_queues };
  hpx_call(HPX_HERE, _sssp_delete_queues, HPX_NULL, &delete_args,
           sizeof(delete_args));
  for(size_t i = 0; i < handle_queues_count; ++i) {
    hpx_call(HPX_HERE, _handle_queue, HPX_NULL, &sssp_queues,
             sizeof(sssp_queues));
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED, _sssp_init_queues_dc1,
          _sssp_init_queues_dc1_handler, HPX_POINTER, HPX_SIZE_T);

static int handle_queue_handler(sssp_queue_t *const *const *const args) {
  const int thread_id = HPX_THREAD_ID;
  sssp_queue_t *const *const queues = *args;
  sssp_queue_t *const queue = queues[thread_id];
  // printf("In the handle queue action with queue %" PRIxPTR "\n", (uintptr_t)queues[thread_id]);
  if (!queue) return HPX_SUCCESS;
  hpx_call(HPX_HERE, _handle_queue, HPX_NULL, args, sizeof(*args));
  const size_t current_size = sssp_queue_size(queue);
  // We only need to increse yield_count if it was 0; otherwise, _maybe_process_one will set it if the queue is small.
  // If last emptying of the queue did not lead to size increase, speed up emptying by 2 and otherwise reset emptying factor to 1;
  if (current_size <= last_sizes[thread_id] && last_sizes[thread_id] != 0) {
    empty_factors[thread_id] *= 2;
    if(current_size < yield_size_threshold) yield_counts[thread_id] = yield;
  } else {
    if(current_size == 0) yield_counts[thread_id] = yield;
    empty_factors[thread_id] = 1;
  }
  //  if(rand() % 5000 == 1) printf("DC1 handle_queue emptying loop with factor %zu on thread %d.\n", empty_factor, HPX_THREAD_ID);
  for(size_t i = 0; i < empty_factors[thread_id] && i < current_size && _maybe_process_one(thread_id); ++i) ;
  last_sizes[thread_id] = sssp_queue_size(queue);

  // printf("In handle queue action with empty priority queue\n");
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_TASK, HPX_MARSHALLED | HPX_COALESCED, _handle_queue,
           handle_queue_handler, HPX_POINTER, HPX_SIZE_T);

void _switch_to_dc1() {
  // printf("Switching to DC1\n");
  _sssp_dc_process_vertex = _sssp_dc1_process_vertex;
  sssp_init_dc = sssp_init_dc1;
  _sssp_init_queues = _sssp_init_queues_dc1;
}
