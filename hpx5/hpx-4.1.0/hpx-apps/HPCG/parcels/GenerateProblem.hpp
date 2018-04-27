
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

#ifndef GENERATEPROBLEM_HPP
#define GENERATEPROBLEM_HPP
#include "Geometry.hpp"
#include "SparseMatrix.hpp"
#include "Vector.hpp"
#include "hpcg.h"

void GenerateProblem(SparseMatrix & A, Vector * b, Vector * x, Vector * xexact,
 Domain *domain);


void generateproblem_reduction(global_int_t * totalNumberOfNonzeros,local_int_t localNumberOfNonzeros,
                               hpx_addr_t reduction_sum,hpx_addr_t allreduce,int rank);

#endif // GENERATEPROBLEM_HPP
