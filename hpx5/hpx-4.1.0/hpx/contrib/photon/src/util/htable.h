// =============================================================================
//  Photon RDMA Library (libphoton)
//
//  Copyright (c) 2016, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifndef HTABLE_H
#define HTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "libsync/locks.h"

typedef struct hash_element {
  uint64_t key;
  void *value;
  struct hash_element *prev;
  struct hash_element *next;
} hash_element_t;

typedef struct htable {
  hash_element_t **table;
  int size;
  int elements;
  tatas_lock_t tlock;
} htable_t;

htable_t *htable_create(int size);
int htable_insert(htable_t *table, uint64_t key, void *value);
int htable_update(htable_t *table, uint64_t key, void *value, void **old_value);
int htable_update_if_exists(htable_t *table, uint64_t key, void *value, void **old_value);
int htable_lookup(htable_t *table, uint64_t key, void **value);
int htable_remove(htable_t *table, uint64_t key, void **value);
int htable_count(htable_t *table);
void htable_free(htable_t *table);
void htable_print(htable_t *htable);

#ifdef __cplusplus
}
#endif

#endif
