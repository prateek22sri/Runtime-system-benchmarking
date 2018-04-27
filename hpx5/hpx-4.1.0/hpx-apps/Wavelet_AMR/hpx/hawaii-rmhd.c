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
#include <stdatomic.h>
#include <math.h>
#include <assert.h>
#include "hawaii.h"
#include "hawaii-rmhd.h"

#define DIE_ON_C2P_ERRORS 1

double max_eigenval;

#ifdef HPX
hpx_action_t _hlle_rhs_stage1_a = 0;

int _action_hlle_rhs_stage1_a(int *gen, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *stcl_point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&stcl_point))
    return HPX_RESEND;

  assert( stcl_point != NULL );
  if (stcl_point->time_stamp < time_stamp) {
    assert(stcl_point->status[CURRENT_STATUS] != essential);
    stcl_point->status[CURRENT_STATUS] = nonessential;
    advance_time_stamp(stcl_point, *gen);
  }

  //do con_to_prim if needed, but only for nonessential points
  if(stcl_point->status[CURRENT_STATUS] == nonessential) {
    int skipp2c = __atomic_fetch_add(&stcl_point->stage[RHS_HLLE], 1, memory_order_acq_rel);
    if (!skipp2c) {
      con_to_prim(stcl_point->u[*gen], stcl_point->u[*gen], stcl_point->coords.pos);
    }
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}
#endif

// get_local_dt {{{
/*-------------------------------------------------------------------------
 *
 *
 *
 *-------------------------------------------------------------------------*/
double get_local_dt (const coll_point_t *point) {
  double dt = 1.e34;

  assert(max_eigenval > 0.0);

  for (dir_t dir = x_dir; dir < n_dim; dir++) {
#ifdef HPX
    int closest_level = hpx_get_closest_level(point, dir);
#else
    int closest_level = get_closest_level(point, dir);
#endif
    double dx0 = L_dim[dir] / max_index[dir]; 
    int64_t h = (int64_t)1 << (JJ - closest_level); 
    double dx = dx0 * h; 
    dt = fmin(dt, pars.cfl * dx / max_eigenval);
  }
  
  if (fabs(dt) > 100.0) {
    fprintf(stdout, ".");
  }

  return dt;
}
// }}}

// fluxes {{{
/*-------------------------------------------------------------------------
 *
 *  rhs_cal_flux: Calculate flux functions
 *
 *-------------------------------------------------------------------------*/
void flux_x(double *u)
{
  double vx = u[V_VX];
  double vy = u[V_VY];
  double vz = u[V_VZ];
  double P     = u[V_P];
  double rho   = u[V_RHO];

  double e = P/((pars.gamma-1)*rho);
  double E = rho*(0.5*(vx*vx+vy*vy+vz*vz) + e);

  u[F_D]   = rho*vx;
  u[F_SX]  = rho*vx*vx + P;
  u[F_SY]  = rho*vx*vy;
  u[F_SZ]  = rho*vx*vz;
  u[F_TAU] = vx*(E + P);

}

/*-------------------------------------------------------------------------
 *
 *  flux_y
 *
 *-------------------------------------------------------------------------*/
void flux_y(double *u)
{

  double vx = u[V_VX];
  double vy = u[V_VY];
  double vz = u[V_VZ];
  double P   = u[V_P];
  double rho = u[V_RHO];

  double e = P/((pars.gamma-1)*rho);
  double E = rho*(0.5*(vx*vx+vy*vy+vz*vz) + e);

  u[F_D]   = rho*vy;
  u[F_SX]  = rho*vy*vx;
  u[F_SY]  = rho*vy*vy + P;
  u[F_SZ]  = rho*vy*vz;
  u[F_TAU] = vy*(E+P);

}

/*-------------------------------------------------------------------------
 *
 *  flux_z
 *
 *-------------------------------------------------------------------------*/
