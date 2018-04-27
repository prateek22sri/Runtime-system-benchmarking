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

#include "Continuation.h"
#include <libhpx/debug.h>
#include <cinttypes>
#include <string.h>

namespace {
using libhpx::process::Continuation;
}

Continuation::Continuation(size_t bytes) : bytes_(bytes), parcels_(16) {
}

Continuation::~Continuation() {
  for (auto&& p : parcels_) {
    if (p) {
      parcel_release(p);
      parcel_delete(p);
    }
  }
}

unsigned
Continuation::insert(hpx_action_t op, hpx_addr_t addr)
{
  log_coll("registering continuation (%d, %" PRIu64") at %p\n", op, addr, this);
  hpx_parcel_t *p = hpx_parcel_acquire(nullptr, bytes_);
  p->action = op;
  p->target = addr;
  parcel_retain(p);

  // fill any spots left by leavers
  for (unsigned i = 0, e = parcels_.size(); i < e; ++i) {
    if (parcels_[i] == nullptr) {
      parcels_[i] = p;
      return i;
    }
  }
  parcels_.push_back(p);
  return parcels_.size() - 1;
}

void
Continuation::remove(unsigned id)
{
  dbg_assert(id < parcels_.size());
  parcel_release(parcels_[id]);
  parcel_delete(parcels_[id]);
  parcels_[id] = nullptr;
}

void
Continuation::trigger(const void *value) const
{
  log_coll("continuing %zu from %p\n", parcels_.size(), this);

  for (auto&& i : parcels_) {
    if (i) {
      hpx_parcel_t *p = parcel_clone(i);
      hpx_parcel_set_data(p, value, bytes_);
      parcel_launch(p);
    }
  }
}

