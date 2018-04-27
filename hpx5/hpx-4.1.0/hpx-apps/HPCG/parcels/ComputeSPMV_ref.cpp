
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
 @file ComputeSPMV_ref.cpp

 HPCG routine
 */

#include "ComputeSPMV_ref.hpp"

#include "ExchangeHalo.hpp"

#include <cassert>
#include "hpcg-main.h"

/*!
  Routine to compute matrix vector product y = Ax where:
  Precondition: First call exchange_externals to get off-processor values of x

  This is the reference SPMV implementation.  It CANNOT be modified for the
  purposes of this benchmark.

  @param[in]  A the known system matrix
  @param[in]  x the known vector
  @param[out] y the On exit contains the result: Ax.

  @return returns 0 upon success and non-zero otherwise

  @see ComputeSPMV
*/
int ComputeSPMV_ref( const SparseMatrix & A, Vector & x, Vector & y,Domain *domain) {

  assert(x.localLength>=A.localNumberOfColumns); // Test vector lengths
  assert(y.localLength>=A.localNumberOfRows);

  ExchangeHalo(A,x,domain);
  double * xv = x.values;
  double * yv = y.values;
  local_int_t nrow = A.localNumberOfRows;
  if ( domain->params.no_spmv_parfor == 0 ) { 
    spmv_parfor(yv,xv,nrow,A.matrixValues,A.mtxIndL,A.nonzerosInRow,domain);
  } else {
    for (local_int_t i=0; i< nrow; i++)  {
      double sum = 0.0;
      const double * const cur_vals = A.matrixValues[i];
      const local_int_t * const cur_inds = A.mtxIndL[i];
      const int cur_nnz = A.nonzerosInRow[i];

      for (int j=0; j< cur_nnz; j++)
        sum += cur_vals[j]*xv[cur_inds[j]];
      yv[i] = sum;
    }
  } 
  return(0);
}
