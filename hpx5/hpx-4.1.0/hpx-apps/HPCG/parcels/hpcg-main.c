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
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "hpcg.h"
#include "GenerateProblem.hpp"
#include "SetupHalo.hpp"
#include "GenerateCoarseProblem.hpp"
#include "OptimizeProblem.hpp"
#include "TestCG.hpp"
#include "TestSymmetry.hpp"
#include "ComputeSPMV_ref.hpp"
#include "ComputeMG_ref.hpp"
#include "CG_ref.hpp"
#include "CG.hpp"
#include "TestNorms.hpp"
#include "ReportResults.hpp"

HPCG_Params gparams;
typedef int local_int_t;
typedef long long global_int_t;

hpx_addr_t vector_lco;

double *node_local;
int max_dot_gen = 1000;
hpx_addr_t phoenix_lco[2];
hpx_addr_t * phoenix_finish_lco;

int HPCG_Init(int * argc_p, char ** *argv_p);

static inline double getticks(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (double) (tv.tv_sec * 1e6 + tv.tv_usec);
}

static inline double elapsed(double t1, double t0) {
  return (double) (t1 - t0);
}

static void initmaxint_handler(int *input, const size_t size) {
  assert(sizeof(int) == size);
  *input = 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, initmaxint, initmaxint_handler);

static void maxint_handler(int *output,const int *input, const size_t size) {
  assert(sizeof(int) == size);
  if ( *output < *input ) *output = *input;
}
static HPX_ACTION(HPX_FUNCTION, 0, maxint, maxint_handler);

//hpx_action_t _main = 0;
hpx_action_t _param = 0;
hpx_action_t _initnodereduce = 0;
hpx_action_t _print = 0;
hpx_action_t _generategeometry = 0;
hpx_action_t _initDomain = 0;
hpx_action_t _initializesparsematrix = 0;
hpx_action_t _generateproblem = 0;
hpx_action_t _setuphalo = 0;
hpx_action_t _generatecoarse = 0;
hpx_action_t _initializesparsecgdata = 0;
hpx_action_t _optimizeproblem = 0;
hpx_action_t _setuplcos = 0;
hpx_action_t _testcg = 0;
hpx_action_t _spmv_mg = 0;
hpx_action_t _cg_ref = 0;

global_config_t global_config;

int _print_action(void) {
  printf(" HELLO WORLD\n");

  //double point;
  //int length = params.nx*params.ny*params.nz;
  //int rt = 2;
  //int ofs = 1;
  //hpx_addr_t u = hpx_addr_add(gasvectors.bt,ofs*sizeof(double)+rt*length*sizeof(double),length*sizeof(double));
  //hpx_gas_memget_sync(&point,u,sizeof(point));
  //printf(" HELLO WORLD point %g\n",point);

  return HPX_SUCCESS;
}

hpx_addr_t get_vector_addr(hpx_addr_t base, int index,int newlocalLength) {
  int nd = global_config.num_domains;
  assert(nd > 0);
  int nc = hpx_get_num_threads();
  int nr = hpx_get_num_ranks();
  if (nc * nr < nd) { // handle oversubscription
  // if the user oversubscribes, put as many domains as we need to one each
  // rank to fit all domains in
    nc = (nd + nr - 1) / nr;
  }
  int new_index = (global_config.core_major) ? index / nc + (index % nc) * nr :
                            index;
  return hpx_addr_add(base, newlocalLength*sizeof(double) * new_index, newlocalLength*sizeof(double));
}

hpx_addr_t get_domain_addr(hpx_addr_t base, int index) {
  int nd = global_config.num_domains;
  assert(nd > 0);
  int nc = hpx_get_num_threads();
  int nr = hpx_get_num_ranks();
  if (nc * nr < nd) { // handle oversubscription
  // if the user oversubscribes, put as many domains as we need to one each
  // rank to fit all domains in
    nc = (nd + nr - 1) / nr;
  }
  int new_index = (global_config.core_major) ? index / nc + (index % nc) * nr :
                            index;
  return hpx_addr_add(base, sizeof(Domain) * new_index, sizeof(Domain));
}

int getNumNeighbors(int nl,Domain *domain)
{
  SparseMatrix * curLevelMatrix = &domain->A;
  for (int level = 1; level< nl; ++level) {
    curLevelMatrix = curLevelMatrix->Ac; // Make the just-constructed coarse grid the next level
  }

  // make sure receive length is nonzero for each of the neighbors
  int count = 0;
  for (int i = 0; i < curLevelMatrix->numberOfSendNeighbors; i++) {
    if ( curLevelMatrix->receiveLength[i] > 0 ) {
      count++;
    }
  }

  return count;
}

