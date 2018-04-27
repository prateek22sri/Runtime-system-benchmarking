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

#include "hpcg.h"
#ifdef USE_HPCG_MPI
#include <mpi.h>
#endif

typedef int local_int_t;
typedef long long global_int_t;

extern double *node_local;
extern hpx_addr_t phoenix_lco[2];
extern hpx_addr_t *phoenix_finish_lco;

double tglobal_result;

void generateproblem_reduction(global_int_t * totalNumberOfNonzeros,local_int_t localNumberOfNonzeros,
                           hpx_addr_t reduction_sum,hpx_addr_t allreduce,int rank)
{
  global_int_t lnnz = localNumberOfNonzeros;
  hpx_lco_allreduce_join(reduction_sum, rank, sizeof(global_int_t), &lnnz,
                             hpx_lco_set_action, allreduce);
  hpx_lco_get_reset(allreduce,sizeof(global_int_t),totalNumberOfNonzeros);
}

void dotproduct_reduction(double * global_result,double local_result,
                          hpx_addr_t dreduction_sum,hpx_addr_t dallreduce,int rank)
{
  hpx_lco_allreduce_join(dreduction_sum, rank, sizeof(double), &local_result,
                             hpx_lco_set_action, dallreduce);
  hpx_lco_get_reset(dallreduce,sizeof(double),global_result);
}

void dotproduct_mpi_reduction(double * global_result,double local_result,
                          hpx_addr_t dreduction_sum,hpx_addr_t dallreduce,
                          int rank,int size,int generation)
{
#ifdef HPCG_USE_MPI
  int n = (generation & 1) ? 1 : 0;
  node_local[rank + generation*size] = local_result;

  hpx_lco_and_set(phoenix_lco[n], HPX_NULL);

  if ( rank < hpx_get_num_ranks() ) {
    hpx_lco_wait_reset(phoenix_lco[n]);

    double tlocal_result = 0.0;
    for (int i=rank;i<size;i += hpx_get_num_ranks()) {
      tlocal_result += node_local[i+generation*size];
    }

    tglobal_result = 0.0;
    MPI_Allreduce(&tlocal_result, &tglobal_result, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    //printf(" TEST rank %d generation %d value %g final %g\n",rank,generation,tlocal_result,tglobal_result);
    hpx_lco_and_set(phoenix_finish_lco[generation], HPX_NULL);
  }

  hpx_lco_wait(phoenix_finish_lco[generation]);
  *global_result = tglobal_result;
#else
  hpx_abort();
#endif
}

void computeresidual_reduction(double * global_residual,double local_residual,
                               hpx_addr_t dreduction_max,hpx_addr_t dallreduce_max,int rank)
{
  hpx_lco_allreduce_join(dreduction_max, rank, sizeof(double), &local_residual,
                             hpx_lco_set_action, dallreduce_max);
  hpx_lco_get_reset(dallreduce_max,sizeof(double),global_residual);

}

void dmin_reduction(double * global,double local,
                    hpx_addr_t dreduction_min,hpx_addr_t dallreduce_min,int rank)
{
  hpx_lco_allreduce_join(dreduction_min, rank, sizeof(double), &local,
                             hpx_lco_set_action, dallreduce_min);
  hpx_lco_get_reset(dallreduce_min,sizeof(double),global);

}
