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


/// @file  libhpx/gas/agas/global.c
/// @brief This file implements the address-space allocator interface for global
///        memory management.
///
/// In practice this is trivial. We just use these to bind the heap_chunk
/// allocation routines to the global bitmap instance.
#include "AGAS.h"
#include "libhpx/debug.h"
#include "libhpx/locality.h"
#include "libhpx/memory.h"
#include <cstring>

namespace {
using libhpx::gas::agas::AGAS;
}

static void *
_chunk_alloc(void *addr, size_t n, size_t align, bool *zero,
             bool *commit, bool cyclic)
{
  void *chunk = AGAS::Instance()->chunkAllocate(addr, n, align, cyclic);
  if (!chunk) {
    return NULL;
  }

  // According to the jemalloc man page, if addr is set, then we *must* return a
  // chunk at that address.
  if (addr && addr != chunk) {
    AGAS::Instance()->chunkDeallocate(addr, n, cyclic);
    return NULL;
  }

  // If we are asked to zero a chunk, then we do so.
  if (*zero) {
    std::memset(chunk, 0, n);
  }

  // Commit is not relevant for linux/darwin.
  *commit = true;
  return chunk;
}

static void *
_cyclic_chunk_alloc(void *addr, size_t n, size_t align, bool *zero,
                    bool *commit, unsigned)
{
  return _chunk_alloc(addr, n, align, zero, commit, true);
}

static void *
_global_chunk_alloc(void *addr, size_t n, size_t align, bool *zero,
                    bool *commit, unsigned)
{
  return _chunk_alloc(addr, n, align, zero, commit, false);
}

static bool
_cyclic_chunk_free(void *addr, size_t n, bool, unsigned)
{
  AGAS::Instance()->chunkDeallocate(addr, n, true);
  return false;
}

static bool
_global_chunk_free(void *addr, size_t n, bool, unsigned)
{
  AGAS::Instance()->chunkDeallocate(addr, n, false);
  return false;
}

static const chunk_hooks_t _cyclic_hooks = {
  .alloc    = _cyclic_chunk_alloc,
  .dalloc   = _cyclic_chunk_free,
  .commit   = as_null_commit,
  .decommit = as_null_decommit,
  .purge    = as_null_purge,
  .split    = as_null_split,
  .merge    = as_null_merge
};

static const chunk_hooks_t _global_hooks = {
  .alloc    = _global_chunk_alloc,
  .dalloc   = _global_chunk_free,
  .commit   = as_null_commit,
  .decommit = as_null_decommit,
  .purge    = as_null_purge,
  .split    = as_null_split,
  .merge    = as_null_merge
};

void
AGAS::initAllocators(unsigned rank)
{
  if (rank == 0) {
    as_set_allocator(AS_CYCLIC, &_cyclic_hooks);
  }
  as_set_allocator(AS_GLOBAL, &_global_hooks);
}
