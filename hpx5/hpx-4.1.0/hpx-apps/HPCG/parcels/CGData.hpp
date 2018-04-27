
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

#ifndef CGDATA_HPP
#define CGDATA_HPP

#include "hpcg.h"
#include "SparseMatrix.hpp"
#include "Vector.hpp"

#include "CGData_upper.hpp"

/*!
 Constructor for the data structure of CG vectors.

 @param[in]  A    the data structure that describes the problem matrix and its structure
 @param[out] data the data structure for CG vectors that will be allocated to get it ready for use in CG iterations
 */
inline void InitializeSparseCGData(SparseMatrix & A, CGData & data,
                   hpx_addr_t newlocalLength,
                   int32_t reduceid,
                   hpx_addr_t allreduce,
                   int rank,int nDoms) {
  local_int_t nrow = A.localNumberOfRows;
  local_int_t ncol = A.localNumberOfColumns;
  InitializeVector(data.r, nrow,newlocalLength,
                                reduceid,
                                allreduce,
                                rank,
                                nDoms);
  InitializeVector(data.z, ncol,newlocalLength,
                                reduceid,
                                allreduce,
                                rank,
                                nDoms);
  InitializeVector(data.p, ncol,newlocalLength,
                                reduceid,
                                allreduce,
                                rank,
                                nDoms);
  InitializeVector(data.Ap, nrow,newlocalLength,
                                 reduceid,
                                 allreduce,
                                 rank,
                                 nDoms);
  return;
}

/*!
 Destructor for the CG vectors data.

 @param[inout] data the CG vectors data structure whose storage is deallocated
 */
inline void DeleteCGData(CGData & data,int rank) {

  DeleteVector (data.r,rank);
  DeleteVector (data.z,rank);
  DeleteVector (data.p,rank);
  DeleteVector (data.Ap,rank);
  return;
}

#endif // CGDATA_HPP