void flux_z(double *u)
{
  double vx = u[V_VX];
  double vy = u[V_VY];
  double vz = u[V_VZ];
  double P  = u[V_P];
  double rho = u[V_RHO];

  double e = P/((pars.gamma-1)*rho);
  double E = rho*(0.5*(vx*vx+vy*vy+vz*vz) + e);

  u[F_D]   = rho*vz;
  u[F_SX]  = rho*vz*vx;
  u[F_SY]  = rho*vz*vy;
  u[F_SZ]  = rho*vz*vz + P;
  u[F_TAU] = vz*(E+P);

}
// }}}

// HLLE numerical flux {{{
/*-------------------------------------------------------------------------
 *
 *   HLLE update
 *
 *-------------------------------------------------------------------------*/
void num_flux_hlle(coll_point_t *point, const int gen, const int dir)
{
  index_t index = point->index;
  const int NS = 5;
  double *u[NS], *pos[NS];
  double pos1[n_dim], pos2[n_dim];
  //coll_point_t *stcl[5];

  index_t id;
  for (int i = x_dir; i < n_dim; i++) {
    id.idx[i] = index.idx[i];
  }

#ifdef HPX
  double vals[5][n_variab + n_aux] = {{0.0}};
  coord_t coords[5];

  int closest_level = hpx_get_closest_level(point, dir);
#else
  int closest_level = get_closest_level(point, dir);
#endif
  assert( closest_level >= 1 );

  // get the grid spacing
  double dx;
  dx = L_dim[dir] / ns[dir] / (1 << closest_level);
  int h = 1 << (JJ - closest_level); 


  /* the stencil */
  for (int ll=0;ll<5;ll++) {
    if ( ll == 2 ) {
      u[ll] = point->u[gen];
      pos[ll] = point->coords.pos;
     // stcl[ll] = point;
    } else {
      id.idx[dir] = index.idx[dir] + (ll-2)*h;
      if (check_index(&id)) {
#ifdef HPX
        hpx_addr_t cpm = hpx_get_coll_point(&id); 
        hpx_addr_t value_offset = hpx_addr_add(cpm,gen*sizeof(vals[0]),sizeof(coll_point_t));      
        hpx_gas_memget_sync(&vals[ll],value_offset,sizeof(vals[0]));
        // TEST
        //if ( is_array(&index) == 0 || is_array(&id) == 0 ) {
        //  uint64_t m1,m2;
        //  m1 = morton_key(&index);
        //  m2 = morton_key(&id);
        //  printf("%ld %ld %d %d %d is array %d %d\n",m1,m2,m1 % HPX_LOCALITIES, m2%HPX_LOCALITIES,time_stamp,is_array(&index),is_array(&id));
        //}
        //END TEST

        hpx_addr_t coord_offset = hpx_addr_add(cpm,offsetof(coll_point_t,coords),sizeof(coll_point_t));
        hpx_gas_memget_sync(&coords[ll],coord_offset,sizeof(coords[0]));
        u[ll] = vals[ll];
        pos[ll] = coords[ll].pos;
#else
        coll_point_t *cpm=NULL;
        cpm = get_coll_point(&id);
        //Stage 1 should have assured that all points exist, so...
        assert(cpm != NULL);   
 
        u[ll] = cpm->u[gen];
      //  stcl[ll] = cpm;
        pos[ll] = cpm->coords.pos;
#endif
      } else {
        u[ll] = NULL;
        pos[ll] = NULL;
      //  stcl[ll] = NULL;
      }
    }
  }

  int nn = 0;
  int bi;
  for (int ll=0;ll<5;ll++) {
    if (pos[ll] == NULL) {
      nn++;
      bi = ll-2;
    }
  }
  if (nn > 0) {
    for (int ll = 0; ll < n_dim; ll++) {
      pos1[ll] = pos[2][ll];
      pos2[ll] = pos[2][ll];
    }
    if (bi < 0) {
     /* left boundary */
      if (pos[1] ==NULL) {
        pos[1] = pos1;
        pos[0] = pos2;
        pos[1][dir] = pos[2][dir] - dx;
        pos[0][dir] = pos[2][dir] - 2.0*dx;
      }
      else if (pos[0] == NULL) {
        pos[0] = pos2;
        pos[0][dir] = pos[1][dir] - dx;
      }
    }
    else {
     /* right boundary */
      if (pos[3] ==NULL) {
        pos[3] = pos1;
        pos[4] = pos2;
        pos[3][dir] = pos[2][dir] + dx;
        pos[4][dir] = pos[2][dir] + 2.0*dx;
      }
      else if (pos[4] == NULL) {
        pos[4] = pos2;
        pos[4][dir] = pos[3][dir] + dx;
      }
    }
  }

 /* storage for boundary conditions */
  double ubx0[n_variab + n_aux];
  double ubx1[n_variab + n_aux];
  double *ub[2];
  ub[0] = ubx0;
  ub[1] = ubx1;

 /* LEFT boundary conditions */
  if ( u[0] == NULL || u[1] == NULL ) {
    int nbpts, ib;
    if (u[1] == NULL) {
      nbpts = 2;
      ib = 2;
    }
    else {
      nbpts = 1;
      ib = 1;
    }
//  printf("bc = %d, %d\n",pars.bcs[2*dir],bc_wall);
    if (pars.bcs[2*dir] == bc_wall) {
     /* left boundary is a wall */
      for (int pp = 0; pp < nbpts; pp++) {
        for (int i = 0; i < n_variab + n_aux; i++) {
          ub[pp][i] = u[ib+pp+1][i];
        }
/*      printf("nbpts=%d, ib=%d, setting (%d) = (%d)\n",
 *                                       nbpts,ib,ib-pp-1,ib+pp+1); */
        ub[pp][U_SX+dir] *= -1.0;
        ub[pp][V_VX+dir] *= -1.0;
        u[ib-pp-1] = ub[pp];
      }
    }
        else {
     /* left boundary is outflow */
      u[0] = u[ib];
      if (u[1] == NULL) u[1] = u[ib];
    }
  }

  
  //U COMMENT THIS TO SEE THE POINTS THAT DO NOT HAVE COLINEAR S and V
  /* 
  for (int i = 0; i < 5; ++i) {
    if (u[i] == NULL) continue;
    
    index_t idx = point->index;
    idx.idx[dir] = point->index.idx[dir] + (i - 2) * h;
    coll_point_t *cpm;
    if (check_index(&idx))
      cpm = get_coll_point(&idx);
    
    if (fabs(u[i][V_VX]) > 1.0e-12 && fabs(u[i][V_VY]) > 1.0e-12) {
      //is S_i / v_i the same value for each field
      double ratios[3];
      ratios[0] = u[i][U_SX] / u[i][V_VX];
      ratios[1] = u[i][U_SY] / u[i][V_VY];
      if( fabs(ratios[0] - ratios[1]) > 1.0e-12 ) {
        //if( cpm->status[CURRENT_STATUS] == essential) {
        fprintf(stdout, "GEN %d - %d : %lg %lg (%lg) - %lg %lg - %d - %d %d\n", gen, i, ratios[0], ratios[1], ratios[0]/ratios[1], u[i][V_VX], u[i][V_VY], cpm->status[0], cpm->time_stamp, time_stamp);
        //fprintf(stdout, "%d %lg %lg %lg %lg %lg %lg\n", point->time_stamp, point->coords.pos[0], point->coords.pos[1], ratios[0], ratios[1], u[i][V_VX], u[i][V_VY]);
        //}
      } 
    }
  }*/
  

  double ubl[n_variab + n_aux];
  double ubr[n_variab + n_aux];
 /* LEFT boundary conditions */
  if ( u[0] == NULL || u[1] == NULL ) {
    int ib = (u[1] == NULL) ? 2 : 1;
    if (pars.bcs[2*dir] == bc_wall) {
     /* left boundary is a wall */
      for (int i = 0; i < n_variab + n_aux; i++) {
        ubl[i] = u[ib][i];
      }
      ubl[U_SX+dir] *= -1.0;
      ubl[V_VX+dir] *= -1.0;
      u[0] = ubl;
      if (u[1] == NULL) u[1] = ubl;
    }
    else {
     /* left boundary is outflow */
      u[0] = u[ib];
      if (u[1] == NULL) u[1] = u[ib];
    }
  }

 /* RIGHT boundary conditions */
  if ( u[3] == NULL || u[4] == NULL ) {
    int ib = (u[3] == NULL) ? 2 : 3;
    if (pars.bcs[2*dir+1] == bc_wall) {
     /* right boundary is a wall */
      for (int i = 0; i < n_variab + n_aux; i++) {
        ubr[i] = u[ib][i];
      }
      ubr[U_SX+dir] *= -1.0;
      ubr[V_VX+dir] *= -1.0;
      u[4] = ubr;
      if (u[3] == NULL) u[3] = ubr;
    }
    else {
     /* right boundary is outflow */
      u[4] = u[ib];
      if (u[3] == NULL) u[3] = u[ib];
    }
  }
 
  double ul[NS][n_variab + n_aux];
  double ur[NS][n_variab + n_aux];
  double f[NS][n_variab + n_aux];

  // Reconstruct the primitive variables.
  for (int i = 1; i < NS-1; i++) {
    // Loop over primitive vars.
    for (int p = n_variab; p < n_variab + n_prims; p++) {  
      double df1, df2, sdf1, sdf2, slp;
      df1 = u[i][p] - u[i-1][p];
      df2 = u[i+1][p] - u[i][p];
// FIXME-make more efficient
      sdf1 = (df1 > 0.0) ? 1.0 : ((df1 < 0.0) ? -1.0 : 0.0); 
      sdf2 = (df2 > 0.0) ? 1.0 : ((df2 < 0.0) ? -1.0 : 0.0);
// -end FIXME
      slp = 0.5*(sdf1 + sdf2)*fmin(fabs(df1), fabs(df2));
      ul[i+1][p] = u[i][p] + 0.5*slp;
      ur[i][p]   = u[i][p] - 0.5*slp;
    }
  }

  // Apply floor to the primitive variables -- applied to ul and ur
  for (int i = 1; i < NS-1; i++) {
    if (ul[i+1][V_RHO] < pars.vacuum ) ul[i+1][V_RHO] = pars.vacuum;
    if (ul[i+1][V_P] < pars.vacuum) ul[i+1][V_P] = pars.vacuum;
    if (ur[i][V_RHO] < pars.vacuum ) ur[i][V_RHO] = pars.vacuum;
    if (ur[i][V_P] < pars.vacuum) ur[i][V_P] = pars.vacuum;
  }

  // Call primtocon for reconstructed conservative vars
  for (int i = 1; i < NS-1; i++) {
    prim_to_con(ul[i+1]);
    prim_to_con(ur[i]);
  }

  // HLLE flux
  for (int i = 2; i < 4; i++) {
    if (dir == x_dir) {
      flux_x(ul[i]);
      flux_x(ur[i]);
    }
    else if (dir == y_dir) {
      flux_y(ul[i]);
      flux_y(ur[i]);
    }
    else {
      flux_z(ul[i]);
      flux_z(ur[i]);
    }
    double Lpl, Lml, Lpr, Lmr;
    eigen_values(&Lpl, &Lml, ul[i], dir);
    eigen_values(&Lpr, &Lmr, ur[i], dir);
    double bp = fmax(fmax(0.0, Lpl), Lpr);
    double bm = fmin(fmin(0.0, Lml), Lmr);
    for (int m = 0; m < n_variab; m++) {
      f[i][m] = (bp*ul[i][m+F_D] - bm*ur[i][m+F_D] 
                     + bp*bm*(ur[i][m] - ul[i][m])) / (bp - bm);
    }
  }

  /* only call the source routine once per update. enforce this
  * by checking that dir==0.
  */
  if ( dir == 0 ) {
    cal_source(point->rhs[gen], point->u[gen], pos[2]);
  }
  
  for (int m = 0; m < n_variab; m++) {
    point->rhs[gen][m] -= ( f[3][m] - f[2][m] ) / dx;
  }
}

