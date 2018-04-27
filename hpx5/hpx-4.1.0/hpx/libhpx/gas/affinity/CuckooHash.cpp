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
#include "libhpx/gas/Affinity.h"
#include "libhpx/debug.h"
#include "libhpx/locality.h"
#include "libhpx/Scheduler.h"
#include <cinttypes>

namespace {
using libhpx::Scheduler;
using libhpx::gas::Affinity;
using libhpx::gas::affinity::CuckooHash;
}

CuckooHash::CuckooHash() : Affinity(), map_()
{
}

CuckooHash::~CuckooHash()
{
}

void
CuckooHash::setAffinity(hpx_addr_t gva, int worker)
{
  DEBUG_IF(here->gas->ownerOf(gva) != here->rank) {
    dbg_error("Attempt to set affinity of %" PRIu64 " at %d (owned by %d)\n",
              gva, here->rank, here->gas->ownerOf(gva));
  }
  DEBUG_IF(worker < 0 || here->sched->getNWorkers() <= worker) {
    dbg_error("Attempt to set affinity of %" PRIu64
              " to %d is outside range [0, %d)\n",
              gva, worker, here->sched->getNWorkers());
  }
  // @todo: Should we be pinning gva? The interface doesn't require it, but it
  //        could prevent usage errors in AGAS? On the other hand, it could
  //        result in debugging issues with pin reference counting.
  map_.insert(gva, worker);
}

void
CuckooHash::clearAffinity(hpx_addr_t gva)
{
  DEBUG_IF(here->gas->ownerOf(gva) != here->rank) {
    dbg_error("Attempt to clear affinity of %" PRIu64 " at %d (owned by %d)\n",
              gva, here->rank, here->gas->ownerOf(gva));
  }
  map_.erase(gva);
}

int
CuckooHash::getAffinity(hpx_addr_t gva) const
{
  int worker = -1;
  map_.find(gva, worker);
  return worker;
}
