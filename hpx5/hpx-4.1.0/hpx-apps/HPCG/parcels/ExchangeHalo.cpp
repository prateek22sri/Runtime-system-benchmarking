
//@HEADER
// ***************************************************
//
// HPCG: High Performance Conjugate Gradient Benchmark
//
// Contact:
// Michael A. Heroux ( maherou@sandia.gov)
// Jack Dongarra     (dongarra@eecs.utk.edu)
// Piotr Luszczek    (luszczek@eecs.utk.edu)
//
// ***************************************************
//@HEADER

/*!
 @file ExchangeHalo.cpp

 HPCG routine
 */

#include "Geometry.hpp"
#include "ExchangeHalo.hpp"
#include <cstdlib>
#include <stdio.h>
#include "hpcg-main.h"
#include <iostream>

/*!
  Communicates data that is at the border of the part of the domain assigned to this processor.

  @param[in]    A The known system matrix
  @param[inout] x On entry: the local vector entries followed by entries to be communicated; on exit: the vector with non-local entries updated by other processors
 */
void ExchangeHalo(const SparseMatrix & A, Vector & x,Domain *domain) {

  // Extract Matrix pieces

  //local_int_t localNumberOfRows = A.localNumberOfRows;
  int num_neighbors = A.numberOfSendNeighbors;
  //local_int_t * receiveLength = A.receiveLength;
  local_int_t * sendLength = A.sendLength;
  int * neighbors = A.neighbors;
  local_int_t totalToBeSent = A.totalToBeSent;
  local_int_t * elementsToSend = A.elementsToSend;

  double * const xv = x.values;

  if ( domain->params.use_memput == 1 || domain->params.use_memput_copyless == 1 ) {
    local_int_t localNumberOfRows = A.localNumberOfRows;
    local_int_t * receiveLength = A.receiveLength;
    double * sendBuffer = A.sendBuffer;

    //std::cout << " rank " << params.comm_rank  << " TEST in HALO base " << x.addr << std::endl;
    memput_exchange_helper(num_neighbors,sendLength,totalToBeSent,
                       elementsToSend,xv,sendBuffer,neighbors,
                       localNumberOfRows,receiveLength,x.addr,x.newlocalLength,
                       x.localLength,
                       A.rn_order,A.grecv_length,domain->generation,domain);
  } else {
    swapX(A,x,domain->generation,domain);
    exchange_halo_helper(num_neighbors, sendLength, elementsToSend, xv,
                         neighbors, domain->generation, domain);
  }
  domain->generation += 1;
#if 0
  FIXME
  int size, rank; // Number of MPI processes, My process ID
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  //
  //  first post receives, these are immediate receives
  //  Do not wait for result to come, will do that at the
  //  wait call below.
  //

  int MPI_MY_TAG = 99;

  MPI_Request * request = new MPI_Request[num_neighbors];

  //
  // Externals are at end of locals
  //
  double * x_external = (double *) xv + localNumberOfRows;

  // Post receives first
  // TODO: Thread this loop
  for (int i = 0; i < num_neighbors; i++) {
    local_int_t n_recv = receiveLength[i];
    MPI_Irecv(x_external, n_recv, MPI_DOUBLE, neighbors[i], MPI_MY_TAG, MPI_COMM_WORLD, request+i);
    x_external += n_recv;
  }


  //
  // Fill up send buffer
  //

  // TODO: Thread this loop
  for (local_int_t i=0; i<totalToBeSent; i++) sendBuffer[i] = xv[elementsToSend[i]];

  //
  // Send to each neighbor
  //

  // TODO: Thread this loop
  for (int i = 0; i < num_neighbors; i++) {
    local_int_t n_send = sendLength[i];
    MPI_Send(sendBuffer, n_send, MPI_DOUBLE, neighbors[i], MPI_MY_TAG, MPI_COMM_WORLD);
    sendBuffer += n_send;
  }

  //
  // Complete the reads issued above
  //

  MPI_Status status;
  // TODO: Thread this loop
  for (int i = 0; i < num_neighbors; i++) {
    if ( MPI_Wait(request+i, &status) ) {
      std::exit(-1); // TODO: have better error exit
    }
  }

  delete [] request;
#endif
  return;
}
