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

#include "BlockStatisticsTable.h"
#include "AGAS.h"
#include "libhpx/action.h"
#include "libhpx/config.h"
#include "libhpx/debug.h"
#include "libhpx/Worker.h"
#include "rebalancer.h"

namespace {
using libhpx::self;
using GVA = libhpx::gas::agas::GlobalVirtualAddress;
using BST = libhpx::gas::agas::BlockStatisticsTable;
using HierarchicalBST = libhpx::gas::agas::HierarchicalBST;
}

BST::BlockStatisticsTable()
    : rank_(here->rank),
      map_()
{
}

BST::~BlockStatisticsTable()
{
  map_.clear();
}

void
BST::add(GVA gva, unsigned src, int count, size_t size)
{
  auto fn = [&](Entry& e) {
    e.counts[src] += count;
    e.sizes[src]  += size;
  };
  map_.upsert(gva, fn, Entry(here->ranks, src, count, size));
}

void
BST::add(GVA gva, Entry& entry)
{
  auto fn = [&](Entry& e) {
    for (unsigned i = 0; i < e.n; ++i) {
      e.counts[i] += entry.counts[i];
      e.sizes[i]  += entry.sizes[i];
    }
  };
  map_.upsert(gva, fn, entry);
}

void
BST::clear()
{
  map_.clear();
}

// Serialization format:
// member : Size (in multiples of uint64_t)
//
// nvtxs  : 1
// vtxs   : nvtxs 
// vwgts  : nvtxs
// vsizes : nvtxs
// xadj   : nvtxs
// nedges : 1
// adjncy : max(ranks * nvtxs)
// adjwgt : max(ranks * nvtxs)
// lsizes : ranks
// lnbrs  : max(ranks * nvtxs)

size_t
BST::serializeMaxBytes(void) const
{
  const size_t n = map_.size();
  const unsigned ranks = here->ranks;
  size_t count = 
      2                   // nvtx, nedges
      + 4 * n             // vtxs, vwgts, vsizes, xadj
      + 3 * ranks * n     // adjncy, adjwgt, lnbrs
      + ranks;            // lsizes
  return count * sizeof(uint64_t);
}

size_t
BST::toBuffer(unsigned char* output)
{
  const unsigned ranks = here->ranks;
  const size_t map_size = map_.size();

  uint64_t* buf = reinterpret_cast<uint64_t*>(output);

  // cursor into the serialized buffer
  size_t n = 0;

  uint64_t* nvtxs  = buf+n; n += 1;
  uint64_t* vtxs   = buf+n; n += map_size;
  uint64_t* vwgt   = buf+n; n += map_size;
  uint64_t* vsizes = buf+n; n += map_size;
  uint64_t* xadj   = buf+n; n += map_size;
  uint64_t* nedges = buf+n; n += 1;
  uint64_t* adjncy = buf+n;

  // store the vertex count first
  *nvtxs = map_size;

  // store the vtxs, vwgt, vsizes and xadj array each map_size bytes long
  int id   = 0;
  int nbrs = 0;
  std::vector<uint64_t> adjwgt;
  std::vector<uint64_t> lnbrs[ranks];
  {
    auto lt = map_.lock_table();
    for (const auto& item : lt) {
      auto& entry = item.second;
      uint64_t total_vwgt  = 0;
      uint64_t total_vsize = 0;
      int prev_nbrs = nbrs;
      for (unsigned k = 0; k < ranks; ++k) {
        uint64_t count = entry.counts[k];
        uint64_t size = entry.sizes[k];
        if (count != 0) {
          lnbrs[k].push_back(id);
          adjncy[nbrs++] = k;
          adjwgt.push_back(count * size);
          total_vwgt  += count;
          total_vsize += size;
        }
      }

      vtxs[id]   = item.first;
      vwgt[id]   = total_vwgt;
      vsizes[id] = total_vsize;
      xadj[id]   = nbrs - prev_nbrs;
      id++;
    }
  }

  // copy edge data into the serialized buffer
  *nedges = nbrs;
  n += nbrs;
  std::copy(adjwgt.begin(), adjwgt.end(), buf+n);
  n += adjwgt.size();

  uint64_t* lsizes = buf+n; n += ranks;
  for (unsigned k = 0; k < ranks; ++k) {
    lsizes[k] = lnbrs[k].size();
    std::copy(lnbrs[k].begin(), lnbrs[k].end(), buf+n);
    n += lsizes[k];
  }
  return n * sizeof(uint64_t);
}

hpx_parcel_t*
BST::toParcel()
{
  auto lt = map_.lock_table();
  size_t max_bytes = serializeMaxBytes();
  hpx_parcel_t* p = hpx_parcel_acquire(NULL, max_bytes);
  unsigned char* buf = static_cast<unsigned char*>(hpx_parcel_get_data(p));
  p->size = toBuffer(buf);
  map_.clear();
  return p;
}

HierarchicalBST::HierarchicalBST()
{
  const int n = here->config->threads;
  const size_t size = n * sizeof(PaddedMap);

  void* ptr;
  if (posix_memalign(&ptr, HPX_CACHELINE_SIZE, size)) {
    throw std::bad_alloc();
  }

  mapArray_ = reinterpret_cast<PaddedMap*>(ptr);
}

HierarchicalBST::~HierarchicalBST()
{
  free(mapArray_);
}

void
HierarchicalBST::add(GVA gva, unsigned src, int count, size_t size)
{
  if (!self) return;

  const unsigned id = self->getId();
  const uint64_t block = gva.getAddr();
  auto& map = mapArray_[id].map_;
  auto it = map.find(block);
  if (it != map.end()) {
    auto entry = it->second;
    entry.counts[src] += count;
    entry.sizes[src]  += size;
    return;
  }

  map[block] = Entry(here->ranks, src, count, size);
}

// This function takes the thread-local BST and merges it with the
// per-node global BST.
static int
mergeBST(int id, void* b)
{
  HierarchicalBST *bst = static_cast<HierarchicalBST*>(b);
  auto& m = bst->getMap(id);
  unsigned count = m.size();
  log_gas("Adding %u entries from %d thread-local BST to global BST.\n", id, count);
  for (auto it = m.begin(); it != m.end(); ++it) {
    bst->BST::add(it->first, it->second);
  }
  m.clear();
  return HPX_SUCCESS;
}

hpx_parcel_t*
HierarchicalBST::toParcel()
{
  hpx_par_for_sync(mergeBST, 0, HPX_THREADS, this);
  return BST::toParcel();
}
