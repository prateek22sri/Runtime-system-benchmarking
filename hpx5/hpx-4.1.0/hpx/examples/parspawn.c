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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include "hpx/hpx.h"


/// ----------------------------------------------------------------------------
/// @file examples/hpx/parspawn.c
/// This file implements a parallel (tree) spawn, that uses HPX
/// threads to spawn many NOP operations in parallel.
/// ----------------------------------------------------------------------------

static void _usage(FILE *stream, int error) {
  fprintf(stream, "Usage: parspawn [options] NUMBER\n"
          "\t-h, this help display\n");
  hpx_print_help();
  fflush(stream);
  exit(error);
}

static hpx_action_t _nop     = 0;
static hpx_action_t _main    = 0;


/// The empty action
static int _nop_action(void *args, size_t size) {
  return HPX_SUCCESS;
}

static int _main_action(int *args, size_t size) {
  int n = *args;
  printf("parspawn(%d)\n", n); fflush(stdout);

  hpx_time_t now = hpx_time_now();
  hpx_par_call_sync(_nop, 0, n, 8, 1000, 0, NULL, 0, 0);
  double elapsed = hpx_time_elapsed_ms(now)/1e3;

  printf("seconds: %.7f\n", elapsed);
  printf("localities:   %d\n", HPX_LOCALITIES);
  printf("threads:      %d\n", HPX_THREADS);
  hpx_exit(0, NULL);
}

/// The main function parses the command line, sets up the HPX runtime system,
/// and initiates the first HPX thread to perform parspawn(n).
int main(int argc, char *argv[]) {
  // register the actions
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _nop, _nop_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _main, _main_action, HPX_POINTER, HPX_SIZE_T);

  if (hpx_init(&argc, &argv)) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return -1;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "h?")) != -1) {
    switch (opt) {
     case 'h':
       _usage(stdout, EXIT_SUCCESS);
     case '?':
     default:
       _usage(stderr, EXIT_FAILURE);
    }
  }

  argc -= optind;
  argv += optind;

  int n = 0;
  switch (argc) {
   case 0:
     fprintf(stderr, "\nMissing spawn count.\n");
   default:
     _usage(stderr, EXIT_FAILURE);
   case 1:
     n = atoi(argv[0]);
     break;
  }

  // run the main action
  int e = hpx_run(&_main, NULL, &n, sizeof(n));
  hpx_finalize();
  return e;
}
