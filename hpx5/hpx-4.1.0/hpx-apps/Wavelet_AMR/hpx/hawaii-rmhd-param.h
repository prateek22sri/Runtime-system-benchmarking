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

#ifndef HAWAII_RMHD_PARAM_H
#define HAWAII_RMHD_PARAM_H

#include "hawaii-rmhd.h"

typedef enum {
  bc_unknown = 0,
  bc_outflow = 1,
  bc_wall = 2
} boundary_condition_t;

// this can be expanded to delineate types of output
typedef enum {
  no_output = 0,
  normal_output = 1,
} output_condition_t;

typedef struct {
 /* simulation parameters */
  int max_iter; // max number of allowed iterations in time integrator
  double t0;    // initial time stamp of the simulation
  double tf;    // final time stamp of the simulation
  double xmin, ymin, zmin;
  double xmax, ymax, zmax;
  double cfl;

  int prune_frequency;

  double epsilon;

  int output_by_level;
  int output_frequency;
  int print_frequency;

 /* internally used variables */
  int base_step_size;
  int max_index_x;
  int num_req_fields;

 /* fluid parameters */
  int contoprimwarn;
  int use_wave_speeds;
  int reconstruct_Wv;
  double gamma;
  double vacuum;

  /* output */
  int output_fields[n_variab+n_aux];

 /* boundary conditions */
  boundary_condition_t bcs[6];

 /*---------------------------------------------------- 
  * initial data parameters 
  *----------------------------------------------------*/
  int id_type;

  int id_indep_coord;

  // Gaussian initial data
  double id_x0;
  double id_y0;
  double id_z0;
  double id_sigma;
  double id_amp;
  double id_kappa;
  double id_gauss_floor;

  // Riemann problem
  double id_rho_left;
  double id_vx_left;
  double id_vy_left;
  double id_vz_left;
  double id_P_left;

  double id_rho_right;
  double id_vx_right;
  double id_vy_right;
  double id_vz_right;
  double id_P_right;

  // Rayleigh Taylor problem
  int potential;
  double potential_offset;
  double g;
  double p_base;
  double rho1;
  double rho2;

} Pars;

extern Pars pars;

int read_param_file(char *pfile);
int default_params();
int read_int_param(char* pfile, char *name, int *var, const int def_val,
                   const int min_val, const int max_val);
int read_real_param(char* pfile, char *name, double *var, const double def_val,
                   const double min_val, const double max_val);


#endif
