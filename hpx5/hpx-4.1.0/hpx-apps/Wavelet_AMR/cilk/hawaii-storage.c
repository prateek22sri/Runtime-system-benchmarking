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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "hawaii.h"

int warned = 0;

#if n_dim == 1
const index_t neighbor_offset[2] = { {{-1}}, {{1}} }; 
#elif n_dim == 2
const index_t neighbor_offset[8] = { 
  {{-1, -1}}, {{0, -1}}, {{1, -1}}, {{-1, 0}}, 
  {{1, 0}}, {{-1, 1}}, {{0, 1}}, {{1, 1}} }; 
#elif n_dim == 3
const index_t neighbor_offset[26] = 
  { {{-1, -1, -1}}, {{0, -1, -1}}, {{1, -1, -1}}, {{-1, 0, -1}}, {{0, 0, -1}}, 
    {{1, 0, -1}}, {{-1, 1, -1}}, {{0, 1, -1}}, {{1, 1, -1}}, {{-1, -1, 0}}, 
    {{0, -1, 0}}, {{1, -1, 0}}, {{-1, 0, 0}}, {{1, 0, 0}}, {{-1, 1, 0}},     
    {{0, 1, 0}}, {{1, 1, 0}}, {{-1, -1, 1}}, {{0, -1, 1}}, {{1, -1, 1}},
    {{-1, 0, 1}}, {{0, 0, 1}}, {{1, 0, 1}}, {{-1, 1, 1}}, {{0, 1, 1}}, 
    {{1, 1, 1}} }; 
#endif

const int base_step_size = 1 << JJ;
const int level_one_step_size = 1 << (JJ - 1); 
const int ns[3] = {ns_x, ns_y, ns_z}; 
const int max_index[3] = {ns_x * (1 << JJ), 
                          ns_y * (1 << JJ), 
                          ns_z * (1 << JJ)}; 
const int npts_in_array = (2 * ns_x + 1) * (2 * ns_y + 1) * (2 * ns_z + 1); 
hawaii_storage_t *coll_points = NULL; 
int max_level; 

static inline uint64_t hash(const uint64_t k) {
  return (k % HASH_TBL_SIZE);
}

uint64_t split(const unsigned k) {
#if n_dim == 1
  uint64_t split = k;
#elif n_dim == 2
  uint64_t split = k & 0xffffffff;
  split = (split | split << 16) & 0xffff0000ffff;
  split = (split | split << 8) & 0xff00ff00ff00ff;
  split = (split | split << 4) & 0xf0f0f0f0f0f0f0f;
  split = (split | split << 2) & 0x3333333333333333;
  split = (split | split << 1) & 0x5555555555555555;
#elif n_dim == 3
  uint64_t split = k & 0x1fffff; 
  split = (split | split << 32) & 0x1f00000000ffff;
  split = (split | split << 16) & 0x1f0000ff0000ff;
  split = (split | split << 8)  & 0x100f00f00f00f00f;
  split = (split | split << 4)  & 0x10c30c30c30c30c3;
  split = (split | split << 2)  & 0x1249249249249249;
#endif
  return split;
}

uint64_t morton_key(const index_t *index) {
  uint64_t key = 0;
#if n_dim == 1
  key = index->idx[x_dir]; 
#elif n_dim == 2
  key |= split(index->idx[x_dir]) | split(index->idx[y_dir]) << 1;
#elif n_dim == 3
  key |= split(index->idx[x_dir]) | split(index->idx[y_dir]) << 1 | 
    split(index->idx[z_dir]) << 2;
#endif
  return key;
}

void storage_init(void) {
  coll_points = calloc(1, sizeof(hawaii_storage_t));
  assert(coll_points != NULL);
  coll_points->array = calloc(npts_in_array, sizeof(coll_point_t)); 
  assert(coll_points->array != NULL);
}

void storage_cleanup(void) {
  free(coll_points->array);
  for (int i = 0; i < HASH_TBL_SIZE; i++) {
    hash_entry_t *ptr = coll_points->hash_table[i]; 
    while (ptr != NULL) {
      hash_entry_t *tmp = ptr->next;
      free(ptr->point); 
      free(ptr);
      ptr = tmp; 
    }
  }
  free(coll_points);
}

