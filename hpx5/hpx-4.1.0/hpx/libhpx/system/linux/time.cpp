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

/// @file libhpx/system/linux/time.c
/// @brief Implements HPX's time interface on linux.
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <hpx/hpx.h>
#include <libhpx/debug.h>

hpx_time_t hpx_time_now(void) {
  hpx_time_t time;
  if (clock_gettime(CLOCK_MONOTONIC, &time)) {
    dbg_error("system: hpx_time_now() failed to get time.\n");
  }
  return time;
}

double hpx_time_ns(hpx_time_t time) {
  return (time.tv_sec * 1e9) + time.tv_nsec;
}

void hpx_time_diff(hpx_time_t start, hpx_time_t end, hpx_time_t *diff) {
  if (end.tv_nsec < start.tv_nsec) {
    diff->tv_sec = end.tv_sec - start.tv_sec - 1;
    diff->tv_nsec = (1e9 + end.tv_nsec) - start.tv_nsec;
  } else {
    diff->tv_sec = end.tv_sec - start.tv_sec;
    diff->tv_nsec = end.tv_nsec - start.tv_nsec;
  }
}

hpx_time_t hpx_time_construct(unsigned long s, unsigned long ns) {
  hpx_time_t t;
  t.tv_sec = s;
  t.tv_nsec = ns;
  return t;
}


