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

#ifndef PXGL_QUEUE_H
#define PXGL_QUEUE_H

#include "pqueue.h"
#include "libsync/locks.h" 

typedef struct {
  pqueue_t *pq;
  tatas_lock_t mutex; //mutex associated with the priority queue
} locked_pq;

#endif // PXGL_QUEUE_H
