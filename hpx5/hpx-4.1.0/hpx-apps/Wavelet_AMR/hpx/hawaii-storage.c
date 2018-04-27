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
#include <stdatomic.h>
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
uint64_t max_key;

static inline uint64_t hash(const uint64_t k) {
  return (k % HASH_TBL_SIZE);
}

#ifdef HPX
hpx_hawaii_storage_t hpx_coll_points = {0,{0}};

hpx_action_t _create_full_grids_a = 0;
hpx_action_t _create_full_grids_b = 0;
hpx_action_t _create_adap_grids_a = 0;
hpx_action_t _init_neighboring_point = 0;
hpx_action_t _second_init_neighboring_point = 0;
hpx_action_t _hpx_get_or_insert_point_local = 0;
hpx_action_t _hpx_get_coll_point_local = 0;
hpx_action_t _update_max_level = 0;
hpx_action_t _adjust_grids_a = 0;
hpx_action_t _adjust_grids_b = 0;
hpx_action_t _adjust_grids_c = 0;
hpx_action_t _adjust_grids_d = 0;
hpx_action_t _adjust_grids_e = 0;
hpx_action_t _update_all_statuses = 0;
hpx_action_t _update_all_statuses_b = 0;
hpx_action_t _clear_all_adjust_stages = 0;
hpx_action_t _clear_all_adjust_stages_b = 0;
hpx_action_t _prune_grids = 0;
hpx_action_t _count_points = 0;

int _action_count_points(CP *ld, size_t size)
{

  unsigned stamp = ld->stamp;
  coll_status_t minstat = ld->minstat;

  coll_point_t *point;
  point = (coll_point_t *) calloc(1,sizeof(coll_point_t));

  int retval = 0;

  if ( HPX_LOCALITY_ID == 0 ) {
    for (int i = 0; i < npts_in_array; i++) {
      hpx_addr_t pt = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
      hpx_gas_memget_sync(point, pt, sizeof(coll_point_t));

      if (point->status[CURRENT_STATUS] >= minstat
                    && point->time_stamp == stamp) {
        ++retval;
      }
    }
  }

  for (int i = 0; i < HASH_TBL_SIZE; i++) {
    hpx_hash_entry_t *ptr = hpx_coll_points.hash_table[i];
    while (ptr != NULL) {
      hpx_gas_memget_sync(point, ptr->point, sizeof(coll_point_t));
      if (point->time_stamp == stamp &&
               point->status[CURRENT_STATUS] >= minstat ) {
        ++retval;
      }
      ptr = ptr->next;
    }
  }

  free(point);
  return HPX_THREAD_CONTINUE(retval);
}

int prune_grids_par1(int i, void *UNUSED)
{
  hpx_hash_entry_t *head = hpx_coll_points.hash_table[i];
  if (head != NULL) {
    hpx_hash_entry_t *prev = head;
    hpx_hash_entry_t *curr = prev->next;
    // Leave the head of the list for the last
    while (curr != NULL) {
      unsigned ptime_stamp;
      hpx_addr_t time_offset = hpx_addr_add(curr->point,offsetof(coll_point_t,time_stamp),sizeof(coll_point_t));
      hpx_gas_memget_sync(&ptime_stamp,time_offset,sizeof(ptime_stamp));
      if ( ptime_stamp < time_stamp ) {
        hpx_hash_entry_t *tmp = curr;
        prev->next = curr->next;
        curr = curr->next;
        hpx_gas_free_sync(tmp->point);
        free(tmp);
      } else {
        prev = curr;
        curr = curr->next;
      }
    }
    // Process the head of the list
    unsigned ptime_stamp;
    hpx_addr_t time_offset = hpx_addr_add(head->point,offsetof(coll_point_t,time_stamp),sizeof(coll_point_t));
    hpx_gas_memget_sync(&ptime_stamp,time_offset,sizeof(ptime_stamp));

    if (ptime_stamp < time_stamp) {
      hpx_hash_entry_t *tmp = head;
      hpx_coll_points.hash_table[i] = head->next;
      hpx_gas_free_sync(tmp->point);
      free(tmp);
    }
  }
  return HPX_SUCCESS;
}

int _action_prune_grids(void *UNUSED, size_t size)
{

  hpx_par_for_sync(prune_grids_par1,0,HASH_TBL_SIZE,NULL);

  return HPX_SUCCESS;
}

