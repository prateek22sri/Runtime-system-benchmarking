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


#include "AGAS.h"
#include "libhpx/memory.h"
#include <tbb/scalable_allocator.h>
#include <cassert>

namespace {
using namespace rml;
using libhpx::gas::agas::AGAS;
}

static void *
_cyclic_chunk_alloc(intptr_t pool_id, size_t &bytes)
{
  assert(pool_id == AS_CYCLIC);
  return AGAS::Instance()->chunkAllocate(nullptr, bytes, bytes, true);
}

static int
_cyclic_chunk_free(intptr_t pool_id, void* raw_ptr, size_t raw_bytes)
{
  assert(pool_id == AS_CYCLIC);
  AGAS::Instance()->chunkDeallocate(raw_ptr, raw_bytes, true);
  return 0;
}

static void *
_global_chunk_alloc(intptr_t pool_id, size_t &bytes)
{
  assert(pool_id == AS_GLOBAL);
  return AGAS::Instance()->chunkAllocate(nullptr, bytes, bytes, false);
}

static int
_global_chunk_free(intptr_t pool_id, void* raw_ptr, size_t raw_bytes)
{
  assert(pool_id == AS_GLOBAL);
  AGAS::Instance()->chunkDeallocate(raw_ptr, raw_bytes, false);
  return 0;
}

void
AGAS::initAllocators(unsigned rank)
{
  MemoryPool* pool = nullptr;
  size_t chunkSize = as_bytes_per_chunk();
  if (rank == 0) {
    const MemPoolPolicy policy(_cyclic_chunk_alloc, _cyclic_chunk_free,
                               chunkSize);
    pool_create_v1(AS_CYCLIC, &policy, &pool);
    pools[AS_CYCLIC] = pool;
  }

  const MemPoolPolicy policy(_global_chunk_alloc, _global_chunk_free,
                             chunkSize);
  pool_create_v1(AS_GLOBAL, &policy, &pool);
  pools[AS_GLOBAL] = pool;
}
