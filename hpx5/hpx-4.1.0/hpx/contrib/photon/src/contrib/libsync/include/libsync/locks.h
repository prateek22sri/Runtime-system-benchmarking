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

#ifndef HPX_SYNC_LOCKS_H_
#define HPX_SYNC_LOCKS_H_

#include <hpx/attributes.h>
#include "sync.h"

struct tatas_lock {
    volatile int lock;
};

typedef struct tatas_lock tatas_lock_t;

#define SYNC_TATAS_LOCK_INIT {0}

void sync_tatas_init(tatas_lock_t *l) HPX_NON_NULL(1) HPX_PUBLIC;
void sync_tatas_acquire_slow(tatas_lock_t *l) HPX_NON_NULL(1) HPX_PUBLIC;
static inline void sync_tatas_acquire(tatas_lock_t *l)
  HPX_NON_NULL(1);
static inline void sync_tatas_release(tatas_lock_t *l)
  HPX_NON_NULL(1);

void sync_tatas_acquire(tatas_lock_t *l) {
  if (sync_swap(&l->lock, 1, SYNC_ACQUIRE))
    sync_tatas_acquire_slow(l);
}

void sync_tatas_release(tatas_lock_t *l) {
  sync_store(&l->lock, 0, SYNC_RELEASE);
}

struct clh_node {
  struct clh_node *prev;
};

#define SYNC_CLH_NODE_INIT {                    \
    .prev = NULL                                \
    ._padding = {0}                             \
  }

struct clh_node * sync_clh_node_new(void)
  HPX_MALLOC HPX_PUBLIC;

void sync_clh_node_delete(struct clh_node *node)
  HPX_NON_NULL(1) HPX_PUBLIC;

uintptr_t sync_clh_node_must_wait(struct clh_node *node)
  HPX_NON_NULL(1) HPX_PUBLIC;

struct clh_lock {
  struct clh_node *tail;
};

#define SYNC_CLH_LOCK_INIT {                    \
    .tail = NULL                                \
    }

void sync_clh_lock_init(struct clh_lock *lock)
  HPX_NON_NULL(1) HPX_PUBLIC;

void sync_clh_lock_fini(struct clh_lock *lock)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Perform the first half of a CLH acquire operation.
///
/// The returned node can be passed to sync_clh_node_must_wait() to test and see
/// if the caller has acquired the lock. It should not be freed or modified in
/// any other way during the spin.
///
/// @param         lock The CLH lock itself.
/// @param            n The node to use for this operation. This is also the
///                       node that should be passed into
///                       sync_clh_lock_release().
///
/// @return             This returns the node that the caller must wait for in
///                     order to acquire the lock.
struct clh_node *sync_clh_lock_start_acquire(struct clh_lock *lock,
                                             struct clh_node *n)
  HPX_NON_NULL(1, 2) HPX_PUBLIC;


/// Perform a blocking acquire operation for a CLH lock.
///
/// @param         lock The CLH lock itself.
/// @param            n The node to use for this operation. This is also the
///                       node that should be passed into
///                       sync_clh_lock_release().
void sync_clh_lock_acquire(struct clh_lock *lock, struct clh_node *n)
  HPX_NON_NULL(1, 2) HPX_PUBLIC;


/// Release a CLH lock.
///
/// @param         lock The CLH lock itself.
/// @param            n The node to use for this operation. This is also the
///                       node that should be passed into
///                       sync_clh_lock_release().
///
/// @return             This returns an unused node that can either be freed (if
///                       appropriate based on where nodes are coming from) or
///                       used in a subsequent call to
///                       sync_clh_lock_start_acquire() or
///                       sync_clh_lock_acquire().
struct clh_node *sync_clh_lock_release(struct clh_lock *lock, struct clh_node *n)
  HPX_NON_NULL(1, 2) HPX_PUBLIC;


struct mcs_node {
  struct mcs_node *next;
  uintptr_t owner;
};

#define SYNC_MCS_NODE_INIT {                    \
    .next = NULL,                               \
    .owner = 0,                                 \
    }

uint64_t sync_mcs_node_test(struct mcs_node *)
  HPX_NON_NULL(1) HPX_PUBLIC;

struct mcs_lock {
  struct mcs_node *tail;
};

#define SYNC_MCS_LOCK_INIT {                    \
    .tail = NULL                                \
    }

void sync_mcs_lock_init(struct mcs_lock *mcs)
  HPX_NON_NULL(1) HPX_PUBLIC;

void sync_mcs_lock_fini(struct mcs_lock *mcs)
  HPX_NON_NULL(1) HPX_PUBLIC;

void sync_mcs_lock_enter(struct mcs_lock *mcs, struct mcs_node *node)
  HPX_NON_NULL(1, 2) HPX_PUBLIC;

void sync_mcs_lock_acquire(struct mcs_lock *mcs, struct mcs_node *node)
  HPX_NON_NULL(1, 2) HPX_PUBLIC;

void sync_mcs_lock_release(struct mcs_lock *mcs, struct mcs_node *node)
  HPX_NON_NULL(1, 2) HPX_PUBLIC;

#endif /* HPX_SYNC_LOCKS_H_ */
