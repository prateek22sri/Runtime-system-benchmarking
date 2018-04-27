
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
 @file ComputeResidual.cpp

 HPCG routine
 */

#include "Vector.hpp"

#ifdef HPCG_DETAILED_DEBUG
#include <fstream>
#endif

#include <cmath>  // needed for fabs
#include "ComputeResidual.hpp"
#ifdef HPCG_DETAILED_DEBUG
#include <iostream>
#endif

/*!
  Routine to compute the inf-norm difference between two vectors where:

  @param[in]  n        number of vector elements (local to this processor)
  @param[in]  v1, v2   input vectors
  @param[out] residual pointer to scalar value; on exit, will contain result: inf-norm difference

  @return Returns zero on success and a non-zero value otherwise.
*/
int ComputeResidual(const local_int_t n, const Vector & v1, const Vector & v2, double & residual,Domain *domain) {

  double * v1v = v1.values;
  double * v2v = v2.values;
  double local_residual = 0.0;

  {
    double threadlocal_residual = 0.0;
    for (local_int_t i=0; i<n; i++) {
      double diff = std::fabs(v1v[i] - v2v[i]);
      if (diff > threadlocal_residual) threadlocal_residual = diff;
    }
    {
      if (threadlocal_residual>local_residual) local_residual = threadlocal_residual;
    }
  }

  // Use MPI's reduce function to collect all partial sums
  double global_residual = 0;
  computeresidual_reduction(&global_residual,local_residual,domain->reduce.dreduction_max,
                            domain->reduce.dallreduce_max,domain->params.comm_rank); 
  //MPI_Allreduce(&local_residual, &global_residual, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  residual = global_residual;

  return(0);
}
