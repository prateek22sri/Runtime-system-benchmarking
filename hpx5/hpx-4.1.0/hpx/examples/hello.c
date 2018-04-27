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
#include <hpx/hpx.h>

static HPX_ACTION_DECL(_hello);
static int _hello_action(void) {
  printf("Hello World from %u.\n", hpx_get_my_rank());
  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _hello, _hello_action);

int main(int argc, char *argv[argc]) {
  if (hpx_init(&argc, &argv) != 0)
    return -1;
  int e = hpx_run(&_hello, NULL);
  hpx_finalize();
  return e;
}
