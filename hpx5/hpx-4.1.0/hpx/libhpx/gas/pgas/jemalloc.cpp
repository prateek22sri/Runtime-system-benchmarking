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

#include "Allocator.h"
#include "HeapSegment.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include <cstring>

namespace {
using HeapSegment = libhpx::gas::pgas::HeapSegment;
using ChunkType = libhpx::gas::pgas::HeapSegment::ChunkType;
using GlobalAllocator = libhpx::gas::pgas::GlobalAllocator;
using CyclicAllocator = libhpx::gas::pgas::CyclicAllocator;
}

/// @file  libhpx/gas/pgas/global.c
/// @brief This file implements the address-space allocator interface for global
///        memory management.
static void *
_chunk_alloc(void *addr, size_t n, size_t align, bool *zero,
             bool *commit, unsigned arena, ChunkType type)
{
  dbg_assert(zero);
  dbg_assert(commit);
  void *chunk = HeapSegment::Instance()->allocateChunk(addr, n, align, type);
  if (!chunk) {
    return NULL;
  }

  // According to the jemalloc man page, if addr is set, then we *must* return a
  // chunk at that address.
  if (addr && addr != chunk) {
    HeapSegment::Instance()->deallocateChunk(chunk, n);
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
_global_chunk_alloc(void *addr, size_t n, size_t align, bool *zero,
                    bool *commit, unsigned arena)
{
  return _chunk_alloc(addr, n, align, zero, commit, arena, ChunkType::GLOBAL);
}

static void *
_cyclic_chunk_alloc(void *addr, size_t n, size_t align, bool *zero,
                      bool *commit, unsigned arena)
{
  return _chunk_alloc(addr, n, align, zero, commit, arena, ChunkType::CYCLIC);
}


static bool
_chunk_free(void *addr, size_t n, bool commited, unsigned arena)
{
  HeapSegment::Instance()->deallocateChunk(addr, n);
  return 0;
}

static const chunk_hooks_t _global_hooks = {
  .alloc    = _global_chunk_alloc,
  .dalloc   = _chunk_free,
  .commit   = as_null_commit,
  .decommit = as_null_decommit,
  .purge    = as_null_purge,
  .split    = as_null_split,
  .merge    = as_null_merge
};

GlobalAllocator::GlobalAllocator(int rank) : hpx143Fix_() {
  as_set_allocator(AS_GLOBAL, &_global_hooks);
  if (rank == 0) {
    hpx143Fix_ = as_malloc(AS_GLOBAL, sizeof(int));
  }
}

GlobalAllocator::~GlobalAllocator() {
  if (hpx143Fix_) {
    as_free(AS_GLOBAL, hpx143Fix_);
  }
}

static const chunk_hooks_t _cyclic_hooks = {
  .alloc    = _cyclic_chunk_alloc,
  .dalloc   = _chunk_free,
  .commit   = as_null_commit,
  .decommit = as_null_decommit,
  .purge    = as_null_purge,
  .split    = as_null_split,
  .merge    = as_null_merge
};

CyclicAllocator::CyclicAllocator(int rank) {
  if (rank == 0) {
    as_set_allocator(AS_CYCLIC, &_cyclic_hooks);
  }
}

CyclicAllocator::~CyclicAllocator() {
}
