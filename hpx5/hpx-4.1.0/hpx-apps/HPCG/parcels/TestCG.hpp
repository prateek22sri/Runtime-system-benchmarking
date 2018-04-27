
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
 @file TestCG.hpp

 HPCG data structure
 */

#ifndef TESTCG_HPP
#define TESTCG_HPP

#include "SparseMatrix.hpp"
#include "Vector.hpp"
#include "CGData.hpp"
#include "hpcg.h"

#include <fstream>
extern std::ofstream HPCG_fout;


#include "TestCG_upper.hpp"

extern int TestCG(SparseMatrix & A, CGData & data, Vector & b, Vector & x, TestCGData & testcg_data,
                  Domain *domain);

#endif  // TESTCG_HPP

