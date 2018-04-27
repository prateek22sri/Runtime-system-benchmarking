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
/// @file examples/hpx/seqspawn.c
/// This file implements a sequential task spawn microbenchmark. A
/// series of NOP threads are spawned sequentially.
/// ----------------------------------------------------------------------------

static void _usage(FILE *stream, int error) {
  fprintf(stream, "Usage: seqspawn [options] NUMBER\n"
          "\t-h, this help display\n");
  hpx_print_help();
  fflush(stream);
  exit(error);
}

static hpx_action_t _nop  = 0;
static hpx_action_t _main = 0;

/// The empty action
static int _nop_action(hpx_addr_t *args, size_t size) {
  return hpx_thread_continue(NULL, 0);
}

static int _main_action(int *args, size_t size) {
  int n = *args;
  printf("seqspawn(%d)\n", n); fflush(stdout);

  hpx_addr_t and = hpx_lco_and_new(n);
  hpx_time_t now = hpx_time_now();
  for (int i = 0; i < n; i++)
    hpx_call(HPX_HERE, _nop, and, 0, 0);
  hpx_lco_wait(and);
  double elapsed = hpx_time_elapsed_ms(now)/1e3;
  hpx_lco_delete(and, HPX_NULL);

  printf("seconds: %.7f\n", elapsed);
  printf("localities:   %d\n", HPX_LOCALITIES);
  printf("threads:      %d\n", HPX_THREADS);
  hpx_exit(0, NULL);
}

/// The main function parses the command line, sets up the HPX runtime system,
/// and initiates the first HPX thread to perform seqspawn(n).
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
