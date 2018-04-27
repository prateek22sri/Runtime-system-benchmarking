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

#ifndef LIBPXGL_TERMINATION_H
#define LIBPXGL_TERMINATION_H

#include "pxgl/pxgl.h"
#include <stdatomic.h>
#include "libpxgl/atomics.h"

void _increment_active_count(sssp_uint_t n);
void _increment_finished_count();
sssp_uint_t _relaxed_get_active_count();
sssp_uint_t _relaxed_get_finished_count();
termination_t _get_termination();
void _initialize_termination();
void _detect_termination(const hpx_addr_t termination_lco, const hpx_addr_t internal_termination_lco);
extern hpx_action_t _initialize_termination_detection;
extern _Atomic termination_t _termination;

#endif // LIBPXGL_TERMINATION_H
