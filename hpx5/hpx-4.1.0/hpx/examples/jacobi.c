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

#include "hpx/hpx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BSIZE sizeof(double)
#define IDX(base, offset) \
  hpx_addr_add((base), (offset)*sizeof(double), sizeof(double))

// filename to dump the output to.
static char *_fname;

static void
_usage(FILE *f, int error)
{
  fprintf(f, "Usage: jacobi [options]\n"
          "\t-n, number of discretized points\n"
          "\t-s, number of steps\n"
          "\t-f, dump output to file\n"
          "\t-h, show help\n");
  hpx_print_help();
  fflush(f);
  exit(error);
}

static int
_op_handler(double *lu, int i, double h2, hpx_addr_t u, hpx_addr_t f)
{
  double left, right, lf;

  hpx_gas_memget_sync(&left, IDX(u,i-1), sizeof(left));
  hpx_gas_memget_sync(&right, IDX(u,i+1), sizeof(right));
  hpx_gas_memget_sync(&lf, IDX(f,i), sizeof(lf));
  *lu = left + right + h2*lf/2;
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _op, _op_handler,
                  HPX_POINTER, HPX_INT, HPX_DOUBLE, HPX_ADDR, HPX_ADDR);

// Do sweeps of Jacobi iteration on a 1D Poisson problem
// discretized by n+1 equally spaced mesh points on [0,1].
// u is subject to Dirichlet boundary conditions specified in
// the u[0] and u[n] entries of the initial vector.
static void
_jacobi(int nsteps, int n, hpx_addr_t u, hpx_addr_t f)
{
  double h  = 1.0/n;
  double h2 = h*h;
  hpx_addr_t utmp = hpx_gas_alloc_local((n+1), BSIZE, 0);

  // fill boundary conditions into utmp
  hpx_gas_memcpy_sync(IDX(utmp,0), IDX(u,0), sizeof(double));
  hpx_gas_memcpy_sync(IDX(utmp,n), IDX(u,n), sizeof(double));

  hpx_addr_t and = hpx_lco_and_new(n-1);
  for (int sweep = 0; sweep < nsteps; sweep += 2) {
    for (int i = 1; i < n; ++i) {
      hpx_xcall(IDX(utmp, i), _op, and, i, h2, u, f);
    }
    hpx_lco_wait_reset(and);

    for (int i = 1; i < n; ++i) {
      hpx_xcall(IDX(u, i), _op, and, i, h2, utmp, f);
    }
    hpx_lco_wait_reset(and);
  }

  hpx_lco_delete(and, HPX_NULL);
  hpx_gas_free(utmp, HPX_NULL);
}

static void
_write_solution(int n, hpx_addr_t u, const char *fname)
{
  double h = 1.0/n;
  FILE* fp = fopen(fname, "w+");
  double lu;
  for (int i = 0; i <= n; ++i) {
    hpx_gas_memget_sync(&lu, IDX(u,i), sizeof(lu));
    fprintf(fp, "%g %g\n", i*h, lu);
  }
  fclose(fp);
}

static int
_jacobi_main_handler(int n, int nsteps)
{
  double h = 1.0/n;

  // allocate and initialize arrays
  hpx_addr_t u = hpx_gas_calloc_local_attr((n+1), BSIZE, 0, HPX_GAS_ATTR_LB);
  hpx_addr_t f = hpx_gas_alloc_local((n+1), BSIZE, 0);
  hpx_addr_t and = hpx_lco_and_new(n+1);
  for (int i = 0; i <= n; ++i) {
    double val = i*h;
    hpx_gas_memput_lsync(IDX(f,i), &val, sizeof(val), and);
  }
  hpx_lco_wait(and);
  hpx_lco_delete(and, HPX_NULL);

  printf("starting jacobi iterations...\n");

  hpx_time_t start = hpx_time_now();
  _jacobi(n, nsteps, u, f);
  double elapsed = hpx_time_elapsed_ms(start)/1e3;

  // run the solver
  printf("n: %d\n", n);
  printf("nsteps: %d\n", nsteps);
  printf("seconds: %.7f\n", elapsed);

  // write the results
  if (_fname) {
    _write_solution(n, u, _fname);
  }

  hpx_gas_free(f, HPX_NULL);
  hpx_gas_free(u, HPX_NULL);
  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _jacobi_main, _jacobi_main_handler,
                  HPX_INT, HPX_INT);

int
main(int argc, char **argv)
{
  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "failed to initialize HPX.\n");
    return e;
  }

  int n = 100;
  int nsteps = 100;

  // parse the command line
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:s:f:h?")) != -1) {
    switch (opt) {
      case 'n':
       n = atoi(optarg);
       break;
      case 's':
       nsteps = atoi(optarg);
       break;
      case 'f':
       _fname = optarg;
       break;
      case 'h':
       _usage(stdout, EXIT_SUCCESS);
       break;
     case '?':
     default:
       _usage(stderr, EXIT_FAILURE);
    }
  }

  argc -= optind;
  argv += optind;

  // run the main action
  e = hpx_run(&_jacobi_main, NULL, &n, &nsteps);
  hpx_finalize();
  return e;
}
