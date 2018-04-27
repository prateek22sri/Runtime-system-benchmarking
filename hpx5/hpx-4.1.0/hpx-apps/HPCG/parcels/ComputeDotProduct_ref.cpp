
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
 @file ComputeDotProduct_ref.cpp

 HPCG routine
 */

#include "mytimer.hpp"
#include <cassert>
#include "ComputeDotProduct_ref.hpp"

#include "hpcg-main.h"

extern HPCG_Params gparams;

/*!
  Routine to compute the dot product of two vectors where:

  This is the reference dot-product implementation.  It _CANNOT_ be modified for the
  purposes of this benchmark.

  @param[in] n the number of vector elements (on this processor)
  @param[in] x, y the input vectors
  @param[in] result a pointer to scalar value, on exit will contain result.
  @param[out] time_allreduce the time it took to perform the communication between processes

  @return returns 0 upon success and non-zero otherwise

  @see ComputeDotProduct
*/
int ComputeDotProduct_ref(const local_int_t n, const Vector & x, const Vector & y,
    double & result, double & time_allreduce,Domain *domain) {
  assert(x.localLength>=n); // Test vector lengths
  assert(y.localLength>=n);

  double local_result = 0.0;
  double * xv = x.values;
  double * yv = y.values;
  if (yv==xv) {
    for (local_int_t i=0; i<n; i++) local_result += xv[i]*xv[i];
  } else {
    for (local_int_t i=0; i<n; i++) local_result += xv[i]*yv[i];
  }

  double t0;
  double global_result;
  if ( gparams.use_mpi_reduce == 0 ) {
    t0 = mytimer();
    global_result = 0.0;
    dotproduct_reduction(&global_result,local_result,domain->reduce.dreduction_sum,domain->reduce.dallreduce,domain->params.comm_rank); 
    result = global_result;
    time_allreduce += mytimer() - t0;
  } else if ( gparams.use_mpi_reduce == 1 ) {
    t0 = mytimer();
    global_result = 0.0;
    dotproduct_mpi_reduction(&global_result,local_result,domain->reduce.dreduction_sum,
                            domain->reduce.dallreduce,
                            domain->params.comm_rank,domain->params.comm_size,domain->dot_generation); 
    domain->dot_generation += 1;
    result = global_result;
    time_allreduce += mytimer() - t0;
  }
#if 0
#ifndef HPCG_NOMPI
  // Use MPI's reduce function to collect all partial sums
  double t0 = mytimer();
  double global_result = 0.0;
  MPI_Allreduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM,
      MPI_COMM_WORLD);
  result = global_result;
  time_allreduce += mytimer() - t0;
#else
  result = local_result;
#endif
#endif

  return(0);
}
