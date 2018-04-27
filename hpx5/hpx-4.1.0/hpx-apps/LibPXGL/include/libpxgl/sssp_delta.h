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

#ifndef LIBPXGL_SSSP_DELTA_H
#define LIBPXGL_SSSP_DELTA_H

#include <pxgl/pxgl.h>

typedef struct {
  hpx_addr_t vertex;
  distance_t distance;
} buffer_node_t;

typedef struct {
  size_t current_size;
  size_t current_position;
  double factor;
  buffer_node_t* buffer;
} buffer_t;

typedef struct {
  buffer_t ***buckets;
  size_t current_level;
  size_t *num_buckets;
  size_t *num_vertices;
  size_t delta;
} buckets_t;


int _delta_sssp_send_vertex(const hpx_addr_t vertex, const hpx_action_t action, const hpx_addr_t result, const distance_t *const distance, const size_t len);

extern buckets_t *buckets;
extern hpx_action_t _init_buckets;
extern hpx_action_t _delta_sssp_increase_active_counts;
extern hpx_action_t _send_next_level;
extern hpx_action_t _delete_buckets;
extern hpx_action_t _sssp_klevel_process_vertex;

void _find_next_level();
distance_t _get_level(const distance_t distance);

void _insert_buckets(const int thread_no, const size_t level, const hpx_addr_t vertex, const distance_t distance);

buffer_t* init_buffer(const size_t init_size, const double factor);
void append(buffer_t * const buffer, const hpx_addr_t vertex, const distance_t distance);
void delete_buffer(buffer_t * const buffer);
bool empty(const buffer_t * const buffer);
buffer_node_t pop(buffer_t * const buffer);
size_t size(const buffer_t * const buffer);

extern hpx_action_t _size_t_minimum_op;
extern hpx_action_t _size_t_minimum_init;


#endif // LIBPXGL_SSSP_DELTA_H
