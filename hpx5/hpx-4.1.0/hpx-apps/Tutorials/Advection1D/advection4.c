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
  double T;
  hpx_addr_t reduce;
} point_t;

// handlers and actions
int hpx_main_handler(int print);
int point_init_handler(point_t *P, double T0);
int point_add_contribution_handler(point_t *P, double value);
int point_integrate1_handler(point_t *P, hpx_addr_t grid, int i);
int point_integrate2_handler(point_t *P);

void double_init_handler(double *input, const size_t bytes) {
  *input = 0.0;
}

void double_accum_handler(double *lhs, const double *rhs, size_t UNUSED) {
  *lhs += *rhs;
}

HPX_ACTION(HPX_DEFAULT, 0, hpx_main, hpx_main_handler, HPX_INT);
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, point_init, point_init_handler,
           HPX_POINTER, HPX_DOUBLE);
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, point_add_contribution,
           point_add_contribution_handler, HPX_POINTER, HPX_DOUBLE);
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, point_integrate1,
           point_integrate1_handler, HPX_POINTER, HPX_ADDR, HPX_INT);
HPX_ACTION(HPX_DEFAULT, HPX_PINNED, point_integrate2,
           point_integrate2_handler, HPX_POINTER);
HPX_ACTION(HPX_FUNCTION, 0, double_init, double_init_handler);
HPX_ACTION(HPX_FUNCTION, 0, double_accum, double_accum_handler);

// grid interface
void grid_init(hpx_addr_t grid);
void grid_integrate(hpx_addr_t grid);
void grid_print(FILE *f, hpx_addr_t grid, int iteration);
hpx_addr_t grid_element_at(hpx_addr_t grid, int i);

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

  // integrate for each iteration, printing out the results each time if so
  // requested.
  printf("integrating\n");
  for (int i = 0; i < I; i++) {
    int iteration = i + 1;
    grid_integrate(grid);
    grid_print(f, grid, iteration);
    printf("step [%d]\n", iteration);
  }

  // free global memory
  hpx_gas_free(grid, HPX_NULL);

  // close the file
  if (f) fclose(f);

  hpx_exit(0, NULL);
}

// initialize a grid point with an initial value
int point_init_handler(point_t *P, double T0) {
  P->T = T0;

  // each timestep we will perform a reduction on the contribution from the
  // neighbors.  we will allocate a reduction LCO for this purpose
  P->reduce = hpx_lco_reduce_new(2, sizeof(double), double_init,
                                 double_accum);
  return HPX_SUCCESS;
}

// add a contribution to the reduction at a point
int point_add_contribution_handler(point_t *P, double value) {
  hpx_lco_set(P->reduce, sizeof(value), &value, HPX_NULL, HPX_NULL);
  return HPX_SUCCESS;
}

// perform integration at a grid point
int point_integrate1_handler(point_t *P, hpx_addr_t grid, int i) {
  double contrib[2] = {dT / h / 2 * P->T, -dT / h / 2 * P->T};
  hpx_addr_t neighbors[2] = {
    grid_element_at(grid, i + 1),
    grid_element_at(grid, i - 1)
  };

  // send my contribution to each neighbor
  hpx_call(neighbors[0], point_add_contribution, HPX_NULL, &contrib[0]);
  hpx_call(neighbors[1], point_add_contribution, HPX_NULL, &contrib[1]);

  // update myself
  P->T = P->T;

  hpx_addr_t point = hpx_thread_current_target();
  return hpx_call_when_cc(P->reduce, point, point_integrate2);
}

int point_integrate2_handler(point_t *P) {
  double contrib = 0.0;
  hpx_lco_get(P->reduce, sizeof(contrib), &contrib);
  P->T += contrib;

  hpx_lco_reset_sync(P->reduce);
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

hpx_addr_t grid_element_at(hpx_addr_t grid, int i) {
  // the tutorial assumes periodic boundary condition so i is never going to be
  // out of the bounds.
  int j = i % N + (i < 0) * N;
  return hpx_addr_add(grid, sizeof(point_t) * j, sizeof(point_t));
}

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

void grid_print(FILE *f, hpx_addr_t grid, int iteration) {
  if (!f) return;

  double time = iteration * dT;

  for (int i = 0; i < N; i++) {
    // coordinate of the point
    double x = i * h;
    double T = 0.0;

    hpx_addr_t point = grid_element_at(grid, i);
    hpx_gas_memget_sync(&T, point, sizeof(T));
    fprintf(f, "%d %e %e %e\n", i, x, T, time);
  }
}

void grid_integrate(hpx_addr_t grid) {
  hpx_addr_t done = hpx_lco_and_new(N);
  for (int i = 0; i < N; i++) {
    hpx_addr_t point = grid_element_at(grid, i);
    hpx_call(point, point_integrate1, done, &grid, &i);
  }

  // wait for the integration to complete
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);
}
