// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2016, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifndef LIBHPX_QUEUES_H
#define LIBHPX_QUEUES_H

/// ----------------------------------------------------------------------------
/// @file include/libsync/queues.h
/// ----------------------------------------------------------------------------
#include <config.h>
#include <hpx/attributes.h>
#include "cptr.h"


/// M&S two lock queue from
/// http://www.cs.rochester.edu/u/scott/papers/1996_PODC_queues.pdf, implemented
/// based on
/// https://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html#tlq

typedef struct two_lock_queue_node two_lock_queue_node_t;
struct two_lock_queue_node {
  two_lock_queue_node_t *next;
  void *value;
};


/// Using SWAP on the head and tail pointers for locking. Could use something
/// more scalable if higher contention.
typedef struct {
  two_lock_queue_node_t *head;
  const char _paddinga[HPX_CACHELINE_SIZE - sizeof(two_lock_queue_node_t*)];
  two_lock_queue_node_t *tail;
  const char _paddingb[HPX_CACHELINE_SIZE - sizeof(two_lock_queue_node_t*)];
} HPX_ALIGNED(64) two_lock_queue_t;

#define SYNC_TWO_LOCK_QUEUE_INIT {              \
    .head = NULL,                               \
    ._paddinga = {0},                           \
    .tail = NULL,                               \
    ._paddingb = {0}                            \
  }

two_lock_queue_t *sync_two_lock_queue_new(void) HPX_MALLOC HPX_PUBLIC;
void sync_two_lock_queue_delete(two_lock_queue_t *q) HPX_PUBLIC;

void sync_two_lock_queue_init(two_lock_queue_t *q, two_lock_queue_node_t *init)
  HPX_NON_NULL(1) HPX_PUBLIC;

void sync_two_lock_queue_fini(two_lock_queue_t *q)
  HPX_NON_NULL(1) HPX_PUBLIC;

void sync_two_lock_queue_enqueue(two_lock_queue_t *q, void *val)
  HPX_NON_NULL(1) HPX_PUBLIC;

void *sync_two_lock_queue_dequeue(two_lock_queue_t *q)
  HPX_NON_NULL(1) HPX_PUBLIC;

void sync_two_lock_queue_enqueue_node(two_lock_queue_t *q,
                                      two_lock_queue_node_t *node)
  HPX_NON_NULL(2) HPX_PUBLIC;

two_lock_queue_node_t *sync_two_lock_queue_dequeue_node(two_lock_queue_t *q)
  HPX_NON_NULL(1) HPX_PUBLIC;

typedef struct {
  volatile cptr_t head;
  const char _paddinga[HPX_CACHELINE_SIZE - sizeof(cptr_t)];
  volatile cptr_t tail;
  const char _paddingc[HPX_CACHELINE_SIZE - sizeof(cptr_t)];
} ms_queue_t;

#define SYNC_MS_QUEUE_INIT                      \
  {                                             \
    .head = SYNC_CPTR_INITITIALIZER,            \
    ._paddinga = {0},                           \
    .tail = SYNC_CPTR_INITITIALIZER,            \
 ._paddingb = {0}                               \
  }

ms_queue_t *sync_ms_queue_new(void) HPX_MALLOC HPX_PUBLIC;
void sync_ms_queue_delete(ms_queue_t *q) HPX_PUBLIC;

void  sync_ms_queue_init(ms_queue_t *q, void *val) HPX_NON_NULL(1) HPX_PUBLIC;
void  sync_ms_queue_fini(ms_queue_t *q) HPX_NON_NULL(1) HPX_PUBLIC;
void  sync_ms_queue_enqueue(ms_queue_t *q, void *val)
  HPX_NON_NULL(1) HPX_PUBLIC;
void *sync_ms_queue_dequeue(ms_queue_t *q) HPX_NON_NULL(1) HPX_PUBLIC;

#endif // LIBHPX_QUEUES_H
