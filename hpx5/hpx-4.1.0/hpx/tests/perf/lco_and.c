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

#define BENCHMARK "HPX COST OF LCO AND"

#define HEADER "# " BENCHMARK "\n"
#define FIELD_WIDTH 10
#define HEADER_FIELD_WIDTH 5

static int num[] = {
  10000,
  20000,
  30000,
  40000,
  50000
};

static void _usage(FILE *stream) {
  fprintf(stream, "Usage: time_lco_and overhead \n"
          "\t-h, this help display\n");
  hpx_print_help();
  fflush(stream);
}

static hpx_action_t _lco_set  = 0;
static hpx_action_t _main = 0;
static hpx_action_t _empty = 0;


static int _lco_set_action(hpx_addr_t *args, size_t size) {
  hpx_lco_and_set(*args, HPX_NULL);
  return HPX_SUCCESS;
}

static int _empty_action(hpx_addr_t *args, size_t size) {
  return HPX_SUCCESS;
}

static int _main_action(void) {
  printf(HEADER);
  printf("# Latency in (ms)\n");
  printf("%s%*s%*s%*s\n", "# Iters " , FIELD_WIDTH, "Init time ",
          FIELD_WIDTH, "LCO Set", FIELD_WIDTH, "Delete");

  for (int i = 0; i < sizeof(num)/sizeof(num[0]) ; i++) {
    printf("%d", num[i]);


    hpx_time_t t = hpx_time_now();
    hpx_addr_t setlco = hpx_lco_and_new(num[i]);
    printf("%*g", FIELD_WIDTH, hpx_time_elapsed_ms(t));

    // Time it takes to set empty action
    hpx_addr_t completed = hpx_lco_and_new(num[i]);
    hpx_time_t t1 = hpx_time_now();
    for (int j = 0; j < num[i]; j++)
      hpx_call(HPX_HERE, _empty, completed, NULL, 0);
    hpx_lco_wait(completed);
    double empty_t = hpx_time_elapsed_ms(t1);
    hpx_lco_delete(completed, HPX_NULL);

    // Time to set for LCO argument
    t = hpx_time_now();
    hpx_addr_t done = hpx_lco_and_new(num[i]);
    for (int j = 0; j < num[i]; j++)
      hpx_call(HPX_HERE, _lco_set, done, &setlco, sizeof(setlco));
    hpx_lco_wait(setlco);
    double end_t = hpx_time_elapsed_ms(t);
    printf("%*g",FIELD_WIDTH, end_t - empty_t);

    t = hpx_time_now();
    hpx_lco_delete(setlco, HPX_NULL);
    printf("%*g\n",FIELD_WIDTH, hpx_time_elapsed_ms(t));

    hpx_lco_wait(done);
    hpx_lco_delete(done, HPX_NULL);
  }

  hpx_exit(0, NULL);
}

int main(int argc, char *argv[]) {
  // register the actions
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _lco_set, _lco_set_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _main, _main_action);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _empty, _empty_action, HPX_POINTER, HPX_SIZE_T);

  if (hpx_init(&argc, &argv)) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return 1;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "h?")) != -1) {
    switch (opt) {
     case 'h':
      _usage(stdout);
      return 0;
     case '?':
     default:
      _usage(stderr);
      return -1;
    }
  }

  // run the main action
  int e = hpx_run(&_main, NULL);
  hpx_finalize();
  return e;
}
