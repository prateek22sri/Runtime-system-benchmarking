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
#include <inttypes.h>
#include <stdatomic.h>

#include "hpx/hpx.h"
#include "libpxgl/adjacency_list.h"
#include "libpxgl/sssp_common.h"
#include "libpxgl/termination.h"

/// SSSP Chaotic-relaxation

hpx_action_t _sssp_chaotic_process_vertex = 0;

static int _sssp_chaotic_process_vertex_action(distance_t *const distance) {
  const hpx_addr_t target = hpx_thread_current_target();

  adj_list_vertex_t *vertex;
  if (!hpx_gas_try_pin(target, (void**)&vertex))
    return HPX_RESEND;

  // printf("Distance Action on %" SSSP_UINT_PRI " with distance %" SSSP_UINT_PRI "\n", target, distance);

  if (_try_update_vertex_distance(vertex, *distance)) {
    _send_update_to_neighbors(vertex, *distance);
  }
  hpx_gas_unpin(target);

  if (_get_termination() == COUNT_TERMINATION) {
    _increment_finished_count();
  }

  // printf("Distance Action finished on %" SSSP_UINT_PRI "\n", target);

  return HPX_SUCCESS;
}

static __attribute__((constructor)) void _sssp_register_actions() {
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
		      _sssp_chaotic_process_vertex, _sssp_chaotic_process_vertex_action,
		      HPX_POINTER, HPX_SIZE_T);
}
