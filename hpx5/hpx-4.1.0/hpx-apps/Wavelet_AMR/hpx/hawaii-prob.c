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
#include <math.h>
#include "hawaii.h"
#include "hawaii-rmhd.h"

double t0; 
double tf; 

#ifdef HPX
hpx_action_t _compute_rhs_a = 0;
hpx_action_t _compute_rhs_b = 0;
hpx_action_t _compute_rhs_c = 0;
hpx_action_t _compute_rhs_d = 0;

int _action_compute_rhs_a(int *gen, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  hlle_rhs_stage1(point, *gen); 
  function_pointer_t type = hllerhsstage1;
  if (point->level == 1 && point->status[CURRENT_STATUS] == essential) 
    rhs_helper(point, *gen, type);

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_compute_rhs_b(int *gen, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  hlle_rhs_stage2(point, *gen); 
  function_pointer_t type = hllerhsstage2;
  if (point->level == 1 && point->status[CURRENT_STATUS] == essential) 
    rhs_helper(point, *gen, type);

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_compute_rhs_c(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  for (int j = 0; j < RHS_STAGES; ++j) {
    point->stage[j] = 0;
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_compute_rhs_d(void *UNUSED, size_t size)
{
  for (int i = 0; i < HASH_TBL_SIZE; ++i) {
    hpx_hash_entry_t *curr = hpx_coll_points.hash_table[i];
    while (curr != NULL) {
      int e = hpx_call_sync(curr->point, _compute_rhs_c, NULL, 0, NULL, 0);
      if (e != HPX_SUCCESS) hpx_abort();
      curr = curr->next;
    }
  }

  return HPX_SUCCESS;
}
#endif

/// ---------------------------------------------------------------------------
/// The equation to solve is the relativistic MHD equations
/// ---------------------------------------------------------------------------

void problem_init(void) {
  t0 = 0.0; // Simulation starting time 
  tf = 1;   // Simulation stopping time 
  int i;
  
  double ranges[n_variab];
  initial_ranges(ranges);
  
  for (i=0;i<n_variab;i++) {
    eps[i] = pars.epsilon; // Error tolerance for grid refinement
    eps_scale[i] = eps[i] * ranges[i];
  }
  
#if n_dim == 1
    L_dim[x_dir] = pars.xmax - pars.xmin;
#elif n_dim == 2
    L_dim[x_dir] = pars.xmax - pars.xmin;
    L_dim[y_dir] = pars.ymax - pars.ymin;
#elif n_dim == 3
    L_dim[x_dir] = pars.xmax - pars.xmin;
    L_dim[y_dir] = pars.ymax - pars.ymin;
    L_dim[z_dir] = pars.zmax - pars.zmin;
#endif

  for (i=0;i<F_D;i++) {
    deriv_mapping[i] = -1;
  }
  for (i=F_D;i<=F_TAU;i++) {
    deriv_mapping[i] = i-F_D;
  }

  validate_parameters(); 
  time_stamp = 0; // Initialize global time stamp 
}


//WARNING: This currently only works for the RMHD case with no B field and
// no initial velocity. This routine should be strengthened.
void initial_ranges(double *ranges) {
  //compute the initial ranges of the primary fields
  double rho_max = pars.id_gauss_floor + pars.id_amp;
  double rho_min = pars.id_gauss_floor;
  double P_max = pars.id_kappa * pow(rho_max, pars.gamma);
  double P_min = pars.id_kappa * pow(rho_min, pars.gamma);
  double inte_max = P_max / (rho_max * (pars.gamma - 1.0));
  double inte_min = P_min / (rho_min * (pars.gamma - 1.0));
  double he_max = rho_max * (1.0 + inte_max) + P_max;
  double he_min = rho_min * (1.0 + inte_min) + P_min;
  
  //stationary gas initially, so W = 1
  double D_max = rho_max;
  double D_min = rho_min;
  double tau_max = he_max - P_max - rho_max;
  double tau_min = he_min - P_min - rho_min;
  
  for (int i = 0; i < n_variab; ++i) {
    ranges[i] = 1.0;
  }
  ranges[U_D] = D_max - D_min;
  ranges[U_TAU] = tau_max - tau_min;
}

void initial_condition(const coord_t *coord, double *u) {
  if (pars.id_type == 1) {
    int dir = pars.id_indep_coord;
    riemann_problem(coord, u, dir);
  }
  else if (pars.id_type == 2) {
    gaussian_data(coord, u);
  } else if ( pars.id_type == 3 ) {
    rayleigh_taylor(coord, u);
  }
  else {
    printf("initial_condition: unknown id_type = %d\n",pars.id_type);
    exit(-2);
  }

}

void compute_rhs(const double t, const int gen) {
#ifdef HPX
  hpx_addr_t complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_compute_rhs_a,complete,&gen,sizeof(gen));  
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  //Eventually we can add a stage here that will find and fix the con_to_prim
  // failures via interpolation. 
  complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_compute_rhs_b,complete,&gen,sizeof(gen));  
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  //This resets the stage variables after this RHS computation
  complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_compute_rhs_c,complete,NULL,0);  
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  complete = hpx_lco_and_new(hpx_get_num_ranks());
  for (int i=0;i<hpx_get_num_ranks();i++) {
    hpx_call(HPX_THERE(i),_compute_rhs_d,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

#else
  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i]; 
    hlle_rhs_stage1(point, gen); 
    if (point->level == 1 && point->status[CURRENT_STATUS] == essential) 
      rhs_helper(point, gen, hlle_rhs_stage1);
  }
  
  //Eventually we can add a stage here that will find and fix the con_to_prim
  // failures via interpolation. 
  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i]; 
    hlle_rhs_stage2(point, gen); 
    if (point->level == 1 && point->status[CURRENT_STATUS] == essential) 
      rhs_helper(point, gen, hlle_rhs_stage2);
  }
  
  //This resets the stage variables after this RHS computation
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
  
#endif

}