int _setuplcos_action(Domain *domain) {
  int num_neighbors = getNumNeighbors(0,domain);
  domain->lcos.exchange_and[0] = hpx_lco_and_new(num_neighbors);
  domain->lcos.exchange_and[1] = hpx_lco_and_new(num_neighbors);
  domain->lcos.exchange_recv[0] = hpx_lco_sema_new(0); // hpx_lco_future_new(0);
  domain->lcos.exchange_recv[1] = hpx_lco_sema_new(0); // hpx_lco_future_new(0);
  domain->lcos.exchange_swap[0] = hpx_lco_future_new(0);
  domain->lcos.exchange_swap[1] = hpx_lco_future_new(0);
  domain->lcos.exchange_and2[0] = hpx_lco_and_new(num_neighbors);
  domain->lcos.exchange_and2[1] = hpx_lco_and_new(num_neighbors);

  if ( domain->params.use_memput == 1 || domain->params.use_memput_copyless == 1 ) {
    // create some more and-gates for overlapping generations
    for (int i=2;i<num_and;i++) {
      domain->lcos.exchange_and[i] = hpx_lco_and_new(num_neighbors);
      domain->lcos.exchange_and2[i] = hpx_lco_and_new(num_neighbors);
    }

    for (int i=0;i<num_and;i++) {
      hpx_addr_t desta = hpx_addr_add(domain->gasvectors.lco_xt,
               i*sizeof(hpx_addr_t) + domain->params.comm_rank*num_and*sizeof(hpx_addr_t),num_and*sizeof(hpx_addr_t));

      hpx_gas_memput_rsync(desta,&(domain->lcos.exchange_and[i]),sizeof(hpx_addr_t));

      hpx_addr_t destb = hpx_addr_add(domain->gasvectors.lco_xt2,
               i*sizeof(hpx_addr_t) + domain->params.comm_rank*num_and*sizeof(hpx_addr_t),num_and*sizeof(hpx_addr_t));

      hpx_gas_memput_rsync(destb,&(domain->lcos.exchange_and2[i]),sizeof(hpx_addr_t));
    }
  }
  return HPX_SUCCESS;
}

int _testcg_action(Domain *domain) {
  if ( domain->params.use_memput == 1 || domain->params.use_memput_copyless == 1 ) {
    // make sure the global addresses of the lcos are known to everyone
    domain->lcos.memput_and = ( hpx_addr_t *) malloc(num_and*domain->params.comm_size*sizeof(hpx_addr_t));
    domain->lcos.memput_and2 = ( hpx_addr_t *) malloc(num_and*domain->params.comm_size*sizeof(hpx_addr_t));
    for (int i=0;i<domain->params.comm_size;i++) {
      for (int j=0;j<num_and;j++) {
        hpx_addr_t u = hpx_addr_add(domain->gasvectors.lco_xt,
                   j*sizeof(hpx_addr_t) + i*num_and*sizeof(hpx_addr_t),num_and*sizeof(hpx_addr_t));
        hpx_gas_memget_sync(&(domain->lcos.memput_and[j+num_and*i]),u,sizeof(hpx_addr_t));

        hpx_addr_t v = hpx_addr_add(domain->gasvectors.lco_xt2,
                   j*sizeof(hpx_addr_t) + i*num_and*sizeof(hpx_addr_t),num_and*sizeof(hpx_addr_t));
        hpx_gas_memget_sync(&(domain->lcos.memput_and2[j+num_and*i]),v,sizeof(hpx_addr_t));
      }
    }
  }

  domain->testcg_data.count_pass = 0;
  domain->testcg_data.count_fail = 0;
  TestCG(domain->A, domain->data, domain->b, domain->x, domain->testcg_data,domain);

  TestSymmetry(domain->A, domain->b, domain->xexact, domain->testsymmetry_data,domain);
  return HPX_SUCCESS;
}

