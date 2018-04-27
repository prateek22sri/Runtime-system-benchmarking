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

#ifndef HAWAII_H
#define HAWAII_H

#define __STDC_LIMIT_MACROS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>

#include <hpx/hpx.h>
#include "actions.h"
#include "hpcg-main.h"

#include "Geometry.hpp"
#include "GenerateGeometry.hpp"
#include "Vector.hpp"
#include "SparseMatrix.hpp"
#include "CGData_upper.hpp"
#include "TestCG_upper.hpp"
#include "TestSymmetry_upper.hpp"
#include "mytimer.hpp"

hpx_addr_t get_domain_addr(hpx_addr_t base, int index);
void computeresidual_reduction(double * global_residual,double local_residual,
                               hpx_addr_t dreduction_max,hpx_addr_t dallreduce_max,int rank);
void dotproduct_reduction(double * global_result,double local_result,
                          hpx_addr_t dreduction_sum,hpx_addr_t dallreduce,int rank);
void dotproduct_mpi_reduction(double * global_result,double local_result,
                          hpx_addr_t dreduction_sum,hpx_addr_t dallreduce,
                          int rank,int size,int generation);
void dmin_reduction(double * global,double local,
                    hpx_addr_t dreduction_min,hpx_addr_t dallreduce_min,int rank);

typedef struct {
  bool core_major;
  int num_domains;
} global_config_t;

typedef struct Reductions {
  hpx_addr_t allreduce;
  hpx_addr_t reduction_sum;
  hpx_addr_t dallreduce;
  hpx_addr_t dreduction_sum;
  hpx_addr_t dallreduce_max;
  hpx_addr_t dreduction_max;
  hpx_addr_t dallreduce_min;
  hpx_addr_t dreduction_min;
} Reductions;

// has to be 2 or greater
const int num_and = 3;

typedef struct
{
  int locality_count;
  int nDoms;
} DP;

typedef struct
{
  hpx_addr_t reduction_sum;
  hpx_addr_t dreduction_sum;
  hpx_addr_t dreduction_max;
  hpx_addr_t dreduction_min;

  int no_spmv_parfor;
  int aggregate_parfor;
  int num_chunks;
  int use_memput;
  int use_memput_copyless;

  int index;
  int nDoms;

  hpx_addr_t e_xt;
  hpx_addr_t f_xt;
  hpx_addr_t g_xt;
  hpx_addr_t h_xt;
  hpx_addr_t lco_xt;
  hpx_addr_t lco_xt2;
  hpx_addr_t rn_order;
  hpx_addr_t grecv_length;
  hpx_addr_t rn_complete[4];
  hpx_addr_t r_complete[4];
  hpx_addr_t gen_complete[num_and];
  hpx_addr_t newlocalLength;
  hpx_addr_t base;
  hpx_addr_t mpi_hpx_local;
} CP;

typedef struct {
  hpx_addr_t xt[2];
  hpx_addr_t send[2];
  hpx_addr_t lco_xt;
  hpx_addr_t lco_xt2;
  hpx_addr_t rn_order;
  hpx_addr_t grecv_length;
  hpx_addr_t rn_complete[4];
  hpx_addr_t r_complete[4];
  hpx_addr_t gen_complete[num_and];
  hpx_addr_t newlocalLength;
  hpx_addr_t allreduce;
  int32_t reduceid;
} GAS_Vectors;

typedef struct LCOS {
  hpx_addr_t exchange_recv[2]; // serialize recv for halo exchange
  hpx_addr_t exchange_and[num_and];  // finish all recvs for halo exchange
  hpx_addr_t exchange_and2[num_and];  // finish all recvs for halo exchange
  hpx_addr_t exchange_swap[2];  // make sure vector swap has occurred prior to recv
  hpx_addr_t *memput_and;
  hpx_addr_t *memput_and2;
} LCOS;

typedef struct Domain {
  HPCG_Params params;
  Reductions reduce;
  GAS_Vectors gasvectors;
  LCOS lcos;
  hpx_addr_t base;
  int generation;
  int gen;
  int dot_generation;
  double eh_t;
  long exchange_messages;
  long exchange_bytes;

  Geometry geom;
  SparseMatrix A;
  Vector b,x,xexact;
  Vector x_overlap, b_computed;
  Vector * eX;
  const SparseMatrix * eA;
  Vector * fX;
  const SparseMatrix * fA;
  CGData data;
  TestCGData testcg_data;
  TestSymmetryData testsymmetry_data;
  double times[9];
} Domain;

#endif