void cal_source(double *rhs, const double *u, const double *pos)
{

  if (pars.potential == 1 ) {
    rhs[U_SY] += -u[V_RHO]*pars.g;
    rhs[U_TAU] += -u[V_RHO]*u[V_VY]*pars.g;
    //rhs[U_SX] += u[V_RHO]/pow(pos[0]-pars.potential_offset,2);
    //rhs[U_TAU] += u[V_RHO]*u[V_VX]/pow(pos[0]-pars.potential_offset,2);
  }
}
// }}}

// prim_to_con {{{
/*-------------------------------------------------------------------------
 *
 *  
 *
 *-------------------------------------------------------------------------*/
void prim_to_con(double *u)
{
  double rho    = u[V_RHO];
  double vx  = u[V_VX];
  double vy  = u[V_VY];
  double vz  = u[V_VZ];
  double P      = u[V_P];

  double e = P/((pars.gamma-1)*rho);
  double E = rho*(0.5*(vx*vx+vy*vy+vz*vz) + e);

  u[U_D]   = rho;
  u[U_TAU] = E;
  u[U_SX]  = rho*vx;
  u[U_SY]  = rho*vy;
  u[U_SZ]  = rho*vz;

}
// }}}

// eigenvalues {{{
/*-------------------------------------------------------------------------
 *
 *
 *
 *-------------------------------------------------------------------------*/
