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

#ifndef HAWAII_STORAGE_H
#define HAWAII_STORAGE_H

#include <stdbool.h>
#include <stdint.h>
#include "hawaii-types.h"

#define HASH_TBL_SIZE 1572869
// some larger primes for hash table 
// #define HASH_TBL_SIZE 3145739
// #define HASH_TBL_SIZE 6291469
// #define HASH_TBL_SIZE 12582917
// #define HASH_TBL_SIZE 25165843
// #define HASH_TBL_SIZE 50331653
// #define HASH_TBL_SIZE 100663319
// #define HASH_TBL_SIZE 201326611
// #define HASH_TBL_SIZE 402653189
// #define HASH_TBL_SIZE 805306457
// #define HASH_TBL_SIZE 1610612741

#define LOCK_TBL_SIZE 3145739

extern int max_level; 
extern const int max_index[3]; 
extern const int ns[3]; 
extern const int base_step_size; 
extern const int level_one_step_size; 
extern const int npts_in_array; 

typedef struct hash_entry_t{
  coll_point_t *point; 
  uint64_t mkey;  
  struct hash_entry_t *next; 
} hash_entry_t; 

typedef struct {
  coll_point_t *array; 
  hash_entry_t *hash_table[HASH_TBL_SIZE]; 
} hawaii_storage_t; 

extern hawaii_storage_t *coll_points; 

uint64_t morton_key(const index_t *index);

void storage_init(void); 

void storage_cleanup(void); 

void create_full_grids(void);

void create_adap_grids(void); 

void create_neighboring_point(coll_point_t *essen_point); 

void create_nonessential_point(coll_point_t *nonessen_point, 
                               const index_t *index); 

coll_point_t *get_coll_point(const index_t *index); 

coll_point_t *add_coll_point(const index_t *index, int *flag); 

void adjust_grids(void); 

void prune_grids(void); 
#endif

