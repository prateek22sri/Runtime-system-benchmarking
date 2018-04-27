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

#define BENCHMARK "HPX COST OF LCO Future"

#define HEADER "# " BENCHMARK "\n"
#define FIELD_WIDTH 10
#define HEADER_FIELD_WIDTH 5

static void _usage(FILE *stream) {
  fprintf(stream, "Usage: time_lco_future [options] \n"
          "\t-h, this help display\n");
  hpx_print_help();
  fflush(stream);
}

static hpx_action_t _main = 0;
static hpx_action_t _set_value = 0;
static hpx_action_t _get_value = 0;

#define T int

static T value;

static int num_readers[]  ={
  1,
  4,
  8,
  32,
  64,
 128,
 192
};

static int action_get_value(void) {
  return HPX_THREAD_CONTINUE(value);
}

static int action_set_value(void *args, size_t size) {
  value = *(T*)args;
  return HPX_SUCCESS;
}

static int _main_action(void) {
  hpx_time_t t;
  int count;

  fprintf(stdout, HEADER);
  fprintf(stdout, "# Latency in (ms)\n");

  t = hpx_time_now();
  hpx_addr_t done = hpx_lco_future_new(0);
  fprintf(stdout, "Creation time: %g\n", hpx_time_elapsed_ms(t));

  value = 1234;

  t = hpx_time_now();
  hpx_call(HPX_HERE, _set_value, done, &value, sizeof(value));
  fprintf(stdout, "Value set time: %g\n", hpx_time_elapsed_ms(t));

  t = hpx_time_now();
  hpx_lco_wait(done);
  fprintf(stdout, "Wait time: %g\n", hpx_time_elapsed_ms(t));

  t = hpx_time_now();
  hpx_lco_delete(done, HPX_NULL);
  fprintf(stdout, "Deletion time: %g\n", hpx_time_elapsed_ms(t));

  fprintf(stdout, "%s\t%*s%*s%*s\n", "# NumReaders " , FIELD_WIDTH,
         "Get_Value ", FIELD_WIDTH, " LCO_Getall ", FIELD_WIDTH, "Delete");

  for (int i = 0; i < sizeof(num_readers)/sizeof(num_readers[0]); i++) {
    fprintf(stdout, "%d\t\t", num_readers[i]);
    count = num_readers[i];
    int values[count];
    void *addrs[count];
    size_t sizes[count];
    hpx_addr_t futures[count];

    for (int j = 0; j < count; j++) {
      addrs[j] = &values[j];
      sizes[j] = sizeof(int);
      futures[j] = hpx_lco_future_new(sizeof(int));
    }

    t = hpx_time_now();
    for (int j = 0; j < count; j++) {
      t = hpx_time_now();
      hpx_call(HPX_HERE, _get_value, futures[j], NULL, 0);
      hpx_lco_wait(futures[j]);
    }
    fprintf(stdout, "%*g", FIELD_WIDTH, hpx_time_elapsed_ms(t));

    t = hpx_time_now();
    hpx_lco_get_all(count, futures, sizes, addrs, NULL);
    fprintf(stdout, "%*g", FIELD_WIDTH, hpx_time_elapsed_ms(t));

    t = hpx_time_now();
    for (int j = 0; j < count; j++)
      hpx_lco_delete(futures[j], HPX_NULL);
    fprintf(stdout, "%*g\n", FIELD_WIDTH, hpx_time_elapsed_ms(t));
  }
  hpx_exit(0, NULL);
}

int main(int argc, char *argv[]) {
  // register the actions
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _main, _main_action);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _set_value, action_set_value, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _get_value, action_get_value);

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