void eigen_values(double *Lp, double *Lm, double *u, const int dir)
{

  const int ltrace = 0;

  if (ltrace) {
    printf("CalMaxSpeeds:  begin\n");
  }

  if (pars.use_wave_speeds == 1) {
    PFEigenVals(Lp, Lm, u, dir);
  }
  else {
    DefaultEigenVals(Lp, Lm, u, dir);
  }
  
}

/*-------------------------------------------------------------------------
 *
 *  
 *
 *-------------------------------------------------------------------------*/
void PFEigenVals(double *Lp, double *Lm, double *u, const int dir)
{
  double gamma = pars.gamma;
  double rho   = u[V_RHO];
  double p     = u[V_P];

  double vu[3];
  double cs = cal_cs(rho, p, gamma);

  vu[0] = u[V_VX];
  vu[1] = u[V_VY];
  vu[2] = u[V_VZ];

  *Lp = vu[dir] + cs;
  *Lm = vu[dir] - cs;

  double local_max_speed = fmax(fabs(*Lp), fabs(*Lm));
  max_eigenval = fmax(max_eigenval, local_max_speed);

}

/*-------------------------------------------------------------------------
 *
 *  Calculate the sound speed.
 *
 *-------------------------------------------------------------------------*/
double cal_cs(double rho, double p, double gamma) 
{
  assert(rho > 0.0);
  assert(p >= 0.0);
  return sqrt(gamma*p/rho);
}

