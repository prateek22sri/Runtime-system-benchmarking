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

#ifndef HAWAII_TYPES_H
#define HAWAII_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "hawaii-param.h"

#ifdef CILK
#include "libsync/locks.h"
#include "libsync/sync.h"
#endif

typedef enum {
  essential    = 3, 
  neighboring  = 2,
  nonessential = 1,
  uninitialized = 0
} coll_status_t; 

typedef enum {
  x_dir = 0, 
  y_dir = 1, 
  z_dir = 2
} dir_t; 

typedef struct index_t {
  int idx[n_dim]; 
} index_t; 

typedef struct coord_t {
  double pos[n_dim]; 
} coord_t; 

#define ADJUST_WAVELET 0
#define ADJUST_NGBADD  1
#define ADJUST_UP      2
#define ADJUST_DOWN    3
#define ADJUST_HLLE    4
#define ADJUST_STAGES  5        //this needs to be last


#define RHS_WAVELET 0
#define RHS_HLLE    1
#define RHS_FLUX    2
#define RHS_DT      3           //not strictly a RHS operation... 
#define RHS_STAGES  4

typedef struct coll_point_t {
  double u[n_gen][n_variab + n_aux]; 
  double du[n_gen][n_deriv]; 
  double rhs[n_gen][n_rhs]; 
  double wavelet[n_gen][n_variab + n_aux]; 
  index_t index; 
  coord_t coords; 
  int level; 
  coll_status_t status[2]; 

  //These two are values used to avoid redundant work. They start out at zero,
  // and are postincremented to see if work should be done. If the postincrement
  // evaluates to nonzero, we skip the point. They are reset after each RHS
  // or adjust_grids().
  int stage[RHS_STAGES];
  int adjust[ADJUST_STAGES];
  unsigned time_stamp; // It tracks the number of times right-hand side has been
                       // computed. The value is incremented by n_gen each time
                       // the integrator is applied. 
#ifdef RMHD
  int c2p;
  //int closest_level;
#endif
} coll_point_t; 

#define CURRENT_STATUS 0
#define FUTURE_STATUS 1

#if n_dim == 1
#define n_neighbors 2
#elif n_dim == 2
#define n_neighbors 8
#elif n_dim == 3
#define n_neighbors 26
#endif 
extern const index_t neighbor_offset[n_neighbors]; 

#endif
