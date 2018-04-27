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

void set_rn(int *neighbors,int num_neighbors,int *recv_length,
            int * grecv_length, int * rn_order,Domain *domain)
{

  int rank = domain->params.comm_rank;
  int nDoms = domain->params.comm_size;

  // put the data in the global address space
  for (int i=0;i<num_neighbors;i++) {
    hpx_addr_t desta = hpx_addr_add(domain->gasvectors.rn_order,
                rank*sizeof(int) + 
                neighbors[i]*nDoms*sizeof(int),nDoms*sizeof(int)); 
    hpx_gas_memput_rsync(desta,&i,sizeof(int));
  }

  hpx_addr_t destb = hpx_addr_add(domain->gasvectors.grecv_length,
              rank*nDoms*sizeof(int),nDoms*sizeof(int)); 

  hpx_gas_memput_rsync(destb,recv_length,num_neighbors*sizeof(int));

  hpx_lco_set(domain->gasvectors.rn_complete[domain->gen], 0, NULL, HPX_NULL, HPX_NULL);

  if (domain->gen >= 4)
    printf("WARNING: TOO MANY MATRICES\n");

  // wait for the memputs to finish
  hpx_lco_wait(domain->gasvectors.rn_complete[domain->gen]);

  // once everyone has finished these operations, get what we need
  hpx_addr_t where = hpx_addr_add(domain->gasvectors.rn_order,
                rank*nDoms*sizeof(int),nDoms*sizeof(int)); 
  hpx_gas_memget_sync(rn_order, where, nDoms*sizeof(int)); 

  for (int i = 0; i < nDoms; ++i) {
    hpx_addr_t destc = hpx_addr_add(domain->gasvectors.grecv_length,
      (i*nDoms*sizeof(int)),nDoms*sizeof(int));
    hpx_gas_memget_sync(grecv_length+(i*nDoms),destc,nDoms*sizeof(int));
  }
 
  //printf(" TEST recv length %d\n",grecv_length[0]);

  hpx_lco_set(domain->gasvectors.r_complete[domain->gen], 0, NULL, HPX_NULL, HPX_NULL);

  hpx_lco_wait(domain->gasvectors.r_complete[domain->gen]);

  domain->gen += 1;
  
} 