/*-------------------------------------------------------------------------
 *
 * Calculate local light cone speeds
 *
 *-------------------------------------------------------------------------*/
void DefaultEigenVals(double *Lp, double *Lm, double *u, const int dir)
{
  printf("DefaultEigenVals: should not be here.\n");
  exit(2);
#if 0
  double Alpha, Beta[3];


  *Lp =  Alpha*sqrt(gu[dir][dir]) - Beta[dir];
  *Lm = -Alpha*sqrt(gu[dir][dir]) - Beta[dir];
#endif
}
// }}}

// apply_floor {{{
/*-------------------------------------------------------------------------
 *
 * Floor D and rescale the momentum to satisfy S^2 < (tau + D)^2 if necessary.
 *
 *-------------------------------------------------------------------------*/
void apply_floor(double *u)
{
  double Sd[3];
  if ( u[U_D] < pars.vacuum) {
    u[U_D]  = pars.vacuum;
    u[U_SX] = 0.0;
    u[U_SY] = 0.0;
    u[U_SZ] = 0.0;
    u[U_TAU] = pars.vacuum;
  }

  u[U_TAU] = fmax(pars.vacuum,u[U_TAU]);
  Sd[0] = u[U_SX];
  Sd[1] = u[U_SY];
  Sd[2] = u[U_SZ];
  double Ssq = Sd[0]*Sd[0]+Sd[1]*Sd[1]+Sd[2]*Sd[2];
  double S2max = 2.0*u[U_D]*u[U_TAU] + pars.vacuum;
  if (S2max < Ssq) {
    double t_min = sqrt(S2max/Ssq);
    //printf("XX t_min=%g, S2max=%g, Ssq=%g\n",t_min,S2max,Ssq);
    u[U_SX] *= t_min;
    u[U_SY] *= t_min;
    u[U_SZ] *= t_min;
  }
}
// }}}

