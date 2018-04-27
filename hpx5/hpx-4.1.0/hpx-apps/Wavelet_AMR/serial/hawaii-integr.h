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

#ifndef HAWAII_INTEGR_H
#define HAWAII_INTEGR_H

// Tracks the number of times right-hand side computation has been
// performed. Each application of the integrator advances the value of
// time_stamp by n_gen 
extern unsigned time_stamp;

void apply_time_integrator(const double t, const double dt); 

void forward_euler_helper(coll_point_t *point, const double dt);

void rk4_helper1(coll_point_t *point, const double dt);
void rk4_helper2(coll_point_t *point, const double dt);
void rk4_helper3(coll_point_t *point, const double dt);
void rk4_helper4(coll_point_t *point, const double dt); 

#endif
