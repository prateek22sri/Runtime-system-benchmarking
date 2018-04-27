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

// The program heats one of the sides as the initial condition, and iterates to
// study how the heat transfers across the surface.
#include "hpx/hpx++.h"
#include <float.h>
#include <iostream>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define N 30

hpx_addr_t grid;
hpx_addr_t new_grid;

typedef struct {
  hpx_addr_t grid;
  hpx_addr_t new_grid;
} global_args_t;

typedef struct {
  int rank;
  hpx_addr_t runtimes;
  hpx_addr_t dTmax;
} Domain;

typedef struct {
  int index;
  hpx_addr_t runtimes;
  hpx_addr_t dTmax;
} InitArgs;

#define BLOCKSIZE (N + 2) * (N + 2) * sizeof(double)

// static hpx_action_t _main          = 0;
static hpx_action_t _initGlobals = 0;
// static hpx_action_t _initDomain = 0;
// static hpx_action_t _initGrid = 0;
// static hpx_action_t _updateGrid = 0;
static hpx_action_t _write_double = 0;
static hpx_action_t _read_double = 0;
static hpx_action_t _stencil = 0;
static hpx_action_t _spawn_stencil = 0;

static void _usage(std::ostream &os, int error) {
  os << "Usage: Heat Sequence [options]\n"
     << "\t-h, show help\n";
  hpx_print_help();
  exit(error);
}

static void _register_actions(void);

/// Initialize a double zero
static void initDouble_handler(double *input, size_t UNUSED) {
  *input = 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, initDouble, initDouble_handler);

/// Update *lhs with with the max(lhs, rhs);
static void maxDouble_handler(double *lhs, const double *rhs, size_t UNUSED) {
  *lhs = (*lhs > *rhs) ? *lhs : *rhs;
}
static HPX_ACTION(HPX_FUNCTION, 0, maxDouble, maxDouble_handler);

static int _write_double_action(double *d, size_t size) {
  hpx_addr_t target = hpx_thread_current_target();
  double *addr = NULL;
  if (!hpx_gas_try_pin(target, (void **)&addr))
    return HPX_RESEND;

  *addr = d[0];
  hpx_gas_unpin(target);
  return HPX_THREAD_CONTINUE(d[1]);
}

static int _read_double_action(void *unused, size_t size) {
  hpx_addr_t target = hpx_thread_current_target();
  double *addr = NULL;
  if (!hpx_gas_try_pin(target, (void **)&addr))
    return HPX_RESEND;

  double d = *addr;

  hpx_gas_unpin(target);
  return HPX_THREAD_CONTINUE(d);
}

static int offset_of(int i, int j) {
  return (i * (N + 2) * sizeof(double)) + (j * sizeof(double));
}

struct spawn_stencil_args {
  int i;
  int j;
  hpx_addr_t max;
};

static int _stencil_action(struct spawn_stencil_args *args, size_t size) {
  // read the value in this cell
  hpx_addr_t target = hpx_thread_current_target();
  double *addr = NULL;
  if (!hpx_gas_try_pin(target, (void **)&addr))
    return HPX_RESEND;

  double v = *addr;
  hpx_gas_unpin(target);

  // read the four neighbor cells asynchronously
  double vals[4] = {0.0, 0.0, 0.0, 0.0};

  void *addrs[4] = {&vals[0], &vals[1], &vals[2], &vals[3]};

  hpx_addr_t futures[4] = {
      hpx_lco_future_new(sizeof(double)), hpx_lco_future_new(sizeof(double)),
      hpx_lco_future_new(sizeof(double)), hpx_lco_future_new(sizeof(double))};

  size_t sizes[4] = {sizeof(double), sizeof(double), sizeof(double),
                     sizeof(double)};

  int i = args->i;
  int j = args->j;

  hpx_addr_t neighbors[4] = {
      hpx_addr_add(grid, offset_of(i + 1, j), BLOCKSIZE),
      hpx_addr_add(grid, offset_of(i - 1, j), BLOCKSIZE),
      hpx_addr_add(grid, offset_of(i, j - 1), BLOCKSIZE),
      hpx_addr_add(grid, offset_of(i, j + 1), BLOCKSIZE)};

  for (int i = 0; i < 4; ++i) {
    hpx_call(neighbors[i], _read_double, futures[i], &vals[i], sizes[i]);
  }

  hpx_lco_get_all(4, futures, sizes, addrs, NULL);

  for (int n = 0; n < 4; ++n) {
    hpx_lco_delete(futures[n], HPX_NULL);
  }

  // compute the new T and dT
  double T = 0.25 * (vals[0] + vals[1] + vals[2] + vals[3]); // stencil
  double dT = T - v;                                         // local variation

  // write out the new T and continue the dT for the min reduction
  double ccargs[2] = {T, fabs(dT)};
  hpx_addr_t new_grid_addr = hpx_addr_add(new_grid, offset_of(i, j), BLOCKSIZE);
  return hpx_call_cc(new_grid_addr, _write_double, ccargs, sizeof(ccargs));
}

