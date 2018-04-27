
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

#ifndef COMPUTEDOTPRODUCT_REF_HPP
#define COMPUTEDOTPRODUCT_REF_HPP
#include "hpcg.h"
#include "Vector.hpp"
int ComputeDotProduct_ref(const local_int_t n, const Vector & x, const Vector & y,
    double & result, double & time_allreduce,Domain *domain);

void dotproduct_reduction(double * global_result,double local_result,
                         hpx_addr_t dreduction_sum,hpx_addr_t dallreduce,int rank);

#endif // COMPUTEDOTPRODUCT_REF_HPP
