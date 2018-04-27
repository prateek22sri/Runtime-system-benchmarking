

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

#ifndef EXCHANGEHALO_HPP
#define EXCHANGEHALO_HPP
#include "hpcg.h"
#include "SparseMatrix.hpp"
#include "Vector.hpp"
#include "swapX.hpp"

void ExchangeHalo(const SparseMatrix & A, Vector & x,Domain *domain);

void exchange_halo_helper(int num_neighbors, local_int_t *sendLength,
                          local_int_t * elementsToSend, double *xv,
                          int *neighbors, int generation, Domain *domain);

void memput_exchange_helper(int num_neighbors, local_int_t *sendLength,
                local_int_t totalToBeSent,local_int_t * elementsToSend,
                double *xv,double *sendBuffer,int *neighbors,
                local_int_t localNumberOfRows,local_int_t * receiveLength,
                hpx_addr_t,local_int_t newlocalLength,local_int_t localLength,
                            int *rn_order, int *grecv_length,
                            int generation,Domain *domain);

#endif // EXCHANGEHALO_HPP
