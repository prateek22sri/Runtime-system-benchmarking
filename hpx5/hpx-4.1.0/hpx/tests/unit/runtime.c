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

#include "hpx/hpx.h"
#include "tests.h"

#define RUNS 10
#define ACTIONS_PER_RUN 10

int _foo_action_handler(void) {
  printf("Foo action : %u : ranks : %u : threads : %u.\n",
         hpx_get_my_rank(), hpx_get_num_ranks(), hpx_get_num_threads());
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _foo, _foo_action_handler);

static int _inner(int iteration) {
  printf("Hello World from main action rank : %u : ranks : %u : "
         "threads : %u : iteration : %d.\n",
         hpx_get_my_rank(), hpx_get_num_ranks(), hpx_get_num_threads(),
         iteration);

  hpx_addr_t done = hpx_lco_and_new(ACTIONS_PER_RUN);
  for (int i = 0; i < ACTIONS_PER_RUN; i++) {
    int loc = i % hpx_get_num_ranks();
    hpx_call(HPX_THERE(loc), _foo, done);
  }
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);

  return (iteration & 1) ? HPX_SUCCESS : -42;
}

static int _diffusion_handler(int iteration) {
  int status = _inner(iteration);
  hpx_exit(sizeof(status), &status);
}
HPX_ACTION(HPX_DEFAULT, 0, _diffusion, _diffusion_handler, HPX_INT);

static int _diffusion_return_handler(int *out, size_t bytes) {
  for (int i = 0, e = bytes / sizeof(out[0]); i < e; ++i) {
    out[i] += 3;
  }
  hpx_exit(bytes, out);
}
HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _diffusion_return,
           _diffusion_return_handler, HPX_POINTER, HPX_SIZE_T);

static int _spmd_return_handler(int out) {
  out += HPX_LOCALITY_ID;
  hpx_exit(sizeof(out), &out);
}
HPX_ACTION(HPX_DEFAULT, 0, _spmd_return, _spmd_return_handler, HPX_INT);

static int _spmd_handler(int iteration) {
  printf("spmd at %d\n", HPX_LOCALITY_ID);
  int status = _inner(iteration);
  hpx_exit(sizeof(status), &status);
}
HPX_ACTION(HPX_DEFAULT, 0, _spmd, _spmd_handler, HPX_INT);

int main(int argc, char *argv[argc]) {
  if (hpx_initialized()) {
    fprintf(stderr, "HPX claims to be initialized before hpx_init.\n");
    return -1;
  }

  if (hpx_init(&argc, &argv) != 0) {
    fprintf(stderr, "failed to initialize HPX.\n");
    return -1;
  }

  if (!hpx_initialized()) {
    fprintf(stderr, "HPX claims not to be initialized after hpx_init.\n");
    return -1;
  }

  int status;
  for (int i = 0; i < RUNS; ++i) {
    int success = hpx_run(&_diffusion, &status, &i);
    assert(success == HPX_SUCCESS);
    printf("%i hpx_run returned %d.\n", i+1, status);
  }

  for (int i = 0; i < RUNS; ++i) {
    int success = hpx_run_spmd(&_spmd, &status, &i);
    assert(success == HPX_SUCCESS);
    printf("%i hpx_run_spmd returned %d.\n", i+1, status);
  }

  {
    int input[] = { 0, 1, 2, 3 };
    int success = hpx_run(&_diffusion_return, input, input, sizeof(input));
    assert(success == HPX_SUCCESS);
    for (int i = 0, e = 4; i < e; ++i) {
      if (input[i] != i + 3) {
        abort();
      }
    }
  }

  {
    int rank = HPX_LOCALITY_ID;
    int success = hpx_run_spmd(&_spmd_return, &rank, &rank);
    assert(success == HPX_SUCCESS);
    if (rank != 2 * HPX_LOCALITY_ID) {
      abort();
    }
  }

  hpx_finalize();
  printf("hpx_finalize completed %d.\n", 1);

  if (hpx_initialized()) {
    fprintf(stderr, "HPX claims to be initialized after hpx_finalize.\n");
    return -1;
  }

  return 0;
}
