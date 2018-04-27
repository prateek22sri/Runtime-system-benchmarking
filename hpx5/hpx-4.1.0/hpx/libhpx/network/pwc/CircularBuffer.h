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

#ifndef LIBHPX_NETWORK_PWC_CIRCULAR_BUFFER_H
#define LIBHPX_NETWORK_PWC_CIRCULAR_BUFFER_H

#include "libhpx/debug.h"
#include "libhpx/libhpx.h"
#include <cstdint>
#include <utility>

namespace libhpx {
namespace network {
namespace pwc {

class CircularBufferBase {
 public:
  /// Compute the number of elements in the buffer.
  unsigned size() const {
    uint64_t size = max_ - min_;
    dbg_assert_str(size < UINT32_MAX, "circular buffer size invalid\n");
    return (unsigned)size;
  }

 protected:
  /// Allocate a circular buffer.
  CircularBufferBase(unsigned esize, unsigned cap);

  /// Delete a circular buffer.
  ~CircularBufferBase();

  /// Append an element to the circular buffer.
  ///
  /// @param            b The buffer to append to.
  ///
  /// @returns       NULL There was an error appending to the buffer.
  ///            NON-NULL The address of the start of the next record in the
  ///                       buffer.
  void *append();

  /// Compute the index into the buffer for an abstract index.
  ///
  /// The size must be 2^k because we mask instead of %.
  ///
  /// @param            i The abstract index.
  /// @param     capacity The capacity.
  ///
  /// @returns             @p capacity = 2^k; @p i % @p capacity
  ///                      @p capacity != 2^k; undefined
  static unsigned getIndexOf(unsigned i, unsigned capacity);

  /// Compute the index into the buffer for an abstract value using the current
  /// capacity.
  ///
  /// @param            i The abstract index.
  unsigned getIndexOf(unsigned i) const;

  /// Compute the address of an element in the buffer.
  ///
  /// @param            i The element index.
  ///
  /// @returns            The address of the element in the buffer.
  void *getAddressOf(unsigned i) const;

  /// Reflow the elements in a buffer.
  ///
  /// After expanding a buffer, existing elements are likely to be in the wrong
  /// place, given the new size. This function will reflow the part of the buffer
  /// that is in the wrong place.
  ///
  /// @param  oldCapacity The size of the buffer prior to the expansion.
  void reflow(unsigned oldCapacity);

  /// Expand the capacity of a buffer.
  ///
  /// @param     capacity The new size for the buffer.
  void expand(unsigned capacity);

  unsigned    capacity_;
  unsigned elementSize_;
  uint64_t         max_;
  uint64_t         min_;
  void        *records_;
};

template <typename Record>
class CircularBuffer : public CircularBufferBase
{
 public:
  CircularBuffer(unsigned n) : CircularBufferBase(sizeof(Record), n) {
  }

  ~CircularBuffer() {
  }

  void push(Record&& record) {
    Record* r = static_cast<Record*>(append());
    *r = std::move(record);
  }

  void push(const Record& record) {
    Record* r = static_cast<Record*>(append());
    *r = record;
  }

  template <typename F>
  int progress(F f) {
    while (min_ < max_) {
      void *record = getAddressOf(getIndexOf(min_));
      int e = f(*reinterpret_cast<Record*>(record));
      switch (e) {
       default:
        dbg_error("circular buffer could not progress\n");

       case LIBHPX_RETRY:
        return size();

       case LIBHPX_OK:
        ++min_;
        continue;
      }
    }
    return 0;
  }
};

} // namespace pwc
} // namespace network
} // namespace libhpx

#endif // LIBHPX_NETWORK_PWC_CIRCULAR_BUFFER_H
