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

#include "BlockTranslationTable.h"
#include "AGAS.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/Worker.h"

namespace {
using libhpx::self;
using GVA = libhpx::gas::agas::GlobalVirtualAddress;
using BTT = libhpx::gas::agas::BlockTranslationTable;
LIBHPX_ACTION(HPX_DEFAULT, 0, UpdateOwner, BTT::UpdateOwnerHandler, HPX_ADDR,
              HPX_UINT32);
}

BTT::BlockTranslationTable(size_t size)
    : rank_(here->rank),
      map_()
{
}

BTT::~BlockTranslationTable()
{
}

int
BTT::UpdateOwnerHandler(hpx_addr_t addr, uint32_t owner)
{
  assert(here->gas->type() == HPX_GAS_AGAS);
  AGAS *agas = static_cast<AGAS*>(here->gas);
  agas->updateOwner(GlobalVirtualAddress(addr), owner);
  return HPX_SUCCESS;
}

void
BTT::insert(GVA gva, uint32_t owner, void *lva, size_t blocks, uint32_t attr)
{
  dbg_assert(!map_.contains(gva));
  if (!map_.insert(gva, Entry(owner, lva, blocks, attr))) {
    dbg_error("failed to insert gva\n");
  }
  log_gas("inserted (%zu, %p) at %u\n", gva.getAddr(), lva, rank_);
}

void
BTT::upsert(GVA gva, uint32_t owner, void *lva, size_t blocks, uint32_t attr)
{
  Entry e(owner, lva, blocks, attr);
  auto updatefn = [&](Entry& entry) {
    entry = e;
  };
  map_.upsert(gva, updatefn, e);
  log_gas("upserted (%zu, %p) at %u\n", gva.getAddr(), lva, rank_);
}

void
BTT::updateOwner(GlobalVirtualAddress gva, uint32_t owner) {
  assert(owner != rank_);
  auto fn = [&](Entry& entry) {
    assert(!entry.count);
    entry.owner = owner;
  };
  map_.upsert(gva, fn, Entry(owner, NULL, 1, HPX_GAS_ATTR_NONE));
}

bool
BTT::tryPin(GVA gva, void **lva) {
  unsigned owner = 0;

  // The fn lambda runs while holding the proper locks on the hash table that
  // will provide atomic access to its fields. We increment the count and grab
  // the lva whil we're in there.
  auto fn = [&](Entry& entry) {
    owner = entry.owner;
    if (owner == rank_) {
      assert(entry.count >= 0);
      entry.count++;
      log_gas("pinned %zu (%d)\n", gva.getAddr(), entry.count);
      if (lva) {
        *lva = (char*)(entry.lva) + gva.toBlockOffset();
      }
    }
  };

  // if I don't have and entry for this gva then I couldn't pin it
  if (!map_.update_fn(gva, fn)) {
    dbg_assert(gva.home != rank_);
    return false;
  }

  // if I have an entry and was the owner then I pinned it
  if (owner == rank_) {
    return true;
  }

  // if I have an entry, but don't own the translation, and the current parcel
  // was sent to this entry, then the sender has a bad cached translation and
  // should be updated to my view of the owner
  hpx_parcel_t *p = self->getCurrentParcel();
  if (p->src != rank_) {
    hpx_addr_t addr = gva.getAddr();
    hpx_call(HPX_THERE(p->src), UpdateOwner, HPX_NULL, &addr, &owner);
  }
  return false;
}

void
BTT::unpin(GVA gva)
{
  hpx_parcel_t *p = NULL;

  // The lambda runs while holding the correct locks to provide atomicity for
  // accessing the record's fields. We reduce it's reference count and take its
  // continuation if the count reaches zero.
  auto fn = [&](Entry& entry) {
    assert(entry.owner == rank_);
    assert(entry.count > 0);
    if (--entry.count) {
      std::swap(p, entry.cont);
    }
    log_gas("unpinned %zu (%d)\n", gva.getAddr(), entry.count);
  };

  if (!map_.update_fn(gva, fn)) {
    dbg_error("cannot find gva during unpin\n");
  }

  // If we got a continuation from the reduction then submit it back to the
  // scheduler.
  if (p) {
    self->spawn(p);
  }
}

void*
BTT::getLVA(GVA gva) const
{
  Entry entry;
  return (map_.find(gva, entry)) ? entry.lva : nullptr;
}

