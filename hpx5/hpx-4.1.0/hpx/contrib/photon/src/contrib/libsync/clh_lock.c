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
# include "config.h"
#endif

#include <stddef.h>
#include <stdlib.h>
#include "libsync/locks.h"
#include "libsync/sync.h"

static const uintptr_t _MUST_WAIT_MASK = 0x1;

static void _set(struct clh_node *node, struct clh_node *pred) {
  uintptr_t temp = (uintptr_t)pred;
  temp |= _MUST_WAIT_MASK;
  sync_store(&node->prev, (struct clh_node *)temp, SYNC_RELEASE);
}

static struct clh_node *_reset(struct clh_node *node) {
  struct clh_node *pred = sync_load(&node->prev, SYNC_RELAXED);
  sync_store(&node->prev, NULL, SYNC_RELEASE);
  return (struct clh_node *)((uintptr_t)pred & ~_MUST_WAIT_MASK);
}

struct clh_node *sync_clh_node_new(void) {
  struct clh_node *node = NULL;
  int e = posix_memalign((void**)&node, HPX_CACHELINE_SIZE, sizeof(*node));
  if (e) {
    return NULL;
  }
  node->prev = NULL;
  return node;
}

void sync_clh_node_delete(struct clh_node *node) {
  free(node);
}

uintptr_t sync_clh_node_must_wait(struct clh_node *n) {
  uintptr_t temp = (uintptr_t)sync_load(&n->prev, SYNC_ACQUIRE);
  return temp & _MUST_WAIT_MASK;
}

void sync_clh_lock_init(struct clh_lock *lock) {
  lock->tail = sync_clh_node_new();
}

void sync_clh_lock_fini(struct clh_lock *lock) {
  sync_clh_node_delete(lock->tail);
}

struct clh_node *sync_clh_lock_start_acquire(struct clh_lock *lock,
                                             struct clh_node *node) {
  // the must wait bit must be set before we swap this node in, but the pointer
  // value isn't important yet
  _set(node, NULL);

  // swap this node in to the tail, anyone who sees it will have to wait
  struct clh_node *tail = sync_swap(&lock->tail, node, SYNC_RELEASE);

  // and remember the old tail, because that's the node we're going to take with
  // us when we release this node
  _set(node, tail);

  /// return the node to spin on
  return tail;
}

void sync_clh_lock_acquire(struct clh_lock *lock, struct clh_node *node) {
  // swap this node in to the tail, anyone who sees it will have to wait
  struct clh_node *tail = sync_clh_lock_start_acquire(lock, node);
  // spin on the old tail
  while (sync_clh_node_must_wait(tail))
    ;
}

struct clh_node *sync_clh_lock_release(struct clh_lock *lock,
                                       struct clh_node *node) {
  // we can re-use the predecessor of the current node, but we can't use the
  // current node since we have no idea when our successor will finish.
  return _reset(node);
}
