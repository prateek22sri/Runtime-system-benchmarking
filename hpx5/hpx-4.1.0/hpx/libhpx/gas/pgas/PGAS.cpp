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

#include <cstring>
#include "PGAS.h"
#include "libhpx/Network.h"
#include "libhpx/util/math.h"

namespace {
using libhpx::gas::pgas::PGAS;
using libhpx::util::ceil_log2;
using libhpx::util::ceil_div;
}

static LIBHPX_ACTION(HPX_INTERRUPT, 0, SetCsbrk, PGAS::SetCsbrkHandler,
                     HPX_SIZE_T);
static LIBHPX_ACTION(HPX_INTERRUPT, 0, ZeroBlocks, PGAS::ZeroBlocksHandler,
                     HPX_UINT64, HPX_SIZE_T, HPX_SIZE_T);
static LIBHPX_ACTION(HPX_INTERRUPT, 0, AllocateCyclicBlocks,
                     PGAS::AllocateCyclicBlocksHandler, HPX_SIZE_T,HPX_SIZE_T,
                     HPX_UINT32, HPX_UINT32, HPX_INT);
static LIBHPX_ACTION(HPX_DEFAULT, 0, Free, PGAS::FreeHandler, HPX_ADDR,
                     HPX_ADDR);

PGAS::PGAS(const config_t *cfg)
    : GAS(),
      HeapSegment(cfg->heapsize),
      global_(here->rank),
      cyclic_(here->rank)
{
}

PGAS::~PGAS()
{
}

bool
PGAS::tryPin(hpx_addr_t gpa, void **local)
{
  // we're safe for HPX_HERE/THERE because gpa_to_rank doesn't range-check
  if (gpa_to_rank(gpa) != here->rank) {
    return false;
  }

  // special case messages to "here"
  if (local) {
    *local = (gpa != HPX_HERE) ? gpaToLVA(gpa) : &here;
  }

  return true;
}

void
PGAS::free(hpx_addr_t gpa, hpx_addr_t sync)
{
  if (gpa == HPX_NULL) {
    hpx_lco_set(sync, 0, NULL, HPX_NULL, HPX_NULL);
    return;
  }

  if (here->rank != gpa_to_rank(gpa)) {
    hpx_call(HPX_THERE(gpa_to_rank(gpa)), Free, HPX_NULL, &gpa, &sync);
    return;
  }

  dbg_assert(!isCyclicAddress(gpa) || here->rank == 0);

  // return the memory to the the correct local memory region
  if (isCyclicAddress(gpa)) {
    cyclic_free(gpaToLVA(gpa));
  }
  else {
    global_free(gpaToLVA(gpa));
  }

  hpx_lco_set(sync, 0, NULL, HPX_NULL, HPX_NULL);
}

hpx_addr_t
PGAS::alloc_cyclic(size_t n, size_t bsize, uint32_t boundary, uint32_t attr)
{
  uint64_t offset;
  int zero = 0;
  hpx_call_sync(HPX_THERE(0), AllocateCyclicBlocks, &offset, sizeof(offset), &n,
                &bsize, &boundary, &attr, &zero);
  return offsetToGPA(offset);
}

hpx_addr_t
PGAS::calloc_cyclic(size_t n, size_t bsize, uint32_t boundary, uint32_t attr)
{
  uint64_t offset;
  int zero = 1;
  hpx_call_sync(HPX_THERE(0), AllocateCyclicBlocks, &offset, sizeof(offset), &n,
                &bsize, &boundary, &attr, &zero);
  return offsetToGPA(offset);
}

hpx_addr_t
PGAS::alloc_local(size_t n, size_t bsize, uint32_t boundary,
                  uint32_t attr) {
  return offsetToGPA(allocateBlocks(n, bsize, boundary, attr, false, false));
}

hpx_addr_t
PGAS::calloc_local(size_t n, size_t bsize, uint32_t boundary,
                   uint32_t attr) {
  return offsetToGPA(allocateBlocks(n, bsize, boundary, attr, true, false));
}

inline uint64_t
PGAS::allocateBlocks(size_t n, size_t bsize, uint32_t boundary,
                     uint32_t attr, bool zero, bool cyclic)
{
  uint64_t padded = uint64_t(1) << ceil_log2(bsize);
  dbg_assert_str(padded <= maxBlockSize(), "block size too large.\n");

  void* lva;
  if (cyclic && boundary) {
    lva = cyclic_memalign(boundary, n * padded);
  }
  else if (cyclic) {
    lva = cyclic_malloc(n * padded);
  }
  else if (boundary && zero) {
    lva = global_memalign(boundary, n * padded);
    std::memset(lva, 0, n * padded);
  }
  else if (boundary) {
    lva = global_memalign(boundary, n * padded);
  }
  else if (zero) {
    lva = global_calloc(n, padded);
  }
  else {
    lva = global_malloc(n * padded);
  }

  dbg_assert(lva);
  return lvaToOffset(lva);
}

void
PGAS::zeroBlocks(uint64_t offset, size_t n, size_t bsize)
{
  dbg_assert(isCyclicOffset(offset));
  uint64_t blocks = ceil_div(uint64_t(n), uint64_t(here->ranks));
  uint32_t align = ceil_log2(bsize);
  uint64_t padded = uint64_t(1) << align;
  std::memset(offsetToLVA(offset), 0, blocks * padded);
}

