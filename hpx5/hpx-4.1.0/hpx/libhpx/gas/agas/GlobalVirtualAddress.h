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

#ifndef LIBHPX_GAS_AGAS_GLOBAL_VIRTUAL_ADDRESS_H
#define LIBHPX_GAS_AGAS_GLOBAL_VIRTUAL_ADDRESS_H

#include "libhpx/gpa.h"
#include "libhpx/util/math.h"
#include "hpx/hpx.h"
#include <cstdint>
#include <city_hasher.hh>

namespace libhpx {
namespace gas {
namespace agas {
static constexpr int GVA_RANK_BITS = 16;
static constexpr int GVA_SIZE_BITS = 5;
static constexpr int GVA_OFFSET_BITS = 42;

union GlobalVirtualAddress {
 private:
  hpx_addr_t addr_;                         //!< the underlying hpx address bits

 public:
  struct {
    uint64_t offset : GVA_OFFSET_BITS;         //!< the actual address
    uint64_t   size : GVA_SIZE_BITS;           //!< ceil_log2 of the block size
    uint64_t cyclic : 1;                       //!< interpret the home as cyclic
    uint64_t   home : GVA_RANK_BITS;           //!< home rank for the address
  };

  GlobalVirtualAddress() : addr_(HPX_NULL) {
  }

  GlobalVirtualAddress(hpx_addr_t addr) : addr_(addr) {
  }

  GlobalVirtualAddress(uint64_t o, int s, int c, uint16_t h)
      : offset(o), size(s), cyclic(c), home(h) {
  }

  GlobalVirtualAddress add(hpx_gas_ptrdiff_t bytes, size_t bsize) const {
    assert(size == util::ceil_log2(bsize));
    if (cyclic) {
      return addCyclic(bytes, bsize);
    }
    else {
      return addLocal(bytes, bsize);
    }
  }

  GlobalVirtualAddress& operator+=(hpx_gas_ptrdiff_t bytes) {
    *this = add(bytes, getBlockSize());
    return *this;
  }

  hpx_gas_ptrdiff_t sub(GlobalVirtualAddress rhs, size_t bsize) const {
    assert(size == rhs.size);
    assert(size == util::ceil_log2(bsize));
    assert(!cyclic || rhs.cyclic);
    assert(!rhs.cyclic || cyclic);

    if (cyclic) {
      return gpa_sub_cyclic(addr_, rhs.addr_, bsize);
    }
    else {
      assert(home == rhs.home);
      return subLocal(rhs, bsize);
    }
  }

  operator hpx_addr_t() const {
    return addr_;
  }

  uint64_t toKey() const {
    uint64_t mask = ~((uint64_t(1) << size) - 1);
    return addr_ & mask;
  }

  uint64_t toBlockOffset() const {
    uint64_t mask = ((uint64_t(1) << size) - 1);
    return addr_ & mask;
  }

  hpx_addr_t getAddr() const {
    return addr_;
  }

  size_t getBlockSize() const {
    return uint64_t(1) << size;
  }

  bool operator==(const GlobalVirtualAddress& rhs) const {
    return toKey() == rhs.toKey();
  }

 private:
  GlobalVirtualAddress addCyclic(hpx_gas_ptrdiff_t bytes, size_t bsize) const {
    GlobalVirtualAddress lhs(gpa_add_cyclic(addr_, bytes, bsize));
    lhs.size = size;
    lhs.cyclic = 1;
    return lhs;
  }

  GlobalVirtualAddress addLocal(int64_t n, size_t bsize) const {
    uint64_t block_size = (uint64_t(1) << size);
    int64_t blocks = n / bsize;
    int64_t bytes = n % bsize;
    uint64_t addr = addr_ + blocks * block_size + bytes;
    dbg_assert((addr & (block_size - 1)) < bsize);
    return GlobalVirtualAddress(addr);
  }

  hpx_gas_ptrdiff_t subLocal(GlobalVirtualAddress rhs, size_t bsize) const {
    uint64_t mask = (uint64_t(1) << size) - 1;
    uint64_t plhs = offset & mask;
    uint64_t prhs = rhs.offset & mask;
    uint64_t blhs = offset >> size;
    uint64_t brhs = rhs.offset >> size;
    return (plhs - prhs) + (blhs - brhs) * bsize;
  }
};

static_assert(sizeof(GlobalVirtualAddress) == sizeof(hpx_addr_t),
              "GVA structure packing failed\n");

} // namespace agas
} // namespace gas
} // namespace libhpx

template <>
class CityHasher<libhpx::gas::agas::GlobalVirtualAddress> {
 public:
  size_t operator()(const libhpx::gas::agas::GlobalVirtualAddress& gva) const {
    uint64_t key = gva.toKey();
    return CityHash64(reinterpret_cast<const char*>(&key), sizeof(key));
  }
};

#endif // LIBHPX_GAS_AGAS_GLOBAL_VIRTUAL_ADDRESS_H
