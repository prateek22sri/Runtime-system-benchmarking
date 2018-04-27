
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

#ifndef GENERATECOARSEPROBLEM_HPP
#define GENERATECOARSEPROBLEM_HPP
#include "SparseMatrix.hpp"
#include "hpcg.h"

void GenerateCoarseProblem(const SparseMatrix & A,Domain *domain);

#endif // GENERATECOARSEPROBLEM_HPP
