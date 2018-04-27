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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include "hawaii.h"

Pars pars;

static inline double getticks(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (double) (tv.tv_sec * 1e6 + tv.tv_usec);
}

static inline double elapsed(double t1, double t0) {
  return (double) (t1 - t0);
}

// this is used to calculate the time step for classical fluid.
extern double max_eigenval;


int primary_mask[n_variab + n_aux];
int primitive_mask[n_variab + n_aux];
int auxiliary_mask[n_variab + n_aux];
int allvars_mask[n_variab + n_aux];

void setup_default_masks(void) {
  for (int ivar = 0; ivar < n_variab + n_aux; ++ivar) {
    if (ivar < n_variab) {
      primary_mask[ivar] = 1;
    } else {
      primary_mask[ivar] = 0;
    }
    if (ivar >= n_variab) {
      auxiliary_mask[ivar] = 1;
    } else {
      auxiliary_mask[ivar] = 0;
    }
    if (ivar < n_variab + n_prims) {
      primitive_mask[ivar] = 1;
    }
    allvars_mask[ivar] = 1;
  }
}

static void register_actions(void);
#ifdef HPX

hpx_addr_t _table_locks[LOCK_TBL_SIZE];

hpx_action_t _main = 0;
hpx_action_t _gas = 0;
hpx_action_t _param = 0;
hpx_action_t _print = 0;
hpx_action_t _alloc_locks = 0;
hpx_action_t _problem_init = 0;
hpx_action_t _eigenval_init = 0;
hpx_action_t _eigenval_broad = 0;
hpx_action_t _distkey = 0;

int _print_action(void *UNUSED, size_t size) {
  printf("Check pars: max_iter %d\n", pars.max_iter);
  return HPX_SUCCESS;
}

int _alloc_locks_action(void *UNUSED, size_t size) {
  // allocate some locks to protect against data race conditions
  for (int i = 0; i < LOCK_TBL_SIZE; i++) {
     _table_locks[i] = hpx_lco_sema_new(1);
  }

  return HPX_SUCCESS;
}

int _distkey_action(uint64_t *mkey, size_t size) {
  max_key = *mkey;
  return HPX_SUCCESS;
}

int _gas_action(hpx_addr_t *array, size_t size) {
  hpx_coll_points.array  = *array;
  return HPX_SUCCESS;
}

static int _param_action(Pars *cfg, size_t size) {
  if (hpx_get_my_rank() != 0)
    pars = *cfg;
  return HPX_SUCCESS;
}

int _problem_init_action(void *UNUSED, size_t size) {

  // Setup common masking patterns
  setup_default_masks();

  // Initialize problem parameters
  max_eigenval = -1.0;    /* initialize to negative value */
  problem_init();

  return HPX_SUCCESS;
}

int _eigenval_init_action(double *meigen, size_t size) {

  if ( *meigen > max_eigenval ) {
    max_eigenval = *meigen;
  }

  return HPX_THREAD_CONTINUE(max_eigenval);
}

int _eigenval_broad_action(double *meigen, size_t size) {

  max_eigenval = *meigen;

  return HPX_SUCCESS;
}
#endif

