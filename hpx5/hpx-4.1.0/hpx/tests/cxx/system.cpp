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
#include "config.h"
#endif

#include <hpx/hpx++.h>
#include <iostream>

namespace {
int echo_handler(char *message, size_t size) {
  std::cout << message << " from " << HPX_LOCALITY_ID << "\n";
  return hpx::SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, echo, echo_handler, HPX_POINTER,
           HPX_SIZE_T);

int test_handler(void) {
  const char message[] = {'h', 'e', 'l', 'l', 'o', '\0'};
  hpx::system::broadcast(echo, message, sizeof(message));
  hpx::exit();
}
auto test = hpx::make_action(test_handler);
}

int main(int argc, char *argv[]) {
  if (int e = hpx::init(&argc, &argv)) {
    return e;
  }

  if (int e = hpx::run(test)) {
    return e;
  }

  hpx::finalize();
  return 0;
}
