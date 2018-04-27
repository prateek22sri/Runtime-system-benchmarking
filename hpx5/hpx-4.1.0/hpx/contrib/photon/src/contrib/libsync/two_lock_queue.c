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

#include <stddef.h>
#include <stdlib.h>

#include <hpx/builtins.h>

#include "libsync/queues.h"
#include "libsync/backoff.h"

static two_lock_queue_node_t *_node_new(void *value) {
  two_lock_queue_node_t *node = malloc(sizeof(*node));
  node->next = NULL;
  node->value = value;
  return node;
}


static void _node_delete(two_lock_queue_node_t *node) {
  free(node);
}


static two_lock_queue_node_t *_acquire(two_lock_queue_node_t **ptr) {
  static const unsigned int base = 16;
  unsigned int i = base;
  two_lock_queue_node_t *node = sync_swap(ptr, (void*)1, SYNC_ACQUIRE);

  while (unlikely(node == (void*)1)) {
    sync_backoff_exp_r(&i);
    node = sync_swap(ptr, (void*)1, SYNC_ACQUIRE);
  }

  return node;
}


void sync_two_lock_queue_init(two_lock_queue_t *q, two_lock_queue_node_t *n) {
  q->head = q->tail = (n) ? n : _node_new(NULL);
}


two_lock_queue_t *sync_two_lock_queue_new(void) {
  two_lock_queue_t *q = NULL;
  int e = posix_memalign((void**)&q, HPX_CACHELINE_SIZE, sizeof(*q));
  if (e == 0)
    sync_two_lock_queue_init(q, NULL);
  return q;
}


void sync_two_lock_queue_fini(two_lock_queue_t *q) {
  while (sync_two_lock_queue_dequeue(q))
    ;
  _node_delete(q->head);
}


void sync_two_lock_queue_delete(two_lock_queue_t *q) {
  if (q) {
    sync_two_lock_queue_fini(q);
    free(q);
  }
}


void sync_two_lock_queue_enqueue_node(two_lock_queue_t *q,
                                      two_lock_queue_node_t *node) {
  two_lock_queue_node_t *tail = _acquire(&q->tail);
  tail->next = node;
  sync_store(&q->tail, node, SYNC_RELEASE);
}


void sync_two_lock_queue_enqueue(two_lock_queue_t *q, void *val) {
  sync_two_lock_queue_enqueue_node(q, _node_new(val));
}


two_lock_queue_node_t *sync_two_lock_queue_dequeue_node(two_lock_queue_t *q) {
  two_lock_queue_node_t *head = _acquire(&q->head);
  two_lock_queue_node_t *next = head->next;
  if (next == NULL) {
    sync_store(&q->head, head, SYNC_RELEASE);
    return NULL;
  }

  void *value = next->value;
  sync_store(&q->head, next, SYNC_RELEASE);
  head->value = value;
  return head;
}

void *sync_two_lock_queue_dequeue(two_lock_queue_t *q) {
  two_lock_queue_node_t *n = sync_two_lock_queue_dequeue_node(q);
  if (!n)
    return NULL;
  void *value = n->value;
  _node_delete(n);
  return value;
}
