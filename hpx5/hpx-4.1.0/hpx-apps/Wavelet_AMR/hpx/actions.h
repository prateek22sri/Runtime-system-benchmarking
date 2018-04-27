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

#ifndef ACTIONS_H
#define ACTIONS_H

#include "hawaii-types.h"
#include "hawaii-wavelet.h"

typedef struct
{
  double dt;
  int gen;
} ATI;

typedef struct
{
  index_t index;
  int h2;
} SRH;

typedef struct
{
  index_t index;
  int flag;
  int gen;
} CES;

typedef struct
{
  int gen;
  wavelet_trans_t type;
  int wavelet_mask[n_variab + n_aux];
} WIH;

typedef struct
{
  hpx_addr_t point;
  int found;
} hpx_get_or_insert_point_local_return_t;

typedef struct {
  index_t index;
  int level;
} INP;

typedef struct {
  double global_dt;
  double valmin[n_variab];
  double valmax[n_variab];
} GGD;

typedef struct
{
  unsigned int stamp;
  int filenum;
} OUT;

typedef struct
{
  unsigned stamp;
  coll_status_t minstat;
} CP;

typedef struct
{
  function_pointer_t func_type;
  int gen;
} RHSH;

typedef struct
{
  double valmin[n_variab];
  double valmax[n_variab];
} EPSS;

/* actions */
extern hpx_action_t _main;
extern hpx_action_t _evolve;
extern hpx_action_t _gas;
extern hpx_action_t _distkey;
extern hpx_action_t _create_full_grids_a;
extern hpx_action_t _create_full_grids_b;
extern hpx_action_t _create_adap_grids_a;
extern hpx_action_t _init_neighboring_point;
extern hpx_action_t _second_init_neighboring_point;
extern hpx_action_t _hpx_get_or_insert_point_local;
extern hpx_action_t _complete_hlle_stencils_a;
extern hpx_action_t _complete_hlle_stencils_b;
extern hpx_action_t _complete_hlle_stencils_c;
extern hpx_action_t _hpx_get_coll_point_local;
extern hpx_action_t _check_extension;
extern hpx_action_t _get_global_dt;
extern hpx_action_t _global_dt_helper;
extern hpx_action_t _problem_init;
extern hpx_action_t _update_max_level;
extern hpx_action_t _2doutput;
extern hpx_action_t _apply_time_a;
extern hpx_action_t _wavelet_int_helper;
extern hpx_action_t _compute_rhs_a;
extern hpx_action_t _rhs_helper_a;
extern hpx_action_t _hlle_rhs_stage1_a;
extern hpx_action_t _compute_rhs_b;
extern hpx_action_t _compute_rhs_c;
extern hpx_action_t _compute_rhs_d;
extern hpx_action_t _eigenval_init;
extern hpx_action_t _eigenval_broad;
extern hpx_action_t _augment_timestamp;
extern hpx_action_t _apply_time_b;
extern hpx_action_t _apply_time_c;
extern hpx_action_t _apply_time_d;
extern hpx_action_t _integrator_helper_a;
extern hpx_action_t _wavelet_adjust_grids_helper;
extern hpx_action_t _adjust_grids_a;
extern hpx_action_t _adjust_grids_b;
extern hpx_action_t _adjust_grids_c;
extern hpx_action_t _adjust_grids_d;
extern hpx_action_t _adjust_grids_e;
extern hpx_action_t _adjust_add_neighbors_helper;
extern hpx_action_t _status_validate_up_helper;
extern hpx_action_t _status_validate_down_helper;
extern hpx_action_t _update_all_statuses;
extern hpx_action_t _update_all_statuses_b;
extern hpx_action_t _clear_all_adjust_stages;
extern hpx_action_t _clear_all_adjust_stages_b;
extern hpx_action_t _prune_grids;
extern hpx_action_t _count_points;
extern hpx_action_t _set_eps_scale;
int _distkey_action(uint64_t *mkey, size_t size);
int _create_full_grids_a_action(int *, size_t size);
int _create_full_grids_b_action(void *, size_t size);
int _create_adap_grids_a_action(void *, size_t size);
int _init_neighboring_point_action(INP *ld, size_t size);
int _second_init_neighboring_point_action(void *, size_t size);
int _hpx_get_or_insert_point_local_action(uint64_t *mkey, size_t size);
int _complete_hlle_stencils_a_action(void *UNUSED, size_t size);
int _complete_hlle_stencils_b_action(void *UNUSED, size_t size);
int _complete_hlle_stencils_c_action(void *UNUSED, size_t size);
int _action_hpx_get_coll_point_local(uint64_t *mkey, size_t size);
int _action_check_extension(CES *ld, size_t size);
int _action_global_dt_helper(GGD *ld, size_t size);
int _action_get_global_dt(GGD *ld, size_t size);
int _problem_init_action(void *UNUSED, size_t size);
int _update_max_level_action(int *maxlevel, size_t size);
int _2doutput_action(OUT *ld,size_t size);
int _action_apply_time_a(int *gen, size_t size);
int _action_wavelet_int_helper(WIH *ld, size_t size);
int _action_rhs_helper_a(RHSH *ld, size_t size);
int _action_hlle_rhs_stage1_a(int *gen, size_t size);
int _action_compute_rhs_a(int *gen, size_t size);
int _action_compute_rhs_b(int *gen, size_t size);
int _action_compute_rhs_c(void *UNUSED, size_t size);
int _action_compute_rhs_d(void *UNUSED, size_t size);
int _eigenval_init_action(double *meigen, size_t size);
int _eigenval_broad_action(double *meigen, size_t size);
int _augment_timestamp_action(void *UNUSED, size_t size);
int _action_apply_time_b(ATI *ld, size_t size);
int _action_apply_time_c(void *UNUSED, size_t size);
int _action_apply_time_d(void *UNUSED, size_t size);
int _action_integrator_helper_a(ATI *ld, size_t size);
int _action_wavelet_adjust_grids_helper(void *UNUSED, size_t size);
int _action_adjust_grids_a(void *UNUSED, size_t size);
int _action_adjust_grids_b(void *UNUSED, size_t size);
int _action_adjust_grids_c(void *UNUSED, size_t size);
int _action_adjust_grids_d(void *UNUSED, size_t size);
int _action_adjust_grids_e(void *UNUSED, size_t size);
int _action_adjust_add_neighbors_helper(void *UNUSED, size_t size);
int _action_status_validate_up_helper(void *UNUSED, size_t size);
int _action_status_validate_down_helper(void *UNUSED, size_t size);
int _action_update_all_statuses_b(void *UNUSED, size_t size);
int _action_update_all_statuses(void *UNUSED, size_t size);
int _action_clear_all_adjust_stages(void *UNUSED,size_t size);
int _action_clear_all_adjust_stages_b(void *UNUSED,size_t size);
int _action_prune_grids(void *UNUSED,size_t size);
int _action_count_points(CP *ld,size_t size);
int _action_set_eps_scale(EPSS *ld,size_t size);
#endif
