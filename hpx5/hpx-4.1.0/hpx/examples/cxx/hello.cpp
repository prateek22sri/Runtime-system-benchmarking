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

#include <iostream>
#include <hpx/hpx++.h>

static int _hello_action(void) {
  std::cout << "Hello World from " << hpx_get_my_rank() << std::endl;
  hpx::exit();
}
auto _hello = hpx::make_action(_hello_action);

int main(int argc, char* argv[]) {
  if (hpx::init(&argc, &argv) != 0) {
    return -1;
  }

  int e = _hello.run();

  hpx::finalize();

  return e;
}
