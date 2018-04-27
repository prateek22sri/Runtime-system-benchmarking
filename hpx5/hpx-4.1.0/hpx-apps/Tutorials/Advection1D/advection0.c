// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

#include <stdio.h>
#include <math.h>
#include <hpx/hpx.h>

// problem description
const double L = 1.0; // length scale 
const int I = 10; // number of iterations 
const int N = 81; // grid size N 
const double dT = 1e-3; // timestep 

// globals 
double h; 

// grid point type
typedef struct {
  double T[2]; 
} point_t; 


// handler and action
int hpx_main_handler(int print); 
HPX_ACTION(HPX_DEFAULT, 0, hpx_main, hpx_main_handler, HPX_INT); 

// implementation
int hpx_main_handler(int print) {
  // allocate memory 
  hpx_addr_t grid = hpx_gas_calloc_cyclic(N, sizeof(point_t), 0); 
  assert(grid != HPX_NULL); 
  printf("allocation is done\n"); 

  // free global memory 
  hpx_gas_free(grid, HPX_NULL); 

  // finalize 
  hpx_exit(0, NULL); 
} 

int main(int argc, char *argv[argc]) {
  // initialize hpx 
  if (hpx_init(&argc, &argv)) {
    hpx_print_help(); 
    return -1;
  }

  // parse application arguments
  int print = (argc > 1); 

  h = L / (N - 1); 

  int e = hpx_run(&hpx_main, NULL, &print); 
  hpx_finalize();
  return e;
} 
