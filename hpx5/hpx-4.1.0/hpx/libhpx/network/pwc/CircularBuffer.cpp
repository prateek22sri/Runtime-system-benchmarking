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

#include "CircularBuffer.h"
#include "hpx/builtins.h"
#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace libhpx::network::pwc;

unsigned
CircularBufferBase::getIndexOf(unsigned i, unsigned capacity)
{
  DEBUG_IF(capacity != (1u << ceil_log2_32(capacity))) {
    dbg_error("capacity %u is not 2^k\n", capacity);
  }
  unsigned mask = capacity - 1;
  return (i & mask);
}

unsigned
CircularBufferBase::getIndexOf(unsigned i) const
{
  return getIndexOf(i, capacity_);
}

void *
CircularBufferBase::getAddressOf(unsigned i) const
{
  size_t bytes = i * elementSize_;
  return reinterpret_cast<char*>(records_) + bytes;
}

void
CircularBufferBase::reflow(unsigned oldCapacity)
{
  if (!oldCapacity) {
    return;
  }

  // Resizing the buffer changes where our index mapping is, we need to move
  // data around in the arrays. We do that by memcpy-ing either the prefix or
  // suffix of a wrapped buffer into the new position. After resizing the buffer
  // should never be wrapped.
  int oldMin = getIndexOf(min_, oldCapacity);
  int oldMax = getIndexOf(max_, oldCapacity);
  int prefix = (oldMin < oldMax) ? oldMax - oldMin : oldCapacity - oldMin;
  int suffix = (oldMin < oldMax) ? 0 : oldMax;

  int newMin = getIndexOf(min_);
  int newMax = getIndexOf(max_);

  // This code is slightly tricky. We only need to move one of the ranges,
  // either [oldMin, oldMin + prefix) or [0, suffix). We determine which range
  // we need to move by seeing if the min or max index is different in the new
  // buffer, and then copying the appropriate bytes of the requests and records
  // arrays to the right place in the new buffer.
  if (oldMin == newMin) {
    assert(oldMax != newMax);
    assert(oldMin + prefix == newMax - suffix);

    // copy the old suffix to the new location
    size_t bytes = suffix * elementSize_;
    void *to = getAddressOf(oldMin + prefix);
    const void *from = getAddressOf(0);
    memcpy(to, from, bytes);
  }
  else if (oldMax == newMax) {
    assert((prefix > 0) &&
           (INT32_MAX - newMin >= prefix) &&
           (unsigned(newMin + prefix) <= capacity_));

    // copy the prefix to the new location
    size_t bytes = prefix * elementSize_;
    void *to = getAddressOf(newMin);
    const void *from = getAddressOf(oldMin);
    memcpy(to, from, bytes);
  }
  else {
    log_error("unexpected shift\n");
    return;
  }

  log_net("reflowed a circular buffer from %u to %u\n", oldCapacity, capacity_);
}

void
CircularBufferBase::expand(unsigned capacity)
{
  assert(capacity != 0);

  if (capacity < capacity_) {
    log_error("cannot shrink a circular buffer\n");
    return;
  }

  if (capacity_ == capacity) {
    return;
  }

  // realloc a new records array
  unsigned oldCapacity = capacity_;
  capacity_ = capacity;
  if (!(records_ = realloc(records_, capacity_ * elementSize_))) {
    log_error("failed to resize a circular buffer (%u to %u)\n", oldCapacity, capacity_);
    return;
  }

  // the reallocated buffer has some of its elements in the wrong place, so we
  // need to reflow it
  reflow(oldCapacity);
}

CircularBufferBase::CircularBufferBase(unsigned elementSize, unsigned capacity)
    : capacity_(0), elementSize_(elementSize), max_(0),
      min_(0), records_(nullptr)
{
  capacity = 1 << ceil_log2_32(capacity);
  expand(capacity);
}

CircularBufferBase::~CircularBufferBase()
{
  if (records_) {
    free(records_);
  }
}

void *
CircularBufferBase::append()
{
  uint64_t next = max_++;
  if (size() >= capacity_) {
    expand(capacity_ * 2);
  }
  return getAddressOf(getIndexOf(next));
}