// con_to_prim {{{
/*-------------------------------------------------------------------------
 *
 *
 *
 *-------------------------------------------------------------------------*/
int con_to_prim(double *du, double *du_old, double *pos) 
{
  double    vacuum  = pars.vacuum;
  double    u[n_variab + n_aux];

  int ii;
  for (ii = 0; ii < n_variab; ii++) {
    u[ii] = du[ii];
    if (!isfinite(u[ii])) {
      printf("con_to_prim. u[%d] =%g is not finite.\n",ii,u[ii]);
    }
  }

 /* copy the prim vars (only) into the solution at the advanced time.
  */
  for (ii = n_variab; ii < n_variab + n_prims; ii++) {
    u[ii] = du_old[ii];
  }

  /* Floor the primitive variables.  When running with AMR, there have been
     cases where interpolation has resulted in the variables D, tau, rho, or
     P having negative values.  D and tau are floored in contoprim.  rho and
     P are used here for the initial guess.  Floor them just to be sure. */
  if (u[V_RHO] < vacuum) u[V_RHO] = vacuum;
  if (u[V_P]   < vacuum) u[V_P]   = vacuum;

 /* apply the floor here. */
  apply_floor(u);

  double rho = u[U_D];
  double vx  = u[U_SX]/rho;
  double vy  = u[U_SY]/rho;
  double vz  = u[U_SZ]/rho;
  double vsq = vx*vx + vy*vy + vz*vz;
  double rhoe = u[U_TAU] - 0.5*rho*vsq;
  double P = (pars.gamma-1.0)*rhoe;
  double Pmin = vacuum*vacuum;
  if (!isfinite(P)) {
    printf("P is a nan.\n");
    for (int i = 0; i < 5; i++){
      printf("i=%d, u=%g, v=%g\n",i,u[U_D+i],u[V_RHO+i]);
    }
  }
  if (P < Pmin) {
    P = Pmin;
    printf("P < 0. Resetting P = %g\n",Pmin);
  }

  //printf("rho = %g,   P = %g\n",rho,P);
  assert(P > 0.0);

  u[V_RHO] = rho;
  u[V_VX]  = vx;
  u[V_VY]  = vy;
  u[V_VZ]  = vz;
  u[V_P]   = P;

  for (ii = n_variab; ii < n_variab + n_prims; ii++) {
    du[ii] = u[ii];
  }

  assert(u[V_RHO] > 0.0);
  assert(u[V_P] >= 0.0);

  return 0;
}

// }}}

// rk4 rhs routines {{{
/*-------------------------------------------------------------------------
 *    This routine will verify the existence of the hlle flux stencil points
 *    and will call con_to_prim on any nonessential points that have not had
 *    their prims computed.
 *-------------------------------------------------------------------------*/
