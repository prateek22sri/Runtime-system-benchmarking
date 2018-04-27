// =============================================================================
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

/// @brief Implements pthread_barrier for Darwin (Mac OS X).
/// Credit :
/// http://blog.albertarmea.com/post/47089939939/using-pthread-barrier-on-mac-os-x

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <errno.h>
#include <libhpx/debug.h>
#include <libhpx/system.h>

int system_barrier_init(system_barrier_t *barrier,
                        const system_barrierattr_t *attr, unsigned int count) {
  if (count == 0) {
    errno = EINVAL;
    return -1;
  }

  if (pthread_mutex_init(&barrier->mutex, 0) < 0) {
    return -1;
  }

  if (pthread_cond_init(&barrier->cond, 0) < 0) {
    pthread_mutex_destroy(&barrier->mutex);
    return -1;
  }
  
  barrier->trip_count = count;
  barrier->count = 0;
  return 0;
}

int system_barrier_destroy(system_barrier_t *barrier) {
  pthread_cond_destroy(&barrier->cond);
  pthread_mutex_destroy(&barrier->mutex);
  return 0;
}

int system_barrier_wait(system_barrier_t *barrier) {
  pthread_mutex_lock(&barrier->mutex);
  ++(barrier->count);
  
  if (barrier->count >= barrier->trip_count) {
    barrier->count = 0;
    pthread_cond_broadcast(&barrier->cond);
    pthread_mutex_unlock(&barrier->mutex);
    return 1;
  }
  else {
    pthread_cond_wait(&barrier->cond, &(barrier->mutex));
    pthread_mutex_unlock(&barrier->mutex);
    return 0;
  }
}

