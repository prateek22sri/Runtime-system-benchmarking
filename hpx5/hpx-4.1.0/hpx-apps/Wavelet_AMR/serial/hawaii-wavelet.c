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

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "hawaii.h"

double deriv_mapping[(n_variab + n_aux) * n_dim];
double scale = 1.0 / (1 << JJ); 
const int wavelet_offset[4] = {-3, -1, 1, 3}; 
const double wavelet_factor[4] = {-1.0 / 16.0, 9.0 / 16.0, 
                                    9.0 / 16.0, -1.0 / 16.0};

//NOTE: This assumes that the incoming index is just one off the grid at 
// whatever relevant level we are talking about. If this is not true, this will
// not work.
//
// Further, the 'offness' of the point will give the step size to find the 
// needed points in the grid.
void ext_func(const index_t *index, const int mask[n_variab + n_aux],
              const int gen, double var[n_variab + n_aux]) {
  //figure out the first out of bounds index direction
  int idir;
  int delta = 0;
  for (idir = x_dir; idir < n_dim; ++idir) {
    if (index->idx[idir] < 0) {
      delta = -index->idx[idir];
      break;
    } else if (index->idx[idir] > max_index[idir]) {
      delta = -(index->idx[idir] - max_index[idir]);  
      //delta is negative so we step into the grid 
      break;
    }
  }
  assert(idir < n_dim);
  
  //loop over the needed points, collecting their field values
  double vals[4][n_variab + n_aux] = {{0.0}};
  index_t stcl = *index;
  for (int loop = 0; loop < 4; ++loop) {
    //step to next point
    stcl.idx[idir] += delta;
    
    //get the value - from the grid or from extension
    if (check_index(&stcl)) {
      coll_point_t *stcl_point = get_coll_point(&stcl);
      memcpy(vals[loop], stcl_point->u[gen], 
                    sizeof(double) * (n_variab + n_aux));
    } else {
      ext_func(&stcl, mask, gen, vals[loop]);
    }
  }
  
  //now add them up
  for (int ivar = 0; ivar < n_variab + n_aux; ++ivar) {
    if (mask[ivar]) {
      for (int loop = 0; loop < 4; ++loop) {
        var[ivar] = 4.0 * vals[0][ivar] - 6.0 * vals[1][ivar]
                    + 4.0 * vals[2][ivar] - vals[3][ivar];
      }
    }
  }
}

void wavelet_trans(coll_point_t *point, const wavelet_trans_t type, 
                   const int gen, const int mask[n_variab + n_aux]) {
  if (point->level == 0) {
    return;
  }

  int h = 1 << (JJ - point->level);      //spacing at the level of the point
  int h2 = 2 * h;                        //spacing at the next coarser level
  
  //how many directions should we interpolate?
  int interp_count = 0;
  int dirs[n_dim];
  for (int idir = 0; idir < n_dim; ++idir) {
    dirs[idir] = -1;
    if (point->index.idx[idir] % h2) {
      dirs[interp_count] = idir;
      ++interp_count;
    }
  }
  
  
  //work through the cases to compute approx
  double approx[n_variab + n_aux] = {0.0};
  double buffer[n_variab + n_aux] = {0.0};
  double *contrib;
  if (interp_count == 1) {
    for (int loopa = 0; loopa < 4; ++loopa) {
      //shift to correct location
      index_t stcl = point->index;
      stcl.idx[dirs[0]] += h * wavelet_offset[loopa];
      
      //get the point, or extension
      if (check_index(&stcl)) {
        coll_point_t *stcl_point = get_coll_point(&stcl);
        assert(stcl_point->level < point->level);
        contrib = stcl_point->u[gen];
      } else {
        ext_func(&stcl, mask, gen, buffer);
        contrib = buffer;
      }
      
      //contribute to approx
      for (int ivar = 0; ivar < n_variab + n_aux; ++ivar) {
        if (mask[ivar]) {
          approx[ivar] += wavelet_factor[loopa] * contrib[ivar];
        }
      }
    }
  }
#if n_dim > 1 
  else if (interp_count == 2) {
    for (int loopa = 0; loopa < 4; ++loopa) {
      index_t stcla = point->index;
      stcla.idx[dirs[0]] += h * wavelet_offset[loopa];
      for (int loopb = 0; loopb < 4; ++loopb) {
        index_t stcl = stcla;
        stcl.idx[dirs[1]] += h * wavelet_offset[loopb];
        
        //get the point
        if (check_index(&stcl)) {
          coll_point_t *stcl_point = get_coll_point(&stcl);
          assert(stcl_point->level < point->level);
          contrib = stcl_point->u[gen];
        } else {
          ext_func(&stcl, mask, gen, buffer);
          contrib = buffer;
        }
        
        //contribute
        for (int ivar = 0; ivar < n_variab + n_aux; ++ivar) {
          if (mask[ivar]) {
            approx[ivar] += wavelet_factor[loopa] 
                            * wavelet_factor[loopb] * contrib[ivar];
          }
        }
      }
    }
  } 
#endif
#if n_dim > 2
  else if (interp_count == 3) {
    for (int loopa = 0; loopa < 4; ++loopa) {
      index_t stcla = point->index;
      stcla.idx[dirs[0]] += h * wavelet_offset[loopa];
      for (int loopb = 0; loopb < 4; ++loopb) {
        index_t stclb = stcla;
        stclb.idx[dirs[1]] += h * wavelet_offset[loopb];
        for (int loopc = 0; loopc < 4; ++loopc) {
          index_t stcl = stclb;
          stcl.idx[dirs[2]] += h * wavelet_offset[loopc];
          
          //get the point
          if (check_index(&stcl)) {
            coll_point_t *stcl_point = get_coll_point(&stcl);
            assert(stcl_point->level < point->level);
            contrib = stcl_point->u[gen];
          } else {
            ext_func(&stcl, mask, gen, buffer);
            contrib = buffer;
          }
          
          //contribute
          for (int ivar = 0; ivar < n_variab + n_aux; ++ivar) {
            if (mask[ivar]) {
              approx[ivar] += wavelet_factor[loopa] 
                            * wavelet_factor[loopb] 
                            * wavelet_factor[loopc] * contrib[ivar];
            }
          }
        }
      }
    }
  } 
#endif
  else {
    assert(0 && "Problem identifying interpolation directions for wavelet");
  }
  
  
  //compute what needs computing from approx
  switch (type) {
  case primary_set_wavelet:
    for (int ivar = 0; ivar < n_variab; ++ivar) {
      point->wavelet[gen][ivar] = point->u[gen][ivar] - approx[ivar];
    }
    break;
  case primary_set_approx:
    for (int ivar = 0; ivar < n_variab; ++ivar) {
      point->u[gen][ivar] = approx[ivar];
      point->wavelet[gen][ivar] = 0.0;
    }
    break;
  case auxiliary_set_wavelet:
    for (int ivar = n_variab; ivar < n_variab + n_aux; ++ivar) {
      if (mask[ivar]) {
        point->wavelet[gen][ivar] = point->u[gen][ivar] - approx[ivar];
      }
    }
    break;
  case auxiliary_set_approx:
    for (int ivar = n_variab; ivar < n_variab + n_aux; ++ivar) {
      if (mask[ivar]) {
        point->u[gen][ivar] = approx[ivar];
        point->wavelet[gen][ivar] = 0.0;
      }
    }
    break;
  }
}

