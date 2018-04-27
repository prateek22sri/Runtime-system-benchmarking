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
#include "SparseMatrix.hpp"
#include "Vector.hpp"
#include "actions2.h"
#include "string.h"
#include <assert.h>
#include <stdio.h>
#include "hpcg-main.h"

void unpack(NodalArgs *args,int n,Domain *domain)
{
  double * xv;
  local_int_t localNumberOfRows;
  double * x_external;
  int num_neighbors;
  local_int_t * receiveLength;
  int * neighbors;
  if ( n == 0 ) {
    xv = domain->eX->values;  
    localNumberOfRows = domain->eA->localNumberOfRows;
    x_external = (double *) xv + localNumberOfRows;

    num_neighbors = domain->eA->numberOfSendNeighbors;
    receiveLength = domain->eA->receiveLength;
    neighbors = domain->eA->neighbors;
  } else {
    xv = domain->fX->values;  
    localNumberOfRows = domain->fA->localNumberOfRows;
    x_external = (double *) xv + localNumberOfRows;

    num_neighbors = domain->fA->numberOfSendNeighbors;
    receiveLength = domain->fA->receiveLength;
    neighbors = domain->fA->neighbors;
  } 

  for (int i = 0; i < num_neighbors; i++) {
    local_int_t n_recv = receiveLength[i];
    if ( neighbors[i] == args->src ) {
      assert(n_recv == args->send_length);
      memcpy(x_external,args->buf,sizeof(double)*n_recv);
      break;
    }
    x_external += n_recv;
  }

  return;
}
