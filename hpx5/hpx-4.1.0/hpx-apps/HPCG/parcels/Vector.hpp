
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
 @file Vector.hpp

 HPCG data structures for dense vectors
 */

#ifndef VECTOR_HPP
#define VECTOR_HPP
#include <cassert>
#include <cstdlib>
#include <iostream>
#include "Geometry.hpp"
#include <hpx/hpx.h>
#include "hpcg-main.h"

extern HPCG_Params gparams;

hpx_addr_t get_vector_addr(hpx_addr_t base, int index,int newlocalLength);

struct Vector_STRUCT {
  local_int_t localLength;  //!< length of local portion of the vector
  local_int_t newlocalLength;  //!< gas length of local portion of the vector
  double * values;          //!< array of values
  hpx_addr_t addr;          //!< global address of the vector
  hpx_addr_t local_addr;
  /*!
   This is for storing optimized data structres created in OptimizeProblem and
   used inside optimized ComputeSPMV().
   */
  void * optimizationData;

};
typedef struct Vector_STRUCT Vector;

/*!
  Initializes input vector.

  @param[in] v
  @param[in] localLength Length of local portion of input vector
 */
inline void InitializeVector(Vector & v, local_int_t localLength,
                             hpx_addr_t newlocalLength,int32_t reduceid,
                             hpx_addr_t allreduce,
                             int rank,int ndoms) {
  if (gparams.use_global_vector) {
    v.addr = HPX_NULL;
    // A resetable barrier is implemented with 3 LCOs.  It makes for
    // a bit complicated protocol, making sure that one LCO is
    // always ready to be used.
    hpx_process_collective_allreduce_join(newlocalLength, 
                                          reduceid,
                                          sizeof(int), &localLength);
    local_int_t newlocalLength;
    hpx_lco_get_reset(allreduce,sizeof(int),&newlocalLength);
    v.newlocalLength = newlocalLength;
    if (rank == 0) {
      // Assuming that `newlocalLength` is the same on all the nodes.
      // Because block size of an allocation is limited by the number of
      // bits used to represent it, this allocation may limit the
      // problem sizes we will run.  This code is very ad hoc.
      v.addr = hpx_gas_alloc_cyclic(ndoms, newlocalLength * sizeof(double), 0);
      // Share the address with other nodes.
      hpx_lco_set(gparams.vector_future_lco, sizeof(hpx_addr_t), &v.addr, HPX_NULL, HPX_NULL);
      // Make sure everybody is here.
      hpx_lco_and_set(gparams.vector_wait_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait_lco);
      // We need to reset `params.vector_wait3_lco` for the next use.
      hpx_lco_reset_sync(gparams.vector_wait3_lco);
      // Make the sharing future ready for next vector initialization.
      hpx_lco_reset_sync(gparams.vector_future_lco);
      // We need to reset `params.vector_wait3_lco` for the next use.
      hpx_lco_reset_sync(gparams.vector_wait3_lco);
      // Make sure everybody is here.
      hpx_lco_and_set(gparams.vector_wait2_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait2_lco);
      // We need to reset `params.vector_wait_lco` for the next use.
      hpx_lco_reset_sync(gparams.vector_wait_lco);
      // Make sure everybody is here.
      hpx_lco_and_set(gparams.vector_wait3_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait3_lco);
      // We need to reset `params.vector_wait2_lco` for the next use.
      hpx_lco_reset_sync(gparams.vector_wait2_lco);
    } else {
      // Get the address from node 0.
      hpx_lco_get(gparams.vector_future_lco, sizeof(v.addr), &v.addr);
      // Waits corresponding to rank 0.
      hpx_lco_and_set(gparams.vector_wait_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait_lco);
      hpx_lco_and_set(gparams.vector_wait2_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait2_lco);
      hpx_lco_and_set(gparams.vector_wait3_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait3_lco);
    }
    // Relies on consecutive numbering of ranks.  We pin our fragment
    // of the vector.
    v.local_addr = get_vector_addr(v.addr,rank,newlocalLength); 

    // If pin did not succeed, abort (what happened?).
    if (!hpx_gas_try_pin(v.local_addr, (void**) &v.values)) { 
      printf(" FAILURE ! \n");
      hpx_abort();
    } 
  } else {
    v.values = new double[localLength];
  }
  v.localLength = localLength;
  v.optimizationData = 0;
  return;
}

/*!
  Fill the input vector with zero values.

  @param[inout] v - On entrance v is initialized, on exit all its values are zero.
 */
inline void ZeroVector(Vector & v) {
  local_int_t localLength = v.localLength;
  double * vv = v.values;
  for (int i=0; i<localLength; ++i) vv[i] = 0.0;
  return;
}
/*!
  Multiply (scale) a specific vector entry by a given value.

  @param[inout] v Vector to be modified
  @param[in] index Local index of entry to scale
  @param[in] value Value to scale by
 */
inline void ScaleVectorValue(Vector & v, local_int_t index, double value) {
  assert(index>=0 && index < v.localLength);
  double * vv = v.values;
  vv[index] *= value;
  return;
}
/*!
  Fill the input vector with pseudo-random values.

  @param[in] v
 */
inline void FillRandomVector(Vector & v) {
  local_int_t localLength = v.localLength;
  double * vv = v.values;
  for (int i=0; i<localLength; ++i) vv[i] = rand() / (double)(RAND_MAX) + 1.0;
  return;
}
/*!
  Copy input vector to output vector.

  @param[in] v Input vector
  @param[in] w Output vector
 */
inline void CopyVector(const Vector & v, Vector & w) {
  local_int_t localLength = v.localLength;
  assert(w.localLength >= localLength);
  double * vv = v.values;
  double * wv = w.values;
  for (int i=0; i<localLength; ++i) wv[i] = vv[i];
  return;
}


/*!
  Deallocates the members of the data structure of the known system matrix provided they are not 0.

  @param[in] A the known system matrix
 */
inline void DeleteVector(Vector & v,int rank) {
  if (gparams.use_global_vector) {
    // First unpin memory.
    hpx_gas_unpin(v.local_addr);
    if (rank == 0) {
      // A resetable barrier is implemented with 3 LCOs.  It makes for
      // a bit complicated protocol, making sure that one LCO is
      // always ready to be used.
      
      // Make sure everybody is here.
      hpx_lco_and_set(gparams.vector_wait_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait_lco);
      // Free the memory.
      hpx_gas_free_sync(v.addr);
      // We need to reset `params.vector_wait3_lco` for the next use.
      hpx_lco_reset_sync(gparams.vector_wait3_lco);
      // Make sure everybody is here.
      hpx_lco_and_set(gparams.vector_wait2_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait2_lco);
      // We need to reset `params.vector_wait_lco` for the next use.
      hpx_lco_reset_sync(gparams.vector_wait_lco);
      // Make sure everybody is here.
      hpx_lco_and_set(gparams.vector_wait3_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait3_lco);
      // We need to reset `params.vector_wait2_lco` for the next use.
      hpx_lco_reset_sync(gparams.vector_wait2_lco);
    } else {
      // Waits corresponding to rank 0.
      hpx_lco_and_set(gparams.vector_wait_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait_lco);
      hpx_lco_and_set(gparams.vector_wait2_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait2_lco);
      hpx_lco_and_set(gparams.vector_wait3_lco, HPX_NULL);
      hpx_lco_wait(gparams.vector_wait3_lco);
    }
  } else {
    // If we are not using global vectors, just delete the values.
    delete [] v.values;
  }
  v.localLength = 0;
  return;
}

#endif // VECTOR_HPP
