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
#include <time.h>
#include <inttypes.h>
#include "libhpx/libhpx.h"
#include "hpx/hpx.h"

/// This is a microbenchmark to determine the effectiveness of
/// automatic load balancing in HPX.
///

// Fixed Work Quantum (FWQ)
static int _fwq(int work) {
  // printf("executing fwq(%d) on thread %d locality %d.\n",
  //        work, HPX_THREAD_ID, HPX_LOCALITY_ID);
  register unsigned long done;
  register long long count;
  register long long wl = -(1 << work);

  for (count=wl; count<0;) {
    register int k;
    for (k=0;k<16;k++)
      count++;
    for (k=0;k<15;k++)
      count--;
  }
  return HPX_SUCCESS;
}

// par-for handler to execute FWQ in parallel.
static int _par_fwq(const int UNUSED, void *args) {
  int *work = args;
  assert(work);
  return _fwq(*work);
}

// HPX handler to execute par-fwq on a remote locality.
static HPX_ACTION_DECL(_fwq_action);
int _fwq_handler(int *element, int n) {
  return hpx_par_for_sync(_par_fwq, 0, n, element);
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _fwq_action, _fwq_handler,
                  HPX_POINTER, HPX_INT);

// HPX handler to initialize a global array
static int _initialize_handler(int *element, int n, int initializer) {
  for (int i = 0; i < n; ++i) {
    element[i] = initializer;
  }
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _initialize, _initialize_handler,
                  HPX_POINTER, HPX_INT, HPX_INT);

static void _usage(FILE *f, int error) {
  fprintf(f, "Usage: lbbench -i iters -w work -n nelem\n"
             "\t -i iters: number of iterations\n"
             "\t -w  work: computational work per reduction\n"
             "\t -n nelem: number of elems per node\n"
             "\t -h      : show help\n");
  hpx_print_help();
  fflush(f);
  exit(error);
}

int _kernel(hpx_addr_t array, size_t bsize, int iters, int work,
            int nelem) {
  // initialize the global array
  hpx_time_t start = hpx_time_now();
  int e = hpx_gas_bcast_sync(_initialize, array, HPX_LOCALITIES, 0, bsize,
                             &nelem, &work);
  assert(e == HPX_SUCCESS);
  double itime = hpx_time_elapsed_us(start);

  start = hpx_time_now();
  for (int i = 0; i < iters; ++i) {
    hpx_gas_bcast_sync(_fwq_action, array, HPX_LOCALITIES, 0, bsize, &nelem);
  }
  double ctime = hpx_time_elapsed_us(start);

  printf("init (s): %.7f\n", itime/1e6);
  printf("total (s): %.7f\n", (itime+ctime)/1e6);
  printf("per-iteration (s): %.7f\n", (itime+ctime)/(iters*1e6));
  return HPX_SUCCESS;
}

hpx_addr_t _random_dist(uint32_t i, size_t n, size_t bsize) {
  return HPX_THERE(rand() % HPX_LOCALITIES);
}

static HPX_ACTION_DECL(_main);
static int _main_action(int iters, int work, int nelem) {
  printf("lbbench(iters=%d, work=%d, nelem=%d)\n", iters, work, nelem);
  printf("threads: %d, localities: %d\n", HPX_THREADS, HPX_LOCALITIES);
  printf("time resolution: microseconds\n");
  fflush(stdout);

  size_t bsize = nelem*HPX_LOCALITIES*sizeof(int);

  // create a global cyclic array
  hpx_addr_t cyclic = hpx_gas_calloc_cyclic(HPX_LOCALITIES, bsize, 0);
  int e = _kernel(cyclic, bsize, iters, work, nelem);
  assert(e == HPX_SUCCESS);

  // create a global randomly distributed array
  hpx_addr_t random = hpx_gas_calloc(HPX_LOCALITIES, bsize, 0,
                                     _random_dist, HPX_GAS_ATTR_LB);
  e = _kernel(random, bsize, iters, work, nelem);
  assert(e == HPX_SUCCESS);

  hpx_gas_free(cyclic, HPX_NULL);
  hpx_gas_free(random, HPX_NULL);

  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _main, _main_action,
                  HPX_INT, HPX_INT, HPX_INT);

int main(int argc, char *argv[]) {
  srand(time(NULL));
  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return e;
  }

  const libhpx_config_t *cfg = libhpx_get_config();
  if (cfg->gas != HPX_GAS_AGAS) {
    fprintf(stderr, "error: re-run with --hpx-gas=agas.\n");
    hpx_finalize();
    return e;
  }

  int iters = 4;
  int work = 4;
  int nelem = 100;
  int opt = 0;
  while ((opt = getopt(argc, argv, "i:w:n:h?")) != -1) {
    switch (opt) {
     case 'i':
       iters = atoi(optarg);
       break;
     case 'w':
       work = atoi(optarg);
       break;
      case 'n':
       nelem = atoi(optarg);
       break;
     case 'h':
       _usage(stdout, EXIT_SUCCESS);
     default:
       _usage(stderr, EXIT_FAILURE);
    }
  }

  argc -= optind;
  argv += optind;

  e = hpx_run(&_main, NULL, &iters, &work, &nelem);
  hpx_finalize();
  return e;
}

