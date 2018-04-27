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

#ifndef SSSP_QUEUE_H
#define SSSP_QUEUE_H

bool sssp_queue_empty(sssp_queue_t *q);
size_t sssp_queue_pop(sssp_queue_t *q, hpx_addr_t *v, distance_t *d);
bool sssp_queue_push(sssp_queue_t *q, hpx_addr_t v, distance_t d);
sssp_queue_t *sssp_queue_create(size_t num_elem);
void sssp_queue_free(sssp_queue_t *q);
size_t sssp_queue_size(sssp_queue_t *q);

// TBD: The push and the empty calls could be merged since we use empty 
// followed by push.  That could help us avoid one lock acquire.

#endif // SSSP_QUEUE_H
