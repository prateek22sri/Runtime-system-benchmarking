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

#ifndef LIBHPX_GAS_H
#define LIBHPX_GAS_H

#include "libhpx/config.h"
#include "libhpx/MemoryOps.h"
#include "libhpx/StringOps.h"
#include "libhpx/gas/Affinity.h"
#include "libhpx/gas/Allocator.h"
#include "libhpx/boot/Network.h"

namespace libhpx {
class GAS : public virtual libhpx::gas::Affinity, public StringOps,
            public gas::Allocator
{
 public:
  static GAS* Create(config_t *cfg, const boot::Network* const boot);

  virtual ~GAS();

  virtual libhpx_gas_t type() const = 0;
  virtual uint64_t maxBlockSize() const = 0;

  virtual void* pinHeap(MemoryOps& memOps, void* key) const = 0;
  virtual void unpinHeap(MemoryOps&) const = 0;

  virtual hpx_gas_ptrdiff_t sub(hpx_addr_t lhs, hpx_addr_t rhs, size_t bsize) const = 0;
  virtual hpx_addr_t add(hpx_addr_t gva, hpx_gas_ptrdiff_t n, size_t bsize) const = 0;

  virtual hpx_addr_t there(uint32_t i) const = 0;
  virtual uint32_t ownerOf(hpx_addr_t gva) const = 0;
  virtual bool tryPin(hpx_addr_t gva, void **local) = 0;
  virtual void unpin(hpx_addr_t gva) = 0;
  virtual void free(hpx_addr_t gca, hpx_addr_t rsync) = 0;
  virtual uint32_t getAttribute(hpx_addr_t gva) const = 0;
  virtual void setAttribute(hpx_addr_t gva, uint32_t attr) = 0;
  virtual void move(hpx_addr_t src, hpx_addr_t dst, hpx_addr_t lco) = 0;
};

static const char* const GAS_ATTR_TO_STRING[] = {
  "NONE",
  "READONLY",
  "LOAD-BALANCE",
  "LCO"
};

} // namespace libhpx

#endif // LIBHPX_GAS_H
