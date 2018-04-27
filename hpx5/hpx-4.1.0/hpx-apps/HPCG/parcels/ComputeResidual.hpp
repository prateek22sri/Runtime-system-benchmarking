
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

#ifndef COMPUTERESIDUAL_HPP
#define COMPUTERESIDUAL_HPP
#include "hpcg.h"
#include "Vector.hpp"
int ComputeResidual(const local_int_t n, const Vector & v1, const Vector & v2, double & residual,
                    Domain *domain);

//void computeresidual_reduction(double * global_residual,double local_residual,
//                               hpx_addr_t dreduction_max,hpx_addr_t dallreduce_max,int rank);

#endif // COMPUTERESIDUAL_HPP