int _spmv_mg_action(Domain *domain) {
  // Call Reference SpMV and MG. Compute Optimization time as ratio of times in these routines
  //
  local_int_t nrow = domain->A.localNumberOfRows;
  local_int_t ncol = domain->A.localNumberOfColumns;

  InitializeVector(domain->x_overlap, ncol,domain->gasvectors.newlocalLength,
                                           domain->gasvectors.reduceid,
                                           domain->gasvectors.allreduce,
                                           domain->params.comm_rank,
                                           domain->params.comm_size); // Overlapped copy of x vector
  InitializeVector(domain->b_computed, nrow,domain->gasvectors.newlocalLength,
                                            domain->gasvectors.reduceid,
                                            domain->gasvectors.allreduce,
                                            domain->params.comm_rank,
                                            domain->params.comm_size); // Computed RHS vector

  // Record execution time of reference SpMV and MG kernels for reporting times
  // First load vector with random values
  FillRandomVector(domain->x_overlap);

  int ierr;
  int numberOfCalls = 10;
  double t_begin = mytimer();
  for (int i=0; i< numberOfCalls; ++i) {
    ierr = ComputeSPMV_ref(domain->A, domain->x_overlap, domain->b_computed,domain); // b_computed = A*x_overlap
    if (ierr) HPCG_fout << "Error in call to SpMV: " << ierr << ".\n" << std::endl;
    ierr = ComputeMG_ref(domain->A, domain->b_computed, domain->x_overlap,domain); // b_computed = Minv*y_overlap
    if (ierr) HPCG_fout << "Error in call to MG: " << ierr << ".\n" << std::endl;
  }
  domain->times[8] = (mytimer() - t_begin)/((double) numberOfCalls);  // Total time divided by number of calls.
  return HPX_SUCCESS;
}

