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

#ifndef LIBHPX_GAS_AGAS_BST_H
#define LIBHPX_GAS_AGAS_BST_H

#include "GlobalVirtualAddress.h"
#include "libhpx/parcel.h"
#include "libhpx/padding.h"
#include "libhpx/util/Aligned.h"
#include "hpx/hpx.h"
#include <cuckoohash_map.hh>
#include <city_hasher.hh>
#include <cinttypes>
#include <unordered_map>
#include "rebalancer.h"

namespace libhpx {
namespace gas {
namespace agas {

class BlockStatisticsTable
{
 public:
  using GVA = GlobalVirtualAddress;

  BlockStatisticsTable();
  virtual ~BlockStatisticsTable();

  // Block Statistics Table (BST) entry.
  //
  // The BST entry maintains statistics about block accesses such as the
  // number of times a block was accessed (@p counts) and the size of
  // data transferred (@p sizes). The index in the @p counts and @p
  // sizes array represents the node which accessed this block.
  struct Entry {
    Entry() {}
    Entry(int n) : n(n)
    {
      counts = new uint64_t[n]();
      sizes  = new uint64_t[n]();
    }
    Entry(int n, const unsigned i, int count, size_t size) : n(n)
    {
      counts = new uint64_t[n]();
      sizes  = new uint64_t[n]();
      counts[i] = count;
      sizes[i] = size;
    }
    ~Entry()
    {
      delete[] counts;
      delete[] sizes;
    }
    size_t         n;
    uint64_t* counts;
    uint64_t*  sizes;
  };

  /// Update a block statistics record for a GVA.
  virtual void add(GVA gva, unsigned src, int count, size_t size);

  /// Add an entry to the block statistics table.
  void add(GVA gva, Entry& e);

  /// Clear the block statistics table.
  virtual void clear();

  // Constructs a sparse graph in the compressed sparse row (CSR)
  // format from the global BST, and serializes it into a parcel.
  virtual hpx_parcel_t* toParcel();

 protected:
  // Maximum bytes required for the serialized parcel.
  size_t serializeMaxBytes() const;

  // Constructs a sparse graph in the compressed sparse row (CSR)
  // format from the global BST, and serializes it into an output
  // buffer.
  size_t toBuffer(unsigned char* buf);

 private:
  using Map = cuckoohash_map<GVA, Entry, CityHasher<GVA>>;

  const unsigned rank_;                         //!< cache the local rank
  Map map_;                                     //!< the hashtable
};

// A hierarchical BST.

class HierarchicalBST final : public BlockStatisticsTable
{
 public:
  using GVA = GlobalVirtualAddress;
  using CacheAligned = libhpx::util::Aligned<HPX_CACHELINE_SIZE>;
  using Map = std::unordered_map<uint64_t, Entry>;

  HierarchicalBST();
  ~HierarchicalBST();

  /// Update a block statistics record for a GVA.
  virtual void add(GVA gva, unsigned src, int count, size_t size);

  // Constructs a sparse graph in the compressed sparse row (CSR)
  // format from the global BST, and serializes it into a parcel.
  virtual hpx_parcel_t* toParcel();

  // Get a thread-local map at index id.
  Map& getMap(const unsigned id) {
    return mapArray_[id].map_;
  }

 private:
  struct PaddedMap {
    Map map_;
    alignas(HPX_CACHELINE_SIZE) char end_[];
  };
  PaddedMap* mapArray_;   //!< thread-local map array
};

} // namespace agas
} // namespace gas
} // namespace libhpx

#endif // LIBHPX_GAS_AGAS_BST_H
