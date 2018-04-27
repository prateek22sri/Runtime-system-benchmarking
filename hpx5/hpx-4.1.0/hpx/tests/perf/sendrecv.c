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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "hpx/hpx.h"

static int counts[24] = {
  1,
  2,
  3,
  4,
  25,
  50,
  75,
  100,
  125,
  500,
  1000,
  2000,
  3000,
  4000,
  25000,
  50000,
  75000,
  100000,
  125000,
  500000,
  1000000,
  2000000,
  3000000,
  4000000
};

static int _recv_handler(double *args, size_t size) {
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _recv, _recv_handler,
                  HPX_POINTER, HPX_SIZE_T);

static int _main_action(int levels, int work) {
  int avg = 10000;

  printf("Running sendrecv on %d levels with %d work\n", levels, work);

  hpx_time_t tick = hpx_time_now();
  printf(" Tick: %g\n", hpx_time_us(tick));

  for (int i = 0, e = levels; i < e; ++i) {
    size_t bytes = sizeof(double) * counts[i];
    double *buf = malloc(bytes);
    for (int j = 0, e = counts[i]; j < e; ++j) {
      buf[j] = j * rand();
    }

    printf("%d, %d: ", i, counts[i]);
    hpx_time_t t1 = hpx_time_now();

    // for completing the entire loop
    hpx_addr_t done = hpx_lco_and_new(avg);

    for (int j = 0, e = avg; j < e; ++j) {
      hpx_call_async(HPX_HERE, _recv, done, HPX_NULL, buf, bytes);

      // do the useless work
      double volatile d = 0.0;
      for (int k = 0, e = work; k < e; ++k) {
        d += 1.0 / (2.0 * k + 1.0);
      }
    }

    hpx_lco_wait(done);
    hpx_lco_delete(done, HPX_NULL);

    double elapsed = hpx_time_elapsed_ms(t1);
    printf("Elapsed: %g\n", elapsed/avg);
    free(buf);
  }

  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _main, _main_action, HPX_INT, HPX_INT);

static void
_usage(FILE *f) {
  fprintf(f, "Usage: sendrecv [options] [LEVELS < 24]\n"
          "\t-w, amount of work\n"
          "\t-h, show help\n");
  hpx_print_help();
  fflush(f);
}

int main(int argc, char *argv[argc]) {

  if (hpx_init(&argc, &argv)) {
    fprintf(stderr, "HPX failed to initialize.\n");
    return -1;
  }

  int levels = 24;
  int work = 10000;

  int opt = 0;
  while ((opt = getopt(argc, argv, "w:h?")) != -1) {
    switch (opt) {
     case 'w':
      work = atoi(optarg);
      break;
     case 'h':
     case '?':
      _usage(stdout);
      return 0;
     default:
      _usage(stderr);
      return -1;
    }
  }

  argc -= optind;
  argv += optind;

  switch (argc) {
   case 1:
     levels = atoi(argv[0]);
     break;
   case 0:
     break;
   default:
    _usage(stderr);
    return -1;
  }

  if (levels > 24) {
    _usage(stderr);
    return -1;
  }

  int e = hpx_run(&_main, NULL, &levels, &work);
  hpx_finalize();
  return e;
}