int _cg_ref_action(int *nmglevels, size_t size) {
  hpx_addr_t local = hpx_thread_current_target();
  Domain *domain = NULL;
  if (!hpx_gas_try_pin(local, (void**)&domain))
    return HPX_RESEND;

  //hpx_thread_set_affinity(domain->params.comm_rank%hpx_get_num_threads());

  int global_failure = 0; // assume all is well: no failures
  int numberOfMgLevels = *nmglevels;

  int niters = 0;
  int totalNiters_ref = 0;
  double normr = 0.0;
  double normr0 = 0.0;
  int refMaxIters = 50;
  int numberOfCalls = 1; // Only need to run the residual reduction analysis once
  int ierr;
  int rank = domain->params.comm_rank;

  // Compute the residual reduction for the natural ordering and reference kernels
  std::vector< double > ref_times(9,0.0);
  double tolerance = 0.0; // Set tolerance to zero to make all runs do maxIters iterations
  int err_count = 0;
  for (int i=0; i< numberOfCalls; ++i) {
    ZeroVector(domain->x);
    ierr = CG_ref( domain->A, domain->data, domain->b, domain->x, refMaxIters, tolerance, niters, normr, normr0, &ref_times[0], true,domain);
    if (ierr) ++err_count; // count the number of errors in CG
    totalNiters_ref += niters;
  }
  if (rank == 0 && err_count) HPCG_fout << err_count << " error(s) in call(s) to reference CG." << std::endl;
  double refTolerance = normr / normr0;

  //////////////////////////////
  // Optimized CG Setup Phase //
  //////////////////////////////

  niters = 0;
  normr = 0.0;
  normr0 = 0.0;
  err_count = 0;
  int tolerance_failures = 0;

  int optMaxIters = 10*refMaxIters;
  int optNiters = 0;
  double opt_worst_time = 0.0;

  std::vector< double > opt_times(9,0.0);

  // Compute the residual reduction and residual count for the user ordering and optimized kernels.
  for (int i=0; i< numberOfCalls; ++i) {
    ZeroVector(domain->x); // start x at all zeros
    double last_cummulative_time = opt_times[0];
    ierr = CG( domain->A, domain->data, domain->b, domain->x, optMaxIters, refTolerance, niters, normr, normr0, &opt_times[0], true,domain);
    if (ierr) ++err_count; // count the number of errors in CG
    if (normr / normr0 > refTolerance) ++tolerance_failures; // the number of failures to reduce residual
    // pick the largest number of iterations to guarantee convergence
    if (niters > optNiters) optNiters = niters;

    double current_time = opt_times[0] - last_cummulative_time;
    if (current_time > opt_worst_time) opt_worst_time = current_time;
  }

  // Get the absolute worst time
  double local_opt_worst_time = opt_worst_time;

  computeresidual_reduction(&opt_worst_time,local_opt_worst_time,domain->reduce.dreduction_max,
                            domain->reduce.dallreduce_max,domain->params.comm_rank);

  if (rank == 0 && err_count) HPCG_fout << err_count << " error(s) in call(s) to optimized CG." << std::endl;
  if (tolerance_failures) {
    global_failure = 1;
    if (rank == 0)
      HPCG_fout << "Failed to reduce the residual " << tolerance_failures << " times." << std::endl;
  }

  ///////////////////////////////
  // Optimized CG Timing Phase //
  ///////////////////////////////

  // Here we finally run the benchmark phase
  // The variable total_runtime is the target benchmark execution time in seconds

  double total_runtime = gparams.runningTime;
  int numberOfCgSets = int(total_runtime / opt_worst_time) + 1; // Run at least once, account for rounding
  if (rank==0) {
    HPCG_fout << "Projected running time: " << total_runtime << " seconds" << std::endl;
    HPCG_fout << "Number of CG sets: " << numberOfCgSets << std::endl;
  }

  /* This is the timed run for a specified amount of time. */
  optMaxIters = optNiters;
  double optTolerance = 0.0;  // Force optMaxIters iterations
  TestNormsData testnorms_data;
  testnorms_data.samples = numberOfCgSets;
  testnorms_data.values = new double[numberOfCgSets];

  for (int i=0; i< numberOfCgSets; ++i) {
    ZeroVector(domain->x); // Zero out x
    ierr = CG( domain->A, domain->data, domain->b, domain->x, optMaxIters, optTolerance, niters, normr, normr0, &domain->times[0], true,domain);
    if (ierr) HPCG_fout << "Error in call to CG: " << ierr << ".\n" << std::endl;
    if (rank==0) HPCG_fout << "Call [" << i << "] Scaled Residual [" << normr/normr0 << "]" << std::endl;
    testnorms_data.values[i] = normr/normr0; // Record scaled residual from this run
  }

  // Test Norm Results
  ierr = TestNorms(testnorms_data);

  ////////////////////
  // Report Results //
  ////////////////////

  // Report results to YAML file
  ReportResults(domain->A, numberOfMgLevels, numberOfCgSets, refMaxIters, optMaxIters, &domain->times[0], domain->testcg_data, domain->testsymmetry_data, testnorms_data, global_failure,domain);

  // Report exchange halo time
  double global_result;
  printf(" Exchange time: %g\n",domain->eh_t);
  printf(" Exchange time per messages: %g\n",domain->eh_t /
         domain->exchange_messages);
  printf(" Exchange time per byte: %g\n",domain->eh_t / domain->exchange_bytes);
  dotproduct_reduction(&global_result,domain->eh_t,
        domain->reduce.dreduction_sum,
        domain->reduce.dallreduce,domain->params.comm_rank);
  if ( domain->params.comm_rank == 0 ) {
    printf(" Exchange halo cumulative time: %g\n",global_result);
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _generategeometry_action(Domain *domain) {
  //printf(" TEST nx %d domains %d\n",domain->params.nx,domain->params.comm_size);

  GenerateGeometry(domain->params.comm_size,domain->params.comm_rank,domain->params.numThreads,
                   domain->params.nx,domain->params.ny,domain->params.nz,&domain->geom);
  return HPX_SUCCESS;
}

int _initializesparsematrix_action(Domain *domain) {
  InitializeSparseMatrix(domain->A,&(domain->geom));
  return HPX_SUCCESS;
}

int _generateproblem_action(Domain *domain) {
  GenerateProblem(domain->A,&domain->b,&domain->x,&domain->xexact,domain);
  return HPX_SUCCESS;
}

int _setuphalo_action(Domain *domain) {
  SetupHalo(domain->A,domain);
  return HPX_SUCCESS;
}

int _generatecoarse_action(int *numberOfMgLevels, size_t size) {
  hpx_addr_t local = hpx_thread_current_target();
  Domain *domain = NULL;
  if (!hpx_gas_try_pin(local, (void**)&domain))
    return HPX_RESEND;

  SparseMatrix * curLevelMatrix = &domain->A;
  for (int level = 1; level< *numberOfMgLevels; ++level) {
        GenerateCoarseProblem(*curLevelMatrix,domain);
        curLevelMatrix = curLevelMatrix->Ac; // Make the just-constructed coarse grid the next level
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _initializesparsecgdata_action(Domain *domain) {
  InitializeSparseCGData(domain->A, domain->data,
                         domain->gasvectors.newlocalLength,
                         domain->gasvectors.reduceid,
                         domain->gasvectors.allreduce,
                         domain->params.comm_rank,
                         domain->params.comm_size);
  return HPX_SUCCESS;
}

int _optimizeproblem_action(Domain *domain) {
  double t7 = mytimer(); OptimizeProblem(domain->A, domain->b, domain->x, domain->xexact); t7 = mytimer() - t7;
  domain->times[7] = t7;
  return HPX_SUCCESS;
}

static void initint_handler(global_int_t *input, const size_t size) {
  assert(sizeof(global_int_t) == size);
  *input = 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, initint, initint_handler);

static void sumint_handler(global_int_t *output, const global_int_t *input, const size_t size) {
  assert(sizeof(global_int_t) == size);
  *output += *input;
}
static HPX_ACTION(HPX_FUNCTION, 0, sumint, sumint_handler);

static void initdouble_handler(double *input, const size_t size) {
  assert(sizeof(double) == size);
  *input = 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, initdouble, initdouble_handler);

static void sumdouble_handler(double *output, const double *input, const size_t size) {
  assert(sizeof(double) == size);
  *output += *input;
}
static HPX_ACTION(HPX_FUNCTION, 0, sumdouble, sumdouble_handler);

static void maxdouble_handler(double *output, const double *input, const size_t size) {
  assert(sizeof(double) == size);
  if ( *output < *input ) *output = *input;
}
static HPX_ACTION(HPX_FUNCTION, 0, maxdouble, maxdouble_handler);

static void mindouble_handler(double *output, const double *input, const size_t size) {
  assert(sizeof(double) == size);
  if ( *output > *input ) *output = *input;
}
static HPX_ACTION(HPX_FUNCTION, 0, mindouble, mindouble_handler);

static int _param_action(HPCG_Params *cfg, size_t size) {
  if (hpx_get_my_rank() != 0)
    gparams = *cfg;

  gparams.comm_rank = hpx_get_my_rank();
  gparams.comm_size = hpx_get_num_ranks();
  gparams.numThreads = hpx_get_num_threads();

  global_config.num_domains = gparams.nDoms;
  global_config.core_major = gparams.core_major;

  return HPX_SUCCESS;
}

static void register_actions(void);

int _initnodereduce_action(DP *ld,size_t size) {

  node_local = (double *) calloc(ld->nDoms*max_dot_gen,sizeof(double));

  int locality_count = ld->locality_count;
  phoenix_lco[0] = hpx_lco_and_new(locality_count);
  phoenix_lco[1] = hpx_lco_and_new(locality_count);

  phoenix_finish_lco = (hpx_addr_t *) malloc(sizeof(hpx_addr_t) * max_dot_gen);
  for (int i=0;i<max_dot_gen;i++) {
    phoenix_finish_lco[i] = hpx_lco_and_new(1);
  }

  return HPX_SUCCESS;
}

int _initDomain_action(CP *ld,size_t size) {

  hpx_addr_t local = hpx_thread_current_target();
  Domain *domain = NULL;
  if (!hpx_gas_try_pin(local, (void**)&domain))
    return HPX_RESEND;

  new((void *) domain) Domain();

  // set up affinity for the domain
  int affinity = ld->index % hpx_get_num_threads();
  hpx_gas_set_affinity(local, affinity);

  domain->params.comm_rank = ld->index;
  domain->params.comm_size = ld->nDoms;
  domain->params.numThreads = hpx_get_num_threads();

  domain->params.no_spmv_parfor = ld->no_spmv_parfor;
  domain->params.aggregate_parfor = ld->aggregate_parfor;
  domain->params.num_chunks = ld->num_chunks;
  domain->params.use_memput = ld->use_memput;
  domain->params.use_memput_copyless = ld->use_memput_copyless;

  domain->generation = 0;
  domain->dot_generation = 0;
  domain->gen = 0;
  domain->eh_t = 0.0;
  domain->exchange_messages = 0;
  domain->exchange_bytes = 0;

  // Copy over the rest of the global parameters to the domain specific parameters
  domain->params.nx = gparams.nx;
  domain->params.ny = gparams.ny;
  domain->params.nz = gparams.nz;
  domain->params.runningTime = gparams.runningTime;

  // Store the locality information
  // this is only used if use_mpi_allreduce is activated
  int sendBuffer[1];
  sendBuffer[0] = HPX_LOCALITY_ID;
  hpx_addr_t destination = hpx_addr_add(ld->mpi_hpx_local,ld->index*sizeof(int),ld->nDoms*sizeof(int));
  hpx_gas_memput_rsync(destination,sendBuffer,sizeof(int));
  // ---------

  if ( domain->params.use_memput == 1 || domain->params.use_memput_copyless == 1 ) {
    domain->gasvectors.xt[0] = ld->e_xt;
    domain->gasvectors.xt[1] = ld->f_xt;
    domain->gasvectors.send[0] = ld->g_xt;
    domain->gasvectors.send[1] = ld->h_xt;
    domain->gasvectors.lco_xt = ld->lco_xt;
    domain->gasvectors.lco_xt2 = ld->lco_xt2;
    domain->gasvectors.rn_order = ld->rn_order;
    domain->gasvectors.grecv_length = ld->grecv_length;
    for (int i = 0; i < 4; ++i) {
      domain->gasvectors.rn_complete[i] = ld->rn_complete[i];
      domain->gasvectors.r_complete[i] = ld->r_complete[i];
    }
    if ( domain->params.use_memput_copyless == 1 ) {
      for (int i=0;i<num_and;i++) {
        domain->gasvectors.gen_complete[i] = ld->gen_complete[i];
      }
    }
  }
  domain->gasvectors.newlocalLength = ld->newlocalLength;
  domain->gasvectors.allreduce = hpx_lco_future_new(sizeof(int));
  domain->gasvectors.reduceid =
             hpx_process_collective_allreduce_subscribe(domain->gasvectors.newlocalLength,
                                                        hpx_lco_set_action,
                                                        domain->gasvectors.allreduce);

  domain->base = ld->base;

  domain->reduce.allreduce = hpx_lco_future_new(sizeof(global_int_t));
  domain->reduce.reduction_sum = ld->reduction_sum;

  domain->reduce.dallreduce = hpx_lco_future_new(sizeof(double));
  domain->reduce.dreduction_sum = ld->dreduction_sum;

  domain->reduce.dallreduce_max = hpx_lco_future_new(sizeof(double));
  domain->reduce.dreduction_max = ld->dreduction_max;

  domain->reduce.dallreduce_min = hpx_lco_future_new(sizeof(double));
  domain->reduce.dreduction_min = ld->dreduction_min;

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

static int _main_action(int no_spmv_parfor,int aggregate_parfor,int num_chunks,int use_memput,int use_global_vector,int use_memput_copyless,int nDoms,int cm,int use_mpi_reduce) {

  if ( nDoms == -1 ) nDoms = HPX_LOCALITIES;

  bool core_major;
  if ( cm == 0 ) {
    core_major = false;
  } else if ( cm == 1 ) {
    core_major = true;
  } else {
    hpx_abort();
  }
  global_config.num_domains = nDoms;
  global_config.core_major = core_major;

  gparams.nDoms = nDoms;
  gparams.core_major = core_major;

  hpx_addr_t domain = hpx_gas_alloc_cyclic(nDoms,sizeof(Domain),0);

  // Initialize the domains
  hpx_addr_t newlocalLength = hpx_process_collective_allreduce_new(sizeof(int),
                                                          initmaxint, maxint);

  gparams.vector_future_lco = hpx_lco_future_new(sizeof(hpx_addr_t));
  gparams.vector_wait_lco = hpx_lco_and_new(nDoms);
  gparams.vector_wait2_lco = hpx_lco_and_new(nDoms);
  gparams.vector_wait3_lco = hpx_lco_and_new(nDoms);

  gparams.use_global_vector = use_global_vector;
  gparams.use_mpi_reduce = use_mpi_reduce;

  hpx_bcast_rsync(_param, &gparams, sizeof(gparams));

  hpx_addr_t reduction_sum = hpx_lco_allreduce_new(nDoms,nDoms, sizeof(global_int_t),
                                           initint,
                                           sumint);

  hpx_addr_t dreduction_sum = hpx_lco_allreduce_new(nDoms,nDoms, sizeof(double),
                                           initdouble,
                                           sumdouble);

  hpx_addr_t dreduction_max = hpx_lco_allreduce_new(nDoms,nDoms, sizeof(double),
                                           initdouble,
                                           maxdouble);

  hpx_addr_t dreduction_min = hpx_lco_allreduce_new(nDoms,nDoms, sizeof(double),
                                           initdouble,
                                           mindouble);
  hpx_addr_t e_xt,f_xt,g_xt,h_xt,lco_xt,lco_xt2;
  hpx_addr_t rn_order,grecv_length;
  hpx_addr_t rn_complete[4];
  hpx_addr_t r_complete[4];
  hpx_addr_t *gen_complete = (hpx_addr_t*)malloc(num_and * sizeof(hpx_addr_t));
  if ( use_memput == 1 || use_memput_copyless == 1 ) {
    // AGAS vectors
    e_xt = hpx_gas_calloc_cyclic(nDoms,gparams.nx*gparams.ny*gparams.nz*sizeof(double), 0);
    f_xt = hpx_gas_calloc_cyclic(nDoms,gparams.nx*gparams.ny*gparams.nz*sizeof(double), 0);
    g_xt = hpx_gas_calloc_cyclic(nDoms,gparams.nx*gparams.ny*gparams.nz*sizeof(double), 0);
    h_xt = hpx_gas_calloc_cyclic(nDoms,gparams.nx*gparams.ny*gparams.nz*sizeof(double), 0);
    lco_xt = hpx_gas_calloc_cyclic(nDoms,num_and*sizeof(hpx_addr_t), 0);
    lco_xt2 = hpx_gas_calloc_cyclic(nDoms,num_and*sizeof(hpx_addr_t), 0);
    rn_order = hpx_gas_calloc_cyclic(nDoms,nDoms*sizeof(int), 0);
    grecv_length = hpx_gas_calloc_cyclic(nDoms,nDoms*sizeof(int), 0);

    for (int i = 0; i < 4; ++i) {
      rn_complete[i] = hpx_lco_and_new(nDoms);
      r_complete[i] = hpx_lco_and_new(nDoms);
    }
    if ( use_memput_copyless == 1 ) {
      for (int i=0;i<num_and;i++) {
        gen_complete[i] = hpx_lco_and_new(nDoms);
      }
    }
  }

  hpx_addr_t mpi_hpx_local = hpx_gas_alloc_local(1,nDoms*sizeof(int),0);

  hpx_addr_t init = hpx_lco_and_new(nDoms);
  for (int k = 0; k < nDoms; k++) {
    hpx_addr_t block = get_domain_addr(domain, k);

    CP *ld = (CP*) malloc(sizeof(*ld));
    ld->base = domain;
    ld->reduction_sum = reduction_sum;
    ld->dreduction_sum = dreduction_sum;
    ld->dreduction_max = dreduction_max;
    ld->dreduction_min = dreduction_min;
    ld->mpi_hpx_local = mpi_hpx_local;

    ld->index = k;
    ld->nDoms = nDoms;

    ld->no_spmv_parfor = no_spmv_parfor;
    ld->aggregate_parfor = aggregate_parfor;
    ld->num_chunks = num_chunks;
    ld->use_memput = use_memput;
    ld->use_memput_copyless = use_memput_copyless;

    if ( use_memput == 1 || use_memput_copyless == 1 ) {
      ld->e_xt = e_xt;
      ld->f_xt = f_xt;
      ld->g_xt = g_xt;
      ld->h_xt = h_xt;
      ld->lco_xt = lco_xt;
      ld->lco_xt2 = lco_xt2;
      ld->rn_order = rn_order;
      ld->grecv_length = grecv_length;
      for (int j = 0; j < 4; ++j) {
        ld->rn_complete[j] = rn_complete[j];
        ld->r_complete[j] = r_complete[j];
      }
      if ( use_memput_copyless == 1 ) {
        for (int j=0;j<num_and;j++) {
          ld->gen_complete[j] = gen_complete[j];
        }
      }
    }
    ld->newlocalLength = newlocalLength;

    hpx_call(block,_initDomain,init,ld,sizeof(*ld));
    free(ld);
  }
  hpx_lco_wait_reset(init);

  hpx_addr_t complete;
  if ( use_mpi_reduce == 1 ) {
    // Get the locality information
    int *locality_info = NULL;
    hpx_gas_try_pin(mpi_hpx_local, (void**)&locality_info);
    int *locality_counter;
    locality_counter = (int *) calloc(hpx_get_num_ranks(),sizeof(int));
    for (int i=0;i<nDoms;i++) {
      locality_counter[ locality_info[i] ] += 1;
      //printf(" TEST locality info %d %d # ranks %d\n",i,locality_info[i],hpx_get_num_ranks());
    }
    hpx_gas_unpin(mpi_hpx_local);

    complete = hpx_lco_and_new(hpx_get_num_ranks());
    for (int i=0;i<hpx_get_num_ranks();i++) {
      hpx_addr_t block = get_domain_addr(domain, i);

      DP *ld = (DP*) malloc(sizeof(*ld));
      ld->locality_count = locality_counter[ locality_info[i] ];
      ld->nDoms = nDoms;

      hpx_call(block,_initnodereduce,complete,ld,sizeof(*ld));
    }
    hpx_lco_wait(complete);
    hpx_lco_delete(complete, HPX_NULL);
  }

  // Problem setup phase
  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_generategeometry,complete);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_initializesparsematrix,complete);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_generateproblem,complete);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_setuphalo,complete);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  int numberOfMgLevels = 4;

  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_generatecoarse,complete,&numberOfMgLevels,sizeof(numberOfMgLevels));
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_initializesparsecgdata,complete);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_optimizeproblem,complete);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_setuplcos,complete);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  //////////////////////////////
  // Validation Testing Phase //
  //////////////////////////////
  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_testcg,complete);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  ///////////////////////////////////////
  // Reference SpMV+MG Timing Phase //
  ///////////////////////////////////////
  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_spmv_mg,complete);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  ///////////////////////////////
  // Reference CG Timing Phase //
  ///////////////////////////////
  complete = hpx_lco_and_new(nDoms);
  for (int i=0;i<nDoms;i++) {
    hpx_addr_t block = get_domain_addr(domain, i);
    hpx_call(block,_cg_ref,complete,&numberOfMgLevels,sizeof(numberOfMgLevels));
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  free(gen_complete);

  hpx_exit(0, NULL);

  return 0;
}

