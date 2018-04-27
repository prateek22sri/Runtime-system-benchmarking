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

#include "photon_buffertable.h"
#include "libsync/locks.h"
#include "avl.h"

static struct avl_table *btable = NULL;
static tatas_lock_t bt_lock;

// returns 0 if a and b equal
//         1 if a > b
//        -1 if a < b
static int bcmp(const void *a, const void *b, void *p) {
  photonBI tst = (photonBI)a;
  photonBI res = (photonBI)b;

  // below this region
  if (tst->buf.addr < res->buf.addr) {
    return -1;
  }
  // above this region
  else if (tst->buf.addr > (res->buf.addr + res->buf.size)) {
    return 1;
  }
  // fits in the region
  else if ((tst->buf.addr + tst->buf.size) <= (res->buf.addr + res->buf.size)) {
    return 0;
  }
  // overlap but doesn't fit
  else {
    return 1;
  }
}

int buffertable_init(int size) {
  btable = avl_create(bcmp, NULL, NULL);
  if (!btable) {
    return PHOTON_ERROR;
  }
  sync_tatas_init(&bt_lock);
  return PHOTON_OK;
}

void buffertable_finalize() {
  avl_destroy(btable, NULL);
}

int buffertable_find_containing(void* start, uint64_t size, photonBI* result) {
  return buffertable_find_exact(start, size, result);
}

int buffertable_find_exact(void* start, uint64_t size, photonBI* result) {
  sync_tatas_acquire(&bt_lock);
  {
    photon_buffer_internal tst;
    tst.buf.addr = (uintptr_t)start;
    tst.buf.size = size;
    void *res = avl_find(btable, &tst);
    if (res) {
      *result = (photonBI)res;
      sync_tatas_release(&bt_lock);
      return PHOTON_OK;
    }
  }
  sync_tatas_release(&bt_lock);

  return PHOTON_ERROR;
}

int buffertable_insert(photonBI buffer) {
  if (!btable) {
    log_err("buffertable not initialized. Call buffertable_init() first.");
    return PHOTON_ERROR;
  }

  sync_tatas_acquire(&bt_lock);
  {
    void *res = avl_insert(btable, (void*)buffer);
    if (res) {
      log_err("Tried to insert duplicate or overlapping buffer entry");
      sync_tatas_release(&bt_lock);
      return PHOTON_ERROR;
    }
  }
  sync_tatas_release(&bt_lock);

  return PHOTON_OK;
}


int buffertable_remove(photonBI buffer) {
  if (!btable) {
    log_err("buffertable not initialized. Call buffertable_init() first.");
    return PHOTON_ERROR;
  }

  sync_tatas_acquire(&bt_lock);
  {
    void *res = avl_delete(btable, (void*)buffer);
    if (!res) {
      dbg_warn("Tried to delete buffer entry that doesn't exist");
      sync_tatas_release(&bt_lock);
      return PHOTON_ERROR;
    }
  }
  sync_tatas_release(&bt_lock);

  return PHOTON_OK;
}

