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

static void _usage(FILE *stream) {
  fprintf(stream, "Usage: time_gas_addr_trans [options]\n"
          "\t-h, this help display\n");
  hpx_print_help();
  fflush(stream);
}

static hpx_action_t _address_translation = 0;
static hpx_action_t _main = 0;

#define BENCHMARK "HPX COST OF GAS ADDRESS TRANSLATION (ms)"

#define HEADER "# " BENCHMARK "\n"

#define TEST_BUF_SIZE (1024*64) //64k
#define FIELD_WIDTH 20

static int num[] = {
  10000,
  20000,
  30000,
  40000,
  50000
};

static int _address_translation_action(void) {
  hpx_addr_t local = hpx_thread_current_target();

  // The pinned local address
  int *data = NULL;

  // Performs address translation. This will try to perform a global-to-local
  // translation on the global addr, and set local to the local address if it
  // it is successful.
  if (!hpx_gas_try_pin(local, (void **)&data))
    return HPX_RESEND;

  // make sure to unpin the address
  hpx_gas_unpin(local);
  return hpx_thread_continue(NULL, 0);
}

static int _main_action(void) {
  hpx_time_t now;
  double elapsed;
  int size = HPX_LOCALITIES;
  int ranks = hpx_get_num_ranks();
  uint32_t blocks = size;

  fprintf(stdout, HEADER);
  fprintf(stdout, "localities: %d, ranks and blocks per rank = %d, %d\n",
                  size, ranks, blocks/ranks);
  fprintf(stdout, "%s%*s%*s%*s\n", "# Num threads ", FIELD_WIDTH,
          "GAS ALLOC", FIELD_WIDTH, "GLOBAL_ALLOC", FIELD_WIDTH,
          "GLOBAL_CALLOC");

  for (int i = 0; i < sizeof(num)/sizeof(num[0]); i++) {
    fprintf(stdout, "%d", num[i]);

    hpx_addr_t local = hpx_gas_alloc_local(1, TEST_BUF_SIZE, 0);
    hpx_addr_t completed = hpx_lco_and_new(num[i]);
    now = hpx_time_now();
    for (int j = 0; j < num[i]; j++)
      hpx_call(local, _address_translation, completed, 0, 0);
    elapsed = hpx_time_elapsed_ms(now)/1e3;
    hpx_lco_wait(completed);
    fprintf(stdout, "%*.7f", FIELD_WIDTH,  elapsed);
    hpx_lco_delete(completed, HPX_NULL);
    hpx_gas_free(local, HPX_NULL);

    hpx_addr_t global = hpx_gas_alloc_cyclic(blocks, TEST_BUF_SIZE, 0);
    hpx_addr_t done = hpx_lco_and_new(num[i]);
    now = hpx_time_now();
    for (int j = 0; j < num[i]; j++)
      hpx_call(global, _address_translation, done, 0, 0);
    elapsed = hpx_time_elapsed_ms(now)/1e3;
    hpx_lco_wait(done);
    fprintf(stdout, "%*.7f", FIELD_WIDTH,  elapsed);
    hpx_lco_delete(done, HPX_NULL);
    hpx_gas_free(global, HPX_NULL);

    hpx_addr_t callocMem = hpx_gas_calloc_cyclic(blocks, TEST_BUF_SIZE, 0);
    hpx_addr_t and = hpx_lco_and_new(num[i]);
    now = hpx_time_now();
    for (int j = 0; j < num[i]; j++)
      hpx_call(callocMem, _address_translation, and, 0, 0);
    elapsed = hpx_time_elapsed_ms(now)/1e3;
    hpx_lco_wait(and);
    fprintf(stdout, "%*.7f", FIELD_WIDTH,  elapsed);
    hpx_lco_delete(and, HPX_NULL);
    hpx_gas_free(callocMem, HPX_NULL);

    fprintf(stdout, "\n");
  }
  hpx_exit(0, NULL);
}

int
main(int argc, char *argv[]) {
  // register the actions
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _address_translation, _address_translation_action);
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _main, _main_action);

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
