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

#ifndef PXGL_EDGE_LIST_H
#define PXGL_EDGE_LIST_H

#include "defs.h"

#include "hpx/hpx.h"

typedef struct {
  sssp_uint_t source;
  sssp_uint_t dest;
  sssp_uint_t weight;
} edge_list_edge_t;


typedef struct {
  sssp_uint_t num_edges;
  sssp_uint_t num_vertices;
  hpx_addr_t  edge_list;
  uint32_t edge_list_bsize;
} edge_list_t;


// This action "returns" (continues) the constructed edge list as a
// edge_list_t structure.
typedef struct {
  unsigned int locality_readers;
  unsigned int thread_readers;
  char *filename;
} edge_list_from_file_args_t;
extern hpx_action_t edge_list_from_file;
extern int edge_list_from_file_action(const edge_list_from_file_args_t * const args);


typedef struct {
  int scale;
  int edgefactor;
  unsigned int locality_readers;
  unsigned int thread_readers;
} graph500_edge_list_generator_args_t;


typedef struct {
  int64_t edges_skip;
  int64_t edges_no;
  edge_list_t el;
  int log_numverts;
  int64_t M;
  uint64_t userseed1;
  uint64_t userseed2;
  unsigned int locality_readers;
  unsigned int thread_readers;
  //int64_t* nedges_ptr;
  //edge_list_t* result_ptr
} graph500_edge_list_local_args_t;

extern hpx_action_t graph500_edge_list_generator;
extern int graph500_edge_list_generator_action(const graph500_edge_list_generator_args_t * const args);

#endif // PXGL_EDGE_LIST_H
