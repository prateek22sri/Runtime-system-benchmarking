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

/// @file trace.c
///
/// Print a backtrace to stdout
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif

/* Obtain a backtrace and print it to stdout. */
void system_print_trace(void *fd) {
#ifdef HAVE_EXECINFO_H
  void *array[32];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);

  fprintf (static_cast<FILE*>(fd), "Obtained %zd stack frames.\n", size);

  for (i = 0; i < size; i++) {
    printf ("%s\n", strings[i]);
  }
  free (strings);
#endif
}
