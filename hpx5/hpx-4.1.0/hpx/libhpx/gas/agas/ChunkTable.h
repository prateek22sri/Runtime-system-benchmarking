// ==================================================================-*- C++ -*-
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

#ifndef LIBHPX_GAS_AGAS_CHUNK_TABLE_H
#define LIBHPX_GAS_AGAS_CHUNK_TABLE_H

#include <cuckoohash_map.hh>
#include <city_hasher.hh>
#include <cstddef>

/// The chunk table provides inverse mapping from an lva chunk to its base
/// offset in the virtual address space.
namespace libhpx {
namespace gas {
namespace agas {

class ChunkTable {
  using Map = cuckoohash_map<const void*, uint64_t, CityHasher<const void*>>;

 public:
  ChunkTable(size_t size);
  ~ChunkTable();

  /// Lookup the base offset of a global virtual address.
  uint64_t offsetOf(const void* addr) const;

  void insert(const void* chunk, uint64_t base);
  void remove(const void* chunk);

 private:
  const uintptr_t chunkMask_;
  Map map_;
};

} // namespace agas
} // namespace gas
} // namespace libhpx

#endif // LIBHPX_GAS_AGAS_CHUNK_TABLE_H
