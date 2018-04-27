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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "tests.h"

static int _check_null_handler(void) {
  static volatile int *null = NULL;
  hpx_thread_sigmask(HPX_SIG_BLOCK, HPX_SIGSEGV);
  hpx_thread_yield();
  return HPX_THREAD_CONTINUE(*null);
}
static HPX_ACTION(HPX_DEFAULT, 0, _check_null, _check_null_handler);

static int _test_thread_sigmask_handler(void) {
  int out;
  CHECK(hpx_call_sync(HPX_HERE, _check_null, &out, sizeof(out)));
  int mask = hpx_thread_sigmask(HPX_SIG_BLOCK, HPX_SIGNONE);
  test_assert((mask & HPX_SIGSEGV) == 0);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_thread_sigmask,
                  _test_thread_sigmask_handler);

TEST_MAIN({
  ADD_TEST(_test_thread_sigmask, 0);
  });
