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
#include "hawaii-rmhd.h"

unsigned time_stamp; 

#ifdef HPX
hpx_action_t _apply_time_a = 0;
hpx_action_t _apply_time_b = 0;
hpx_action_t _apply_time_c = 0;
hpx_action_t _apply_time_d = 0;
hpx_action_t _augment_timestamp = 0;

int _augment_timestamp_action(void *UNUSED, size_t size) {

  time_stamp++;

  return HPX_SUCCESS;
}

int _action_apply_time_a(int *gen, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  const wavelet_trans_t type = primary_set_wavelet; 

  if (point->level == 1) {
    wavelet_trans(point, type, *gen, primary_mask); 
          
    if (point->status[CURRENT_STATUS] == essential) {
      wavelet_integrator_helper(point, *gen, type, primary_mask); 
    }
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_apply_time_b(ATI *ld, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  if ( ld->gen == 0 ) {
    rk4_helper1(point,ld->dt);
  } else if ( ld->gen == 1 ) {
    rk4_helper2(point,ld->dt);
  } else if ( ld->gen == 2 ) {
    rk4_helper3(point,ld->dt);
  } else if ( ld->gen == 3 ) {
    rk4_helper4(point,ld->dt);
  } else {
    printf(" invalid generation %d\n",ld->gen);
    assert(0);
  }

  if (point->level == 1 && point->status[CURRENT_STATUS] == essential)
    integrator_helper(point, ld->dt,ld->gen);

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_apply_time_c(void *UNUSED, size_t size)
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

int _action_apply_time_d(void *UNUSED, size_t size) {

  for (int i = 0; i < HASH_TBL_SIZE; ++i) {
    hpx_hash_entry_t *curr = hpx_coll_points.hash_table[i];
    while (curr != NULL) {
      int e = hpx_call_sync(curr->point, _apply_time_c, NULL, 0, NULL, 0);
      if (e != HPX_SUCCESS) hpx_abort();
      curr = curr->next;
    }
  }

  return HPX_SUCCESS;
}

#endif

void apply_time_integrator(const double t, const double dt) 
{

#ifndef HPX
  void(*rk4_helpers[4])(coll_point_t *, const double) = 
    {rk4_helper1, rk4_helper2, rk4_helper3, rk4_helper4}; 

  const wavelet_trans_t type = primary_set_wavelet; 
#endif

  for (int gen = 0; gen < n_gen; gen++) {
    if (gen) {
#ifdef HPX
      hpx_addr_t complete = hpx_lco_and_new(npts_in_array);
#endif
      for (int i = 0; i < npts_in_array; i++) {
#ifdef HPX
        hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
        hpx_call(point,_apply_time_a,complete,&gen,sizeof(gen));
#else
        coll_point_t *point = &coll_points->array[i]; 
        if (point->level == 1) {
          wavelet_trans(point, type, gen, primary_mask); 
          
          if (point->status[CURRENT_STATUS] == essential) {
            wavelet_integrator_helper(point, gen, type, primary_mask); 
          }
        }
#endif
      }
#ifdef HPX
      hpx_lco_wait(complete);
      hpx_lco_delete(complete, HPX_NULL);
#endif
    } 

    compute_rhs(t, gen); 

#ifdef HPX
    hpx_addr_t complete = hpx_lco_and_new(hpx_get_num_ranks());
    for (int i=0;i<hpx_get_num_ranks();i++) {
      hpx_call(HPX_THERE(i),_augment_timestamp,complete,NULL,0);
    }
    hpx_lco_wait(complete);
    hpx_lco_delete(complete, HPX_NULL);
#else
    time_stamp++; 
#endif
   
#ifdef HPX
    complete = hpx_lco_and_new(npts_in_array);
    for (int i = 0; i < npts_in_array; i++) {
      ATI ld;
      ld.gen = gen;
      ld.dt = dt;
      hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
      hpx_call(point,_apply_time_b,complete,&ld,sizeof(ld));
    }
    hpx_lco_wait(complete);
    hpx_lco_delete(complete, HPX_NULL);
#else
    void(*rk4_helper)(coll_point_t *, const double) = rk4_helpers[gen]; 
    for (int i = 0; i < npts_in_array; i++) {
      coll_point_t *point = &coll_points->array[i]; 
      rk4_helper(point, dt); 
      if (point->level == 1 && point->status[CURRENT_STATUS] == essential) 
        integrator_helper(point, dt, rk4_helper); 
    }
#endif
  }

#ifdef HPX
  hpx_addr_t complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; ++i) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_apply_time_c,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  for (int i = 0; i < npts_in_array; ++i) {
    for (int j = 0; j < RHS_STAGES; ++j) {
      coll_points->array[i].stage[j] = 0;
    }
  }
#endif

#ifdef HPX
  complete = hpx_lco_and_new(hpx_get_num_ranks());
  for (int i=0;i<hpx_get_num_ranks();i++) {
    hpx_call(HPX_THERE(i),_apply_time_d,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
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

void rk4_helper1(coll_point_t *point, const double dt) {
  double *u_curr = point->u[0]; 
  double *u_next = point->u[1]; 
  double *rhs = point->rhs[0]; 
  for (int i = 0; i < n_variab; i++)
    u_next[i] = u_curr[i] + 0.5 * dt * rhs[i];
  point->time_stamp++; 
  point->c2p = con_to_prim(u_next, point->u[0], point->coords.pos);
}

void rk4_helper2(coll_point_t *point, const double dt) {
  double *u_curr = point->u[0]; 
  double *u_next = point->u[2]; 
  double *rhs = point->rhs[1]; 
  for (int i = 0; i < n_variab; i++)
    u_next[i] = u_curr[i] + 0.5 * dt * rhs[i];
  point->time_stamp++;
  point->c2p = con_to_prim(u_next, point->u[1], point->coords.pos);
}

void rk4_helper3(coll_point_t *point, const double dt) {
  double *u_curr = point->u[0];
  double *u_next = point->u[3];
  double *rhs = point->rhs[2]; 
  for (int i = 0; i < n_variab; i++) 
    u_next[i] = u_curr[i] + dt * rhs[i]; 
  point->time_stamp++; 
  point->c2p = con_to_prim(u_next, point->u[2], point->coords.pos);
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
  point->c2p = con_to_prim(u_curr, point->u[3], point->coords.pos);
}