void create_full_grids(void) {
  assert(coll_points != NULL);
  max_level = 1;
  const int gen = 0; 

  int wavelet_mask[n_variab + n_aux] = {0};
  for (int ivar = 0; ivar < n_variab; ivar++) 
    wavelet_mask[ivar] = 1; 

  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i]; 
    point->index = linear_to_index(i); 
    point->coords = set_coordinate(&point->index); 
    initial_condition(&point->coords, point->u[gen]); 
    point->level = 0; 
    point->status[CURRENT_STATUS] = essential; 
    for (dir_t dir = x_dir; dir < n_dim; dir++) {
      if (point->index.idx[dir] % base_step_size) {
        point->level = 1; 
        point->status[CURRENT_STATUS] = neighboring; 
        break;
      }
    }
  }
    
  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i]; 
    if (point->level == 1) {
      wavelet_trans(point, primary_set_wavelet, gen, wavelet_mask); 
      coll_status_t temp = set_point_status(point->wavelet[gen]); 
      point->status[CURRENT_STATUS] = superior_status(temp, neighboring); 
    }
  }
}

void create_adap_grids(void) {
  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i]; 
    if (point->level == 1 && point->status[CURRENT_STATUS] == essential) 
      create_neighboring_point(point);
  }
}

void create_neighboring_point(coll_point_t *essen_point) {
  int wavelet_mask[n_variab + n_aux] = {0};
  for (int ivar = 0; ivar < n_variab; ivar++) 
    wavelet_mask[ivar] = 1; 
  const int gen = 0; 
  const unsigned initial_stamp = 0; 

  if ( essen_point->level >= JJ ) {
    if ( warned == 0 ) {
      printf(" WARNING:  JJ is insufficient to adequately resolve the system.\n");
      printf("           However, we are running anyways.  Good luck!\n");
      warned = 1;
    }
    return;
  }

  if ( JJ != 1 ) assert(essen_point->level < JJ);

  int level = essen_point->level;
  int step_size_essen = 1 << (JJ - level);
  int step_size_neighbor = step_size_essen / 2;

  for (int i = 0; i < n_neighbors; i++) {
    index_t index = add_index(&essen_point->index, &neighbor_offset[i], 
                              step_size_neighbor); 
    if (check_index(&index)) { // Neighbor point's index is within range
      int flag; 
      coll_point_t *neighbor = add_coll_point(&index, &flag); 
      if (flag == 0) { // The point was just created
        neighbor->index = index; 
        neighbor->coords = set_coordinate(&index); 
        neighbor->level = essen_point->level + 1; 
        neighbor->time_stamp = time_stamp;  
        check_wavelet_stencil(neighbor, gen); 
        check_derivative_stencil(neighbor, gen); 
        for (int igen = 0; igen < RHS_STAGES; igen++) 
          neighbor->stage[igen] = 0; 

        if (time_stamp == initial_stamp) {
          neighbor->status[CURRENT_STATUS] = neighboring; 
          initial_condition(&neighbor->coords, neighbor->u[gen]); 
          wavelet_trans(neighbor, primary_set_wavelet, gen, wavelet_mask); 
          coll_status_t temp = set_point_status(neighbor->wavelet[gen]); 
          neighbor->status[CURRENT_STATUS] = superior_status(temp, neighboring);
          
          if (neighbor->status[CURRENT_STATUS] == essential) {
            create_neighboring_point(neighbor); 
          } else { // Update max_level when local refinement terminates
            max_level = fmax(max_level, neighbor->level); 
          }
        } else {
          neighbor->status[1] = neighboring; 
          wavelet_trans(neighbor, primary_set_approx, gen, wavelet_mask);
#ifdef RMHD
          neighbor->c2p = con_to_prim(neighbor->u[gen], neighbor->u[gen], 
                                neighbor->coords.pos);
#endif
          max_level = fmax(max_level, neighbor->level); 
        }
      } else { // The point already exists in the data store
        if (time_stamp == initial_stamp) {
          // This branch occurs when the point was previously created as an
          // nonessential point in the grid construction process. Now we need to
          // promote its status to neighboring and further checks if it can be
          // an essential point
          if (neighbor->status[CURRENT_STATUS] == nonessential) {
            neighbor->status[CURRENT_STATUS] = neighboring; 
            check_derivative_stencil(neighbor, gen); 
            wavelet_trans(neighbor, primary_set_wavelet, gen, wavelet_mask);
            coll_status_t temp = set_point_status(neighbor->wavelet[gen]); 
            neighbor->status[CURRENT_STATUS] = superior_status(temp, 
                                                               neighboring); 
            if (neighbor->status[CURRENT_STATUS] == essential) {
              create_neighboring_point(neighbor); 
            } else {
              max_level = fmax(max_level, neighbor->level);
            }
          }
        } else {
          // This branch occurs when a neighboring point becomes an essential
          // point. The neighboring points it tries to add already exist in the
          // grid. 
          if (neighbor->status[CURRENT_STATUS] == nonessential) {
            // The point exists in the grid as a nonessential point 
            
            // Promote the status 
            neighbor->status[FUTURE_STATUS] = neighboring;           
            
            // Check wavelet and derivative
            check_wavelet_stencil(neighbor, gen); 
            
            // Set neighboring point's value 
            wavelet_trans(neighbor, primary_set_approx, gen, wavelet_mask);
#ifdef RMHD
neighbor->c2p = con_to_prim(neighbor->u[gen], neighbor->u[gen], 
                                neighbor->coords.pos);
#endif
            // Update time stamp of the point
            neighbor->time_stamp = time_stamp; 
            
            // Check derivative stencils
            check_derivative_stencil(neighbor, gen); 
          } else if (neighbor->status[CURRENT_STATUS] == neighboring) {
            // make sure the future status of this neighboring point is at least
            // neighboring 
            neighbor->status[FUTURE_STATUS] = 
              superior_status(neighboring, neighbor->status[FUTURE_STATUS]); 
          }
        }
      }
    } // Move on to the next neighboring point
  } // End of i
}

