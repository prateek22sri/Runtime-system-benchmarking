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
#include <stdio.h>
#include "hawaii.h"
#ifdef RMHD
#include "hawaii-rmhd.h"
#endif

unsigned time_stamp; 

#ifdef FORWARD_EULER 
void apply_time_integrator(const double t, const double dt) {
  const int gen = 0; 
  compute_rhs(t, gen); 
  time_stamp++; 

  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i]; 
    forward_euler_helper(point, dt); 
    if (point->level == 1 && point->status[CURRENT_STATUS] == essential) 
      integrator_helper(point, dt, forward_euler_helper); 
  }

  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i]; 
    if (point->level == 1 && point->status[CURRENT_STATUS] == essential) 
      stage_reset_helper(point);
  }
}

void forward_euler_helper(coll_point_t *point, const double dt) {
  const int gen = 0; 
  double *u = point->u[gen]; 
  double *rhs = point->rhs[gen]; 
  for (int i = 0; i < n_variab; i++) 
    u[i] += dt * rhs[i];
  point->time_stamp++; 
}
#endif 

#ifdef RK4

void apply_time_integrator(const double t, const double dt) 
{
  void(*rk4_helpers[4])(coll_point_t *, const double) = 
    {rk4_helper1, rk4_helper2, rk4_helper3, rk4_helper4}; 

  const wavelet_trans_t type = primary_set_wavelet; 

  for (int gen = 0; gen < n_gen; gen++) {
    if (gen) {
      for (int i = 0; i < npts_in_array; i++) {
        coll_point_t *point = &coll_points->array[i]; 
        if (point->level == 1) {
          wavelet_trans(point, type, gen, primary_mask); 
          
          if (point->status[CURRENT_STATUS] == essential) {
            wavelet_integrator_helper(point, gen, type, primary_mask); 
          }
        }
      }
    } 

    compute_rhs(t, gen); 
    time_stamp++; 

    void(*rk4_helper)(coll_point_t *, const double) = rk4_helpers[gen]; 
    for (int i = 0; i < npts_in_array; i++) {
      coll_point_t *point = &coll_points->array[i]; 
      rk4_helper(point, dt); 
      if (point->level == 1 && point->status[CURRENT_STATUS] == essential) 
        integrator_helper(point, dt, rk4_helper); 
    }
  }

  for (int i = 0; i < npts_in_array; ++i) {
    for (int j = 0; j < RHS_STAGES; ++j) {
      coll_points->array[i].stage[j] = 0;
    }
  }
  for (int i = 0; i < HASH_TBL_SIZE; ++i) {
    hash_entry_t *curr = coll_points->hash_table[i];
    while (curr != NULL) {
      for (int j = 0; j < RHS_STAGES; ++j) {
        curr->point->stage[j] = 0;
      }
      curr = curr->next;
    }
  }
}

void rk4_helper1(coll_point_t *point, const double dt) {
  double *u_curr = point->u[0]; 
  double *u_next = point->u[1]; 
  double *rhs = point->rhs[0]; 
  for (int i = 0; i < n_variab; i++)
    u_next[i] = u_curr[i] + 0.5 * dt * rhs[i];
  point->time_stamp++; 
#ifdef RMHD
  point->c2p = con_to_prim(u_next, point->u[0], point->coords.pos);
#endif
}

void rk4_helper2(coll_point_t *point, const double dt) {
  double *u_curr = point->u[0]; 
  double *u_next = point->u[2]; 
  double *rhs = point->rhs[1]; 
  for (int i = 0; i < n_variab; i++)
    u_next[i] = u_curr[i] + 0.5 * dt * rhs[i];
  point->time_stamp++;
#ifdef RMHD
  point->c2p = con_to_prim(u_next, point->u[1], point->coords.pos);
#endif
}

void rk4_helper3(coll_point_t *point, const double dt) {
  double *u_curr = point->u[0];
  double *u_next = point->u[3];
  double *rhs = point->rhs[2]; 
  for (int i = 0; i < n_variab; i++) 
    u_next[i] = u_curr[i] + dt * rhs[i]; 
  point->time_stamp++; 
#ifdef RMHD
  point->c2p = con_to_prim(u_next, point->u[2], point->coords.pos);
#endif
}

void rk4_helper4(coll_point_t *point, const double dt) {
  double *u_curr = point->u[0];
  double *k1 = point->rhs[0];
  double *k2 = point->rhs[1];
  double *k3 = point->rhs[2];
  double *k4 = point->rhs[3];

  for (int i = 0; i < n_variab; i++) 
    u_curr[i] += dt * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]) / 6.0;

  point->time_stamp++; 
#ifdef RMHD
  point->c2p = con_to_prim(u_curr, point->u[3], point->coords.pos);
#endif
}
#endif
