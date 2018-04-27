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
#include "config.h"
#endif

/// @file libhpx/scheduler/future.c
/// Defines the future structure.

#include "LCO.h"
#include "Condition.h"
#include "hpx/builtins.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include <cstring>
#include <mutex>
#include <new>

namespace {
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;

class Future final : public LCO
{
 public:
  Future(size_t size);

  ~Future() {
    lock();                                     // release in ~LCO()
  }

  int set(size_t size, const void *value);
  hpx_status_t get(size_t size, void *value, int reset);
  hpx_status_t getRef(size_t size, void **out, int *unpin);
  bool release(void *out);
  hpx_status_t attach(hpx_parcel_t *p);

  void error(hpx_status_t code) {
    std::lock_guard<LCO> _(*this);
    setTriggered();
    full_.signalError(code);
  }

  hpx_status_t wait(int reset) {
    return get(0, NULL, reset);
  }

  void reset() {
    std::lock_guard<LCO> _(*this);
    resetFull();
  }
  size_t size(size_t bytes) const {
    return sizeof(Future) + bytes;
  }

 public:
  /// Static action interface.
  /// @{
  static int NewHandler(void* buffer, size_t size) {
    auto lco = new(buffer) Future(size);
    return HPX_THREAD_CONTINUE(lco);
  }

  static int NewBlockHandler(char* blocks, unsigned n, size_t size);
  /// @}

 private:
  /// Reset the full condition.
  ///
  /// This must be called while holding the LCO lock.
  void resetFull() {
    log_lco("resetting future %p\n", (void*)this);
    resetTriggered();
    full_.reset();
  }

  /// Wait until the full condition is true.
  ///
  /// This must be called while holding the LCO lock.
  hpx_status_t waitFull() {
    return (getTriggered()) ? full_.getError() : waitFor(full_);
  }

  Condition full_;
  char   value_[];
};

LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, New, Future::NewHandler, HPX_POINTER,
              HPX_SIZE_T);
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, NewBlock, Future::NewBlockHandler,
              HPX_POINTER, HPX_UINT, HPX_SIZE_T);
}

Future::Future(size_t size)
    : LCO(LCO_FUTURE),
      full_()
{
  log_lco("initializing future %p\n", (void*)this);
  if (size) {
    if (DEBUG) {
      setUser();
    }
    memset(value_, 0, size);
  }
}

/// Copies @p from into the appropriate location.
int
Future::set(size_t size, const void *from)
{
  std::lock_guard<LCO> _(*this);

  DEBUG_IF (size && !getUser()) {
    dbg_error("setting 0-sized future with %zu bytes\n", size);
  }

  log_lco("setting future %p\n", (void*)this);
  // futures are write-once
  if (setTriggered()) {
    dbg_error("cannot set an already set future\n");
    return 0;
  }

  if (from && size) {
    memcpy(value_, from, size);
  }

  full_.signalAll();
  return 1;
}

/// Copies the appropriate value into @p out, waiting if the lco isn't set yet.
hpx_status_t
Future::get(size_t size, void *out, int reset) {
  std::lock_guard<LCO> _(*this);

  DEBUG_IF (size && !getUser()) {
    dbg_error("getting %zu bytes from a 0-sized future\n", size);
  }

  log_lco("getting future %p (%zu bytes)\n", (void*)this, size);

  if (hpx_status_t status = waitFull()) {
    return status;
  }

  if (size && out) {
    memcpy(out, &value_, size);
  }
  else {
    dbg_assert(!size && !out);
  }

  if (reset) {
    resetFull();
  }

  return HPX_SUCCESS;
}

/// Returns the reference to the future's value in @p out, waiting if the lco
/// isn't set yet.
hpx_status_t
Future::getRef(size_t size, void **out, int *unpin)
{
  dbg_assert(size && out);

  if (hpx_status_t status = wait(0)) {
    return status;
  }

  // no need for a lock here, synchronization happened in _wait(), and the LCO
  // is pinned externally
  *out = value_;
  *unpin = 0;
  return HPX_SUCCESS;
}

// Release a reference to the buffer. There is no such thing as a "release
// remote reference", the caller knows that if the LCO is not local then it has
// a temporary buffer---that code path doesn't make it here. Just return '1' to
// indicate that the caller should unpin the LCO.
bool
Future::release(void *out)
{
  dbg_assert(out && out == value_);
  return true;
}