static int _main_action(void *UNUSED,size_t size) {

  double startup = getticks();
#ifdef HPX
  hpx_addr_t complete = hpx_lco_and_new(hpx_get_num_ranks());
  for (int i=0;i<hpx_get_num_ranks();i++) {
    hpx_call(HPX_THERE(i),_problem_init,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  // Setup common masking patterns
  setup_default_masks();

  // Initialize problem parameters
  max_eigenval = -1.0;    /* initialize to negative value */
  problem_init();
#endif

#ifdef HPX
  hpx_addr_t array = hpx_gas_calloc_cyclic(npts_in_array,sizeof(coll_point_t), 0);
  hpx_bcast_rsync(_gas, &array, sizeof(array));
  hpx_bcast_rsync(_param, &pars, sizeof(pars));
  // Check that parameters are distributed
  //hpx_bcast_rsync(_print,NULL,0);
  assert(array==hpx_coll_points.array);
#else
  storage_init();
#endif

#ifdef HPX
  // allocate some locks to protect against data race conditions
  for (int i=0;i<hpx_get_num_ranks();i++) {
    hpx_call_sync(HPX_THERE(i),_alloc_locks,NULL,0,NULL,0);
  }
#endif

  // Generate adptive grid from initial profile
  create_full_grids();

#ifdef HPX
  // get max index -- used in distribution
  index_t indx;
  hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,(npts_in_array-1)*sizeof(coll_point_t),sizeof(coll_point_t));
  hpx_addr_t index_offset = hpx_addr_add(point,offsetof(coll_point_t,index),sizeof(coll_point_t));
  hpx_gas_memget_sync(&indx,index_offset,sizeof(indx));
  uint64_t mkey = morton_key(&indx);
  hpx_bcast_rsync(_distkey, &mkey, sizeof(mkey));
#endif

  create_adap_grids();
  complete_hlle_stencils();

  printf("max level at initial step is %d\n", max_level);

  // Advance simulation in time
  double t = t0; // Current simulation real time

#if defined(RNPL) || defined(SILO)
  if ( pars.output_frequency != 0 ) {
    grid_sdf_output(time_stamp,t);
  }
#endif

  double e1 = 0;
  double e2 = 0;
  double e3 = elapsed(getticks(),startup);
  double e4 = 0;
  double e5 = 0;
  while (time_stamp/n_gen < pars.max_iter && t < pars.tf) {
#ifdef HPX
    for (int i=0;i<hpx_get_num_ranks();i++) {
      if ( i != hpx_get_my_rank() ) {
        hpx_call_sync(HPX_THERE(i),_eigenval_init,&max_eigenval,sizeof(max_eigenval),&max_eigenval,sizeof(max_eigenval));
      }
    }

    hpx_addr_t complete = hpx_lco_and_new(hpx_get_num_ranks()-1);
    for (int i=0;i<hpx_get_num_ranks();i++) {
      if ( i != hpx_get_my_rank() ) {
        hpx_call(HPX_THERE(i),_eigenval_broad,complete,&max_eigenval,sizeof(max_eigenval));
      }
    }
    hpx_lco_wait(complete);
    hpx_lco_delete(complete, HPX_NULL);
#endif
    double dt = get_global_dt();
    dt = fmin(dt, pars.tf - t);
    if ( pars.print_frequency != 0 ) {
      if (time_stamp/n_gen % pars.print_frequency == 0 ) {
        int numpoints = count_points(neighboring, time_stamp);
        printf("step %d: t_next = %14.7e, dt = %10.3e, npts = %d\n",
         time_stamp / n_gen, t+dt, dt,numpoints);
      }
    }
    double s1 = getticks();
    apply_time_integrator(t, dt);
    e1 += elapsed(getticks(), s1);
    t += dt;

    double s2 = getticks();
    adjust_grids();
    e2 += elapsed(getticks(), s2);
    double s4 = getticks();
    if (time_stamp % (n_gen * pars.prune_frequency) == 0)
      prune_grids();
    e5 += elapsed(getticks(),s4);

    double s3 = getticks();
#if defined(RNPL) || defined(SILO)
    if ( pars.output_frequency != 0 ) {
      if (time_stamp % (n_gen * pars.output_frequency) == 0 ) {
        grid_sdf_output(time_stamp,t);
        //char buffer[100];
        //sprintf(buffer, "grid%.3d.txt", time_stamp / n_gen);
        //visualize_grid(buffer, 0);
      }
    }
#endif
    e4 += elapsed(getticks(), s3);
  }
  printf(" Time integrator loop: %f, Adjust grid: %f  \n startup: %f output: %f prune grids: %f seconds\n",e1/1e6, e2/1e6,e3/1e6,e4/1e6,e5/1e6);

#ifdef HPX
  hpx_exit(0, NULL);
#else
  storage_cleanup();
#endif

  return 0;
}

int main(int argc, char *argv[]) {
  register_actions();

#ifdef HPX

  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "Failed to initialize hpx\n");
    return -1;
  }
