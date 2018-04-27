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

typedef int local_int_t;
typedef long long global_int_t;

hpx_action_t _spmv_loop = 0;

struct spmv_helper {
  double *xv;
  double *yv;
  char *nonzerosInRow;
  double **matrixValues;
  local_int_t **mtxIndL;
};

int spmv_loop(int i,void *args)
{
  struct spmv_helper *args1 = (struct spmv_helper *) args;
  double sum = 0.0;
  double * cur_vals = args1->matrixValues[i];
  local_int_t * cur_inds = args1->mtxIndL[i];
  int cur_nnz = args1->nonzerosInRow[i];
  
  for (int j=0;j< cur_nnz; j++) 
    sum += cur_vals[j]*args1->xv[cur_inds[j]];     
  args1->yv[i] = sum;

  return 0;
}

int _spmv_loop_action(SPMV_LOOP *ld,size_t size) {
  for (local_int_t i=ld->start;i<ld->finish;i++) {
    double sum = 0.0;
    double * cur_vals = ld->matrixValues[i];
    local_int_t * cur_inds = ld->mtxIndL[i];
    int cur_nnz = ld->nonzerosInRow[i];
  
    for (int j=0;j< cur_nnz; j++) 
      sum += cur_vals[j]*ld->xv[cur_inds[j]];     
    ld->yv[i] = sum;
  }
  return HPX_SUCCESS;
}

void spmv_parfor(double *yv,double *xv,local_int_t nrow,double **matrixValues,
                 local_int_t **mtxIndL,char *nonzerosInRow,Domain *domain)
{
  struct spmv_helper args;
  args.xv = xv;
  args.yv = yv;
  args.nonzerosInRow = nonzerosInRow;
  args.matrixValues = matrixValues; 
  args.mtxIndL = mtxIndL;
  if (domain->params.aggregate_parfor == 0 ) {
    hpx_par_for_sync(spmv_loop,0,nrow,&args);
  } else {
    int number_of_chunks = domain->params.num_chunks;
    local_int_t *start;
    local_int_t *finish;
    start = (local_int_t *) malloc(number_of_chunks*sizeof(local_int_t));
    finish = (local_int_t *) malloc(number_of_chunks*sizeof(local_int_t));
    local_int_t drow = nrow/number_of_chunks;
    for (int i=0;i<number_of_chunks;i++) {
      start[i] = drow*i;
      finish[i] = drow*(i+1);
      if (i==number_of_chunks-1) {
        finish[i] = nrow;
      }
    }

    hpx_addr_t complete = hpx_lco_and_new(number_of_chunks);
    for (int i=0;i<number_of_chunks;i++) {
      SPMV_LOOP ld;
      ld.xv = xv;
      ld.yv = yv;
      ld.nonzerosInRow = nonzerosInRow;
      ld.matrixValues = matrixValues; 
      ld.mtxIndL = mtxIndL;
      ld.start = start[i]; 
      ld.finish = finish[i];
      hpx_call(HPX_HERE,_spmv_loop,complete,&ld,sizeof(ld));
    }
    hpx_lco_wait(complete);
    hpx_lco_delete(complete, HPX_NULL);
    free(start);
    free(finish);
  }
}