uint64_t
PGAS::allocateCyclicBlocks(size_t n, size_t bsize, uint32_t boundary,
                           uint32_t attr, int zero)
{
  dbg_assert(here->rank == 0);
  uint64_t blocks = ceil_div(uint64_t(n), uint64_t(here->ranks));
  uint64_t offset = allocateBlocks(blocks, bsize, boundary, attr, zero, true);
  uint64_t csbrk = getCsbrk();
  hpx_bcast_rsync(SetCsbrk, &csbrk);
  if (zero) {
    dbg_check(hpx_bcast_rsync(ZeroBlocks, &offset, &blocks, &bsize), "\n");
  }
  return offset;
}

void
PGAS::memcpy(hpx_addr_t to, hpx_addr_t from, size_t n, hpx_addr_t sync)
{
  if (!n) {
    hpx_lco_error(sync, HPX_SUCCESS, HPX_NULL);
  }
  else if (gpa_to_rank(to) != here->rank) {
    here->net->memcpy(to, from, n, sync);
  }
  else if (gpa_to_rank(from) != here->rank) {
    here->net->memcpy(to, from, n, sync);
  }
  else {
    void *lto = gpaToLVA(to);
    void *lfrom = gpaToLVA(from);
    std::memcpy(lto, lfrom, n);
    hpx_lco_error(sync, HPX_SUCCESS, HPX_NULL);
  }
}

void
PGAS::memcpy(hpx_addr_t to, hpx_addr_t from, size_t n)
{
  if (n) {
    hpx_addr_t sync = hpx_lco_future_new(0);
    dbg_assert_str(sync, "could not allocate an LCO for memcpy_sync.\n");

    memcpy(to, from, n, sync);
    dbg_check(hpx_lco_wait(sync), "failed agas_memcpy_sync\n");
    hpx_lco_delete(sync, HPX_NULL);
  }
}

void
PGAS::memput(hpx_addr_t to, const void *from, size_t n, hpx_addr_t lsync,
             hpx_addr_t rsync)
{
  if (!n) {
    hpx_lco_error(lsync, HPX_SUCCESS, HPX_NULL);
    hpx_lco_error(rsync, HPX_SUCCESS, HPX_NULL);
  }
  else if (gpa_to_rank(to) == here->rank) {
    void *lto = gpaToLVA(to);
    std::memcpy(lto, from, n);
    hpx_lco_error(lsync, HPX_SUCCESS, HPX_NULL);
    hpx_lco_error(rsync, HPX_SUCCESS, HPX_NULL);
  }
  else {
    here->net->memput(to, from, n, lsync, rsync);
  }
}

void
PGAS::memput(hpx_addr_t to, const void *from, size_t n, hpx_addr_t rsync)
{
  if (!n) {
    hpx_lco_error(rsync, HPX_SUCCESS, HPX_NULL);
  }
  else if (gpa_to_rank(to) == here->rank) {
    void *lto = gpaToLVA(to);
    std::memcpy(lto, from, n);
    hpx_lco_set(rsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
  else {
    here->net->memput(to, from, n, rsync);
  }
}

void
PGAS::memput(hpx_addr_t to, const void *from, size_t n)
{
  if (n) {
    if (gpa_to_rank(to) == here->rank) {
      void *lto = gpaToLVA(to);
      std::memcpy(lto, from, n);
    }
    else {
      here->net->memput(to, from, n);
    }
  }
}

void
PGAS::memget(void *to, hpx_addr_t from, size_t n, hpx_addr_t lsync,
             hpx_addr_t rsync)
{
  if (!n) {
    hpx_lco_error(lsync, HPX_SUCCESS, HPX_NULL);
    hpx_lco_error(rsync, HPX_SUCCESS, HPX_NULL);
  }
  else if (gpa_to_rank(from) == here->rank) {
    void *lfrom = gpaToLVA(from);
    std::memcpy(to, lfrom, n);
    hpx_lco_error(lsync, HPX_SUCCESS, HPX_NULL);
    hpx_lco_error(rsync, HPX_SUCCESS, HPX_NULL);
  }
  else {
    here->net->memget(to, from, n, lsync, rsync);
  }
}

void
PGAS::memget(void *to, hpx_addr_t from, size_t n, hpx_addr_t lsync)
{
  if (!n) {
    hpx_lco_error(lsync, HPX_SUCCESS, HPX_NULL);
  }
  else if (gpa_to_rank(from) == here->rank) {
    const void *lfrom = gpaToLVA(from);
    std::memcpy(to, lfrom, n);
    hpx_lco_set(lsync, 0, NULL, HPX_NULL, HPX_NULL);
  }
  else {
    here->net->memget(to, from, n, lsync);
  }
}

void
PGAS::memget(void *to, hpx_addr_t from, size_t n)
{
  if (n) {
    if (gpa_to_rank(from) == here->rank) {
      const void *lfrom = gpaToLVA(from);
      std::memcpy(to, lfrom, n);
    }
    else {
      here->net->memget(to, from, n);
    }
  }
}
