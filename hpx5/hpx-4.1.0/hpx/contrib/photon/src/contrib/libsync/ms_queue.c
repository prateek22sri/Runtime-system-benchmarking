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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "libsync/queues.h"

typedef struct {
  cptr_t next;
  void *value;
} _node_t;

static _node_t * _node_new(void *value) {
  _node_t *node = malloc(sizeof(*node));
  assert(node);

  node->next.p = NULL;
  node->next.c = 0;
  node->value = value;
  return node;
}


static void _node_delete(void *node) {
  free(node);
}


void sync_ms_queue_init(ms_queue_t *q, void *val) {
  q->head.c             = 0;
  q->tail.c             = 0;
  q->head.p = q->tail.p = _node_new(val);
}


ms_queue_t *sync_ms_queue_new(void) {
  ms_queue_t *q = malloc(sizeof(*q));
  if (q)
    sync_ms_queue_init(q, NULL);
  return q;
}


void sync_ms_queue_fini(ms_queue_t *q) {
  while (sync_ms_queue_dequeue(q))
    ;
  _node_delete(q->head.p);
}


void sync_ms_queue_delete(ms_queue_t *q) {
  sync_ms_queue_fini(q);
  free(q);
}


void sync_ms_queue_enqueue(ms_queue_t *q, void *val) {
#ifdef __clang_analyzer__
  // The static analyzer thinks that we leak the *node because it is written in
  // inline asm. This prevents the analyzer from processing this function,
  // though the produced code doesn't "work" in any meaningful way.
  return;
#endif

  _node_t *node = _node_new(val);
  cptr_t tail, next;

 retry:
  // load consistent snapshot
  sync_cptr_load(&q->tail, &tail);
  volatile cptr_t *tail_next = &((_node_t*)tail.p)->next;
  sync_cptr_load(tail_next, &next);

  // if we didn't get a consistent snapshot, continue
  if (!sync_cptr_is_consistent(&q->tail, &tail))
    goto retry;

  // if the tail->next pointer isn't null, try and help the active enqueue,
  // builtin expect convinces gcc to layout the fastpath linearly
  if (__builtin_expect((next.p != NULL), 0)) {
    sync_cptr_cas_val(&q->tail, &tail, next.p);
    goto retry;
  }

  // no active enqueue, try and perform the first half of ours (updating
  // tail->next), if we fail we try again
  if (!sync_cptr_cas(tail_next, &next, node))
    goto retry;

  // perform the second half of our enqueue (updating the queue itself)
  sync_cptr_cas_val(&q->tail, &tail, node);
}

void *
sync_ms_queue_dequeue(ms_queue_t *q) {
  cptr_t head, next, tail;

 retry:
  // consistent snapshot read
  sync_cptr_load(&q->head, &head);
  sync_cptr_load(&q->tail, &tail);
  volatile cptr_t *head_next = &((_node_t*)head.p)->next;
  sync_cptr_load(head_next, &next);

  // if we didn't get a consistent snapshot, try again
  if (!sync_cptr_is_consistent(&q->head, &head))
    goto retry;

  // if the head and the tail pointed at the same node, and the next pointer was
  // null, then the queue is empty
  if (head.p == tail.p && next.p == NULL)
    return NULL;

  // if the head and tail point at the same thing, then we saw inside a enqueue,
  // and we should help finish it
  if (head.p == tail.p) {
    sync_cptr_cas_val(&q->tail, &tail, next.p);
    goto retry;
  }

  // we're going to try and dequeue, read the value that we're going to dequeue
  // and then try and swing head---read first so that a concurrent next.p
  // dereference doesn't delete it before we get to it
  void *result = ((_node_t*)next.p)->value;
  if (!sync_cptr_cas(&q->head, &head, next.p))
    goto retry;

  // delete the node that we just dequeued, and return the result
  _node_delete(head.p);
  return result;
}

