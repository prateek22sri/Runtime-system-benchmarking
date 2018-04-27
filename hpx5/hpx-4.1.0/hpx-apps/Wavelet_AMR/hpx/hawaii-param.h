// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

#ifndef HAWAII_PARAM_H
#define HAWAII_PARAM_H
#include <stdbool.h>

#define ns_x 10  // number of points in the x-direction of the base grid 
#define ns_y 10  // number of points in the y-direction of the base grid 
#define ns_z 0  // number of points in the z-direction of the base grid 
#define JJ   10  // maximum number of resolution levels to use 
#define n_dim 2 // dimensionality of the problem

// RK4
#define n_gen 4

#define n_variab 5 // number of primary variables 
#define n_aux 10 // number of auxiliary variables 
#define n_deriv 5 // number of derivatives needed in the right-hand side
#define n_rhs 5 // number of right-hand side functions 

extern double t0; // initial time stamp of the simulation
extern double tf; // final time stamp of the simulation 

//These set up common masking patterns once; they are set in main();
extern int primary_mask[n_variab + n_aux];
extern int primitive_mask[n_variab + n_aux];
extern int auxiliary_mask[n_variab + n_aux];
extern int allvars_mask[n_variab + n_aux];
#endif
