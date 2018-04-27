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

#include "libsync/locks.h"
#include "libsync/backoff.h"

void
sync_tatas_acquire_slow(tatas_lock_t *l) {
  static const unsigned int base = 16;
  unsigned int i = base;
  do {
    sync_backoff_exp_r(&i);
  } while (sync_swap(&l->lock, 1, SYNC_ACQUIRE));
}

void
sync_tatas_init(tatas_lock_t *l) {
  l->lock = 0;
}
