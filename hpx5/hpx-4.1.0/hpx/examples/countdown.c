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
#include <unistd.h>
#include "hpx/hpx.h"

static __thread unsigned seed = 0;

static hpx_addr_t rand_rank(void) {
  int r = rand_r(&seed);
  int n = hpx_get_num_ranks();
  return HPX_THERE(r % n);
}

static hpx_action_t send = 0;

static int _send_action(void *args, size_t size) {
  int n = *(int*)args;
  printf("locality: %d, thread: %d, count: %d\n", hpx_get_my_rank(),
         hpx_get_my_thread_id(), n);

  if (n-- <= 0) {
    printf("terminating.\n");
    hpx_exit(0, NULL);
  }

  hpx_parcel_t *p = hpx_parcel_acquire(NULL,sizeof(int));
  hpx_parcel_set_target(p, rand_rank());
  hpx_parcel_set_action(p, send);
  hpx_parcel_set_data(p, &n, sizeof(n));
  hpx_parcel_send_sync(p);
  return HPX_SUCCESS;
}

static void usage(FILE *f, int error) {
  fprintf(f, "Usage: countdown [options] ROUNDS \n"
          "\t-h, show help\n");
  hpx_print_help();
  fflush(f);
  exit(error);
}

int main(int argc, char * argv[argc]) {
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, send, _send_action, HPX_POINTER, HPX_SIZE_T);

  if (hpx_init(&argc, &argv)) {
    fprintf(stderr, "HPX failed to initialize.\n");
    return -1;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "h?")) != -1) {
    switch (opt) {
     case 'h':
      usage(stdout, EXIT_SUCCESS);
     case '?':
     default:
      usage(stderr, EXIT_FAILURE);
    }
  }

  argc -= optind;
  argv += optind;

  int n = 0;
  switch (argc) {
   default:
     usage(stderr, EXIT_FAILURE);
   case (1):
     n = atoi(argv[0]);
     break;
  }

  int e = hpx_run(&send, NULL, &n, sizeof(n));
  hpx_finalize();
  return e;
}
