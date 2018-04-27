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

// handlers and actions
int hpx_main_handler(int print); 
int point_init_handler(point_t *P, double T0); 

HPX_ACTION(HPX_DEFAULT, 0, hpx_main, hpx_main_handler, HPX_INT); 
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, point_init, point_init_handler, 
           HPX_POINTER, HPX_DOUBLE); 

// grid interface
hpx_addr_t grid_element_at(hpx_addr_t grid, int i); 
void grid_init(hpx_addr_t grid); 
void grid_print(FILE *f, hpx_addr_t grid, int iteration); 

// implementation 
int hpx_main_handler(int print) {    
  FILE *f = (print) ? fopen("temp.txt", "a") : NULL; 

  hpx_addr_t grid = hpx_gas_calloc_cyclic(N, sizeof(point_t), 0); 
  assert(grid != HPX_NULL); 
  printf("allocation is done\n"); 

  // initialize the grid and print its initial values 
  grid_init(grid); 
  grid_print(f, grid, 0); 
  printf("initialization is done\n"); 

  // free global memory 
  hpx_gas_free(grid, HPX_NULL); 

  // close the file 
  if (f) fclose(f); 

  hpx_exit(0, NULL); 
}

int point_init_handler(point_t *P, double T0) {
  P->T[0] = T0; 
  P->T[1] = 0.0; 
  return HPX_SUCCESS;
}

int main(int argc, char *argv[]) {

  if (hpx_init(&argc, &argv)) {
    hpx_print_help(); 
    return -1;
  }

  int print = (argc > 1); 

  h = L / (N - 1); 

  int e = hpx_run(&hpx_main, NULL, &print); 
  hpx_finalize();
  return e;
} 

// use hpx_addr_add to determine the global address for a grid element
hpx_addr_t grid_element_at(hpx_addr_t grid, int i) {
  // the tutorial assumes periodic boundary condition so i is never going to be
  // out of the bounds. 
  int j = i % N + (i < 0) * N; 
  return hpx_addr_add(grid, sizeof(point_t) * j, sizeof(point_t)); 
}

// initialize the grid 
void grid_init(hpx_addr_t grid) {
  hpx_addr_t done = hpx_lco_and_new(N); 
  for (int i = 0; i < N; i++) {
    double x = i * h; 
    double T0 = sin(2 * M_PI * x); 
    hpx_addr_t point = grid_element_at(grid, i); 
    hpx_call(point, point_init, done, &T0); 
  }

  hpx_lco_wait(done); 
  hpx_lco_delete(done, HPX_NULL); 
}

// print the grid 
void grid_print(FILE *f, hpx_addr_t grid, int iteration) {
  if (!f) return; 

  double time = iteration * dT; 
  int gen = iteration % 2; 

  size_t offset = offsetof(point_t, T[gen]); 

  for (int i = 0; i < N; i++) {
    // coordinate of the point 
    double x = i * h; 
    
    hpx_addr_t point = grid_element_at(grid, i); 
    hpx_addr_t temp = hpx_addr_add(point, offset, sizeof(point_t)); 

    double T; 
    hpx_gas_memget_sync(&T, temp, sizeof(T)); 

    fprintf(f, "%d %e %e %e\n", i, x, T, time); 
  }
}
 
