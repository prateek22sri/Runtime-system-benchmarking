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

#include <libsync/backoff.h>
#include <libsync/lockable_ptr.h>
#include <libsync/sync.h>

static const unsigned int _BACKOFF_BASE = 16;
static const uintptr_t       _LOCK_MASK = 0x1;
static const uintptr_t      _STATE_MASK = 0x7;

const void *
sync_lockable_ptr_lock(lockable_ptr_t *p)
{
  while (true) {
    unsigned int backoff = _BACKOFF_BASE;

    // load the ptr
    const void *ptr = sync_load(p, SYNC_ACQUIRE);
    const void *tmp = ptr;

    // check to see if the ptr is locked
    uintptr_t bits = (uintptr_t)ptr;
    if (bits & _LOCK_MASK) {
      sync_backoff_exp_r(&backoff);
      continue;
    }

    // try and lock the ptr
    const void *lock = (const void*)(bits | _LOCK_MASK);
    if (!sync_cas(p, &tmp, lock, SYNC_ACQUIRE, SYNC_RELAXED)) {
      tmp = ptr;
      sync_backoff_exp_r(&backoff);
      continue;
    }

    // success
    return ptr;
  }
}


void
sync_lockable_ptr_unlock(lockable_ptr_t *p)
{
  // assume that we hold the lock already, so we can read it without sync
  uintptr_t bits = *(uintptr_t*)p;
  const void *ptr = (const void*)(bits & ~_LOCK_MASK);
  sync_store(p, ptr, SYNC_RELEASE);
}


const void *
sync_lockable_ptr_read(lockable_ptr_t *p)
{
  const void *ptr = sync_load(p, SYNC_ACQUIRE);
  uintptr_t bits = (uintptr_t)ptr;
  bits = bits & ~_STATE_MASK;
  return (const void*)bits;
}