void create_nonessential_point(coll_point_t *nonessen_point, 
                               const index_t *index) {
  const int gen = 0; 
  const unsigned initial_stamp = 0; 

  int level = get_level(index); 
  assert(level > 1); 

  nonessen_point->level = level; 
  nonessen_point->index = *index; 
  nonessen_point->coords = set_coordinate(index); 
  nonessen_point->time_stamp = time_stamp;  
  check_wavelet_stencil(nonessen_point, gen); 
  for (int igen = 0; igen < RHS_STAGES; igen++) 
    nonessen_point->stage[igen] = 0; 

  if (time_stamp == initial_stamp) {
    initial_condition(&nonessen_point->coords, nonessen_point->u[gen]);
  } else {
    wavelet_trans(nonessen_point, primary_set_approx, gen, primary_mask);
  }
  con_to_prim(nonessen_point->u[gen], nonessen_point->u[gen], 
                                nonessen_point->coords.pos);

  nonessen_point->status[CURRENT_STATUS] = nonessential; 
}

coll_point_t *get_coll_point(const index_t *index) {
  coll_point_t *retval = NULL; 
  bool stored_in_array = true; 
  for (dir_t dir = x_dir; dir < n_dim; dir++) 
    stored_in_array &= (index->idx[dir] % level_one_step_size == 0); 

  if (stored_in_array) {
    retval = &coll_points->array[index_to_linear(index)]; 
  } else {
    uint64_t mkey = morton_key(index); 
    uint64_t hidx = hash(mkey); 
    hash_entry_t *curr = coll_points->hash_table[hidx]; 
    while (curr != NULL) {
      if (curr->mkey == mkey) {
        retval = curr->point; 
        break;
      }
      curr = curr->next;
    }
  }

  return retval;
}

coll_point_t *add_coll_point(const index_t *index, int *flag) {
  coll_point_t *retval = get_coll_point(index); 

  if (retval != NULL) {
    *flag = 1; // the point already exists
  } else {
    *flag = 0; // the point does not exist
    uint64_t mkey = morton_key(index); 
    uint64_t hidx = hash(mkey); 
    hash_entry_t *h_entry = calloc(1, sizeof(hash_entry_t)); 
    retval = calloc(1, sizeof(coll_point_t)); 
    assert(h_entry != NULL);
    assert(retval != NULL);
    h_entry->point = retval; 
    h_entry->mkey = mkey;
    h_entry->next = coll_points->hash_table[hidx]; 
    coll_points->hash_table[hidx] = h_entry;
  }

  return retval;
}

