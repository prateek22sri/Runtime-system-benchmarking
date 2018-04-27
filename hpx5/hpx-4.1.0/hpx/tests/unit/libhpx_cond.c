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
#include "libhpx/libhpx.h"
#include "hpx/hpx.h"

const int I = 10;                             // number of threads
const int N = 1000;                           // number of iterations per thread

libhpx_cond_t empty;
libhpx_cond_t full;
libhpx_mutex_t mutex;
int n;

int producer_handler(int N) {
  for (int i = 0; i < N; ++i) {
    CHECK(libhpx_mutex_lock(&mutex));
    while (n) {
      CHECK(libhpx_cond_wait(&empty, &mutex));
    }
    n = 1;
    CHECK(libhpx_cond_broadcast(&full));
    CHECK(libhpx_mutex_unlock(&mutex));
  }
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, producer, producer_handler, HPX_INT);

int consumer_handler(int N) {
  for (int i = 0; i < N; ++i) {
    CHECK(libhpx_mutex_lock(&mutex));
    while (!n) {
      CHECK(libhpx_cond_wait(&full, &mutex));
    }
    n = 0;
    CHECK(libhpx_cond_broadcast(&empty));
    CHECK(libhpx_mutex_unlock(&mutex));
  }
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, consumer, consumer_handler, HPX_INT);

int libhpx_cond_handler(void) {
  n = 0;
  CHECK(libhpx_cond_init(&empty));
  CHECK(libhpx_cond_init(&full));
  CHECK(libhpx_mutex_init(&mutex));
  hpx_addr_t complete = hpx_lco_and_new(2*I);
  test_assert(complete != HPX_NULL);
  for (int i = 0; i < I; ++i) {
    CHECK(hpx_call(HPX_HERE, consumer, complete, &N));
    CHECK(hpx_call(HPX_HERE, producer, complete, &N));
  }
  CHECK(hpx_lco_wait(complete));
  hpx_lco_delete_sync(complete);
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, 0, libhpx_cond, libhpx_cond_handler);

TEST_MAIN({
 ADD_TEST(libhpx_cond, 0);
});
