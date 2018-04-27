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

#include <stdio.h>
#include <libhpx/debug.h>
#include <libhpx/memory.h>

const char *je_malloc_conf = "lg_chunk:21";

/// Backing declaration for the flags.
__thread int as_flags[AS_COUNT] = {0};

/// Each address space can have a custom allocator configured. This array stores
/// the allocator hooks for each address space.
/// @{
static const chunk_hooks_t *_hooks[AS_COUNT] = {NULL};
/// @}

void as_set_allocator(int id, const chunk_hooks_t *hooks) {
  dbg_assert(0 <= id && id < AS_COUNT);
  dbg_assert(hooks);
  dbg_assert(!_hooks[id]);
  _hooks[id] = hooks;
  as_join(id);
}

void as_join(int id) {
  if (as_flags[id] != 0) {
    log_gas("address space %d already joined\n", id);
    return;
  }

  const chunk_hooks_t *hooks = _hooks[id];

  // If there aren't any custom hooks set for this space, then the basic local
  // allocator is fine, which means that we don't need any special flags for
  // this address space.
  if (!hooks) {
    log_gas("no custom allocator for %d, using local\n", id);
    return;
  }

  // Create an arena that uses the right hooks.
  unsigned arena;
  size_t sz = sizeof(arena);
  dbg_check( je_mallctl("arenas.extend", &arena, &sz, NULL, 0) );

  char path[128];
  snprintf(path, 128, "arena.%u.chunk_hooks", arena);
  dbg_check( je_mallctl(path, NULL, NULL, (void*)hooks, sizeof(*hooks)) );

  // // Disable dirty page purging for this arena
  // snprintf(path, 124, "arena.%u.lg_dirty_mult", arena);
  // ssize_t i = -1;
  // dbg_check( je_mallctl(path, NULL, NULL, (void*)&i, sizeof(i)) );

  // Create a cache.
  unsigned cache;
  sz = sizeof(cache);
  dbg_check( je_mallctl("tcache.create", &cache, &sz, NULL, 0) );

  // And set the flags.
  as_flags[id] = MALLOCX_ARENA(arena) | MALLOCX_TCACHE(cache);
}

void as_leave(void) {
}

size_t as_bytes_per_chunk(void) {
  size_t log2_bytes_per_chunk = 0;
  size_t sz = sizeof(log2_bytes_per_chunk);
  je_mallctl("opt.lg_chunk", &log2_bytes_per_chunk, &sz, NULL, 0);
  return (1lu << log2_bytes_per_chunk);
}


void *as_malloc(int id, size_t bytes) {
  return je_mallocx(bytes, as_flags[id]);
}

void *as_calloc(int id, size_t nmemb, size_t bytes) {
  int flags = as_flags[id] | MALLOCX_ZERO;
  return je_mallocx(nmemb * bytes, flags);
}

void *as_memalign(int id, size_t boundary, size_t size) {
  int flags = as_flags[id] | MALLOCX_ALIGN(boundary);
  return je_mallocx(size, flags);
}

void as_free(int id, void *ptr)  {
  je_dallocx(ptr, as_flags[id]);
}

bool as_null_commit(void *chunk, size_t size, size_t offset, size_t length,
                    unsigned arena_ind) {
  return false;
}

bool as_null_decommit(void *chunk, size_t size, size_t offset, size_t length,
                      unsigned arena_ind) {
  log_mem("decommit of %zu bytes at offset %zu requested and ignored by HPX\n",
          size, offset);
  return true;
}

bool as_null_purge(void *chunk, size_t size, size_t offset, size_t length,
                   unsigned arena_ind) {
  log_mem("purge of %zu bytes at offset %zu requested and ignored by HPX\n",
                   size, offset);
  return true;
}

bool as_null_split(void *chunk, size_t size, size_t size_a, size_t size_b,
                   bool committed, unsigned arena_ind) {
  return true;
}

bool as_null_merge(void *chunk_a, size_t size_a, void *chunk_b, size_t size_b,
                   bool committed, unsigned arena_ind) {
  return true;
}
