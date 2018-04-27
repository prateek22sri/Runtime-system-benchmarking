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

#ifndef HAWAII_UTILS_H
#define HAWAII_UTILS_H

#include <stdbool.h>
#include "hawaii-types.h"
#include "hawaii-wavelet.h"

#ifdef RNPL
#include "bbhutil.h"
#endif

#ifdef SILO
#include <silo.h>
#endif

extern double eps[n_variab]; 
extern double eps_scale[n_variab];
extern double L_dim[n_dim]; 
extern const int deriv_offset[4];

typedef struct vizpoint {
  double x;
  double y;
  double z;
  double value[n_variab+n_aux];
} vizpoint;

#ifdef HPX
int hpx_get_closest_level(const coll_point_t *point, const int dir);
#endif

void validate_parameters(void); 

index_t linear_to_index(const int rank); 

int index_to_linear(const index_t *index); 

index_t add_index(const index_t *lhs, const index_t *rhs, const int scalar); 

bool check_index(const index_t *index); 

coord_t set_coordinate(const index_t *index); 

double norm2(const coord_t *point1, const coord_t *point2); 

coll_status_t set_point_status(const double *wavelet); 

coll_status_t superior_status(const coll_status_t lhs, 
                              const coll_status_t rhs);

coll_status_t inferior_status(const coll_status_t lhs, 
                              const coll_status_t rhs);

bool has_superior_status(const coll_status_t lhs, 
                         const coll_status_t rhs);

bool has_inferior_status(const coll_status_t lhs, 
                         const coll_status_t rhs);

bool has_same_status(const coll_status_t lhs, 
                     const coll_status_t rhs);

int get_level(const index_t *index); 

void check_wavelet_stencil(coll_point_t *point, const int gen); 

void check_derivative_stencil(coll_point_t *point, const int gen); 

void check_extension_stencil(coll_point_t *point, const int gen, 
                             const int dir, const char type); 

void advance_time_stamp(coll_point_t *point, const int gen); 

double get_global_dt(void); 

void global_dt_helper(coll_point_t *point, double *dt, 
                                double *mins, double *maxs); 

void minmax_range_reducer(double *u, double *mins, double *maxs);

void set_eps_scale(double *mins, double *maxs);

#ifdef HPX
int rhs_helper(coll_point_t *point, const int gen,function_pointer_t func_type);
#else
int rhs_helper(coll_point_t *point, const int gen, 
                void(*func)(coll_point_t *, const int)); 
#endif

#ifdef HPX
int integrator_helper(coll_point_t *point, const double dt,int);
#else
int integrator_helper(coll_point_t *point, const double dt,
                       void(*func)(coll_point_t *, const double));
#endif

int stage_reset_helper(coll_point_t *point);

void point_stage_reset_helper(coll_point_t *point);

void wavelet_adjust_grids_helper(coll_point_t *point); 
void adjust_add_neighbors_helper(coll_point_t *point);

int wavelet_integrator_helper(coll_point_t *point, const int gen, 
                               const wavelet_trans_t type, 
                               const int *wavelet_mask); 

void status_validate_up_helper(coll_point_t *point); 

void status_validate_down_helper(coll_point_t *point);

void complete_hlle_stencils(void);

void hlle_stencil_completion_helper(coll_point_t *point);

void visualize_grid(char *fname, int noness); 

int get_closest_level(const coll_point_t *point, const int dir);

#if defined(RNPL) || defined(SILO)
void grid_sdf_output(unsigned int stamp,double t);
#endif

#endif