static void spawn_stencil_args_init(void *out, const int i, const void *env) {
  struct spawn_stencil_args *args = (struct spawn_stencil_args *)out;

  args->i = 1 + ((i - 1) / (N));
  args->j = 1 + ((i - 1) % (N));
  args->max = *(hpx_addr_t *)env;
}

static int _spawn_stencil_action(struct spawn_stencil_args *args, size_t size) {
  int i = args->i;
  int j = args->j;

  hpx_addr_t cell = hpx_addr_add(grid, offset_of(i, j), BLOCKSIZE);
  return hpx_call(cell, _stencil, args->max, args, sizeof(*args));
}

static int _updateGrid_action(
    /*void *args, size_t size*/) { // looks like args, size is not used?
  struct timeval ts_st, ts_end;
  double time, max_time;
  double dTmax, epsilon, dTmax_global;
  int finished;
  int nr_iter;

  hpx_addr_t local = hpx_thread_current_target();
  Domain *domain = NULL;
  if (!hpx_gas_try_pin(local, (void **)&domain))
    return HPX_RESEND;

  /* Set the precision wanted */
  epsilon = 0.001;
  finished = 0;
  nr_iter = 0;

  gettimeofday(&ts_st, NULL);

  do {
    dTmax = 0.0;

    hpx_addr_t max =
        hpx_lco_allreduce_new(N * N, 1, sizeof(dTmax), initDouble, maxDouble);
    // for (int i = 1; i < N + 1; i++) {
    //  for (int j = 1; j < N + 1; j++) {
    //    int args[2] = { i, j };
    //     hpx_addr_t cell = hpx_addr_add(grid, offset_of(i, j), BLOCKSIZE);
    //     hpx_call(cell, _stencil, max, args, sizeof(args));
    //   }
    // }
    hpx_par_call(_spawn_stencil, 1, N * N + 1, HPX_THREADS,
                 (N * N) / HPX_THREADS, sizeof(struct spawn_stencil_args),
                 spawn_stencil_args_init, sizeof(max), &max, HPX_NULL);

    hpx_lco_get(max, sizeof(dTmax), &dTmax);
    hpx_lco_delete(max, HPX_NULL);

    // printf("%g\n", dTmax);

    // reduce to get the max of dTmax
    hpx_lco_set(domain->dTmax, sizeof(double), &dTmax, HPX_NULL, HPX_NULL);
    hpx_lco_get(domain->dTmax, sizeof(double), &dTmax_global);

    dTmax = dTmax_global;

    if (dTmax < epsilon) // is the precision reached good enough ?
      finished = 1;
    else {
      hpx_addr_t tmp_grid = grid;
      grid = new_grid;
      new_grid = tmp_grid;
    }
    nr_iter++;
  } while (finished == 0);

  gettimeofday(&ts_end, NULL);

  /* compute the execution time */
  time = ts_end.tv_sec + (ts_end.tv_usec / 1000000.0);
  time -= ts_st.tv_sec + (ts_st.tv_usec / 1000000.0);

  printf("Rank = #%d: %d iteration in %.3lf sec\n", domain->rank, nr_iter,
         time);

  hpx_lco_set(domain->runtimes, sizeof(double), &time, HPX_NULL, HPX_NULL);
  hpx_lco_get(domain->runtimes, sizeof(double), &max_time);

  if (domain->rank == 0) {
    printf("Max Execution time =  %.3lf sec\n", max_time);
  }
  hpx_gas_unpin(local);
  return hpx::SUCCESS;
}
auto _updateGrid = hpx::make_action(_updateGrid_action);

static int _initGlobals_action(global_args_t *args, size_t size) {
  grid = args->grid;
  new_grid = args->new_grid;
  return hpx::SUCCESS;
}

void init_globals(hpx_addr_t grid, hpx_addr_t new_grid) {
  const global_args_t init_args = {.grid = grid, .new_grid = new_grid};
  int e = hpx_bcast_rsync(_initGlobals, &init_args, sizeof(init_args));
  assert(e == hpx::SUCCESS);
}

static int _initDomain_action(const InitArgs *args, std::size_t size) {
  hpx_addr_t local = hpx_thread_current_target();
  Domain *ld = NULL;
  if (!hpx_gas_try_pin(local, (void **)&ld))
    return HPX_RESEND;

  ld->rank = args->index;
  ld->runtimes = args->runtimes;
  ld->dTmax = args->dTmax;

  hpx_gas_unpin(local);

  return hpx::SUCCESS;
}
auto _initDomain = hpx::make_action(_initDomain_action);

