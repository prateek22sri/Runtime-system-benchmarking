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

#define BENCHMARK "OSU HPX MEMPUT Test"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <hpx/hpx.h>

#define MAX_MSG_SIZE        (1<<22)
#define SKIP_LARGE          10
#define LOOP_LARGE          100
#define LARGE_MESSAGE_SIZE  (1<<13)

int skip = 1000;
int loop = 10000;

#ifdef PACKAGE_VERSION
#   define HEADER "# " BENCHMARK " v" PACKAGE_VERSION "\n"
#else
#   define HEADER "# " BENCHMARK "\n"
#endif

#ifndef FIELD_WIDTH
#   define FIELD_WIDTH 20
#endif

#ifndef FLOAT_PRECISION
#   define FLOAT_PRECISION 2
#endif

void wtime(double *t)
{
  static int sec = -1;
  struct timeval tv;
  gettimeofday(&tv, (void *)0);
  if (sec < 0) sec = tv.tv_sec;
  *t = (tv.tv_sec - sec)*1.0e+6 + tv.tv_usec;
}

static int _init_array_handler(char *local, size_t n) {
  for(int i = 0; i < n; i++) {
    local[i] = (HPX_LOCALITY_ID == 0) ? 'a' : 'b';
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _init_array,
                  _init_array_handler, HPX_POINTER, HPX_SIZE_T);

static HPX_ACTION_DECL(_main);
static int _main_handler(void) {
  double t_start = 0.0, t_end = 0.0;
  int rank = HPX_LOCALITY_ID;
  int size = HPX_LOCALITIES;
  int peerid = (rank+1)%size;
  int i;

  if (size == 1) {
    fprintf(stderr, "This test requires at least two HPX threads\n");
    hpx_abort();
  }

  hpx_addr_t data = hpx_gas_alloc_cyclic(size, MAX_MSG_SIZE*2, 0);
  hpx_addr_t remote = hpx_addr_add(data, MAX_MSG_SIZE*2 * peerid, MAX_MSG_SIZE*2);

  fprintf(stdout, HEADER);
  fprintf(stdout, "# [ pairs: %d ]\n", size/2);
  fprintf(stdout, "%-*s%*s\n", 10, "# Size", FIELD_WIDTH, "Latency (us)");
  fflush(stdout);

  for (size_t size = 1; size <= MAX_MSG_SIZE; size*=2) {
    hpx_addr_t rfut = hpx_lco_future_new(0);
    hpx_call(remote, _init_array, rfut, &size);
    char *local = hpx_malloc_registered(size);
    _init_array_handler(local, size);
    hpx_lco_wait(rfut);
    hpx_lco_delete(rfut, HPX_NULL);

    if (size > LARGE_MESSAGE_SIZE) {
      loop = LOOP_LARGE;
      skip = SKIP_LARGE;
    }

    for (i = 0; i < loop + skip; i++) {
      if(i == skip) {
        wtime(&t_start);
      }

      hpx_gas_memput_rsync(remote, local, size);
    }

    wtime(&t_end);

    hpx_free_registered(local);

    double latency = (t_end - t_start)/(1.0 * loop);
    fprintf(stdout, "%-*zu%*.*f\n", 10, size, FIELD_WIDTH,
            FLOAT_PRECISION, latency);
    fflush(stdout);
  }
  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _main, _main_handler);

static void usage(FILE *f) {
  fprintf(f, "Usage: memput [options]\n"
          "\t-h, show help\n");
  hpx_print_help();
  fflush(f);
}

int main(int argc, char *argv[argc]) {
  if (hpx_init(&argc, &argv)) {
    fprintf(stderr, "HPX failed to initialize.\n");
    return 1;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "h?")) != -1) {
    switch (opt) {
     case 'h':
      usage(stdout);
      return 0;
     case '?':
     default:
      usage(stderr);
      return -1;
    }
  }

  if (HPX_LOCALITIES < 2) {
    fprintf(stderr, "A minimum of 2 localities are required to run this test.\n");
    return -1;
  }

  int e = hpx_run(&_main, NULL);
  hpx_finalize();
  return e;
}