#endif

#ifdef RNPL
  // read parameter file
  char pfile[128];
  if ( argc < 2 ) {
#ifdef HPX
    if ( HPX_LOCALITY_ID == 0 ) printf(" Usage: %s <parameter file>\n",argv[0]);
#else
    printf(" Usage: %s <parameter file>\n",argv[0]);
#endif
    exit(-1);
  }
  strcpy(pfile, argv[1]);
  if (read_param_file(pfile) != 1) {
#ifdef HPX
    if ( HPX_LOCALITY_ID == 0 ) printf("Error reading parameters. Die here.\n");
#else
    printf("Error reading parameters. Die here.\n");
#endif
    exit(-1);
  }
#else
  default_params();
#endif

#ifdef HPX
  e = hpx_run(&_main, NULL, NULL, 0);
  hpx_finalize();
  return e;
#else
  return _main_action(NULL,0);
#endif
}


void register_actions(void) {
#ifdef HPX
  /* register action for parcel (must be done by all ranks) */
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _main, _main_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _gas, _gas_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _distkey, _distkey_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _param, _param_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _print, _print_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _alloc_locks, _alloc_locks_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _problem_init, _problem_init_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _create_full_grids_a, _create_full_grids_a_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _create_full_grids_b, _create_full_grids_b_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _create_adap_grids_a, _create_adap_grids_a_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _init_neighboring_point, _init_neighboring_point_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _second_init_neighboring_point, _second_init_neighboring_point_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _hpx_get_or_insert_point_local, _hpx_get_or_insert_point_local_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _complete_hlle_stencils_a, _complete_hlle_stencils_a_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _complete_hlle_stencils_b, _complete_hlle_stencils_b_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _complete_hlle_stencils_c, _complete_hlle_stencils_c_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _hpx_get_coll_point_local, _action_hpx_get_coll_point_local, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _check_extension, _action_check_extension, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _global_dt_helper, _action_global_dt_helper, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _get_global_dt, _action_get_global_dt, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _update_max_level, _update_max_level_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _2doutput, _2doutput_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _apply_time_a, _action_apply_time_a, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _apply_time_b, _action_apply_time_b, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _wavelet_int_helper, _action_wavelet_int_helper, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _compute_rhs_a, _action_compute_rhs_a, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _hlle_rhs_stage1_a, _action_hlle_rhs_stage1_a, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _rhs_helper_a, _action_rhs_helper_a, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _compute_rhs_b, _action_compute_rhs_b, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _compute_rhs_c, _action_compute_rhs_c, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _compute_rhs_d, _action_compute_rhs_d, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _eigenval_init, _eigenval_init_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _eigenval_broad, _eigenval_broad_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _augment_timestamp, _augment_timestamp_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _integrator_helper_a, _action_integrator_helper_a, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _apply_time_c, _action_apply_time_c, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _apply_time_d, _action_apply_time_d, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _wavelet_adjust_grids_helper, _action_wavelet_adjust_grids_helper, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _adjust_grids_a, _action_adjust_grids_a, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _adjust_grids_b, _action_adjust_grids_b, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _adjust_grids_c, _action_adjust_grids_c, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _adjust_grids_d, _action_adjust_grids_d, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _adjust_grids_e, _action_adjust_grids_e, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _adjust_add_neighbors_helper, _action_adjust_add_neighbors_helper, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _status_validate_up_helper, _action_status_validate_up_helper, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _status_validate_down_helper, _action_status_validate_down_helper, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _update_all_statuses, _action_update_all_statuses, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _update_all_statuses_b, _action_update_all_statuses_b, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _clear_all_adjust_stages, _action_clear_all_adjust_stages, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _clear_all_adjust_stages_b, _action_clear_all_adjust_stages_b, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _prune_grids, _action_prune_grids, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _count_points, _action_count_points, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _set_eps_scale, _action_set_eps_scale, HPX_POINTER, HPX_SIZE_T);
#endif
}
