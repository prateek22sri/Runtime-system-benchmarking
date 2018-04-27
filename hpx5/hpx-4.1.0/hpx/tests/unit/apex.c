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

/// @file
/// A simple fibonacci number computation to demonstrate HPX.
/// This example calculates a fibonacci number using recursion, where each
/// level of recursion is executed by a different HPX thread. (Of course, this
/// is not an efficient way to calculate a fibonacci number but it does
/// demonstrate some of the basic of HPX and it may demonstrate a
/// <em>pattern of computation</em> that might be used in the real world.)

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "hpx/hpx.h"
#include "apex.h"

// using this to validate the APEX result.
volatile int function_count = 0;

static void _usage(FILE *f, int error) {
  fprintf(f, "Usage: fibonacci [options] NUMBER\n"
          "\t-h, show help\n");
  hpx_print_help();
  fflush(f);
  exit(error);
}

static HPX_ACTION_DECL(_fib);
static int _fib_handler(int n)
{
  __sync_fetch_and_add(&function_count, 1); // increment the function count

  if (n < 2) {
    return HPX_THREAD_CONTINUE(n);
  }

  hpx_addr_t peers[] = {
    HPX_HERE,
    HPX_HERE
  };

  int ns[] = {
    n - 1,
    n - 2
  };

  hpx_addr_t futures[] = {
    hpx_lco_future_new(sizeof(int)),
    hpx_lco_future_new(sizeof(int))
  };

  int fns[] = {
    0,
    0
  };

  void *addrs[] = {
    &fns[0],
    &fns[1]
  };

  size_t sizes[] = {
    sizeof(int),
    sizeof(int)
  };

  hpx_call(peers[0], _fib, futures[0], &ns[0]);
  hpx_call(peers[1], _fib, futures[1], &ns[1]);
  hpx_lco_get_all(2, futures, sizes, addrs, NULL);
  hpx_lco_delete(futures[0], HPX_NULL);
  hpx_lco_delete(futures[1], HPX_NULL);

  int fn = fns[0] + fns[1];
  return HPX_THREAD_CONTINUE(fn);
}
static HPX_ACTION(HPX_DEFAULT, 0, _fib, _fib_handler, HPX_INT);

static int _fib_main_handler(int n)
{
  int fn = 0;                                   // fib result
  printf("fib(%d)=", n); fflush(stdout);
  hpx_time_t now = hpx_time_now();
  apex_profiler_handle p = apex_start(APEX_NAME_STRING, "FIB main");

  hpx_call_sync(HPX_HERE, _fib, &fn, sizeof(fn), &n);
  apex_stop(p);
  double elapsed = hpx_time_elapsed_ms(now)/1e3;

  printf("%d\n", fn);
  printf("seconds: %.7f\n", elapsed);
  printf("localities: %d\n", HPX_LOCALITIES);
  printf("threads/locality: %d\n", HPX_THREADS);

  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _fib_main, _fib_main_handler, HPX_INT);

int main(int argc, char *argv[]) {
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

  int n = 10;
  switch (argc) {
   case 0:
     fprintf(stderr, "\nMissing fib number. Using %d.\n", n);
     break;
   case 1:
     n = atoi(argv[0]);
     break;
  }

  // run the main action
  e = hpx_run(&_fib_main, NULL, &n);
  hpx_finalize();

  // validate the APEX counts
  printf("_fib_action Function calls : %d\n", function_count);
  apex_profile * profile = apex_get_profile(APEX_FUNCTION_ADDRESS, (void*)(&_fib_handler));
  if (profile) {
      printf("Number of calls Reported by APEX : %d, total time in action: %f seconds (or cycles, if rdtsc was used).\n", (int)profile->calls, profile->accumulated);
      if (function_count == profile->calls) {
          printf("Test passed.\n");
          e = 0;
      } else {
          e = 1;
      }
  }
  return e;
}