static HPX_ACTION(HPX_DEFAULT, 0, _main, _main_action, HPX_INT, HPX_INT,
                  HPX_INT,HPX_INT,HPX_INT,HPX_INT,HPX_INT,HPX_INT,HPX_INT);

int main(int argc, char *argv[]) {
  register_actions();

  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "Failed to initialize hpx\n");
    return -1;
  }

  if ( HPX_LOCALITY_ID == 0 ) {
    HPCG_Init(&argc,&argv);
  }

  int no_spmv_parfor = 0;
  int aggregate_parfor = 0;
  int num_chunks = 15;
  int use_memput = 0;
  int use_memput_copyless = 0;
  int use_global_vector = 0;
  int use_mpi_reduce = 0;
  int nDoms = -1;
  int core_major = 0;

  int opt = 0;
  while ((opt = getopt(argc, argv, "n:x:m:o:d:c:p:h?")) != -1) {
    switch (opt) {
      case 'n':
        no_spmv_parfor = 1;
        printf(" Option turning off spmv parfor selected.\n");
        break;
      case 'x':
        aggregate_parfor = 1;
        num_chunks = atoi(optarg);
        printf(" Option aggregating parfor selected.  Number of chunks: %d\n",num_chunks);
        break;
      case 'm':
        use_memput = 1;
        printf(" Option memput enabled.\n");
        break;
      case 'o':
        use_memput_copyless = 1;
    use_global_vector = 1;
        printf(" Option memput copyless and global vector enabled.\n");
        break;
      case 'p':
        use_mpi_reduce = 1;
        printf(" Option to use MPI allreduce instead of process allreduce enabled.\n");
        break;
      case 'd':
        nDoms = atoi(optarg);
        printf(" Option number of domains: %d\n",nDoms);
        break;
      case 'c':
        core_major = 1;
        printf(" Core major enabled.  Be sure you are actually overdecomposing!\n");
        break;
      case 'h':
        printf("Usage:  hpcg will read the hpcg.dat file for parameters.\n");
        printf("There are a few command line parameters to tune and experiment with the HPX-5 implementation.\n");
        printf("hpcg [options]\n");
        printf("\t -d <number of domains>, turns on overdecomposition\n");
        printf("\t -n <any int>, turns off SpMV parallel for\n");
        printf("\t -x <chunksize>, turns off SpMV HPX parallel for; uses chunksize to aggregate\n");
        printf("\t -m <any int>, turns on memput copy modality, turns off parcels\n");
        printf("\t -o <any int>, turns on memput copyless modality, turns off parcels\n");
        printf("\t -p <any int>, turns on MPI allreduce option\n");
        printf("\t -c <any int>, turns on core major decomposition\n");
        printf("\t -h, help\n");
        return 0;
    }
  }

  e = hpx_run(&_main,NULL,&no_spmv_parfor,&aggregate_parfor,&num_chunks,&use_memput,&use_global_vector,
                        &use_memput_copyless,&nDoms,&core_major,&use_mpi_reduce);
  hpx_finalize();
  return e;
}

void register_actions(void) {
  /* register action for parcel (must be done by all ranks) */
  //HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _main, _main_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _param, _param_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _initnodereduce, _initnodereduce_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, 0, _print, _print_action);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, _generategeometry, _generategeometry_action, HPX_POINTER);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _initDomain, _initDomain_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, _initializesparsematrix, _initializesparsematrix_action, HPX_POINTER);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, _generateproblem, _generateproblem_action, HPX_POINTER);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, _setuphalo, _setuphalo_action, HPX_POINTER);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _generatecoarse, _generatecoarse_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, _initializesparsecgdata, _initializesparsecgdata_action, HPX_POINTER);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, _optimizeproblem, _optimizeproblem_action, HPX_POINTER);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, _setuplcos, _setuplcos_action, HPX_POINTER);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, _testcg, _testcg_action, HPX_POINTER);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _exchange_sends, _exchange_sends_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, _exchange_result, _exchange_result_action, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _spmv_loop, _spmv_loop_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, _spmv_mg, _spmv_mg_action, HPX_POINTER);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _cg_ref, _cg_ref_action, HPX_POINTER, HPX_SIZE_T);
}
