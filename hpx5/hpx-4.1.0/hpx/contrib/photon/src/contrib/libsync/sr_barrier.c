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

#include <assert.h>
#include <stdlib.h>

#include "libsync/barriers.h"
#include "libsync/sync.h"
#include "nop.h"

/// The sense-reversing barrier.
///
/// Could use some padding here, if we ever wanted this to be useful for
/// common-case code. On the other hand, the sense-reversing barrier is never
/// going to be super-scalable, so we're not worried about it.
typedef struct {
  barrier_t   vtable;
  volatile int sense;
  int          count;
  int         rounds;
  int        threads;
  int         senses[];
} sr_barrier_t;

/// Delete member function.
static void _sr_barrier_delete(barrier_t *barrier) {
  free(barrier);
}

static int _sr_barrier_wait(sr_barrier_t *this, int sense) {
  int temp;
  do {
    sync_pause();
    temp = sync_load(&this->sense, SYNC_ACQUIRE);
  } while (sense != temp);
  return 0;
}

static int _sr_barrier_reset(sr_barrier_t *this, int sense) {
  ++this->rounds;
  this->count = 0;
  sync_store(&this->sense, sense, SYNC_RELEASE);
  return 1;
}

/// Sense-reversing join member function.
///
/// see: http://www.morganclaypool.com/doi/abs/10.2200/S00499ED1V01Y201304CAC023
static int _sr_barrier_join(barrier_t *barrier, int i) {
  sr_barrier_t *this = (sr_barrier_t*)barrier;
  int sense = 1 - this->senses[i];
  this->senses[i] = sense;

  int count = sync_fadd(&this->count, 1, SYNC_ACQ_REL) + 1;
  if (count != this->threads) {
    return _sr_barrier_wait(this, sense);
  }
  else {
    return _sr_barrier_reset(this, sense);
  }
}

barrier_t *sr_barrier_new(int n) {
  sr_barrier_t *barrier = malloc(sizeof(sr_barrier_t) + n * sizeof(int));
  assert(barrier);

  barrier->vtable.delete = _sr_barrier_delete;
  barrier->vtable.join   = _sr_barrier_join;
  barrier->count         = 0;
  barrier->sense         = 1;
  barrier->rounds        = 0;
  barrier->threads       = n;

  for (int i = 0; i < n; ++i) {
    barrier->senses[i]   = 1;
  }

  return &barrier->vtable;
}


