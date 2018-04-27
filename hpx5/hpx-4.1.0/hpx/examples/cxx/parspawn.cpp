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
#include <getopt.h>

#include "hpx/hpx++.h"


/// ----------------------------------------------------------------------------
/// @file examples/hpx/parspawn.c
/// This file implements a parallel (tree) spawn, that uses HPX
/// threads to spawn many NOP operations in parallel.
/// ----------------------------------------------------------------------------
static void _usage(std::ostream& os, int error) {
  os << "Usage: parspawn [options] NUMBER\n" << "\t-h, this help display\n";
  hpx_print_help();
  exit(error);
}

/// The empty action
static int _nop_action() {
  return hpx::SUCCESS;
}
auto _nop = hpx::make_action(_nop_action);

static int _main_action(int n) {
  std::cout << "parspawn(" << n << ")" << std::endl;

  hpx_time_t now = hpx_time_now();
  _nop.par_call_sync(0, n, 8, 1000, 0, NULL, 0, 0);
  double elapsed = hpx_time_elapsed_ms(now)/1e3;

  std::cout << "seconds: " << elapsed << std::endl;
  std::cout << "localities:   " << HPX_LOCALITIES << std::endl;
  std::cout << "threads:      " << HPX_THREADS << std::endl;
  hpx::exit();
}
auto _main = hpx::make_action(_main_action);
/// The main function parses the command line, sets up the HPX runtime system,
/// and initiates the first HPX thread to perform parspawn(n).
int main(int argc, char *argv[]) {

  if (hpx_init(&argc, &argv)) {
    std::cerr << "HPX: failed to initialize." << std::endl;
    return -1;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "h?")) != -1) {
    switch (opt) {
     case 'h':
       _usage(std::cout, EXIT_SUCCESS);
     case '?':
     default:
       _usage(std::cerr, EXIT_FAILURE);
    }
  }

  argc -= optind;
  argv += optind;

  int n = 0;
  switch (argc) {
   case 0:
     std::cerr << "\nMissing spawn count." << std::endl;
   default:
     _usage(std::cerr, EXIT_FAILURE);
   case 1:
     n = atoi(argv[0]);
     break;
  }

  // run the main action
  int e = _main.run(n);
  hpx::finalize();

  return e;
}
