/// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
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

#include "libhpx/percolation.h"

// /// Delete a percolation object.
void
percolation_deallocate(void *obj)
{
  auto p = static_cast<const percolation_t *>(obj);
  p->deallocate(obj);
}

/// Percolate an action to be executed later on a device.
void*
percolation_prepare(const void *obj, const char *key, const char *kernel)
{
  auto p = static_cast<const percolation_t *>(obj);
  return p->prepare(obj, key, kernel);
}

/// Execute a percolation object on an available device.
int
percolation_execute(const void *obj, void *o, int n, void *vargs[],
                    size_t sizes[])
{
  auto p = static_cast<const percolation_t *>(obj);
  return p->execute(obj, o, n, vargs, sizes);
}

/// Destroy or release the state associated with a percolation object.
void
percolation_destroy(const void *obj, void *o)
{
  auto p = static_cast<const percolation_t *>(obj);
  p->destroy(obj, o);
}