void hlle_rhs_stage1(coll_point_t *point, const int gen) {
#ifdef HPX
  int skipit = __atomic_fetch_add(&point->stage[RHS_HLLE], 1, memory_order_acq_rel);
#else
  int skipit = point->stage[RHS_HLLE]++;
#endif
  if (skipit) {
    return;
  }

  for (int dir = x_dir; dir < n_dim; ++dir) {
#ifdef HPX
    int closest_level = hpx_get_closest_level(point, dir);
#else
    int closest_level = get_closest_level(point, dir);
#endif
    assert(closest_level >= 1);
    int h = 1 << (JJ - closest_level);
      
    for (int stcl = 0; stcl < 5; ++stcl) {
      if (stcl == 2) {  //This point must exist, as it is 'point'
        continue;
      } 

      //what is the index of the stencil point?
      index_t index = point->index;
      index.idx[dir] += (stcl - 2) * h;
          
      //check that we are on grid
      if (!check_index(&index)) {
        continue;
      }

#ifdef HPX
      hpx_addr_t stcl_point = hpx_get_coll_point(&index);
      int e = hpx_call_sync(stcl_point, _hlle_rhs_stage1_a, NULL, 0,  &gen, sizeof(gen));
      assert(e == HPX_SUCCESS);
#else
      coll_point_t *stcl_point = get_coll_point(&index);
      assert( stcl_point != NULL );

      if (stcl_point->time_stamp < time_stamp) {
        assert(stcl_point->status[CURRENT_STATUS] != essential);
        stcl_point->status[CURRENT_STATUS] = nonessential;
        advance_time_stamp(stcl_point, gen);
      }

      //do con_to_prim if needed, but only for nonessential points
      if(stcl_point->status[CURRENT_STATUS] == nonessential) {
        int skipp2c = stcl_point->stage[RHS_HLLE]++;
        if (!skipp2c) {
          con_to_prim(stcl_point->u[gen], stcl_point->u[gen], stcl_point->coords.pos);
        }
      }
#endif
    } // stcl points
  } // dir
}


/*-------------------------------------------------------------------------
 *    
 *    
 *
 *-------------------------------------------------------------------------*/
void hlle_rhs_stage2(coll_point_t *point, const int gen) {
#ifdef HPX
  int skipit = __atomic_fetch_add(&point->stage[RHS_FLUX], 1, memory_order_acq_rel);
#else
  int skipit = point->stage[RHS_FLUX]++;
#endif
  if (skipit) {
    return;
  }

  // clear RHS values
  for (int ivar = 0; ivar < n_variab; ivar++) 
    point->rhs[gen][ivar] = 0.0; 

  // compute the fluxes
  for (dir_t dir = x_dir; dir < n_dim; dir++) {
    num_flux_hlle(point, gen, dir);
  }
}
// }}}

// initial data routines {{{
/*-------------------------------------------------------------------------
 *    
 *    
 *
 *-------------------------------------------------------------------------*/
void riemann_problem(const coord_t *coord, double *u, const int dir) {
#define noTANH_DATA
#ifdef TANH_DATA
 /* 
  * A tanh function interpolates between the left and right states.
  * These data are analytic (smooth and differentiable), but do not
  * lead to the same solution as the real shocktube with discontinuous
  * data.
  */
  double w = 0.5 + 0.5*tanh((coord->pos[dir] - pars.id_x0)/pars.id_sigma);
  u[V_RHO] = (1.0 - w) * pars.id_rho_left + w * pars.id_rho_right;
  u[V_VX]  = (1.0 - w) * pars.id_vx_left  + w * pars.id_vx_right;
  u[V_VY]  = (1.0 - w) * pars.id_vy_left  + w * pars.id_vy_right;
  u[V_VZ]  = (1.0 - w) * pars.id_vz_left  + w * pars.id_vz_right;
  u[V_P]   = (1.0 - w) * pars.id_P_left   + w * pars.id_P_right;
#else
 /*
  * Discontinuous initial data for the Riemann problem. 
  */
  if ( coord->pos[dir] < 0.0 ) { 
    u[V_RHO] = pars.id_rho_left;
    u[V_VX] = pars.id_vx_left;
    u[V_VY] = pars.id_vy_left;
    u[V_VZ] = pars.id_vz_left;
    u[V_P] = pars.id_P_left;
  } else {
    u[V_RHO] = pars.id_rho_right;
    u[V_VX] = pars.id_vx_right;
    u[V_VY] = pars.id_vy_right;
    u[V_VZ] = pars.id_vz_right;
    u[V_P] = pars.id_P_right;
  }
#endif

  // convert to conservative variables
  prim_to_con(u);

  // to set the initial time step, we need to find the max characteristic
  // speed on the grid. Call PFEigenVals for every point in every direction.
  double Lp, Lm;
  for (dir_t dir = x_dir; dir < n_dim; dir++) {
    PFEigenVals(&Lp, &Lm, u, dir);
  }

} 

