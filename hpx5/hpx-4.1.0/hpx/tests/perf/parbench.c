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

/// This is a microbenchmark to determine the effectiveness of parallel
/// execution of tasks.
///
/// The users can control two metrics of parallelism: work and
/// span. Work is the total time it takes to execute all of the tasks
/// sequentially. Span is the time to execute the critical path of the
/// task DAG, given an infinite number of processors. These metrics
/// allow quantifying parallelism in a given task DAG and measure the
/// effectiveness of its execution.
///
/// For work, the fixed work quantum (FWQ) microbenchmark is
/// used. The microbenchmark takes as input three options:
///   1.   Iterations (I): Number of times to run the microbenchmark
///   2.         Work (W): The amount of work per task (that acts as
///                        input to FWQ).
///   3. No. of Tasks (N): The number of tasks to execute in parallel
///                        each performing work W.
///
/// The benchmark is weakly-scaled in that that N tasks are spawned to
/// perform work W. The work is fixed to equal the span, such that the
/// task DAG always forms an n-ary tree with depth 1. The parallel
/// efficiency of the generated DAG is 1.0 where T_{1} = T_{n} =
/// T_{\inf}.


int fwq(int work) {
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

static HPX_ACTION_DECL(_fwq);
int _fwq_action(int *work, size_t size) {
  return fwq(*work);
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _fwq, _fwq_action,
                  HPX_POINTER, HPX_SIZE_T);

static void env_to_args(void *out, const int i, const void *env) {
  int *args = out;
  *args = *(int*)env;
}

int _fwq_parfor(const int idx, void *work) {
  return fwq(*(int*)work);
}

static void _usage(FILE *f, int error) {
  fprintf(f, "Usage: parbench -i iters -w work -n tasks\n"
             "\t -i iters: number of iterations\n"
             "\t -w  work: work per task per iteration\n"
             "\t -n tasks: number of parallel tasks per iteration\n"
             "\t -h      : show help\n");
  hpx_print_help();
  fflush(f);
  exit(error);
}

static HPX_ACTION_DECL(_main);
static int _main_action(int iters, int work, int ntasks) {
  if (ntasks == 0) {
    ntasks = HPX_THREADS;
  }

  printf("parbench(iters=%d, work=%d, ntasks=%d)\n", iters, work, ntasks);
  printf("time resolution: microseconds\n");
  fflush(stdout);

  hpx_time_t start = hpx_time_now();
  for (int i = 0; i < iters; ++i) {
    fwq(work);
  }
  double elapsed = hpx_time_elapsed_us(start);
  printf("seq-for: %.7f\n", elapsed/iters);
  printf("perfect-for: %.7f\n", (ntasks*elapsed)/(iters*HPX_THREADS));

  start = hpx_time_now();
  for (int i = 0; i < iters; ++i) {
    hpx_addr_t done = hpx_lco_and_new(ntasks);
    for (int j = 0; j < ntasks; ++j) {
      hpx_call_async(HPX_HERE, _fwq, HPX_NULL, done, &work, sizeof(work));
    }
    hpx_lco_wait(done);
    hpx_lco_delete(done, HPX_NULL);
  }
  elapsed = hpx_time_elapsed_us(start);
  printf("for+hpx_call_async: %.7f\n", elapsed/iters);

  start = hpx_time_now();
  for (int i = 0; i < iters; ++i) {
    hpx_par_for_sync(_fwq_parfor, 0, ntasks, &work);
  }
  elapsed = hpx_time_elapsed_us(start);
  printf("hpx_par_for_sync: %.7f\n", elapsed/iters);

  start = hpx_time_now();
  for (int i = 0; i < iters; ++i) {
    hpx_par_call_sync(_fwq, 0, ntasks, ntasks, 1, sizeof(work),
                      env_to_args, sizeof(work), &work);
  }
  elapsed = hpx_time_elapsed_us(start);
  printf("hpx_par_call_sync: %.7f\n", elapsed/iters);

  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _main, _main_action, HPX_INT, HPX_INT, HPX_INT);

int main(int argc, char *argv[]) {
  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return e;
  }

  int iters = 5;
  int work = 5555;
  int ntasks = 0;
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
       ntasks = atoi(optarg);
       break;
     case 'h':
       _usage(stdout, EXIT_SUCCESS);
     default:
       _usage(stderr, EXIT_FAILURE);
    }
  }

  argc -= optind;
  argv += optind;

  e = hpx_run(&_main, NULL, &iters, &work, &ntasks);
  hpx_finalize();
  return e;
}

