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
#include <iostream>

typedef int local_int_t;
typedef long long global_int_t;

void copyvector2_hpx(double *x_external,local_int_t *receiveLength,
                     int num_neighbors,int localNumberOfRows,int *neighbors,hpx_addr_t base,
                     int localLength,int n,Domain *domain);

hpx_addr_t get_vector_addr(hpx_addr_t base, int index,int newlocalLength);

void memput_exchange_helper(int num_neighbors, local_int_t *sendLength,
                          local_int_t totalToBeSent,local_int_t * elementsToSend,
                          double *xv,double *sendBuffer,int *neighbors,
                          local_int_t localNumberOfRows,local_int_t * receiveLength,
                          hpx_addr_t base,local_int_t newlocalLength,local_int_t localLength,
                          int *rn_order, int *grecv_length,
                          int generation,Domain *domain)
{
  int n;
  if ( generation < num_and ) {
    n = generation;
  } else {
    n = generation % num_and;
  }

  if ( domain->params.use_memput_copyless == 1 ) {
    if ( generation > 0 ) {
      int nm1,nm2;
      if ( generation < num_and ) {
        nm1 = generation-1;
      } else {
        nm1 = (generation-1) % num_and;
      }
      hpx_lco_wait(domain->gasvectors.gen_complete[nm1]);
     
      // reset two generations ago
      if ( domain->params.comm_rank == 0 ) { 
        if ( generation > 1 ) {
          int nm2;
          if ( generation < num_and ) {
            nm2 = generation-2;
          } else {
            nm2 = (generation-2) % num_and;
          }
          hpx_lco_reset_sync(domain->gasvectors.gen_complete[nm2]);
        }
      }
    }
  }

  for (local_int_t i=0; i<totalToBeSent; i++) sendBuffer[i] = xv[elementsToSend[i]]; 

  double * x_external = (double *) xv + localNumberOfRows;

  int offset,length;

  if ( domain->params.use_memput == 1 ) {
    length = domain->params.nx*domain->params.ny*domain->params.nz;
  }

  // perform a memput to each of the neighbors  
  for (int i = 0; i < num_neighbors; i++) {
    local_int_t n_send = sendLength[i];

    offset = 0;
    for (int j=0;j<rn_order[neighbors[i]];j++) offset += grecv_length[j + neighbors[i]*domain->params.comm_size];

    if ( domain->params.use_memput == 1 ) {
      hpx_addr_t destination = hpx_addr_add(domain->gasvectors.xt[n],
                    offset*sizeof(double) + localNumberOfRows*sizeof(double) +
                  neighbors[i]*length*sizeof(double),length*sizeof(double)); 
 
      hpx_gas_memput_lsync(destination,sendBuffer,n_send*sizeof(double), 
                           domain->lcos.memput_and[n + neighbors[i]*num_and]);
    }

    if ( domain->params.use_memput_copyless == 1 ) {
      hpx_addr_t rank_destination = get_vector_addr(base,neighbors[i],newlocalLength);
      hpx_addr_t test_destination = hpx_addr_add(rank_destination,
                    offset*sizeof(double) + localNumberOfRows*sizeof(double),
                    newlocalLength*sizeof(double)); 

      hpx_gas_memput_lsync(test_destination,sendBuffer,n_send*sizeof(double), 
                           domain->lcos.memput_and2[n + neighbors[i]*num_and]);
    }

    sendBuffer += n_send;
  }
  if ( domain->params.use_memput == 1 ) {
    hpx_lco_wait_reset(domain->lcos.memput_and[n+domain->params.comm_rank*num_and]);
  }
  if ( domain->params.use_memput_copyless == 1 ) {
    hpx_lco_wait_reset(domain->lcos.memput_and2[n+domain->params.comm_rank*num_and]);
  }
  

  // All the data is here now -- put it in xv
  if ( domain->params.use_memput == 1 ) {
    copyvector2_hpx(x_external,receiveLength,num_neighbors,localNumberOfRows,neighbors,base,
                     newlocalLength,n,domain);
  }

  if ( domain->params.use_memput_copyless == 1 ) {
    // do not mix generations
    hpx_lco_set(domain->gasvectors.gen_complete[n], 0, NULL, HPX_NULL, HPX_NULL);
  }
}

