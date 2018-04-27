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

uint64_t sync_mcs_node_test(struct mcs_node *node) {
  return sync_load(&node->owner, SYNC_RELAXED);
}

void sync_mcs_lock_init(struct mcs_lock *mcs) {
  mcs->tail = NULL;
}

void sync_mcs_lock_fini(struct mcs_lock *mcs) {
}

void sync_mcs_lock_enter(struct mcs_lock *mcs, struct mcs_node *node) {
  // insert a node into the queue
  node->next = NULL;
  node->owner = 0;
  struct mcs_node *prev = sync_swap(&mcs->tail, node, SYNC_RELEASE);

  // clean up the link structure
  if (prev) {
    sync_store(&prev->next, node, SYNC_RELAXED);
  }
}

void sync_mcs_lock_acquire(struct mcs_lock *mcs, struct mcs_node *node) {
  sync_mcs_lock_enter(mcs, node);
  while (!sync_mcs_node_test(node))
    ;
  sync_fence(SYNC_ACQUIRE);
}

void sync_mcs_lock_release(struct mcs_lock *mcs, struct mcs_node *node) {
  struct mcs_node *succ = sync_load(&node->next, SYNC_ACQUIRE);
  // release the lock to the next queue node
  if (succ) {
    sync_store(&succ->owner, 1, SYNC_RELEASE);
    return;
  }

  // release the lock to no one--don't overwrite "node" during a failed cas
  struct mcs_node *temp = node;
  if (sync_cas(&mcs->tail, &temp, NULL, SYNC_RELEASE, SYNC_RELAXED)) {
    return;
  }

  // wait for the next queue node to finish up its insertion
  do {
    succ = sync_load(&node->next, SYNC_ACQUIRE);
  } while (!succ);

  // release the lock to the next queue node
  sync_store(&succ->owner, 1, SYNC_RELEASE);
}
