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
#include "libhpx/boot/Network.h"
#include "libhpx/locality.h"
#include "hpx/hpx.h"
#include <mutex>

using BootNetwork = libhpx::boot::Network;

#define FAIL(dst, ...) do {                                 \
    fprintf(stderr, __VA_ARGS__);                           \
    exit(EXIT_FAILURE);                                     \
  } while (0)

static int alltoall_handler(const BootNetwork& boot) {
  printf("Entering alltoall_handler at %d\n", HPX_LOCALITY_ID);
  const int NLOC = HPX_LOCALITIES;
  int src[NLOC][2];
  int dst[NLOC][2];

  int base = here->rank * here->ranks;
  for (int i = 0; i < NLOC; ++i) {
    src[i][0] = base + i;
    src[i][1] = here->rank;
    dst[i][0] = here->rank;
    dst[i][1] = here->rank;
  }

  boot.barrier();
  static std::mutex lock;
  {
    std::lock_guard<std::mutex> _(lock);
    printf("src@%d { ", here->rank);
    for (int i = 0; i < NLOC; ++i) {
      printf("{%d,%d} ", src[i][0], src[i][1]);
    }
    printf("}\n");
    fflush(stdout);
  }

  boot.barrier();
  boot.alltoall(dst, src, 1*sizeof(int), 2*sizeof(int));

  {
    std::lock_guard<std::mutex> _(lock);
    printf("dst@%d { ", here->rank);
    for (int i = 0; i < NLOC; ++i) {
      printf("{%d,%d} ", dst[i][0], dst[i][1]);
    }
    printf("}\n");
    fflush(stdout);
  }
  boot.barrier();

  for (int i = 0; i < NLOC; ++i) {
    if (dst[i][0] != int(i * NLOC + here->rank)) {
      FAIL(dst, "%d:dst[%d][0]=%d, expected %d\n", here->rank, i, dst[i][0], i * NLOC + here->rank);
    }
    if (dst[i][1] != int(here->rank)) {
      FAIL(dst, "%d:dst[%d][1]=%d, expected %d\n", here->rank, i, dst[i][1], here->rank);
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (hpx_init(&argc, &argv)) {
    fprintf(stderr, "failed to initialize HPX.\n");
    return 1;
  }
  int e = alltoall_handler(*here->boot);
  hpx_finalize();
  return e;
}