int _action_clear_all_adjust_stages_b(void *UNUSED, size_t size)
{
  for (int i = 0; i < HASH_TBL_SIZE; ++i) {
    hpx_hash_entry_t *curr = hpx_coll_points.hash_table[i];
    while (curr != NULL) {
      int e = hpx_call_sync(curr->point, _clear_all_adjust_stages, NULL, 0, NULL, 0);
      if (e != HPX_SUCCESS) hpx_abort();
      curr = curr->next;
    }
  }

  return HPX_SUCCESS;
}

int _action_clear_all_adjust_stages(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  for (int j = 0; j < ADJUST_STAGES; ++j) {
    point->adjust[j] = 0;
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_update_all_statuses_b(void *UNUSED, size_t size)
{
  for (int i = 0; i < HASH_TBL_SIZE; ++i) {
    hpx_hash_entry_t *curr = hpx_coll_points.hash_table[i];
    while (curr != NULL) {
      int e = hpx_call_sync(curr->point, _update_all_statuses, NULL, 0, NULL, 0);
      if (e != HPX_SUCCESS) hpx_abort();
      curr = curr->next;
    }
  }

  return HPX_SUCCESS;
}

int _action_update_all_statuses(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  if (point->status[FUTURE_STATUS] != uninitialized) {
      point->status[CURRENT_STATUS] = point->status[FUTURE_STATUS];
      point->status[FUTURE_STATUS] = uninitialized;
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_adjust_grids_a(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  int wavelet_mask[n_variab + n_aux] = {0};
  for (int ivar = 0; ivar < n_variab; ivar++)
    wavelet_mask[ivar] = 1;
  const int gen = 0;

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

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_adjust_grids_b(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

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

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_adjust_grids_c(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  if (point->level == 1 && point->status[CURRENT_STATUS] == essential) {
    status_validate_up_helper(point);
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_adjust_grids_d(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  if (point->level == 1 && point->status[FUTURE_STATUS] == essential) {
    status_validate_down_helper(point);
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _action_adjust_grids_e(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  hlle_stencil_completion_helper(point);

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _update_max_level_action(int *maxlevel, size_t size)
{

  max_level = fmax(max_level, *maxlevel);

  return HPX_THREAD_CONTINUE(max_level);
}

int _create_full_grids_a_action(int *i, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;
  const int gen = 0;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  point->index = linear_to_index(*i);
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
  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _create_full_grids_b_action(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;
  const int gen = 0;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  int wavelet_mask[n_variab + n_aux] = {0};
  for (int ivar = 0; ivar < n_variab; ivar++)
    wavelet_mask[ivar] = 1;

  if (point->level == 1) {
    wavelet_trans(point, primary_set_wavelet, gen, wavelet_mask);
    coll_status_t temp = set_point_status(point->wavelet[gen]);
    point->status[CURRENT_STATUS] = superior_status(temp, neighboring);
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _create_adap_grids_a_action(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *point = NULL;

  if (!hpx_gas_try_pin(local, (void**)&point))
    return HPX_RESEND;

  if (point->level == 1 && point->status[CURRENT_STATUS] == essential) {
    create_neighboring_point(point);
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _init_neighboring_point_action(INP *ld, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *neighbor = NULL;

  if (!hpx_gas_try_pin(local, (void**)&neighbor))
    return HPX_RESEND;

  int wavelet_mask[n_variab + n_aux] = {0};
  for (int ivar = 0; ivar < n_variab; ivar++)
    wavelet_mask[ivar] = 1;
  const int gen = 0;
  const unsigned initial_stamp = 0;

  neighbor->index = ld->index;
  neighbor->coords = set_coordinate(&(ld->index));
  neighbor->level = ld->level + 1;
  neighbor->time_stamp = time_stamp;
  check_wavelet_stencil(neighbor, gen);
  //check_derivative_stencil(neighbor, gen);
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
      for (int i=0;i<hpx_get_num_ranks();i++) {
        if ( i != hpx_get_my_rank() ) {
          hpx_call_sync(HPX_THERE(i),_update_max_level,&max_level,sizeof(max_level),&max_level,sizeof(max_level));
        }
      }
    }
  } else {
    neighbor->status[1] = neighboring;
    wavelet_trans(neighbor, primary_set_approx, gen, wavelet_mask);
    neighbor->c2p = con_to_prim(neighbor->u[gen], neighbor->u[gen],
                                neighbor->coords.pos);
    max_level = fmax(max_level, neighbor->level);
    for (int i=0;i<hpx_get_num_ranks();i++) {
      if ( i != hpx_get_my_rank() ) {
        hpx_call_sync(HPX_THERE(i),_update_max_level,&max_level,sizeof(max_level),&max_level,sizeof(max_level));
      }
    }
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

int _second_init_neighboring_point_action(void *UNUSED, size_t size)
{
  hpx_addr_t local = hpx_thread_current_target();
  coll_point_t *neighbor = NULL;

  if (!hpx_gas_try_pin(local, (void**)&neighbor))
    return HPX_RESEND;

  int wavelet_mask[n_variab + n_aux] = {0};
  for (int ivar = 0; ivar < n_variab; ivar++)
    wavelet_mask[ivar] = 1;
  const int gen = 0;
  const unsigned initial_stamp = 0;

  if (time_stamp == initial_stamp) {
    // This branch occurs when the point was previously created as an
    // nonessential point in the grid construction process. Now we need to
    // promote its status to neighboring and further checks if it can be
    // an essential point
    if (neighbor->status[CURRENT_STATUS] == nonessential) {
      neighbor->status[CURRENT_STATUS] = neighboring;
      //check_derivative_stencil(neighbor, gen);
      wavelet_trans(neighbor, primary_set_wavelet, gen, wavelet_mask);
      coll_status_t temp = set_point_status(neighbor->wavelet[gen]);
      neighbor->status[CURRENT_STATUS] = superior_status(temp,
                                                         neighboring);
      if (neighbor->status[CURRENT_STATUS] == essential) {
        create_neighboring_point(neighbor);
      } else {
        max_level = fmax(max_level, neighbor->level);
        for (int i=0;i<hpx_get_num_ranks();i++) {
          if ( i != hpx_get_my_rank() ) {
            hpx_call_sync(HPX_THERE(i),_update_max_level,&max_level,sizeof(max_level),&max_level,sizeof(max_level));
          }
        }
      }
    }
  } else if (neighbor->status[CURRENT_STATUS] == nonessential) {
    // This branch occurs when a neighboring point becomes an essential
    // point. The point to be added already exists in the grid. If the
    // existing point is active, nothing needs to be done. This branch
    // handles the situation where the existing point is nonessential.

    // Promote the status
    neighbor->status[FUTURE_STATUS] = neighboring;

    // Check wavelet and derivative stencils
    check_wavelet_stencil(neighbor, gen);
    //check_derivative_stencil(neighbor, gen);

    // Set neighboring point's value
    wavelet_trans(neighbor, primary_set_approx, gen, wavelet_mask);
    neighbor->c2p = con_to_prim(neighbor->u[gen], neighbor->u[gen],
                                neighbor->coords.pos);

    // Update time stamp of the point
    neighbor->time_stamp = time_stamp;

    // Check derivative stencils
    //check_derivative_stencil(neighbor, gen);
  } else if (neighbor->status[CURRENT_STATUS] == neighboring) {
    // make sure the future status of this neighboring point is at least
    // neighboring
    neighbor->status[FUTURE_STATUS] =
       superior_status(neighboring, neighbor->status[FUTURE_STATUS]);
  }

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

bool is_array(const index_t *index)
{
  bool stored_in_array = true;
  for (int dir = 0; dir < n_dim; dir++)
    stored_in_array &= (index->idx[dir] % level_one_step_size == 0);

  return stored_in_array;
}

int _hpx_get_or_insert_point_local_action(uint64_t *mkey, size_t size) {
 hpx_get_or_insert_point_local_return_t gitp = {.point=HPX_NULL,.found=1};
 static volatile atomic_flag lock = ATOMIC_FLAG_INIT;
 { 
   while (atomic_flag_test_and_set(&lock))
     ;
   gitp.point = hash_lookup(&hpx_coll_points.hash_table[0], *mkey);
   if (gitp.point == HPX_NULL) {
     gitp.found = 0;
     gitp.point = hash_insert(&hpx_coll_points.hash_table[0], *mkey);
   }
   atomic_flag_clear(&lock);
 }

 return HPX_THREAD_CONTINUE(gitp);
}

hpx_addr_t hpx_add_coll_point(const index_t *index, int *flag) {
  hpx_get_or_insert_point_local_return_t gitp;
  uint64_t mkey = morton_key(index);

  // in case this is called on an array (level 0 or 1)
  if ( is_array(index) ) {
    hpx_addr_t there = hpx_addr_add(hpx_coll_points.array,
                 index_to_linear(index)*sizeof(coll_point_t),sizeof(coll_point_t));
    *flag = 1; // the point already exists

    return there;
  } else {
    coord_t coord = set_coordinate(index);
    double x = coord.pos[x_dir];
    double y = coord.pos[y_dir];
    double pi = 4.0*atan(1.0);
    double theta;
    if ( y >= 0.0 && x > 0.0 ) {
      theta = atan(y/x);
    } else if ( y >= 0.0 && x < 0.0 ) {
      theta = atan(y/x) + pi;
    } else if ( y < 0.0 && x < 0.0 ) {
      theta = atan(y/x) + pi;
    } else if ( y < 0.0 && x > 0.0 ) {
      theta = atan(y/x) + 2.0*pi;
    } else if ( y >= 0.0 && x == 0.0) {
      theta = pi/2.0;
    } else {
      theta = 3.0*pi/2.0;
    }
    double dtheta = 2.0*pi/(HPX_LOCALITIES-1);
    int locality = -1;
    for (int i=0;i<HPX_LOCALITIES-1;i++) {
      if ( theta >= i*dtheta && theta <= (i+1)*dtheta ) {
        locality = i;
        break;
      }
    }
    if ( HPX_LOCALITIES == 1 ) locality = 0;
    assert(locality > -1);
    int e = hpx_call_sync(HPX_THERE(locality), _hpx_get_or_insert_point_local, &gitp, sizeof(gitp), &mkey, sizeof(mkey));
    assert(e == HPX_SUCCESS);
    *flag = gitp.found;

    return gitp.point;
  }
}

hpx_addr_t hash_insert(hpx_hash_entry_t **table, uint64_t mkey) {
  // allocate and initialize a point structure in the global address space
  coll_point_t *point = NULL;
  hpx_addr_t retval = hpx_gas_alloc_local(1,sizeof(*point), 0);
  if (!hpx_gas_try_pin(retval, (void**)&point)) {
    hpx_abort(); // can't handle allocation indirection
  }
  memset(point, 0, sizeof(*point));
  hpx_gas_unpin(retval);

  // allocate and initialize a hash bucket for this point
  hpx_hash_entry_t *h_entry = malloc(sizeof(*h_entry));
  h_entry->point = retval;
  h_entry->mkey = mkey;

  // compute the local hash key for this morton key, and push the hash bucket onto the bucket list
  uint64_t hidx = hash(mkey);
  h_entry->next = table[hidx];
  table[hidx] = h_entry;

  // return the global address
  return retval;
}

hpx_addr_t hash_lookup(hpx_hash_entry_t **table, uint64_t mkey) {
  hpx_addr_t found = HPX_NULL;
  uint64_t hidx = hash(mkey);
  hpx_hash_entry_t *curr = table[hidx];
  while (curr != NULL) {
    if (curr->mkey == mkey) {
      found = curr->point;
      break;
    }
    curr = curr->next;
  }
  return found;
}

int _action_hpx_get_coll_point_local(uint64_t *mkey, size_t size) {
 hpx_addr_t there;
 static volatile atomic_flag lock = ATOMIC_FLAG_INIT;
 {
   while (atomic_flag_test_and_set(&lock))
     ;
   there = hash_lookup(&hpx_coll_points.hash_table[0], *mkey);
   atomic_flag_clear(&lock);
 }
 return HPX_THREAD_CONTINUE(there);
}

hpx_addr_t hpx_get_coll_point(const index_t *index) {

  bool stored_in_array = is_array(index);
  if (stored_in_array) {
    hpx_addr_t there = hpx_addr_add(hpx_coll_points.array,
                 index_to_linear(index)*sizeof(coll_point_t),sizeof(coll_point_t));
    return there;
  } else {
    hpx_addr_t there = HPX_NULL;
    uint64_t mkey = morton_key(index);

    coord_t coord = set_coordinate(index);
    double x = coord.pos[x_dir];
    double y = coord.pos[y_dir];
    double pi = 4.0*atan(1.0);
    double theta;
    if ( y >= 0.0 && x > 0.0 ) {
      theta = atan(y/x);
    } else if ( y >= 0.0 && x < 0.0 ) {
      theta = atan(y/x) + pi;
    } else if ( y < 0.0 && x < 0.0 ) {
      theta = atan(y/x) + pi;
    } else if ( y < 0.0 && x > 0.0 ) {
      theta = atan(y/x) + 2.0*pi;
    } else if ( y >= 0.0 && x == 0.0) {
      theta = pi/2.0;
    } else {
      theta = 3.0*pi/2.0;
    }
    double dtheta = 2.0*pi/(HPX_LOCALITIES-1);
    int locality = -1;
    for (int i=0;i<HPX_LOCALITIES-1;i++) {
      if ( theta >= i*dtheta && theta <= (i+1)*dtheta ) {
        locality = i;
        break;
      }
    }
    if ( HPX_LOCALITIES == 1 ) locality = 0;
    assert(locality > -1);
    int e = hpx_call_sync(HPX_THERE(locality), _hpx_get_coll_point_local, &there, sizeof(there), &mkey, sizeof(mkey));
    if (e != HPX_SUCCESS) hpx_abort();
     return there;
  }

  return HPX_NULL;
}

#endif

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
#ifdef HPX
  hpx_addr_t complete = hpx_lco_and_new(npts_in_array);
  // FUTURE IMPROVEMENT: Could use parfor here
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_create_full_grids_a,complete,&i,sizeof(i));
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  complete = hpx_lco_and_new(npts_in_array);
  // FUTURE IMPROVEMENT: Could use parfor here
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_create_full_grids_b,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
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
#endif
}

void create_adap_grids(void) {
#ifdef HPX
  // FUTURE IMPROVEMENT: Could use parfor here
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call_sync(point,_create_adap_grids_a,NULL,0,NULL,0);
  }
#else
  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i];
    if (point->level == 1 && point->status[CURRENT_STATUS] == essential)
      create_neighboring_point(point);
  }
#endif
}

void create_neighboring_point(coll_point_t *essen_point) {
#ifdef HPX
  //int wavelet_mask[n_variab + n_aux] = {0};
  //for (int ivar = 0; ivar < n_variab; ivar++)
  //  wavelet_mask[ivar] = 1;
  //const int gen = 0;
  //const unsigned initial_stamp = 0;

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

  // FUTURE IMPROVEMENT: Could use parfor here
  for (int i = 0; i < n_neighbors; i++) {
    index_t index = add_index(&essen_point->index, &neighbor_offset[i],
                              step_size_neighbor);
    if (check_index(&index)) { // Neighbor point's index is within range
      int flag;
      hpx_addr_t neighbor = hpx_add_coll_point(&index, &flag);
      if (flag == 0) { // The point was just created
        INP ld;
        ld.index = index;
        ld.level = level;
        int e = hpx_call_sync(neighbor, _init_neighboring_point,NULL,0, &ld,
            sizeof(ld));
        assert(e == HPX_SUCCESS);
      } else { // The point already exists in the data store
        int e = hpx_call_sync(neighbor, _second_init_neighboring_point, NULL,
              0, NULL,0);
        assert(e == HPX_SUCCESS);
      }
    } // Move on to the next neighboring point
  } // End of i
#else
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
        //check_derivative_stencil(neighbor, gen);
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
          neighbor->c2p = con_to_prim(neighbor->u[gen], neighbor->u[gen],
                                neighbor->coords.pos);
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
            //check_derivative_stencil(neighbor, gen);
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
            neighbor->c2p = con_to_prim(neighbor->u[gen], neighbor->u[gen],
                                neighbor->coords.pos);
            // Update time stamp of the point
            neighbor->time_stamp = time_stamp;

            // Check derivative stencils
            //check_derivative_stencil(neighbor, gen);
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
#endif
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

void update_all_statuses(void) {

#ifdef HPX
  hpx_addr_t complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_update_all_statuses,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  for (int i = 0; i < npts_in_array; ++i) {
    if (coll_points->array[i].status[FUTURE_STATUS] != uninitialized) {
      coll_points->array[i].status[CURRENT_STATUS] =
                        coll_points->array[i].status[FUTURE_STATUS];
      coll_points->array[i].status[FUTURE_STATUS] = uninitialized;
    }
  }
#endif

#ifdef HPX
  complete = hpx_lco_and_new(hpx_get_num_ranks());
  for (int i=0;i<hpx_get_num_ranks();i++) {
    hpx_call(HPX_THERE(i),_update_all_statuses_b,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  for (int i = 0; i < HASH_TBL_SIZE; ++i) {
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
#endif
}

void clear_all_adjust_stages(void) {
#ifdef HPX
  hpx_addr_t complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; ++i) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_clear_all_adjust_stages,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  for (int i = 0; i < npts_in_array; ++i) {
    for (int j = 0; j < ADJUST_STAGES; ++j) {
      coll_points->array[i].adjust[j] = 0;
    }
  }
#endif

#ifdef HPX
  complete = hpx_lco_and_new(hpx_get_num_ranks());
  for (int i=0;i<hpx_get_num_ranks();i++) {
    hpx_call(HPX_THERE(i),_clear_all_adjust_stages_b,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  for (int i = 0; i < HASH_TBL_SIZE; ++i) {
    hash_entry_t *curr = coll_points->hash_table[i];
    while (curr != NULL) {
      for (int j = 0; j < ADJUST_STAGES; ++j) {
        curr->point->adjust[j] = 0;
      }
      curr = curr->next;
    }
  }
#endif
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

void adjust_grids(void) {

#ifndef HPX
  int wavelet_mask[n_variab + n_aux] = {0};
  for (int ivar = 0; ivar < n_variab; ivar++)
    wavelet_mask[ivar] = 1;
  const int gen = 0;
#endif

#ifdef HPX
  hpx_addr_t complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_adjust_grids_a,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  for (int i = 0; i < npts_in_array; i++) {
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
#endif

  // Here we go over the current active grid, and for whichever points have
  // an essential future status, we add neighboring points.
  //TODO: CILKify? This would be difficult as well, so for now we leave it
  // as serial. Perhaps with a better concurrent data store we could make
  // this happen.
#ifdef HPX
  complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_adjust_grids_b,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
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
#endif


#ifdef HPX
  complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_adjust_grids_c,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  //make sure essential points have essential parents
  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i];
    if (point->level == 1 && point->status[CURRENT_STATUS] == essential) {
      status_validate_up_helper(point);
    }
  }
#endif

  //make sure essential points have neighboring neighbors
#ifdef HPX
  complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_adjust_grids_d,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  for (int i = 0; i < npts_in_array; i++) {
    coll_point_t *point = &coll_points->array[i];
    if (point->level == 1 && point->status[FUTURE_STATUS] == essential) {
      status_validate_down_helper(point);
    }
  }
#endif

  update_all_statuses();

  //Now we perform a stage to make sure that each active point has a full
  // HLLE stencil.
  //TODO: CILKify? This is hard to protect against race conditions, so for the
  // time being, we leave it.
#ifdef HPX
  complete = hpx_lco_and_new(npts_in_array);
  for (int i = 0; i < npts_in_array; i++) {
    hpx_addr_t point = hpx_addr_add(hpx_coll_points.array,i*sizeof(coll_point_t),sizeof(coll_point_t));
    hpx_call(point,_adjust_grids_e,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  for (int i = 0; i < npts_in_array; ++i) {
    coll_point_t *point = &coll_points->array[i];
    hlle_stencil_completion_helper(point);
  }
#endif

  clear_all_adjust_stages();
}

void prune_grids(void) {
#ifdef HPX
  hpx_addr_t complete = hpx_lco_and_new(hpx_get_num_ranks());
  for (int i=0;i<hpx_get_num_ranks();i++) {
    hpx_call(HPX_THERE(i),_prune_grids,complete,NULL,0);
  }
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);
#else
  for (int i = 0; i < HASH_TBL_SIZE; i++) {
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
#endif
}

// THis counts the number of points in the grid which have the given status
// or better at the given timestamp.
int count_points(coll_status_t minstat, unsigned stamp) {
  int retval = 0;
#ifdef HPX
  int numpoints;
  for (int i=0;i<hpx_get_num_ranks();i++) {
    CP ld;
    ld.stamp = stamp;
    ld.minstat = minstat;
    hpx_call_sync(HPX_THERE(i),_count_points,&numpoints,sizeof(numpoints),&ld,sizeof(ld));
    retval += numpoints;
  }
#else
  for (int i = 0; i < npts_in_array; ++i) {
    if (coll_points->array[i].status[CURRENT_STATUS] >= minstat
                  && coll_points->array[i].time_stamp == stamp) {
      ++retval;
    }
  }

  for (int i = 0; i < HASH_TBL_SIZE; ++i) {
    hash_entry_t *ptr = coll_points->hash_table[i];
    while (ptr != NULL) {
      if (ptr->point->time_stamp == time_stamp &&
          ptr->point->status[CURRENT_STATUS] >= minstat) {
        ++retval;
      }
      ptr = ptr->next;
    }
  }
#endif
  return retval;
}
