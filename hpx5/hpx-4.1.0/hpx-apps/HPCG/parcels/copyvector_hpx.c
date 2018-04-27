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

hpx_action_t _assign_vector = 0;
hpx_action_t _printcheck = 0;

int _assign_vector_action(double *value,size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  double *array = NULL; 

  if (!hpx_gas_try_pin(local, (void**)&array))
    return HPX_RESEND;

  // copy everybody over to the array
  memcpy(array,value,size);
  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _printcheck_action(int *rank, size_t size) {
  // make sure the copy is a local copy
  printf("My locality %d %d\n", hpx_get_my_rank(),*rank);
  return HPX_SUCCESS;
}

//void copyvector_hpx(double *values,int varlength,int n)
//{
//  // get the hpx address we are supposed to write to  
//  int length = params.nx*params.ny*params.nz;
//         
//  hpx_addr_t where;
//  where = hpx_addr_add(gasvectors.send[n],
//           params.comm_rank*length*sizeof(double),length*sizeof(double));
//                                             
//  hpx_call_sync(where,_assign_vector,NULL,0,values,varlength*sizeof(double));
//} 

void copyvector2_hpx(double *x_external,local_int_t *receiveLength,
                     int num_neighbors,int localNumberOfRows,int *neighbors,hpx_addr_t base,
                     int localLength,int n,Domain *domain)
{
  int length = domain->params.nx*domain->params.ny*domain->params.nz;
         
  double *recv_buffer;
  recv_buffer = (double *) malloc(length*sizeof(double));
  int offset = 0;
  for (int i = 0; i < num_neighbors; i++) {  
    local_int_t n_recv = receiveLength[i];
    hpx_addr_t where = hpx_addr_add(domain->gasvectors.xt[n],
           offset*sizeof(double) 
         + localNumberOfRows*sizeof(double) 
         + domain->params.comm_rank*length*sizeof(double),length*sizeof(double));

    // make sure the copy is local  
    //hpx_call_sync(where,_printcheck,HPX_NULL,0,&params.comm_rank,sizeof(params.comm_rank));
 
    // this is a local copy 
    hpx_gas_memget_sync(recv_buffer, where, n_recv*sizeof(double)); 

    if ( domain->params.use_memput == 1 && domain->params.use_memput_copyless == 1 ) {
      // This modality can be used for debugging/correctness checking
      if ( fabs(x_external[0]-recv_buffer[0]) > 1.e-5 ) {
        printf("\n\n\n");
        printf(" Difference between puts %g %g\n",x_external[0],recv_buffer[0]);
        printf("rank %d offset %d gen %d\n",domain->params.comm_rank,offset,n);
        printf("\n\n\n");
      }
    }
    memcpy(x_external,recv_buffer,n_recv*sizeof(double));  

    offset += n_recv;
    x_external += n_recv;
  }
  free(recv_buffer);
} 

