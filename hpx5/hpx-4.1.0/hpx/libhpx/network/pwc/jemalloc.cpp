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

/// @brief This file implements the address-space allocator interface for
///        registered memory management using jemalloc.
///
/// We bind a simple mmap-then-pin strategy to the jemalloc chunk allocation
/// routines, augmented with a small LRU cache to try and avoid mmap and memory
/// registration pain for frequent chunk allocation/deallocation patterns.

#include "registered.h"
#include "PhotonTransport.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include "libhpx/system.h"
#include "libhpx/util/LRUCache.h"
#include <cstring>

namespace {
using libhpx::network::pwc::PhotonTransport;
}

static libhpx::util::LRUCache _chunks(8);

static bool
_registered_chunk_free(void *chunk, size_t n, bool committed, unsigned arena)
{
  _chunks.put(chunk, n, [n](void* chunk, size_t bytes) {
      PhotonTransport::Unpin(chunk, bytes);
      system_munmap_huge_pages(nullptr, chunk, bytes);
    });
  return 0;
}

static void *
_registered_chunk_alloc(void *addr, size_t n, size_t align, bool *zero,
                        bool *commit, unsigned arena)
{
  dbg_assert(zero);
  dbg_assert(commit);
  void *chunk = _chunks.get(n, [=]() {
      void* chunk = system_mmap_huge_pages(nullptr, addr, n, align);
      PhotonTransport::Pin(chunk, n, nullptr);
      return chunk;
    });
  if (!chunk) {
    return nullptr;
  }

  // According to the jemalloc man page, if addr is set, then we *must* return a
  // chunk at that address. When this code path fails we choose to bypass the
  // LRU cache because we don't want to keep returning this same chunk if it's a
  // problem.
  if (addr && addr != chunk) {
    PhotonTransport::Unpin(chunk, n);
    system_munmap_huge_pages(nullptr, chunk, n);
    return nullptr;
  }

  // If we are asked to zero a chunk, then we do so.
  if (*zero) {
    memset(chunk, 0, n);
  }

  // Commit is not relevant for linux/darwin.
  *commit = true;
  return chunk;
}

void
libhpx::network::pwc::registered_allocator_init(void)
{
  static const chunk_hooks_t _registered_hooks = {
    .alloc    = _registered_chunk_alloc,
    .dalloc   = _registered_chunk_free,
    .commit   = as_null_commit,
    .decommit = as_null_decommit,
    .purge    = as_null_purge,
    .split    = as_null_split,
    .merge    = as_null_merge
  };
  as_set_allocator(AS_REGISTERED, &_registered_hooks);
}
