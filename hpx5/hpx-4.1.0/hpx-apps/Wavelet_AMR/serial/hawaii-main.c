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

#if defined(RMHD) 
Pars pars;
#endif

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

static int _main_action(int argc, char *argv[]) {

#ifdef RNPL
  // read parameter file
  char pfile[128];
  if ( argc < 2 ) {
    printf(" Usage: %s <parameter file>\n",argv[0]);
    exit(-1);
  }
  strcpy(pfile, argv[1]);
  if (read_param_file(pfile) != 1) {
    printf("Error reading parameters. Die here.\n");
    exit(-1);
  }
#else
  default_params();
#endif

  double startup = getticks();
  // Setup common masking patterns
  setup_default_masks();

  // Initialize problem parameters
  max_eigenval = -1.0;    /* initialize to negative value */ 
  problem_init(); 
 
  // Generate adptive grid from initial profile
  storage_init(); 
  create_full_grids(); 
  create_adap_grids();
  complete_hlle_stencils();
  
  //visualize_grid("grid000.txt",0);
  
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
    double dt = get_global_dt();    
    dt = fmin(dt, pars.tf - t); 
    if ( pars.print_frequency != 0 ) {
      if (time_stamp/n_gen % pars.print_frequency == 0 ) {
        printf("step %d: t_next = %14.7e, dt = %10.3e\n", 
         time_stamp / n_gen, t+dt, dt); 
      }
    }

    double s1 = getticks();
    max_eigenval = -1.0;     
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
  
  storage_cleanup(); 

  return 0;
}


int main(int argc, char *argv[]) {
  return _main_action(argc, argv);
}

