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

#include <string.h>
#include <unistd.h>
#include <hpx/hpx.h>
#include <libhpx/debug.h>
#include <libhpx/libhpx.h>
#include <libhpx/time.h>

static hpx_time_t _beginning_of_time;

void libhpx_time_start(void) {
  _beginning_of_time = hpx_time_now();
}

double hpx_time_us(hpx_time_t time) {
  return hpx_time_ns(time)/1e3;
}

double hpx_time_ms(hpx_time_t time) {
  return hpx_time_ns(time)/1e6;
}

/// Functions to compute the difference between to time samples @p
/// from and @p to.

int64_t hpx_time_diff_ns(hpx_time_t from, hpx_time_t to) {
  return (int64_t)(hpx_time_ns(to) - hpx_time_ns(from));
}

double hpx_time_diff_us(hpx_time_t from, hpx_time_t to) {
  return hpx_time_diff_ns(from, to)/1e3;
}

double hpx_time_diff_ms(hpx_time_t from, hpx_time_t to) {
  return hpx_time_diff_ns(from, to)/1e6;
}

/// Functions to get the elapsed time since previously sampled time
/// value @p from.

double hpx_time_elapsed_us(hpx_time_t from) {
  return hpx_time_diff_us(from, hpx_time_now());
}

double hpx_time_elapsed_ms(hpx_time_t from) {
  return hpx_time_diff_ms(from, hpx_time_now());
}

uint64_t hpx_time_elapsed_ns(hpx_time_t from) {
  return (uint64_t)hpx_time_diff_ns(from, hpx_time_now());
}

void hpx_time_elapsed(hpx_time_t from, hpx_time_t *diff) {
  hpx_time_diff(from, hpx_time_now(), diff);
}

/// Functions for dealing with time relative to "beginning of time".

hpx_time_t libhpx_beginning_of_time(void) {
  return _beginning_of_time;
}

uint64_t hpx_time_from_start_ns(hpx_time_t t) {
  return (uint64_t)hpx_time_diff_ns(_beginning_of_time, t);
}

hpx_time_t hpx_time_add(hpx_time_t time1, hpx_time_t time2) {
  int64_t total = hpx_time_ns(time1) + hpx_time_ns(time2);
  int64_t ns = total % (int64_t)1e9;
  int64_t s = (total / 1e9);
  return hpx_time_construct(s, ns);
}
