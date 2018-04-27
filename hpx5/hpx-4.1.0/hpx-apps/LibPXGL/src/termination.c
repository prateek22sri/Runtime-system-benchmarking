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

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdatomic.h>
#include <inttypes.h>

#include "hpx/hpx.h"
#include "libpxgl/termination.h"
#include "libpxgl/atomics.h"

/// Termination Detection

hpx_action_t _initialize_termination_detection = 0;
static hpx_action_t _set_termination = 0;
static hpx_action_t _send_termination_count = 0;

/// Termination detection counts
static _Atomic sssp_uint_t active_count;
static _Atomic sssp_uint_t finished_count;
/// Termination detection choice
_Atomic termination_t _termination;

void _increment_active_count(sssp_uint_t n) {
  atomic_fetch_add_explicit(&active_count, n, memory_order_relaxed);
}

void _increment_finished_count() {
  atomic_fetch_add_explicit(&finished_count, 1, memory_order_relaxed);
}

sssp_uint_t _relaxed_get_active_count() {
  return atomic_load_explicit(&active_count, memory_order_relaxed);
}

sssp_uint_t _relaxed_get_finished_count() {
  return atomic_load_explicit(&finished_count, memory_order_relaxed);
}

termination_t _get_termination() {
  return atomic_load_explicit(&_termination, memory_order_relaxed);
}

static void _termination_detection_op_handler(SSSP_UINT_T *const output, const SSSP_UINT_T *const input, const size_t size) {
  output[0] = output[0] + input[0];
  output[1] = output[1] + input[1];
}

static HPX_ACTION(HPX_FUNCTION, 0, _termination_detection_op, _termination_detection_op_handler);

static void _termination_detection_init_handler(void *init_val, const size_t init_val_size) {
  ((SSSP_UINT_T*)init_val)[0] = 0;
  ((SSSP_UINT_T*)init_val)[1] = 0;
}

static HPX_ACTION(HPX_FUNCTION, 0, _termination_detection_init, _termination_detection_init_handler);

static int _send_termination_count_action(const hpx_addr_t *const args) {
  SSSP_UINT_T current_counts[2];
  // Order all previous relaxed memory accesses.
  atomic_thread_fence(memory_order_seq_cst);
  current_counts[1] = atomic_load_explicit(&finished_count, memory_order_relaxed);
  current_counts[0] = atomic_load_explicit(&active_count, memory_order_relaxed);

  hpx_thread_yield();
  hpx_lco_set(*args, sizeof(current_counts), current_counts, HPX_NULL, HPX_NULL);
  return HPX_SUCCESS;
}

void set_termination(termination_t termination) {
  hpx_bcast_rsync(_set_termination, &termination,
            sizeof(termination));
  return;
}

int _set_termination_action(termination_t *arg) {
  atomic_store_explicit(&_termination, *arg, memory_order_relaxed);
  atomic_thread_fence(memory_order_seq_cst);
  return HPX_SUCCESS;
}

int _initialize_termination_detection_action(void *arg) {
  // printf("Initializing termination detection.\n");
  atomic_store_explicit(&active_count, 0, memory_order_relaxed);
  atomic_store_explicit(&finished_count, 0, memory_order_relaxed);
  // Order the relaxed memory operations that came before.
  atomic_thread_fence(memory_order_seq_cst);
  return HPX_SUCCESS;
}

void _initialize_termination() {
  hpx_bcast_rsync(_initialize_termination_detection, NULL, 0);
}

void _detect_termination(const hpx_addr_t termination_lco, const hpx_addr_t internal_termination_lco) {
  sssp_uint_t activity_counts[2];
  hpx_addr_t termination_count_lco = hpx_lco_allreduce_new(HPX_LOCALITIES, 1, sizeof(activity_counts), _termination_detection_init, _termination_detection_op);
  enum { PHASE_1, PHASE_2 } phase = PHASE_1;
  SSSP_UINT_T last_finished_count = 0;

  while(true) {
    hpx_bcast(_send_termination_count, HPX_NULL, HPX_NULL, &termination_count_lco,
              sizeof(termination_count_lco));
    hpx_lco_get(termination_count_lco, sizeof(activity_counts), activity_counts);
    const sssp_uint_t active_count = activity_counts[0];
    const sssp_uint_t finished_count = activity_counts[1];
    sssp_int_t activity_count = active_count - finished_count;
    //printf("activity_count: %" SSSP_INT_PRI ", active: %" SSSP_UINT_PRI ", finished %" SSSP_UINT_PRI ", phase: %d\n", activity_count, active_count, finished_count, phase);
    if (activity_count != 0) {
      phase = PHASE_1;
      continue;
    } else if(phase == PHASE_2 && last_finished_count == finished_count) {
      // printf("Setting termination LCOs: %zu %zu\n", termination_lco, internal_termination_lco);
      hpx_lco_set(termination_lco, 0, NULL, HPX_NULL, HPX_NULL);
      hpx_lco_set(internal_termination_lco, 0, NULL, HPX_NULL, HPX_NULL);
      // printf("Termination LCOs set.\n");
      // printf("Finished termination detection with active: %" SSSP_UINT_PRI ", finished %" SSSP_UINT_PRI "\n", active_count, finished_count);
      break;
    } else {
      phase = PHASE_2;
      last_finished_count = finished_count;
    }
  }
  // printf("Finished termination_detection.\n");
  hpx_lco_delete(termination_count_lco, HPX_NULL);
}

static HPX_CONSTRUCTOR void _sssp_register_actions() {
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _initialize_termination_detection, _initialize_termination_detection_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _send_termination_count, _send_termination_count_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _set_termination, _set_termination_action,
              HPX_POINTER, HPX_SIZE_T);
  // Default termination detection algorithm
  _termination = COUNT_TERMINATION;
}