void compute_func_derivative(coll_point_t *point, const int gen, const int dir,
                             const int mask[n_variab + n_aux]) {
  double *var[4]; 
  double buffer[4][n_variab + n_aux] = {{0}}; 

  int h = 1 << (JJ - point->level); 
  for (int loop = 0; loop < 4; loop++) {
    index_t sten = point->index; 
    sten.idx[dir] += deriv_offset[loop] * h; 

    if (check_index(&sten)) {
      coll_point_t *temp = get_coll_point(&sten); 
      var[loop] = temp->u[gen];
    } else {
      ext_func(&sten, mask, gen, buffer[loop]);
      var[loop] = buffer[loop]; 
    }
  }

  for (int ivar = 0; ivar < n_variab + n_aux; ivar++) {
    if (mask[ivar]) {
      double accumulator = 0; 
      // Determine location to save the computed derivative result
      const int i_deriv = deriv_mapping[ivar * n_dim + dir]; 
      double a = var[0][ivar]; 
      double b = var[1][ivar]; 
      double c = var[2][ivar];
      double d = var[3][ivar]; 
      double dx = L_dim[dir] / ns[dir] / (1 << point->level); // Grid spacing
      double term1 = 2.0 / 3.0 * (c - b) / dx; 
      double term2 = 1.0 / 12.0 * (d - a) / dx; 
      accumulator = term1 - term2; // Contribution from current level

      // Accumulate contributions from finer grid points
      index_t index = point->index; 
      if (point->index.idx[dir] > 0) {
        for (int level = point->level + 1; level <= JJ; level++) {
          int h = 1 <<  (JJ - level); 
          index.idx[dir] = point->index.idx[dir] - h;
          coll_point_t *temp = get_coll_point(&index); 
          if (temp != NULL) {
            double dx = L_dim[dir] / ns[dir] / (1 << temp->level); 
            double contrib_m = -2.0 / 3.0 * temp->wavelet[gen][ivar] / dx; 
            accumulator += contrib_m; 
          } else {
            break; // No more neighboring points from finer levels
          }
        }
      } 

      if (point->index.idx[dir] < max_index[dir]) {
        for (int level = point->level + 1; level <= JJ; level++) {
          int h = 1 << (JJ - level); 
          index.idx[dir] = point->index.idx[dir] + h; 
          coll_point_t *temp = get_coll_point(&index); 
          if (temp != NULL) {
            double dx = L_dim[dir] / ns[dir] / (1 << temp->level); 
            double contrib_p = 2.0 / 3.0 * temp->wavelet[gen][ivar] / dx; 
            accumulator += contrib_p;
          } else {
            break; // No more neighboring points from finer levels
          }
        }
      }

      point->du[gen][i_deriv] = accumulator;
    }
  }
}
