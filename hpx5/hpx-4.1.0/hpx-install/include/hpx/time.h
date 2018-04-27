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

#ifndef HPX_TIME_H
#define HPX_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup other
/// @{

/// @file
/// @brief HPX high-resolution timer interface

#if defined(__linux__)
#include <time.h>
typedef struct timespec hpx_time_t;
#define HPX_TIME_INIT {0}
#elif defined(__APPLE__)
#include <stdint.h>
typedef uint64_t hpx_time_t;
#define HPX_TIME_INIT (0)
#endif

/// @struct {hpx_time_t} the type used internally by HPX to represent
/// time

/// The equivalent of NULL for the hpx_time_t type
#define HPX_TIME_NULL hpx_time_construct(0,0)

/// Get the current time
/// @returns the current time at the highest resolution possible
hpx_time_t hpx_time_now(void) HPX_PUBLIC;

/// Convert a time object to an integer counting nanoseconds since some start time
/// "Start" is not specifically defined, but occurs sometime before the first call
/// to hpx_time_now().
uint64_t hpx_time_from_start_ns(hpx_time_t t) HPX_PUBLIC;

/// Get a double representing a time in nanoseconds
/// @param from the time to convert
/// @returns    the time as converted to a double, in nanoseconds
double hpx_time_ns(hpx_time_t from) HPX_PUBLIC;

/// Get a double representing a time in microseconds
/// @param from the time to convert
/// @returns    the time as converted to a double, in microseconds
double hpx_time_us(hpx_time_t from) HPX_PUBLIC;

/// Get a double representing a time in milliseconds
/// @param from the time to convert
/// @returns    the time as converted to a double, in milliseconds
double hpx_time_ms(hpx_time_t from) HPX_PUBLIC;

/// Get a double representing a time span in microseconds
/// @param from the beginning of the time span
/// @param   to the end of the time span to convert
/// @returns the time span as converted to a double, in microseconds
double hpx_time_diff_us(hpx_time_t from, hpx_time_t to) HPX_PUBLIC;

/// Get a double representing a time span in milliseconds
/// @param from the beginning of the time span
/// @param   to the end of the time span to convert
/// @returns the time span as converted to a double, in milliseconds
double hpx_time_diff_ms(hpx_time_t from, hpx_time_t to) HPX_PUBLIC;

/// Get an integer representing a time span in nanoseconds
/// @param from the beginning of the time span
/// @param   to the end of the time span to convert
/// @returns the time span in nanoseconds
int64_t hpx_time_diff_ns(hpx_time_t from, hpx_time_t to) HPX_PUBLIC;

/// Get a time span in `hpx_time_t`
/// @param start the beginning of the time span
/// @param   end the end of the time span to convert
/// @param  diff the time span in `hpx_time_t` units
void hpx_time_diff(hpx_time_t start, hpx_time_t end, hpx_time_t *diff)
  HPX_PUBLIC;

/// Get the time elapsed since @p from, in microseconds
/// @param from the start time to measure from
/// @returns    the difference between @p from and now,
///             in microseconds
double hpx_time_elapsed_us(hpx_time_t from) HPX_PUBLIC;

/// Get the time elapsed since @p from, in milliseconds
/// @param from the start time to measure from
/// @returns    the difference between @p from and now,
///             in milliseconds
double hpx_time_elapsed_ms(hpx_time_t from) HPX_PUBLIC;

/// Get the time elapsed since @p from, in nanoseconds
/// @param from the start time to measure from
/// @returns    the difference between @p from and now,
///             in nanoseconds
uint64_t hpx_time_elapsed_ns(hpx_time_t from) HPX_PUBLIC;

/// Get the time elapsed since @p from, in hpx_time_t.
/// @param  from the start time to measure from
/// @param  diff the difference between @p start and now.
void hpx_time_elapsed(hpx_time_t from, hpx_time_t *diff) HPX_PUBLIC;

/// Construct an hpx time object
/// @param  s seconds
/// @param ns nanoseconds
/// @returns  a pointer to an object of type hpx_time_t that has the specified
///           duration
hpx_time_t hpx_time_construct(unsigned long s, unsigned long ns) HPX_PUBLIC;

/// Add two hpx time objects
/// Given a point in time, and a duration from that point, construct a new
/// point in time. Duration at present must be a positive interval.
/// @param    start A starting time
/// @param duration A duration from the starting time
/// @returns        A pointer to an object of type hpx_time_t that represents
///                 a point in time, based on the start time and duration
hpx_time_t hpx_time_add(hpx_time_t start, hpx_time_t duration) HPX_PUBLIC;

/// @}

#ifdef __cplusplus
}
#endif

#endif
