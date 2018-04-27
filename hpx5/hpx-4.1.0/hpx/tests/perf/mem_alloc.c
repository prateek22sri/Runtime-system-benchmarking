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

#define MAX_BYTES        (1024*1024*1024*1UL)
#define SKIP_LARGE       10
#define LOOP_LARGE       100

int skip = 1000;
int loop = 10000;

#define BENCHMARK "HPX COST OF MEMORY ALLOCATIONS (ms)"

#define HEADER "# " BENCHMARK "\n"
#define FIELD_WIDTH 10
#define HEADER_FIELD_WIDTH 5
/// This file tests cost of memory allocation operations
static void usage(FILE *stream) {
  fprintf(stream, "Usage: mem_alloc [options]\n"
          "\t-h, this help display\n");
  hpx_print_help();
  fflush(stream);
}

static hpx_action_t _main    = 0;

static int _main_action(void) {
  void *local;
  hpx_time_t t;
  int size = HPX_LOCALITIES;
  int ranks = hpx_get_num_ranks();
  uint32_t blocks = size;

  fprintf(stdout, HEADER);
  fprintf(stdout, "localities: %d, ranks and blocks per rank = %d, %d\n",
                  size, ranks, blocks/ranks);

  fprintf(stdout, "%s\t\t%*s%*s%*s%*s\n", "# Size ", HEADER_FIELD_WIDTH,
      " MALLOC ", HEADER_FIELD_WIDTH, " FREE ", HEADER_FIELD_WIDTH,
      " MALLOC_REG ", HEADER_FIELD_WIDTH, " FREE ");

  for (size_t size = 1; size <= MAX_BYTES; size*=2) {
    t = hpx_time_now();
    local = malloc(size);
    fprintf(stdout, "%-*zu%*g", 10,  size, FIELD_WIDTH, hpx_time_elapsed_ms(t));

    t = hpx_time_now();
    free(local);
    fprintf(stdout, "%*g", FIELD_WIDTH, hpx_time_elapsed_ms(t));

    t = hpx_time_now();
    local = hpx_malloc_registered(size);
    fprintf(stdout, "%*g", FIELD_WIDTH, hpx_time_elapsed_ms(t));

    t = hpx_time_now();
    hpx_free_registered(local);
    fprintf(stdout, "%*g", FIELD_WIDTH, hpx_time_elapsed_ms(t));
    fprintf(stdout, "\n");
  }
  hpx_exit(0, NULL);
}

int main(int argc, char *argv[]) {
  // Register the main action
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _main, _main_action);

  if (hpx_init(&argc, &argv)) {
    fprintf(stderr, "HPX: failed to initialize.\n");
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

  fprintf(stdout, "Starting the cost of Memory Allocation benchmark\n");

  // run the main action
  int e = hpx_run(&_main, NULL);
  hpx_finalize();
  return e;
}
