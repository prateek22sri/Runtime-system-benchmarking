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
#include <string.h>
#include "hawaii.h"


#ifdef RNPL
#include "bbhutil.h"

char *read_line(FILE *fin) {
    char *buffer;
    char *tmp;
    int read_chars = 0;
    int bufsize = 512;
    char *line = malloc(bufsize);

    if ( !line ) {
        return NULL;
    }

    buffer = line;

    while ( fgets(buffer, bufsize - read_chars, fin) ) {
        read_chars = strlen(line);

        if ( line[read_chars - 1] == '\n' ) {
            line[read_chars - 1] = '\0';
            return line;
        }

        else {
            bufsize = 2 * bufsize;
            tmp = realloc(line, bufsize);
            if ( tmp ) {
                line = tmp;
                buffer = line + read_chars;
            }
            else {
                free(line);
                return NULL;
            }
        }
    }
    return NULL;
}

int read_param_file(char *pfile)
{

  int ok = 1;
  const double huge  = 1.0e14;
  const double small = 1.0e-14;

  ok *= read_int_param(pfile, "max_iter", &pars.max_iter, 1, 1, 500000000); 
  ok *= read_int_param(pfile, "prune_frequency", &pars.prune_frequency,1,1,50); 
  ok *= read_real_param(pfile, "epsilon", &pars.epsilon, 1.0e-3, small, 0.5);

  pars.t0 = 0.0; // hard code the initial time.
  ok *= read_real_param(pfile, "tf", &pars.tf, 1.0, pars.t0, huge);
  ok *= read_real_param(pfile, "xmin", &pars.xmin, 0.0, -huge, huge);
  ok *= read_real_param(pfile, "xmax", &pars.xmax, 1.0, -huge, huge);
  ok *= read_real_param(pfile, "ymin", &pars.ymin, 0.0, -huge, huge);
  ok *= read_real_param(pfile, "ymax", &pars.ymax, 1.0, -huge, huge);
  ok *= read_real_param(pfile, "zmin", &pars.zmin, 0.0, -huge, huge);
  ok *= read_real_param(pfile, "zmax", &pars.zmax, 1.0, -huge, huge);
  ok *= read_real_param(pfile, "cfl", &pars.cfl, 0.1, small, 0.5);

  // relativistic fluid parameters
  ok *= read_real_param(pfile, "gamma", &pars.gamma, 1.6, 1.0, 2.0);
  ok *= read_real_param(pfile, "vacuum", &pars.vacuum, 1.0e-10, small, 1.0);
  ok *= read_int_param(pfile, "contoprimwarn", &pars.contoprimwarn, 1, 0, 1);
  ok *= read_int_param(pfile, "use_wave_speeds", &pars.use_wave_speeds, 1, 0, 1);
 ok *= read_int_param(pfile, "reconstruct_Wv", &pars.reconstruct_Wv, 1, 0, 1);


  // boundary conditions
  char **bcs;
  bcs = (char **) malloc(2*n_dim*sizeof(char *));
  for (int i = 0; i < 2*n_dim; i++) {
    bcs[i] = (char *) malloc(32*sizeof(char));
  }
  if (get_str_param(pfile, "boundaries", bcs, 2*n_dim)!= 1) {
    for(int i = 0; i < 2*n_dim; i++) {
      pars.bcs[i] = bc_outflow;
    }
  }
  else {
    for (int i = 0; i < 2*n_dim; i++) {
      if ( strcmp(bcs[i],"outflow") == 0 ) {
        pars.bcs[i] = bc_outflow;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf("## i=%d, bc_outflow\n",i);
#else
        printf("## i=%d, bc_outflow\n",i);
#endif
      }
      else if ( strcmp(bcs[i],"wall") == 0) {
        pars.bcs[i] = bc_wall;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf("## i=%d, bc_wall\n",i);
#else
        printf("## i=%d, bc_wall\n",i);
#endif
      }
      else {
        pars.bcs[i] = bc_unknown;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf("## i=%d, bc_unknown\n",i);
#else
        printf("## i=%d, bc_unknown\n",i);
#endif
      }
    }
  }
  for (int i = 0; i < 2*n_dim; i++) {
    free(bcs[i]);
  }
  free(bcs);

  // output fields
  ok *= read_int_param(pfile, "output_by_level", &pars.output_by_level, 0, 0, 1);
  ok *= read_int_param(pfile, "output_frequency", &pars.output_frequency, 
                       0, 0, 1000000);
  ok *= read_int_param(pfile, "print_frequency", &pars.print_frequency,
                       1, 0, 1000000);

  // output fields
  // find out how many fields for which output is requested
  FILE *fp;
  char *line;
  fp = fopen(pfile,"r");
  int count = 0;
  if ( fp ) {
    while ( (line = read_line(fp))) {
      if ( strstr(line,"output_functions") ) {
        // count how many fields listed here
        const char *tmp = line;
        while( (tmp=strstr(tmp,"\"")) ) { 
          count++;
          tmp++;
        }
      }
    }
    fclose(fp);
  }
  int num_req_fields = count/2;
  pars.num_req_fields = num_req_fields;

  // output fields
  // initialize output off
  for (int i=0;i<n_variab+n_aux;i++) {
    pars.output_fields[i] = no_output;
  }

  char **output_fields;
  output_fields = (char **) malloc((num_req_fields)*sizeof(char *));
  for (int i = 0; i < num_req_fields ; i++) {
    output_fields[i] = (char *) malloc(32*sizeof(char));
  }
  
  if (get_str_param(pfile, "output_functions", output_fields, num_req_fields)!= 1) {
    for(int i = 0; i < n_variab+n_aux; i++) {
      pars.output_fields[i] = no_output;
    }
  }
  else {
    for (int i = 0; i < num_req_fields; i++) {
      if ( strcmp(output_fields[i],"rho") == 0 ) {
        pars.output_fields[V_RHO] = normal_output;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf(" rho output selected\n");   
#else
        printf(" rho output selected\n");
#endif
      }
      if ( strcmp(output_fields[i],"vx") == 0 ) {
        pars.output_fields[V_VX] = normal_output;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf(" vx output selected\n");
#else
        printf(" vx output selected\n");
#endif
      }
      if ( strcmp(output_fields[i],"vy") == 0 ) {
        pars.output_fields[V_VY] = normal_output;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf(" vy output selected\n");
#else
        printf(" vy output selected\n");
#endif
      }
      if ( strcmp(output_fields[i],"vz") == 0 ) {
        pars.output_fields[V_VZ] = normal_output;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf(" vz output selected\n");
#else
        printf(" vz output selected\n");
#endif
      }
      if ( strcmp(output_fields[i],"p") == 0 ) {
        pars.output_fields[V_P] = normal_output;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf(" p output selected\n");
#else
        printf(" p output selected\n");
#endif
      }
      if ( strcmp(output_fields[i],"D") == 0 ) {
        pars.output_fields[U_D] = normal_output;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf(" D output selected\n");
#else
        printf(" D output selected\n");
#endif
      }
      if ( strcmp(output_fields[i],"Sx") == 0 ) {
        pars.output_fields[U_SX] = normal_output;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf(" Sx output selected\n");
#else
        printf(" Sx output selected\n");
#endif
      }
      if ( strcmp(output_fields[i],"Sy") == 0 ) {
        pars.output_fields[U_SY] = normal_output;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf(" Sy output selected\n");
#else
        printf(" Sy output selected\n");
#endif
      }
      if ( strcmp(output_fields[i],"Sz") == 0 ) {
        pars.output_fields[U_SZ] = normal_output;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf(" Sz output selected\n");
#else
        printf(" Sz output selected\n");
#endif
      }
      if ( strcmp(output_fields[i],"tau") == 0 ) {
        pars.output_fields[U_TAU] = normal_output;
#ifdef HPX
        if ( HPX_LOCALITY_ID == 0 ) printf(" tau output selected\n");
#else
        printf(" tau output selected\n");
#endif
      }
    }
  }

  for (int i = 0; i < num_req_fields; i++) {
    free(output_fields[i]);
  }
  free(output_fields);

  // initial data parameters
  ok *= read_int_param(pfile, "id_type", &pars.id_type, 1, 1, 3);

  ok *= read_int_param(pfile, "id_indep_coord", &pars.id_indep_coord, 0, 0, 2);

  ok *= read_real_param(pfile, "id_x0", &pars.id_x0, 0.0, -1.0e2,1.0e2);
  ok *= read_real_param(pfile, "id_y0", &pars.id_y0, 0.0, -1.0e2,1.0e2);
  ok *= read_real_param(pfile, "id_z0", &pars.id_z0, 0.0, -1.0e2,1.0e2);
  ok *= read_real_param(pfile, "id_sigma", &pars.id_sigma, 0.1, 1.0e-6,1.0e1);
  ok *= read_real_param(pfile, "id_amp", &pars.id_amp, 1.0, -1.0e-6,1.0e9);
  ok *= read_real_param(pfile, "id_kappa", &pars.id_kappa, 1.0, 1.0e-6, 1.0e9);
  ok *= read_real_param(pfile, "id_gauss_floor", &pars.id_gauss_floor, 
                        0.1, 1.0e-6,1.0e1);

  ok *= read_real_param(pfile, "id_rho_left", &pars.id_rho_left,1.0,small,huge);
  ok *= read_real_param(pfile, "id_vx_left", &pars.id_vx_left, 1.0,-1.0,1.0);
  ok *= read_real_param(pfile, "id_vy_left", &pars.id_vy_left, 1.0,-1.0,1.0);
  ok *= read_real_param(pfile, "id_vz_left", &pars.id_vz_left, 1.0,-1.0,1.0);
  ok *= read_real_param(pfile, "id_P_left", &pars.id_P_left, 1.0,small,huge);

  ok *= read_real_param(pfile,"id_rho_right",&pars.id_rho_right,1.0,small,huge);
  ok *= read_real_param(pfile,"id_vx_right",&pars.id_vx_right,1.0,-1.0,1.0);
  ok *= read_real_param(pfile,"id_vy_right",&pars.id_vy_right,1.0,-1.0,1.0);
  ok *= read_real_param(pfile,"id_vz_right",&pars.id_vz_right,1.0,-1.0,1.0);
  ok *= read_real_param(pfile,"id_P_right",&pars.id_P_right,1.0,small,huge);

  ok *= read_int_param(pfile, "potential", &pars.potential, 0, 0, 1);
  ok *= read_real_param(pfile,"potential_offset",&pars.potential_offset,-40.0,-huge,huge);
  ok *= read_real_param(pfile,"g",&pars.g,-1.0,-huge,huge);
  ok *= read_real_param(pfile,"p_base",&pars.p_base,5.0,0.0,huge);
  ok *= read_real_param(pfile,"rho1",&pars.rho1,1.0,0.0,huge);
  ok *= read_real_param(pfile,"rho2",&pars.rho2,2.0,0.0,huge);

  if (ok != 1) {
    printf("There was an error reading the parameters.\n");
  }

  return ok;
}
 
