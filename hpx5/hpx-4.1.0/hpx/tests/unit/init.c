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

int main(int argc, char *argv[argc]) {
  if (hpx_init(NULL, NULL) != 0) {
    fprintf(stderr, "failed to initialize HPX.\n");
    return -1;
  }

  hpx_finalize();
  printf("hpx_finalize completed %d.\n", 1);
  return 0;
}
