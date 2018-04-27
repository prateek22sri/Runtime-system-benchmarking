// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================
#ifndef LIBHPX_SCHEDULER_CUCKOO_HASHTABLE_H
#define LIBHPX_SCHEDULER_CUCKOO_HASHTABLE_H

#include "hpx/attributes.h"
#include "libsync/locks.h"

typedef struct cuckoo_bucket cuckoo_bucket_t;

typedef struct {
  tatas_lock_t lock;                            // serialize writes
  int size;
  cuckoo_bucket_t *table;
} cuckoo_hashtable_t;

HPX_INTERNAL cuckoo_hashtable_t *sync_cuckoo_hashtable_new(void);


HPX_INTERNAL void sync_cuckoo_hashtable_init(cuckoo_hashtable_t *)
  HPX_NON_NULL(1);


HPX_INTERNAL void sync_cuckoo_hashtable_delete(cuckoo_hashtable_t *)
  HPX_NON_NULL(1);


HPX_INTERNAL int sync_cuckoo_hashtable_insert(cuckoo_hashtable_t *, long key,
                                              const void *val)
  HPX_NON_NULL(1);


HPX_INTERNAL const void *sync_cuckoo_hashtable_lookup(cuckoo_hashtable_t *, long key)
  HPX_NON_NULL(1);


HPX_INTERNAL void sync_cuckoo_hashtable_remove(cuckoo_hashtable_t *, long key)
  HPX_NON_NULL(1);


#define SYNC_CUCKOO_HASHTABLE_INIT {            \
    .lock = SYNC_TATAS_LOCK_INIT,               \
    .size = 0,                                  \
    .table = NULL                               \
    }

#endif // LIBHPX_SCHEDULER_CUCKOO_HASHTABLE_H
