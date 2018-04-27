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

#include <iostream>
#include <hpx/hpx++.h>

using namespace std;

int cont2(int arg) {
  std::cout << "cont2 arg: " << arg << std::endl;
  return hpx::SUCCESS;
}
auto cont2_obj = hpx::make_action(cont2);

int cont1(int arg) {
  std::cout << "cont1 arg: " << arg << std::endl;
//   hpx_call_cc(HPX_HERE, cont2_action_struct::id, &arg);
  return hpx_thread_continue(&arg);
}
auto cont1_obj = hpx::make_action<HPX_DEFAULT, HPX_ATTR_NONE, decltype(cont1), int>(cont1);

int main_act(int arg) {
  std::cout << "main_act arg: " << arg << std::endl;
  
  cont1_obj.call_with_continuation(HPX_HERE, HPX_HERE, cont2_obj, arg);
  
  hpx::exit();
}
auto main_act_obj = hpx::make_action(main_act);

int main(int argc, char* argv[]) {

  int e = hpx::init(&argc, &argv);
  if (e) {
    std::cerr << "HPX: failed to initialize." << std::endl;
    return e;
  }
  int a = hpx_get_my_rank() + 1;
  main_act_obj.run(a);

  hpx::finalize();
  return 0;
}
