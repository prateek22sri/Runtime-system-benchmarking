
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
 @file TestSymmetry.hpp

 HPCG data structures for symmetry testing
 */

#ifndef TESTSYMMETRY_HPP
#define TESTSYMMETRY_HPP

#include "SparseMatrix.hpp"
#include "CGData.hpp"
#include <iostream>
#include "hpcg.h"

extern std::ofstream HPCG_fout;

#include "TestSymmetry_upper.hpp"

extern int TestSymmetry(SparseMatrix & A, Vector & b, Vector & xexact, TestSymmetryData & testsymmetry_data,Domain *domain);

#endif  // TESTSYMMETRY_HPP
