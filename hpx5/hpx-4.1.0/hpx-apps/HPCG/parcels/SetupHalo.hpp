
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

#ifndef SETUPHALO_HPP
#define SETUPHALO_HPP
#include "SparseMatrix.hpp"
#include "hpcg.h"

void SetupHalo(SparseMatrix & A,Domain *domain);

void set_rn(int *neighbors,int num_neighbors,int *recv_length,
            int * grecv_length, int * rn_order,Domain *domain);


#endif // SETUPHALO_HPP
