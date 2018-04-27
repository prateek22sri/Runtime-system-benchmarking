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
#include "libhpx/memory.h"
#include <tbb/scalable_allocator.h>
#include <cassert>

namespace {
using namespace rml;
using HeapSegment = libhpx::gas::pgas::HeapSegment;
using ChunkType = libhpx::gas::pgas::HeapSegment::ChunkType;
using GlobalAllocator = libhpx::gas::pgas::GlobalAllocator;
using CyclicAllocator = libhpx::gas::pgas::CyclicAllocator;
}

static void *
_cyclic_chunk_alloc(intptr_t pool_id, size_t &bytes) {
  assert(pool_id == AS_CYCLIC);
  HeapSegment* segment = HeapSegment::Instance();
  size_t align = segment->getBytesPerChunk();
  return segment->allocateChunk(nullptr, bytes, align, ChunkType::CYCLIC);
}

static void *
_global_chunk_alloc(intptr_t pool_id, size_t &bytes) {
  assert(pool_id == AS_GLOBAL);
  HeapSegment* segment = HeapSegment::Instance();
  size_t align = segment->getBytesPerChunk();
  return segment->allocateChunk(nullptr, bytes, align, ChunkType::GLOBAL);
}

static int
_chunk_free(intptr_t pool_id, void* raw_ptr, size_t raw_bytes) {
  HeapSegment* segment = HeapSegment::Instance();
  segment->deallocateChunk(raw_ptr, raw_bytes);
  return 0;
}

GlobalAllocator::GlobalAllocator(int rank)
{
  int id = AS_GLOBAL;
  size_t granularity = as_bytes_per_chunk();
  const MemPoolPolicy policy(_global_chunk_alloc, _chunk_free, granularity);
  MemoryPool* pool = nullptr;
  pool_create_v1(id, &policy, &pool);
  pools[id] = pool;
}

GlobalAllocator::~GlobalAllocator()
{
}

CyclicAllocator::CyclicAllocator(int rank)
{
  if (rank == 0) {
    int id = AS_CYCLIC;
    size_t granularity = as_bytes_per_chunk();
    const MemPoolPolicy policy(_cyclic_chunk_alloc, _chunk_free, granularity);

    MemoryPool* pool = nullptr;
    pool_create_v1(id, &policy, &pool);
    pools[id] = pool;
  }
}

CyclicAllocator::~CyclicAllocator()
{
}
