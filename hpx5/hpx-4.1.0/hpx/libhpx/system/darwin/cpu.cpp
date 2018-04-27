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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/// @file libhpx/platform/darwin/cpu.c
/// @brief Implements HPX's CPU interface on Darwin (Mac OS X).

#include <inttypes.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <libhpx/debug.h>
#include <libhpx/libhpx.h>
#include <libhpx/system.h>
#include <pthread.h>

void system_get_stack(pthread_t thread, void **base, size_t *size) {
  dbg_assert(base && size);
  *size = pthread_get_stacksize_np(thread);
  *base = pthread_get_stackaddr_np(thread);
}
