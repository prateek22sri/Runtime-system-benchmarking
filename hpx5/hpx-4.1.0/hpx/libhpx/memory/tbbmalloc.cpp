// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>
#include <tbb/scalable_allocator.h>
#include <libhpx/memory.h>
#include <cstdlib>

using namespace rml;

void* pools[AS_COUNT] = {NULL};

void as_join(int id) {
}

void as_leave(void) {
}

size_t
as_bytes_per_chunk(void) {
  return ((size_t)1) << 21;
}

void *as_malloc(int id, size_t bytes) {
  if (MemoryPool* pool = static_cast<MemoryPool*>(pools[id])) {
    return pool_malloc(pool, bytes);
  }
  else {
    return malloc(bytes);
  }
}

void *as_calloc(int id, size_t nmemb, size_t bytes) {
  if (MemoryPool* pool = static_cast<MemoryPool*>(pools[id])) {
    void *p = pool_malloc(pool, nmemb * bytes);
    if (p) {
      memset(p, 0, nmemb * bytes);
    }
    return p;
  }
  else {
    return calloc(nmemb, bytes);
  }
}

void *as_memalign(int id, size_t boundary, size_t size) {
  if (MemoryPool* pool = static_cast<MemoryPool*>(pools[id])) {
    return pool_aligned_malloc(pool, size, boundary);
  }
  else {
    void *p = NULL;
    posix_memalign(&p, boundary, size);
    return p;
  }
}

void as_free(int id, void *ptr) {
  if (MemoryPool* pool = static_cast<MemoryPool*>(pools[id])) {
    pool_free(pool, ptr);
  }
  else {
    free(ptr);
  }
}