/*-------------------------------------------------------------------------
 *    
 *    
 *
 *-------------------------------------------------------------------------*/
void gaussian_data(const coord_t *coord, double *u) 
{
  double arg;
  arg = (coord->pos[x_dir] - pars.id_x0) * (coord->pos[x_dir] - pars.id_x0);
#if n_dim == 2 || n_dim == 3
  arg += (coord->pos[y_dir] - pars.id_y0)*(coord->pos[y_dir] - pars.id_y0);
#endif
#if n_dim == 3
  arg += (coord->pos[z_dir] - pars.id_z0)*(coord->pos[z_dir] - pars.id_z0);
#endif

  u[V_RHO] = pars.id_gauss_floor 
                + pars.id_amp * exp(-arg/(pars.id_sigma * pars.id_sigma));
  u[V_VX] = 0.0;
  u[V_VY] = 0.0;
  u[V_VZ] = 0.0;
  u[V_P] = pars.id_kappa * pow(u[V_RHO], pars.gamma);

  // convert to conservative variables
  prim_to_con(u);

  // to set the initial time step, we need to find the max characteristic
  // speed on the grid. Call PFEigenVals for every point in every direction.
  double Lp, Lm;
  for (dir_t dir = x_dir; dir < n_dim; dir++) {
    PFEigenVals(&Lp, &Lm, u, dir);
  }

}

void rayleigh_taylor(const coord_t *coord, double *u) 
{
  assert(n_dim == 2);

  double x = coord->pos[x_dir];
  double y = coord->pos[y_dir];
  if ( y < 0.5*pars.ymax) {
    u[V_P] = pars.p_base + pars.rho1*pars.g*y;
  } else {
    u[V_P] = pars.p_base + pars.rho1*pars.g*0.5*pars.ymax 
                         + pars.rho2*pars.g*(y-0.5*pars.ymax);
  }
  double h = 0.005;
  double A = 0.01;
  double pi = 4.0*atan(1.0);
  double psi = 0.5*A*( cos(2*pi*x/pars.xmax) + cos(2*pi*(pars.xmax-x)/pars.xmax)) + 0.5*pars.ymax;

  u[V_RHO] = pars.rho1 + 0.5*(pars.rho2-pars.rho1)*(1.0 + tanh( (y-psi)/h ) );
  u[V_VX] = 0.0;
  u[V_VY] = 0.0;
  u[V_VZ] = 0.0;

  // convert to conservative variables
  prim_to_con(u);

  // to set the initial time step, we need to find the max characteristic
  // speed on the grid. Call PFEigenVals for every point in every direction.
  double Lp, Lm;
  for (dir_t dir = x_dir; dir < n_dim; dir++) {
    PFEigenVals(&Lp, &Lm, u, dir);
  }

}

/*-------------------------------------------------------------------------
 *    
 *    
 *
 *-------------------------------------------------------------------------*/
void gaussian(double *rho, const double x, const double x0, const double sigma) 
{
  *rho = exp(-(x-x0)*(x-x0)/(sigma*sigma));
}

// }}}
