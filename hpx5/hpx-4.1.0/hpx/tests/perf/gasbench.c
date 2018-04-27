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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hpx/hpx.h>

static __thread hpx_addr_t *addrs = NULL;

static void _usage(FILE *f, int error) {
  fprintf(f, "Usage: gasbench -i iters -s size\n"
             "\t -i iters: number of iterations\n"
             "\t -s size: size of GAS objects to allocate\n"
             "\t -h      : show help\n");
  hpx_print_help();
  fflush(f);
  exit(error);
}

typedef struct _alloc_args {
  int iters;
  int size;
  hpx_addr_t (*fn)(size_t, size_t, uint32_t);
} _alloc_args_t;

int _alloc_free(const int tid, void *a) {
  _alloc_args_t *args = (_alloc_args_t*)a;
  int iters = args->iters;
  size_t size = args->size;
  for (int i = 0; i < iters; ++i) {
    hpx_addr_t addr = args->fn(1, size, 0);
    hpx_gas_free(addr, HPX_NULL);
  }
  return HPX_SUCCESS;
}

int _allocs(const int tid, void *a) {
  _alloc_args_t *args = (_alloc_args_t*)a;
  int iters = args->iters;
  size_t size = args->size;
  addrs = malloc(sizeof(*addrs)*iters);
  assert(addrs);
  for (int i = 0; i < iters; ++i) {
    addrs[i] = args->fn(1, size, 0);
  }
  return HPX_SUCCESS;
}

int _frees(const int tid, void *a) {
  _alloc_args_t *args = (_alloc_args_t*)a;
  assert(addrs);
  for (int i = 0; i < args->iters; ++i) {
    hpx_gas_free(addrs[i], HPX_NULL);
  }
  free(addrs);
  addrs = NULL;
  return HPX_SUCCESS;
}

static void _run(void *f, _alloc_args_t *args, char *fmt) {
  int iters = args->iters;
  hpx_time_t start = hpx_time_now();
  hpx_par_for_sync(f, 0, HPX_THREADS, args);
  double elapsed = hpx_time_elapsed_us(start);
  printf("%s: %.7f\n", fmt, elapsed/iters);
}

static int _main_action(int iters, size_t size) {
  printf("gasbench(iters=%d, size=%zu, threads=%d)\n",
         iters, size, HPX_THREADS);
  printf("time resolution: microseconds\n");
  fflush(stdout);

  _alloc_args_t args = { .iters = iters, .size = size, .fn = NULL };
  args.fn = hpx_gas_alloc_local;
  _run(_alloc_free, &args, "alloc+free");
  _run(_allocs, &args, "alloc");
  _run(_frees, &args, "free");

  args.fn = hpx_gas_calloc_local;
  _run(_alloc_free, &args, "calloc+free");
  _run(_allocs, &args, "calloc");
  _run(_frees, &args, "free");

  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _main, _main_action, HPX_INT, HPX_SIZE_T);

int main(int argc, char *argv[]) {
  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return e;
  }

  int iters = 5;
  size_t size = 8192;
  int opt = 0;
    while ((opt = getopt(argc, argv, "i:s:h?")) != -1) {
    switch (opt) {
     case 'i':
       iters = atoi(optarg);
       break;
     case 's':
       size = atol(optarg);
       break;
     case 'h':
       _usage(stdout, EXIT_SUCCESS);
     default:
       _usage(stderr, EXIT_FAILURE);
    }
  }

  argc -= optind;
  argv += optind;

  e = hpx_run(&_main, NULL, &iters, &size);
  hpx_finalize();
  return e;
}
