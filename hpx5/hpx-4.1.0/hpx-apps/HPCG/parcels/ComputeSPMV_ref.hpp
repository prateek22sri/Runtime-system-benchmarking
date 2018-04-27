
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

#ifndef COMPUTESPMV_REF_HPP
#define COMPUTESPMV_REF_HPP
#include "hpcg.h"
#include "Vector.hpp"
#include "SparseMatrix.hpp"

int ComputeSPMV_ref( const SparseMatrix & A, Vector  & x, Vector & y,Domain *domain);

void spmv_parfor(double *yv,double *xv,local_int_t nrow,double **matrixValues,
                 local_int_t **mtxIndL,char *nonzerosInRow,Domain *domain);

#endif  // COMPUTESPMV_REF_HPP
