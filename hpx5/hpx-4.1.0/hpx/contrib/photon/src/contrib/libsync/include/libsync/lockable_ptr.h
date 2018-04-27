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

#ifndef HPX_SYNC_LOCKABLE_PTR_H
#define HPX_SYNC_LOCKABLE_PTR_H

#include<hpx/attributes.h>

typedef const void * volatile lockable_ptr_t;
#define SYNC_LOCKABLE_PTR(T) const T * volatile

const void *sync_lockable_ptr_lock(lockable_ptr_t *p) HPX_PUBLIC;
void sync_lockable_ptr_unlock(lockable_ptr_t *p) HPX_PUBLIC;
const void *sync_lockable_ptr_read(lockable_ptr_t *p) HPX_PUBLIC;

#endif // HPX_SYNC_LOCKABLE_PTR_H