void update_all_statuses(void) {
#ifdef CILK
  cilk_for (int i = 0; i < npts_in_array; ++i) {
#else
  for (int i = 0; i < npts_in_array; ++i) {
#endif
    if (coll_points->array[i].status[FUTURE_STATUS] != uninitialized) {
      coll_points->array[i].status[CURRENT_STATUS] =
                        coll_points->array[i].status[FUTURE_STATUS];
      coll_points->array[i].status[FUTURE_STATUS] = uninitialized;
    }
  }
#ifdef CILK
  cilk_for (int i = 0; i < HASH_TBL_SIZE; ++i) {
#else
  for (int i = 0; i < HASH_TBL_SIZE; ++i) {
#endif
    hash_entry_t *curr = coll_points->hash_table[i];
    while (curr != NULL) {
      if (curr->point->status[FUTURE_STATUS] != uninitialized) {
        curr->point->status[CURRENT_STATUS] =
                        curr->point->status[FUTURE_STATUS];
        curr->point->status[FUTURE_STATUS] = uninitialized;
      }
      curr = curr->next;
    }
  }
}

void clear_all_adjust_stages(void) {
#ifdef CILK
  cilk_for (int i = 0; i < npts_in_array; ++i) {
#else
  for (int i = 0; i < npts_in_array; ++i) {
#endif
    for (int j = 0; j < ADJUST_STAGES; ++j) {
      coll_points->array[i].adjust[j] = 0;
    }
  }
#ifdef CILK
  cilk_for (int i = 0; i < HASH_TBL_SIZE; ++i) {
#else
  for (int i = 0; i < HASH_TBL_SIZE; ++i) {
#endif
    hash_entry_t *curr = coll_points->hash_table[i];
    while (curr != NULL) {
      for (int j = 0; j < ADJUST_STAGES; ++j) {
        curr->point->adjust[j] = 0;
      }
      curr = curr->next;
    }
  }
}

void adjust_grids(void) {
  int wavelet_mask[n_variab + n_aux] = {0};
  for (int ivar = 0; ivar < n_variab; ivar++) 
    wavelet_mask[ivar] = 1; 
  const int gen = 0; 

#ifdef CILK
  cilk_for (int i = 0; i < npts_in_array; i++) {
#else
  for (int i = 0; i < npts_in_array; i++) {
#endif
    coll_point_t *point = &coll_points->array[i]; 
    if (point->level == 0) {
      point->adjust[ADJUST_WAVELET] = 1;
      point->status[FUTURE_STATUS] = essential;
    } else {
      point->status[FUTURE_STATUS] = neighboring; 
      wavelet_trans(point, primary_set_wavelet, gen, wavelet_mask); 
      coll_status_t temp = set_point_status(point->wavelet[gen]);
      point->status[FUTURE_STATUS] = superior_status(temp, neighboring); 
      
      if (point->status[CURRENT_STATUS] == essential) {
        wavelet_adjust_grids_helper(point); 
      } else {
        point->adjust[ADJUST_WAVELET] = 1;
      }
    }
  }

  // Here we go over the current active grid, and for whichever points have
  // an essential future status, we add neighboring points.
  //TODO: CILKify? This would be difficult as well, so for now we leave it
  // as serial. Perhaps with a better concurrent data store we could make 
  // this happen.
  for (int i = 0; i < npts_in_array; ++i) {
    coll_point_t *point = &coll_points->array[i];
    if (point->level == 0) {
      point->adjust[ADJUST_NGBADD] = 1;
    } else {
      if (point->status[CURRENT_STATUS] == essential) {
        adjust_add_neighbors_helper(point);
      } else if (point->status[FUTURE_STATUS] == essential) {
        point->adjust[ADJUST_NGBADD] = 1;
        create_neighboring_point(point);
      }
    }
  }


  //make sure essential points have essential parents
  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i]; 
    if (point->level == 1 && point->status[CURRENT_STATUS] == essential) {
      status_validate_up_helper(point); 
    }
  }

  //make sure essential points have neighboring neighbors
#ifdef CILK
  cilk_for (int i = 0; i < npts_in_array; i++) {
#else
  for (int i = 0; i < npts_in_array; i++) {
#endif
    coll_point_t *point = &coll_points->array[i]; 
    if (point->level == 1 && point->status[FUTURE_STATUS] == essential) {
      status_validate_down_helper(point);
    }
  }

  update_all_statuses();

  //Now we perform a stage to make sure that each active point has a full
  // HLLE stencil.
  //TODO: CILKify? This is hard to protect against race conditions, so for the
  // time being, we leave it.
  for (int i = 0; i < npts_in_array; ++i) {
    coll_point_t *point = &coll_points->array[i];
    hlle_stencil_completion_helper(point);
  }

  clear_all_adjust_stages();
}

void prune_grids(void) {
#ifdef CILK
  cilk_for (int i = 0; i < HASH_TBL_SIZE; i++) {
#else
  for (int i = 0; i < HASH_TBL_SIZE; i++) {
#endif
    hash_entry_t *head = coll_points->hash_table[i];     
    if (head != NULL) {
      hash_entry_t *prev = head; 
      hash_entry_t *curr = prev->next; 
      // Leave the head of the list for the last      
      while (curr != NULL) {
        if (curr->point->time_stamp < time_stamp) {
          hash_entry_t *tmp = curr; 
          prev->next = curr->next; 
          curr = curr->next;
          free(tmp->point); 
          free(tmp);
        } else {
          prev = curr; 
          curr = curr->next;
        }
      }
      // Process the head of the list 
      if (head->point->time_stamp < time_stamp) {
        hash_entry_t *tmp = head; 
        coll_points->hash_table[i] = head->next; 
        free(tmp->point); 
        free(tmp);
      }
    }
  }
}