hpx_status_t
Future::attach(hpx_parcel_t *p)
{
  std::lock_guard<LCO> _(*this);

  if (!getTriggered()) {
    return full_.push(p);
  }

  // If we have an error condition, then we release the parcel and return the
  // error.
  // NB: should we actually send some sort of error condition?
  if (hpx_status_t status = full_.getError()) {
    hpx_parcel_release(p);
    return status;
  }

  hpx_parcel_send(p, HPX_NULL);
  return HPX_SUCCESS;
}

int
Future::NewBlockHandler(char* blocks, unsigned n, size_t size)
{
  hpx_addr_t gva = hpx_thread_current_target();
  size_t bytes = sizeof(Future) + size;
  char *i = blocks;
  char *e = blocks + n * bytes;
  while (i < e) {
    auto local = new(i) Future(size);
    LCO_LOG_NEW(gva, local);
    gva = hpx_addr_add(gva, bytes, bytes);
    i += bytes;
  }
  return HPX_SUCCESS;
}

hpx_addr_t
hpx_lco_future_new(int size)
{
  dbg_assert(size >= 0);
  hpx_addr_t gva = HPX_NULL;
  Future* lco = nullptr;
  try {
    lco = new(size, gva) Future(size);
    hpx_gas_unpin(gva);
  }
  catch (const LCO::NonLocalMemory&) {
    size_t n(size);
    hpx_call_sync(gva, New, &lco, sizeof(lco), &n);
  }
  LCO_LOG_NEW(gva, lco);
  return gva;
}

// Allocate a global array of futures.
hpx_addr_t
hpx_lco_future_array_new(int n, int size, int futures_per_block)
{
  dbg_assert(n > 0);
  dbg_assert(size >= 0);
  dbg_assert(futures_per_block > 0);

  // perform the global allocation
  const unsigned blocks = ceil_div_32(n, futures_per_block);
  const size_t future_bytes = sizeof(Future) + size;
  const size_t block_bytes = futures_per_block * future_bytes;
  const hpx_addr_t base = lco_alloc_cyclic(blocks, block_bytes, 0);

  // Cast some arguments that we need to pass to NewBlockHandler
  unsigned fpb = unsigned(futures_per_block);

  // for each block, initialize the future
  const hpx_addr_t bcast = hpx_lco_and_new(blocks);
  for (unsigned i = 0; i < blocks; ++i) {
    size_t unsignedSize(size);
    hpx_addr_t block = hpx_addr_add(base, i * block_bytes, block_bytes);
    int e = hpx_call(block, NewBlock, bcast, &fpb, &unsignedSize);
    dbg_check(e, "call of NewBlock failed\n");
  }
  dbg_check( hpx_lco_wait(bcast) );
  hpx_lco_delete_sync(bcast);

  // return the base address of the allocation
  return base;
}

// Application level programmer doesn't know how big the future is, so we
// provide this array indexer.
hpx_addr_t
hpx_lco_future_array_at(hpx_addr_t array, int i, int size, int bsize)
{
  dbg_assert(size >= 0);
  dbg_assert(bsize > 0);
  size_t future_bytes = sizeof(Future) + size;
  size_t  block_bytes = bsize * future_bytes;
  return hpx_addr_add(array, i * future_bytes, block_bytes);
}

/// Allocate an array of future local to the calling locality.
/// @param          n The (total) number of futures to allocate
/// @param       size The size of each future's value
///
/// @returns the global address of the allocated array lco.
hpx_addr_t
hpx_lco_future_local_array_new(int n, int size)
{
  dbg_assert(n > 0);
  dbg_assert(size > 0);
  size_t bytes = unsigned(size);
  size_t bsize = sizeof(Future) + bytes;
  hpx_addr_t base = lco_alloc_local(n, bsize, 0);
  if (!base) {
    throw std::bad_alloc();
  }

  unsigned one = 1;
  hpx_addr_t bcast = hpx_lco_and_new(n);
  for (int i = 0, e = n; i < e; ++i) {
    hpx_addr_t addr = hpx_addr_add(base, i * bsize, bsize);
    dbg_check( hpx_call(addr, NewBlock, bcast, &one, &bytes) );
  }
  hpx_lco_wait(bcast);
  hpx_lco_delete_sync(bcast);
  return base;
}
