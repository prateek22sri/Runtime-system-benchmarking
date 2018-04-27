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

#define __STDC_FORMAT_MACROS

#include <stdatomic.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>

#include "hpx/hpx.h"
#include "libpxgl/termination.h"
#include "pxgl/edge_list.h"

static hpx_action_t _put_edge_edgelist;
static int _put_edge_edgelist_action(edge_list_edge_t *e)
{
   const hpx_addr_t target = hpx_thread_current_target();

   edge_list_edge_t *edge;
   if (!hpx_gas_try_pin(target, (void**)&edge))
     return HPX_RESEND;

   memcpy(edge, e, sizeof(*e));
   hpx_gas_unpin(target);
   _increment_finished_count();
   return HPX_SUCCESS;
}

typedef struct {
  unsigned int edges_skip;
  unsigned int edges_no;
  edge_list_t el;
  char filename[];
} _edge_list_from_file_local_args_t;
hpx_action_t _edge_list_from_file_local = HPX_ACTION_INVALID;
int _edge_list_from_file_local_action(const _edge_list_from_file_local_args_t *args) {
  // Read from the edge-list filename
  FILE *f = fopen(args->filename, "r");
  assert(f);

  edge_list_edge_t *edge = malloc(sizeof(*edge));
  assert(edge);

  sssp_uint_t skipped = 0;
  sssp_uint_t count = 0;

  char line[LINE_MAX];
  while (fgets(line, sizeof(line), f) != NULL && count < args->edges_no) {
    switch (line[0]) {
    case 'c': continue;
    case 'a':
      if(skipped < args->edges_skip) {
    skipped++;
    continue;
      }
      sscanf(&line[1], " %" SSSP_UINT_PRI " %" SSSP_UINT_PRI " %" SSSP_UINT_PRI, &edge->source, &edge->dest, &edge->weight);
      // printf("%s", &line[1]);
      const sssp_uint_t position = count++ + skipped;
      hpx_addr_t e = hpx_addr_add(args->el.edge_list, position * sizeof(edge_list_edge_t), args->el.edge_list_bsize);
      hpx_call(e, _put_edge_edgelist, HPX_NULL, edge, sizeof(*edge));
      continue;
    case 'p': continue;
    default:
      fprintf(stderr, "invalid command specifier '%c' in graph file. skipping..\n", line[0]);
      continue;
    }
  }
  free(edge);
  fclose(f);

  return HPX_SUCCESS;
}

hpx_action_t edge_list_from_file = HPX_ACTION_INVALID;
int edge_list_from_file_action(const edge_list_from_file_args_t * const args) {
  // Read from the edge-list filename
  FILE *f = fopen(args->filename, "r");
  assert(f);
  edge_list_t *el = malloc(sizeof(*el));
  assert(el);

  printf("Starting DIMACS file reading\n");
  hpx_time_t now = hpx_time_now();
  char line[LINE_MAX];
  while (fgets(line, sizeof(line), f) != NULL) {
    switch (line[0]) {
    case 'c': continue;
    case 'a': continue;
    case 'p':
      sscanf(&line[1], " sp %" SSSP_UINT_PRI " %" SSSP_UINT_PRI, &el->num_vertices, &el->num_edges);
      // Account for the  DIMACS graph format (.gr) where the node
      // ids range from 1..n
      el->num_vertices++;
      // Set an appropriate block size
      const uint64_t no_blocks = HPX_LOCALITIES * 128 ;
      el->edge_list_bsize = ((el->num_edges + no_blocks - 1) / no_blocks) * sizeof(edge_list_edge_t);
      // Allocate an edge_list array in the global address space
      el->edge_list = hpx_gas_alloc_cyclic(no_blocks, el->edge_list_bsize, 0);
      break;
    default:
      fprintf(stderr, "invalid command specifier '%c' in graph file. skipping..\n", line[0]);
      continue;
    }
    break;
  }

  hpx_bcast_rsync(_initialize_termination_detection, NULL, 0);
  _increment_active_count(el->num_edges);

  size_t filename_len = strlen(args->filename) + 1;
  const size_t local_args_size = sizeof(_edge_list_from_file_local_args_t) + filename_len;
  _edge_list_from_file_local_args_t *local_args = malloc(local_args_size);
  local_args->el = *el;
  memcpy(local_args->filename, args->filename, filename_len);
  const sssp_uint_t thread_chunk = el->num_edges / (args->thread_readers * args->locality_readers) + 1;
  local_args->edges_no = thread_chunk;
  for(unsigned int locality_desc = 0; locality_desc < args->locality_readers; ++locality_desc) {
    for(unsigned int thread_desc = 0; thread_desc < args->thread_readers; ++thread_desc) {
      local_args->edges_skip = ((locality_desc * args->thread_readers) + thread_desc) * thread_chunk;
      hpx_call(HPX_THERE(locality_desc), _edge_list_from_file_local,
               HPX_NULL, local_args, local_args_size);
    }
  }
  double elapsed = hpx_time_elapsed_ms(now)/1e3;
  printf("Waiting for completion LCO.  Time took to start local read loops: %fs\n", elapsed);
  now = hpx_time_now();
  hpx_addr_t edges_sync = hpx_lco_and_new(2);
  _detect_termination(edges_sync, edges_sync);
  hpx_lco_wait(edges_sync);
  elapsed = hpx_time_elapsed_ms(now)/1e3;
  printf("Fininshed waiting for edge list completion.  Time waiting: %fs\n", elapsed);
  hpx_lco_delete(edges_sync, HPX_NULL);
  fclose(f);

  int e = hpx_thread_continue(el, sizeof(*el));
  free(el);
  return e;
}


static __attribute__((constructor)) void _edge_list_register_actions() {
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _put_edge_edgelist, _put_edge_edgelist_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              edge_list_from_file, edge_list_from_file_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_COALESCED,
              _edge_list_from_file_local, _edge_list_from_file_local_action,
              HPX_POINTER, HPX_SIZE_T);
}
