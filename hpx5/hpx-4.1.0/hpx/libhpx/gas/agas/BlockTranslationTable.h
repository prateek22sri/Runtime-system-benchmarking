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

#ifndef LIBHPX_GAS_AGAS_BTT_H
#define LIBHPX_GAS_AGAS_BTT_H

#include "GlobalVirtualAddress.h"
#include "libhpx/parcel.h"
#include "hpx/hpx.h"
#include <cuckoohash_map.hh>
#include <cinttypes>

namespace libhpx {
namespace gas {
namespace agas {
class BlockTranslationTable
{
 public:
  using GVA = GlobalVirtualAddress;

  BlockTranslationTable(size_t);
  ~BlockTranslationTable();

  /// Insert a block translation record for a gva.
  void insert(GVA gva, uint32_t owner, void* lva, size_t blocks, uint32_t attr);

  /// Update a block translation record for a gva.
  void upsert(GVA gva, uint32_t owner, void *lva, size_t blocks, uint32_t attr);

  /// Try an pin a translation.
  ///
  /// This will check to see if the translation is available and owned locally,
  /// and if so it will atomically increment the reference count and return the
  /// local translation through the @p lva parameter.
  ///
  /// @param        gva The global virtual address to translate.
  /// @param[out]   lva The pinned translation.
  ///
  /// @returns          TRUE if the translation succeeded, FALSE otherwise.
  bool tryPin(GVA gva, void **lva);

  /// Unpin a translation.
  ///
  /// The @p gva _must_ have participated in a previous successful tryPin() at
  /// the current locality.
  void unpin(GVA gva);

  /// Get the local virtual address associated with the global virtual address.
  void* getLVA(GVA gva) const;

  /// Try and read the owner for a global address.
  ///
  /// Every locality always has an idea of where the owner of a global address
  /// is. Sometimes this is based on an entry in the local cache, and sometimes
  /// its based on the default home mapping. Some clients of this interface want
  /// to know if the result was a miss, so we provide two versions of the call,
  /// one of which returns the cached state through an out parameter.
  ///
  uint16_t getOwner(GVA gva) const;
  uint16_t getOwner(GVA gva, bool& cached) const;

  /// Get the attributes associated with a GVA.
  uint32_t getAttr(GVA gva) const;

  /// Set the attributes stored with a global virtual address.
  void setAttr(GVA gva, uint32_t attr);

  /// Get the "segment" data associated with a mapping.
  ///
  /// The segment is the local virtual address of the mapping. The client of
  /// this routine also always wants to know the number of blocks in the
  /// segment, so we look that up at the same time to minimize locking. We also
  /// return the segment as a char* because the client wants to perform address
  /// arithmetic on it.
  ///
  /// @param        gva The GVA of the base of the segment.
  /// @param[out] blocks The number of blocks in the segment..
  ///
  /// @returns          The local virtual address of the segment.
  char* getSegment(GVA gva, size_t& blocks) const;

  /// Remove a translation once its reference count hits 0.
  ///
  /// This will block until the reference count hits zero, and then remove the
  /// translation, returning the local virtual address that was associated with
  /// the translation, if there is one.
  ///
  ///
  void* remove(GVA gva);

  /// Try to remove the gva from the mapping for a fastpath free operation.
  ///
  /// A fast free operation happens when we have a gva that is just a single
  /// block, that hasn't been relocated, and has a reference count of 0. If this
  /// succeeds it returns the local buffer that was registered for the gva,
  /// otherwise it returns NULL.
  ///
  /// It is crucial for correctness that we perform this optimization, as the
  /// free code depends on being able to allocate and free simple blocks (lcos)
  /// on this fastpath. Without it, we can get an infinite loop.
  void* tryRemoveForFastpathFree(GVA gva);

  /// This function updates the entry to point to the owner.
  ///
  /// This will block until the reference count hits zero, and then update the
  /// translation. It will return the local virtual address that was associated
  /// with the translation, and it will return the attributes that were stored
  /// with the translation using the @p attr output pointer.
  void* move(GVA gva, uint32_t rank, uint32_t& attr);

  /// Update the owner for a block.
  ///
  /// This should only be called for blocks that have a count of 0, and that
  /// aren't owned locally.
  ///
  /// @param        gva The global virtual address.
  /// @param      owner The new owner.
  void updateOwner(GVA gva, uint32_t owner);

  /// The asynchronous entry point for ownerUpdate.
  ///
  /// This verifies that the local GAS instance is an AGAS instance, and then
  /// updates the owner of the passed @p address to @p owner.
  static int UpdateOwnerHandler(hpx_addr_t addr, uint32_t owner);

 private:
  /// Try to remove a global virtual address from the block translation table.
  ///
  /// This will return TRUE if the gva was successfully removed, and in that
  /// case it will return the local virtual address of the translation in the
  /// required @p lva parameter.
  ///
  /// This will return FALSE if the gva is not local, or if the gva is local but
  /// its reference count is not zero.
  bool tryRemove(GVA gva, void*& lva);

  /// Try to update the owner of a block.
  ///
  /// This will return TRUE if the gva was successfully removed, and in that
  /// case it will return a pointer to the previous local virtual address in
  bool tryMove(GVA gva, uint32_t rank, uint32_t& attr, void*& lva);

  /// Block the calling thread until the reference count for the gva hits zero.
  ///
  /// This can suffer from spurious wake up events, so anyone calling this
  /// function must be prepared to check their condition again after the call.
  void waitForZeroCount(GVA gva);

  struct Entry {
    int32_t      count;
    uint32_t     owner;
    void          *lva;
    size_t      blocks;
    hpx_parcel_t *cont;
    uint32_t      attr;
    Entry() : count(0), owner(0), lva(NULL), blocks(1), cont(NULL),
              attr(HPX_GAS_ATTR_NONE) {
    }
    Entry(int32_t o, void *l, size_t b, uint32_t a)
        : count(0), owner(o), lva(l), blocks(b), cont(NULL), attr(a) {
    }
  };

  using Hash = CityHasher<GlobalVirtualAddress>;
  using Map = cuckoohash_map<GlobalVirtualAddress, Entry, Hash>;

  const unsigned rank_;                         //!< cache the local rank
  Map map_;                                     //!< the hashtable
};
} // namespace agas
} // namespace gas
} // namespace libhpx

#endif // LIBHPX_GAS_AGAS_BTT_H
