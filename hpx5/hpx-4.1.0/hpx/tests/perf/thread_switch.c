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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "hpx/hpx.h"


static void _usage(FILE *f, int error) {
  fprintf(f, "Usage: cswitch [options] NUMBER\n"
          "\t-h, show help\n");
  hpx_print_help();
  fflush(f);
  exit(error);
}

static hpx_action_t _setter       = 0;
static hpx_action_t _getter       = 0;
static hpx_action_t _cswitch_main = 0;

static int _setter_action(int n, hpx_addr_t f1, hpx_addr_t f2) {
  for (int i = 0; i < n; ++i) {
    // printf("Setting LCO %d.\n", i);
    hpx_lco_set(f1, 0, NULL, HPX_NULL, HPX_NULL);
    hpx_lco_wait(f2);
    hpx_lco_reset_sync(f2);
  }
  return HPX_SUCCESS;
}

static int _getter_action(int n, hpx_addr_t f1, hpx_addr_t f2) {
  for (int i = 0; i < n; ++i) {
    // printf("Getting LCO %d.\n", i);
    hpx_lco_wait(f1);
    hpx_lco_reset_sync(f1);
    hpx_lco_set(f2, 0, NULL, HPX_NULL, HPX_NULL);
  }
  return HPX_SUCCESS;
}

static int _cswitch_main_action(int *args, size_t size) {
  int n = *args;
  printf("cswitch(%d)\n", n); fflush(stdout);
  hpx_addr_t and = hpx_lco_and_new(2);
  hpx_addr_t f1 = hpx_lco_future_new(0);
  hpx_addr_t f2 = hpx_lco_future_new(0);

  hpx_time_t now = hpx_time_now();
  hpx_call(HPX_HERE, _setter, and, &n, &f1, &f2);
  hpx_call(HPX_HERE, _getter, and, &n, &f1, &f2);
  hpx_lco_wait(and);
  double elapsed = hpx_time_elapsed_ms(now)/1e3;
  hpx_lco_delete(and, HPX_NULL);
  hpx_lco_delete(f1, HPX_NULL);
  hpx_lco_delete(f2, HPX_NULL);

  printf("seconds: %.7f\n", elapsed);
  printf("localities: %d\n", HPX_LOCALITIES);
  printf("threads/locality: %d\n", HPX_THREADS);
  hpx_exit(0, NULL);
}

int main(int argc, char *argv[]) {
  // register the cswitch action
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _setter, _setter_action,
                      HPX_INT, HPX_ADDR, HPX_ADDR);
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _getter, _getter_action,
                      HPX_INT, HPX_ADDR, HPX_ADDR);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _cswitch_main, _cswitch_main_action,
                      HPX_POINTER, HPX_SIZE_T);

  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return e;
  }

  // parse the command line
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

  int n = 100000;
  switch (argc) {
   case 0:
     break;
   default:
     _usage(stderr, EXIT_FAILURE);
   case 1:
     n = atoi(argv[0]);
     break;
  }

  // run the main action
  e = hpx_run(&_cswitch_main, NULL, &n, sizeof(n));
  hpx_finalize();
  return e;
}

