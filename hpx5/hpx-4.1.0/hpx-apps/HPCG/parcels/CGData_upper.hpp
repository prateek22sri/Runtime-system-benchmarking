
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
 @file CGData.hpp

 HPCG data structure
 */

#ifndef CGDATA_upper_HPP
#define CGDATA_upper_HPP

#include "SparseMatrix.hpp"
#include "Vector.hpp"

struct CGData_STRUCT {
  Vector r; //!< pointer to residual vector
  Vector z; //!< pointer to preconditioned residual vector
  Vector p; //!< pointer to direction vector
  Vector Ap; //!< pointer to Krylov vector
};
typedef struct CGData_STRUCT CGData;

#endif // CGDATA_HPP

