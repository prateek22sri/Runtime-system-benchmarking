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

/// @file
/// A simple fibonacci number computation to demonstrate HPX.
/// This example calculates a fibonacci number using recursion, where each
/// level of recursion is executed by a different HPX thread. (Of course, this
/// is not an efficient way to calculate a fibonacci number but it does
/// demonstrate some of the basic of HPX and it may demonstrate a
/// <em>pattern of computation</em> that might be used in the real world.)

#include <iostream>
#include <vector>

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "hpx/hpx++.h"

static void _usage(std::ostream& os, int error) {
  os << "Usage: fibonacci [options] NUMBER\n" << "\t-h, show help\n";
  hpx_print_help();
  exit(error);
}

// static hpx_action_t _fib      = 0;
// static hpx_action_t _fib_main = 0;

int _fib_action(int n);
hpx::Action<HPX_DEFAULT, HPX_ATTR_NONE, decltype(_fib_action), int> _fib;

int _fib_action(int n) {

  if (n < 2) {
    return _fib.thread_continue(n);
  }

  hpx::global_ptr<hpx::lco::Future<int>> f0 = hpx::lco::Future<int>::Alloc();
  hpx::global_ptr<hpx::lco::Future<int>> f1 = hpx::lco::Future<int>::Alloc();

  int v1 = n - 1, v2 = n - 2;

  _fib.call(HPX_HERE, f0.get(), v1);
  _fib.call(HPX_HERE, f1.get(), v2);

  int rv1 = 0, rv2 = 0;
  std::vector<decltype(f0)> futures = {f0, f1};
  std::vector<int> rvars = {rv1, rv2};

  hpx::lco::get_all(futures, rvars);

  hpx::lco::dealloc(f0, nullptr);
  hpx::lco::dealloc(f1, nullptr);

  // rv1, rv2 copied by value into rvars, so they won't be updated
  // hence have to use rvars[0] and rvars[1] here
  int fn = rvars[0] + rvars[1];
  return _fib.thread_continue(fn);
}

static int _fib_main_action(int n) {
  int fn = 0;                                   // fib result

  hpx_time_t now = hpx_time_now();

  _fib.call_sync(HPX_HERE, fn, n);
  double elapsed = hpx_time_elapsed_ms(now)/1e3;

  std::cout << "fib("<< n << ")=";
  std::cout << fn << std::endl;
  std::cout << "seconds: " << elapsed << std::endl;
  std::cout << "localities: " << HPX_LOCALITIES << std::endl;
  std::cout << "threads/locality: " << HPX_THREADS << std::endl;
  hpx::exit();
}
auto _fib_main = hpx::make_action(_fib_main_action);

int main(int argc, char *argv[]) {

  _fib._register(_fib_action);

  int e = hpx::init(&argc, &argv);
  if (e) {
    std::cerr << "HPX: failed to initialize.\n";
    return e;
  }

  // parse the command line
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
     std::cerr << "\nMissing fib number.\n"; // fall through
   default:
     _usage(std::cerr, EXIT_FAILURE);
   case 1:
     n = atoi(argv[0]);
     break;
  }

  // run the main action
  _fib_main.run(n);

  hpx::finalize();

  return e;
}

