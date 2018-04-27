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

#ifndef LIBHPX_TESTS_H_
#define LIBHPX_TESTS_H_

#include <assert.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <hpx/hpx.h>

#ifdef NDEBUG
# define test_assert(e) do {                        \
    if (!(e)) {                                     \
      fprintf(stderr, "assert failed '%s'\n", #e);  \
      exit(EXIT_FAILURE);                           \
    }                                               \
  } while (0)
#else
# define test_assert(e) assert(e)
#endif

#define assert_msg(cond, msg) test_assert(cond && msg)
#define test_assert_msg(cond, msg) test_assert(cond && msg)
#define CHECK(e) test_assert(e == HPX_SUCCESS)

#define ADD_TEST(test, at) do {                             \
    printf("====== TEST %s at %d ======\n", #test, HPX_LOCALITY_ID);    \
    int e = hpx_call_sync(HPX_THERE(at), test, NULL, 0);    \
    if (e == HPX_SUCCESS) {                                 \
      printf("======== PASSED ========\n");                 \
    } else {                                                \
      printf("======== FAILED ========\n");                 \
      exit(EXIT_FAILURE);                                   \
    }                                                       \
  } while (0)

// A helper macro to generate a main function template for the test.
#define TEST_MAIN(tests)                                \
  static int _main_handler(void) {                      \
    tests                                               \
    hpx_exit(0, NULL);                                  \
    return HPX_SUCCESS;                                 \
  }                                                     \
  static HPX_ACTION(HPX_DEFAULT, 0, _main,              \
                    _main_handler);                     \
  static void usage(char *prog, FILE *f) {              \
    fprintf(f, "Usage: %s [options]\n"                  \
            "\t-h, show help\n", prog);                 \
    hpx_print_help();                                   \
    fflush(f);                                          \
  }                                                     \
  int main(int argc, char *argv[]) {                    \
    if (hpx_init(&argc, &argv)) {                       \
      fprintf(stderr, "failed to initialize HPX.\n");   \
      return 1;                                         \
    }                                                   \
    int opt = 0;                                        \
    while ((opt = getopt(argc, argv, "h?")) != -1) {    \
      switch (opt) {                                    \
       case 'h':                                        \
        usage(argv[0], stdout);                         \
        return 0;                                       \
       case '?':                                        \
       default:                                         \
        usage(argv[0], stderr);                         \
        return -1;                                      \
      }                                                 \
    }                                                   \
    int ret_status = hpx_run(&_main, NULL);             \
    hpx_finalize();                                     \
    return ret_status;                                  \
  }                                                     \
  int main(int argc, char *argv[])



#endif /* LIBHPX_TESTS_H_ */
