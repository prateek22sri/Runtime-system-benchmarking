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

#ifndef PXGL_DIMACS_H
#define PXGL_DIMACS_H

#include "defs.h"

#include "hpx/hpx.h"
#include "adjacency_list.h"


// Compute checksum given an adjacency list
extern hpx_action_t dimacs_checksum;
extern int dimacs_checksum_action(const sssp_uint_t *const g);

#endif // PXGL_DIMACS_H
