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

#ifndef __APPLE__
#error The HPX time implementation is configured incorrectly
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/// @file libhpx/system/darwin/time.c
/// @brief Implements HPX's time interface on Darwin (Mac OS X).
#include <assert.h>
#include <mach/mach_time.h>
#include "hpx/hpx.h"

hpx_time_t hpx_time_now(void) {
  return mach_absolute_time();
}

double hpx_time_ns(hpx_time_t time) {
  static mach_timebase_info_data_t tbi;
  if (tbi.denom == 0)
    (void) mach_timebase_info(&tbi);
  assert(tbi.denom != 0);

  return (double)((time * tbi.numer)/tbi.denom);
}

void hpx_time_diff(hpx_time_t start, hpx_time_t end, hpx_time_t *diff) {
  *diff = end - start;
}

hpx_time_t hpx_time_construct(unsigned long s, unsigned long ns) {
  static mach_timebase_info_data_t tbi;
  if (tbi.denom == 0)
    (void) mach_timebase_info(&tbi);
  assert(tbi.denom != 0);

  hpx_time_t t;
  t = (s * 1e9 + ns) * (tbi.denom / tbi.numer);
  return t;
}
