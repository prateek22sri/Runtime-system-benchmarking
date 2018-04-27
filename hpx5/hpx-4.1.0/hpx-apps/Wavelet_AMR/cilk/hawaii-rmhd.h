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

#ifndef HAWAII_RMHD_H
#define HAWAII_RMHD_H

#include "hawaii-param.h"
#include "hawaii-types.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/* conservative variables */
#define U_D 0
#define U_SX 1
#define U_SY 2
#define U_SZ 3
#define U_TAU 4

/* primitive variables */
#define n_prims 5
#define V_RHO 5
#define V_VX 6
#define V_VY 7
#define V_VZ 8
#define V_P  9

/* fluxes */
#define F_D 10
#define F_SX 11
#define F_SY 12
#define F_SZ 13
#define F_TAU 14

double get_local_dt(const coll_point_t *point); 
void flux_x(double *u);
void flux_y(double *u);
void flux_z(double *u);
void prim_to_con(double *u);
void eigen_values(double *Lp, double *Lm, double *u, const int direc);
void PFEigenVals(double *Lp, double *Lm, double *u, const int direc);
double cal_cs(double rho, double p, double gamma);
void DefaultEigenVals(double *Lp, double *Lm, double *u, const int direc);
void apply_floor(double *u);
int con_to_prim(double *du, double *du_old, double *pos);
void MHDWorkVars(double *fpar, double *u, double gd[3][3], 
                 double gu[3][3], double *xpt, double gamma);
int FindFluidBounds(double *ql, double *fpar);
void num_flux_hlle(coll_point_t *point, const int gen, const int dir);
void hlle_rhs_stage1(coll_point_t *point, const int gen);
void hlle_rhs_stage2(coll_point_t *point, const int gen);
void gaussian(double *rho, const double x, const double x0, const double sigma);
void gaussian_data(const coord_t *coord, double *u);
void riemann_problem(const coord_t *coord, double *u, const int dir);
void calv(double *u, double *wv);

void good_guess_con_to_prim(coll_point_t *point, int gen, int stage);
void cal_source(double *rhs, const double *u, const double *pos);
void rayleigh_taylor(const coord_t *coord, double *u);


#endif 
