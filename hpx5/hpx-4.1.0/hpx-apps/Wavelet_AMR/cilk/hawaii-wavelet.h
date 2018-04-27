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

#ifndef HAWAII_WAVELETS_H
#define HAWAII_WAVELETS_H

#include "hawaii-types.h"
#include "hawaii-param.h"

typedef enum {
  primary_set_wavelet = 1, 
  primary_set_approx = 2, 
  auxiliary_set_wavelet = 3, 
  auxiliary_set_approx = 4
} wavelet_trans_t; 

typedef enum {
  extend_upper = 1, 
  extend_lower = 2
} ext_func_t; 

extern double deriv_mapping[(n_variab + n_aux) * n_dim]; 
extern const int wavelet_offset[4]; 

void wavelet_trans(coll_point_t *point, const wavelet_trans_t type, 
                   const int gen, const int mask[n_variab + n_aux]); 

void ext_func(const index_t *index, 
              const int mask[n_variab + n_aux], const int gen, 
              double var[n_variab + n_aux]);

void compute_func_derivative(coll_point_t *point, const int gen, const int dir,
                             const int mask[n_variab + n_aux]);
#endif
