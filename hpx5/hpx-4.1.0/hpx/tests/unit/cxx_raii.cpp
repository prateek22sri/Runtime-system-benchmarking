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

#include <hpx/hpx.h>
#include "tests.h"

namespace {
int resource;

class RAII {
 public:
  RAII() {
    resource = 1;
  }
  ~RAII() {
    resource = 0;
  }
};

int _raii_continue_handler(void) {
  RAII raii;
  hpx_thread_continue();
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, _raii_continue, _raii_continue_handler);

int _raii_thread_exit_handler(void) {
  RAII raii;
  hpx_thread_exit(HPX_SUCCESS);
}
HPX_ACTION(HPX_DEFAULT, 0, _raii_thread_exit, _raii_thread_exit_handler);

int _verify_handler(void) {
  test_assert_msg(resource == 0, "Destructor was not run correctly\n");
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, _verify, _verify_handler);
} // namespace

TEST_MAIN({
    ADD_TEST(_raii_continue, 0);
    ADD_TEST(_verify, 0);
    ADD_TEST(_raii_thread_exit, 0);
    ADD_TEST(_verify, 0);
  });