uint16_t
BTT::getOwner(GVA gva) const
{
  assert(map_.contains(gva));
  Entry entry;
  if (map_.find(gva, entry)) {
    return entry.owner;
  }
  else {
    assert(gva.home != rank_);
    return gva.home;
  }
}

uint16_t
BTT::getOwner(GVA gva, bool& cached) const
{
  Entry entry;
  cached = map_.find(gva, entry);
  return (cached) ? entry.owner : gva.home;
}

uint32_t
BTT::getAttr(GVA gva) const
{
  Entry entry;
  bool found = map_.find(gva, entry);
  return (found) ? entry.attr : HPX_ATTR_NONE;
}

void
BTT::setAttr(GVA gva, uint32_t attr)
{
  auto fn = [&](Entry& entry) {
    entry.attr |= attr;
  };

  if (!map_.update_fn(gva, fn)) {
    dbg_error("failed to find GVA during setAttr\n");
  }
}

char*
BTT::getSegment(GVA gva, size_t& blocks) const
{
  Entry entry;
  if (!map_.find(gva, entry)) {
    dbg_error("No segment mapped for GVA\n");
  }
  blocks = entry.blocks;
  return static_cast<char*>(entry.lva);
}

void
BTT::waitForZeroCount(GlobalVirtualAddress gva)
{
  // We don't have any way to transfer from inside the hash-table's update_fn
  // call. It is holding locks and we'll run into deadlock if that happens. At
  // the same time, we can't really both check the count and modify the record's
  // cont field if the current thread is actually running. Our solution is to
  // force a transfer operation, and run the hashtable check from inside the
  // transfer frame. This continuation lambda is run once the current thread is
  // no longer running.
  auto continuation = [=](hpx_parcel_t* p) {
    // This update_fn lambda checks the count and records the passed parcel as a
    // continuation, if it happens that the count is non-zero. Otherwise it
    // leaves p unmodified to indicate that it can just be rerun. We don't call
    // spawn() inside of this lambda because we don't know if that's going to
    // trigger recursive attempts to access the hashtable.
    auto fn = [&](Entry& entry) {
      if (entry.count != 0) {
        assert(!entry.cont);
        entry.cont = p;
        p = nullptr;
      }
    };

    if (!map_.update_fn(gva, fn)) {
      dbg_assert("cannot wait for reference count for non-local address\n");
    }

    // If p was unchanged then we can spawn it at this point.
    if (p) {
      self->spawn(p);
    }
  };

  // Perform the actual scheduling operation.
  self->schedule(continuation);
}

bool
BTT::tryRemove(GlobalVirtualAddress gva, void*& lva)
{
  int count = 0;
  auto fn = [&](Entry& entry) {
    count = entry.count;
    lva = entry.lva;
  };

  if (!map_.update_fn(gva, fn)) {
    dbg_error("tried to remove a GVA that is not mapped\n");
  }

  if (count == 0) {
    map_.erase(gva);
    return true;
  }

  return false;
}

void*
BTT::remove(GVA gva)
{
  void *lva = nullptr;
  while (!tryRemove(gva, lva)) {
    waitForZeroCount(gva);
  }
  log_gas("removed (%zu, %p) at %u\n", gva.getAddr(), lva, rank_);
  return lva;
}

void*
BTT::tryRemoveForFastpathFree(GVA gva)
{
  void* out = nullptr;
  if (gva.cyclic || gva.home != rank_) {
    return out;
  }

  auto fn = [&](Entry& entry) {
    if (entry.count == 0 && entry.blocks == 1) {
      out = entry.lva;
    }
  };

  if (map_.update_fn(gva, fn) && out) {
    map_.erase(gva);
    log_gas("removed (%zu, %p) at %u\n", gva.getAddr(), out, rank_);
  }

  return out;
}

bool
BTT::tryMove(GVA gva, uint32_t to, uint32_t& attr, void*& out)
{
  auto fn = [&](Entry& entry) {
    if (entry.count == 0) {
      entry.owner = to;
      attr = entry.attr;
      out = entry.lva;
    }
  };

  return (!map_.update_fn(gva, fn)) ? false : out != nullptr;
}

void*
BTT::move(GVA gva, uint32_t to, uint32_t& attr)
{
  void* lva = nullptr;
  while (!tryMove(gva, to, attr, lva)) {
    waitForZeroCount(gva);
  }
  return lva;
}
