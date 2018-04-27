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

#ifndef HAWAIIMAIN_H
#define HAWAIIMAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <hpx/hpx.h>

typedef struct HPCG_Params {
  int comm_size; //!< Number of MPI processes in MPI_COMM_WORLD
  int comm_rank; //!< This process' MPI rank in the range [0 to comm_size - 1]
  int numThreads; //!< This process' number of threads
  int nx; //!< Number of x-direction grid points for each local subdomain
  int ny; //!< Number of y-direction grid points for each local subdomain
  int nz; //!< Number of z-direction grid points for each local subdomain
  int runningTime; //!< Number of seconds to run the timed portion of the benchmark

  int nDoms;
  bool core_major;

  // parameters for tuning HPX performance
  int no_spmv_parfor;
  int aggregate_parfor;
  int num_chunks;
  int use_memput;
  int use_memput_copyless;
  int use_global_vector;
  int use_mpi_reduce;
  /// This LCO is used to store the last vector that was allocated. 
  hpx_addr_t vector_future_lco;
  /// These LCOs are used for a poor man's barrier.
  hpx_addr_t vector_wait_lco, vector_wait2_lco, vector_wait3_lco;
} HPCG_Params;

void GenProblem();
void InitSparseMatrix();
void Sethalo();
void GenCoarse(int numberOfMgLevels);
void InitSparseCG();
void GenGeometry(int size, int rank, int numThreads, int nx, int ny, int nz);
void testCG1();
void OptProblem();
int getNumNeighbors(int nl);
void SpMV_MG();
//void set_rn(int *neighbors,int num_neighbors,int *recv_length,
//            int * grecv_length, int * rn_order,
//            hpx_addr_t gas_rn_order,hpx_addr_t gas_grecv_length,
//            hpx_addr_t gas_rn_complete[],hpx_addr_t gas_r_complete[],
//            int rank, int nDoms);

#endif