int read_int_param(char* pfile, char *name, int *var, const int def_val,
                   const int min_val, const int max_val)
{
  if (get_param(pfile, name, "long", 1, var) != 1) {
    *var = def_val;
  }
  if (*var > max_val) {
    printf("%s = %d is out of range. max(%s) = %d\n",name,*var,name,max_val);
    return 0;
  }
  if (*var < min_val) {
    printf("%s = %d is out of range. min(%s) = %d\n",name,*var,name,min_val);
    return 0;
  }
  return 1;
}

int read_real_param(char* pfile, char *name, double *var, const double def_val,
                   const double min_val, const double max_val)
{
  if (get_param(pfile, name, "double", 1, var) != 1) {
    *var = def_val;
  }
  if (*var > max_val) {
    printf("%s = %g is out of range. max(%s) = %g\n",name,*var,name,max_val);
    return 0;
  }
  if (*var < min_val) {
    printf("%s = %g is out of range. min(%s) = %g\n",name,*var,name,min_val);
    return 0;
  }
  return 1;
}
#else
int default_params()
{
  // no par file read-in supported; hard code parameters
  pars.max_iter = 400;
  pars.prune_frequency = 1;
  pars.epsilon = 1.e-4;

  pars.t0 = 0.0; // hard code the initial time.
  pars.tf = 2.0;
  pars.xmin = -1.0;
  pars.xmax = 1.0;
  pars.ymin = -1.0;
  pars.ymax = 1.0;
  pars.zmin = -1.0;
  pars.zmax = 1.0;
  pars.cfl = 0.2;

  pars.gamma = 1.3333333333333;
  pars.vacuum = 1.e-10;
  pars.contoprimwarn = 1;
  pars.use_wave_speeds = 1;
  pars.reconstruct_Wv = 1;

  for(int i = 0; i < 2*n_dim; i++) {
    pars.bcs[i] = bc_outflow;
  }

  pars.output_by_level = 0;
  pars.output_frequency = 1;
  pars.print_frequency = 1;

  pars.output_fields[V_RHO] = normal_output;
  pars.output_fields[V_VX] = normal_output;
  pars.output_fields[V_P] = normal_output;
  pars.num_req_fields = 3;

  pars.id_type = 2;
  pars.id_indep_coord = 0;

  pars.id_x0    = 0.0;
  pars.id_y0    = 0.0;
  pars.id_z0    = 0.0;
  pars.id_sigma = 0.04;
  pars.id_amp = 100.0;
  pars.id_kappa = 10.0;
  pars.id_gauss_floor = 0.1;

  pars.id_rho_left = 1.0;
  pars.id_vx_left = 1.0;
  pars.id_vy_left = 1.0;
  pars.id_vz_left = 1.0;
  pars.id_P_left = 1.0;

  pars.id_rho_right = 1.0;
  pars.id_vx_right =  1.0;
  pars.id_vy_right = 1.0;
  pars.id_vz_right = 1.0;
  pars.id_P_right = 1.0;

  pars.potential = 0;
  pars.potential_offset = -40.0;
  pars.g = -1.0;
  pars.p_base = 5.0;
  pars.rho1 = 1.0;
  pars.rho2 = 2.0;

  return 1;
}
#endif

