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

#include "actions2.h"
typedef int local_int_t;
typedef long long global_int_t;

struct Domain;

typedef struct
{
  int src;
  int destination;
  int n_send;
  int n;
  hpx_addr_t base;
  double *xv;
  local_int_t *elements;
} EHS;

typedef struct
{
  local_int_t start;
  local_int_t finish;
  double *xv;
  double *yv;
  char *nonzerosInRow;
  double **matrixValues;
  local_int_t **mtxIndL;
} SPMV_LOOP;

/* actions */
//extern hpx_action_t _main;
extern hpx_action_t _generateproblem;
extern hpx_action_t _setuphalo;
extern hpx_action_t _exchange_sends;
extern hpx_action_t _exchange_result;
extern hpx_action_t _spmv_loop;
extern hpx_action_t _assign_vector;
extern hpx_action_t _printcheck;
int _generateproblem_action(Domain *domain);
int _setuphalo_action(Domain *domain);
int _exchange_result_action(Domain *domain, NodalArgs *args, size_t size);
int _exchange_sends_action(EHS *ld,size_t size);
int _spmv_loop_action(SPMV_LOOP *ld,size_t size);
int _assign_vector_action(double *value,size_t size);
int _printcheck_action(int *ld,size_t size);
#endif
