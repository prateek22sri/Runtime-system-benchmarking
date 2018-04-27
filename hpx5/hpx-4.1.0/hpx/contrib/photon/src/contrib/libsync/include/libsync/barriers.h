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

#ifndef HPX_SYNC_BARRIERS_H_
#define HPX_SYNC_BARRIERS_H_

#include <hpx/attributes.h>

/// ----------------------------------------------------------------------------
/// The barrier class.
///
/// A barrier has two member functions, delete() and join().
/// ----------------------------------------------------------------------------
typedef struct barrier {
  void (*delete)(struct barrier*);
  int (*join)(struct barrier*, int i);
} barrier_t;

/// ----------------------------------------------------------------------------
/// Forward to the barrier's specific delete function.
/// ----------------------------------------------------------------------------
static inline void sync_barrier_delete(barrier_t *barrier) {
  barrier->delete(barrier);
}

/// ----------------------------------------------------------------------------
/// Forward to the barrier's specific join function.
/// ----------------------------------------------------------------------------
static inline int sync_barrier_join(barrier_t *barrier, int i) {
  return barrier->join(barrier, i);
}

/// ----------------------------------------------------------------------------
/// Allocate a simple sense-reversing barrier.
///
/// This isn't the most scalable barrier in the world, but it's not terrible for
/// occasional use, and it's really easy to implement and use.
/// ----------------------------------------------------------------------------
barrier_t *sr_barrier_new(int n_threads) HPX_PUBLIC;

#endif /* HPX_SYNC_BARRIERS_H_ */
