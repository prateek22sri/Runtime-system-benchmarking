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

#ifndef LIBHPX_TIME_H
#define LIBHPX_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/// Initialize internal clocks
/// This is presently used mainly to enable hpx_time_from_start_ns(),
/// but could be used to provide other functionality.
void libhpx_time_start(void);

/// Report what is considered the beginning of time
/// This is the value of the clock when libhx_time_start() was called.
/// This function is useful for making sure different parts of libhpx
/// are counting time from the same start time.
hpx_time_t libhpx_beginning_of_time(void);

#ifdef __cplusplus
}
#endif

#endif // LIBHPX_TIME_H