static int _initGrid_action(/*void *args, size_t size*/) { // arguments not used
  hpx_addr_t local = hpx_thread_current_target();
  double *ld = NULL;
  if (!hpx_gas_try_pin(local, (void **)&ld))
    return HPX_RESEND;

  /* Heat one side of the solid */
  for (int j = 1; j < N + 1; j++) {
    ld[j] = 1.0;
  }

  hpx_gas_unpin(local);
  return hpx::SUCCESS;
}
auto _initGrid = hpx::make_action(_initGrid_action);

static int _main_action(int *input, std::size_t size) {
  grid =
      hpx_gas_calloc_cyclic(HPX_LOCALITIES, (N + 2) * (N + 2) * sizeof(double),
                            (N + 2) * (N + 2) * sizeof(double));
  new_grid =
      hpx_gas_calloc_cyclic(HPX_LOCALITIES, (N + 2) * (N + 2) * sizeof(double),
                            (N + 2) * (N + 2) * sizeof(double));

  hpx_addr_t domain = hpx_gas_alloc_cyclic(HPX_LOCALITIES, sizeof(Domain), 0);
  hpx_addr_t done = hpx_lco_and_new(HPX_LOCALITIES);
  hpx_addr_t complete = hpx_lco_and_new(HPX_LOCALITIES);

  hpx_addr_t gDone = hpx_lco_future_new(0);
  //   hpx_call(grid, _initGrid, gDone, NULL, 0);
  _initGrid.call(grid, gDone);
  hpx_lco_wait(gDone);
  hpx_lco_delete(gDone, HPX_NULL);

  hpx_addr_t nDone = hpx_lco_future_new(0);
  //   hpx_call(new_grid, _initGrid, nDone, NULL, 0);
  _initGrid.call(new_grid, nDone);
  hpx_lco_wait(nDone);
  hpx_lco_delete(nDone, HPX_NULL);

  init_globals(grid, new_grid);

  hpx_addr_t runtimes = hpx_lco_allreduce_new(
      HPX_LOCALITIES, HPX_LOCALITIES, sizeof(double), initDouble, maxDouble);

  hpx_addr_t dTmax = hpx_lco_allreduce_new(
      HPX_LOCALITIES, HPX_LOCALITIES, sizeof(double), initDouble, maxDouble);

  for (int i = 0, e = HPX_LOCALITIES; i < e; ++i) {
    InitArgs init = {.index = i, .runtimes = runtimes, .dTmax = dTmax};
    hpx_addr_t block = hpx_addr_add(domain, sizeof(Domain) * i, sizeof(Domain));
    //     hpx_call(block, _initDomain.get_id(), done, &init, sizeof(init));
    _initDomain.call(block, done, &init, sizeof(init));
  }
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);

  for (int i = 0; i < HPX_LOCALITIES; i++) {
    hpx_addr_t block = hpx_addr_add(domain, sizeof(Domain) * i, sizeof(Domain));
    //     hpx_call(block, _updateGrid, complete, NULL, 0);
    _updateGrid.call(block, complete);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  hpx_lco_delete(dTmax, HPX_NULL);
  hpx_lco_delete(runtimes, HPX_NULL);

  hpx_gas_free(grid, HPX_NULL);
  hpx_gas_free(new_grid, HPX_NULL);
  hpx_gas_free(domain, HPX_NULL);

  hpx::exit();
}
auto _main = hpx::make_action(_main_action);
/**
 * Registers functions as actions.
 */
void _register_actions(void) {
  /* register action for parcel (must be done by all ranks) */
  //   HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _main, _main_action,
  //   HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _initGlobals,
                      _initGlobals_action, HPX_POINTER, HPX_SIZE_T);
  //   HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _initDomain,
  //                       _initDomain_action, HPX_POINTER, HPX_SIZE_T);
  //   HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _initGrid,
  //   _initGrid_action,
  //                       HPX_POINTER, HPX_SIZE_T);
  //   HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _updateGrid,
  //                       _updateGrid_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _write_double,
                      _write_double_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _read_double,
                      _read_double_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _stencil, _stencil_action,
                      HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _spawn_stencil,
                      _spawn_stencil_action, HPX_POINTER, HPX_SIZE_T);
}

// Main routine
int main(int argc, char *argv[]) {
  _register_actions();

  int e = hpx::init(&argc, &argv);
  if (e) {
    std::cerr << "HPX: failed to initialize." << std::endl;
    return e;
  }

  // parse the command line
  int opt = 0;
  while ((opt = getopt(argc, argv, "h?")) != -1) {
    switch (opt) {
      case 'h': _usage(std::cout, EXIT_SUCCESS);
      case '?':
      default: _usage(std::cerr, EXIT_FAILURE);
    }
  }

  //   e = hpx_run(&_main, NULL, 0);
  e = _main.run((int *)nullptr, (std::size_t)0);
  hpx_finalize();
  return e;
}
