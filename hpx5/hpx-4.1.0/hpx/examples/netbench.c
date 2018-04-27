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
#include <string.h>
#include <unistd.h>
#include <hpx/hpx.h>

#define DEFAULT_ITERS 100
hpx_action_t echo_pong;
hpx_action_t echo_finish;
unsigned long iterations;
unsigned long yield_iterations;

typedef struct {
  hpx_addr_t lco;
  int src;
  int dst;
  size_t size;
  char data[];
} echo_args_t;

static void _usage(FILE *stream) {
  fprintf(stream, "Usage: netbench [options] [ITERATIONS]\n"
      "\t-y, the number of iterations after which to yield\n"
          "\t-h, show help\n");
  hpx_print_help();
  fflush(stream);
}

void send_ping(hpx_addr_t lco, int src, int dst, size_t size) {
  hpx_parcel_t *p = hpx_parcel_acquire(NULL, size);
  echo_args_t *echo_args = hpx_parcel_get_data(p);
  echo_args->lco = lco;
  echo_args->src = src;
  echo_args->dst = dst;
  echo_args->size = size;
  hpx_parcel_set_action(p, echo_pong);
  hpx_parcel_set_target(p, HPX_THERE(dst));
  // here we use an asynchronous parcel send since we don't care about local completion;
  // the runtime gave us the buffer with the acquire, and we gave it back;
  // and it won't affect our timing because the local send completion isn't relevant
  hpx_parcel_send(p, HPX_NULL);
}

void send_pong(echo_args_t *args) {
  hpx_parcel_t *p = hpx_parcel_acquire(NULL, args->size);
  echo_args_t *return_args = hpx_parcel_get_data(p);
  // unfortunately, can't avoid some at least some copying
  // but we are only copying sizeof(echo_args_t) since this isn't a memory performance test
  memcpy(return_args, args, sizeof(echo_args_t));
  hpx_parcel_set_action(p, echo_finish);
  hpx_parcel_set_target(p, HPX_THERE(args->src));
  // here we use an asynchronous parcel send since we don't care about local completion;
  // the runtime gave us the buffer with the acquire, and we gave it back;
  // and it won't affect our timing because the local send completion isn't relevant
  hpx_parcel_send(p, HPX_NULL);
}

int echo_pong_action(echo_args_t *args, size_t size) {
  if (args->dst != hpx_get_my_rank())
    hpx_abort();
  send_pong(args);
  return HPX_SUCCESS;
}

int echo_finish_action(echo_args_t *args, size_t size) {
  //hpx_lco_gencount_inc(args->lco, HPX_NULL);
  hpx_lco_and_set(args->lco, HPX_NULL);
  return HPX_SUCCESS;
}

int hpx_main_action(void) {
  if (hpx_get_num_ranks() < 2) {
    printf("Too few ranks, need at least two.\n");
    hpx_exit(0, NULL);
  }

  size_t sizes[] = {1, 128, 1024, 4096, 8192, 64*1024, 256*1024, 1024*1024, 4*1024*1024};
  int num_sizes = sizeof(sizes)/sizeof(size_t);
  //  hpx_addr_t lco = hpx_lco_gencount_new(num_sizes);
  hpx_addr_t lco;
  printf("size\tS_time_ms\tavg_time_ms\tmsgs/s\tbytes/s\n");
  for (int i = 0; i < num_sizes; i++) {
    size_t actual_size = sizes[i];
    if (sizeof(echo_args_t) > sizes[i])
      actual_size = sizeof(echo_args_t);

    lco = hpx_lco_and_new(iterations);
    //lco = hpx_lco_gencount_new(iterations);
    hpx_time_t time_start = hpx_time_now();
    for (int j = 0; j < iterations; j++) {
      send_ping(lco, 0, 1, actual_size);
      if (yield_iterations > 0 && j % yield_iterations)
    hpx_thread_yield();
    }
    // hpx_lco_gencount_wait(lco, iterations);
    hpx_lco_wait(lco);
    hpx_time_t time_end = hpx_time_now();
    double time_in_ms = hpx_time_diff_ms(time_start, time_end);
    double avg_time_in_ms = time_in_ms/iterations;
    printf("%zu\t%.4g\t%.4g\t%.6g\t%.4g\n",
       actual_size,
       time_in_ms,
       avg_time_in_ms,
       (iterations*1.0)/time_in_ms*1000.0,
       (iterations*1.0)/time_in_ms*1000.0*actual_size);
    hpx_lco_delete(lco, HPX_NULL);
  }

  hpx_exit(0, NULL);
}



int main(int argc, char *argv[]) {
  yield_iterations = 0;

  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, echo_pong, echo_pong_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, echo_finish, echo_finish_action, HPX_POINTER, HPX_SIZE_T);
  hpx_action_t hpx_main;
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, hpx_main, hpx_main_action);

  if (hpx_init(&argc, &argv)) {
    fprintf(stderr, "HPX failed to initialize.\n");
    return -1;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "y:h?")) != -1) {
    switch (opt) {
      case 'y':
    yield_iterations = atol(optarg);
    break;
      case 'h':
        _usage(stdout);
        return 0;
      case '?':
      default:
        _usage(stderr);
        return -1;
    }
  }

  argc -= optind;
  argv += optind;

  iterations = 0;
  if (argc != 0)
    iterations = strtol(argv[0], NULL, 10);

  if (iterations == 0) {
    iterations = DEFAULT_ITERS;
    printf("read ITERATIONS as 0, setting them to default of %lu.\n", iterations);
  }

  int e = hpx_run(&hpx_main, NULL);
  hpx_finalize();
  return e;
}
