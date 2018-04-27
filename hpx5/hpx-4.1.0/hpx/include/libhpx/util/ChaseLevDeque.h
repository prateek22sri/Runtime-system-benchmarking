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

#ifndef LIBHPX_UTIL_CHASE_LEV_DEQUE_H
#define LIBHPX_UTIL_CHASE_LEV_DEQUE_H

#include "libhpx/util/Aligned.h"             // template Align
#include "libhpx/util/math.h"                // ceil2
#include "hpx/hpx.h"                         // HPX_CACHELINE_SIZE
#include <cstdint>                           // uint64_t
#include <atomic>

namespace libhpx {
namespace util {

/// A chase-lev workstealing deque.
///
/// The workstealing deque provides push and pop operations that must be called
/// serially, along with a steal operation that may be called concurrently with
/// either push or pop.
///
/// http://dl.acm.org/citation.cfm?id=1073974
template <typename T>
class ChaseLevDeque;

template <typename T>
class ChaseLevDeque<T*> : public Aligned<HPX_CACHELINE_SIZE>
{
  // even on 32 bit platforms we want 64 bit index (to avoid overflow)
  using Index = std::uint64_t;

  static constexpr auto RELAXED = std::memory_order_relaxed;
  static constexpr auto ACQUIRE = std::memory_order_acquire;
  static constexpr auto RELEASE = std::memory_order_release;
  static constexpr auto SEQ_CST = std::memory_order_seq_cst;

 public:
  ChaseLevDeque(unsigned capacity)
      : bottom_(1),
        topBound_(1),
        capacity_(ceil2(capacity)),
        buffer_(new(capacity_) Buffer(nullptr, capacity_)),
        top_(1) {
  }

  ChaseLevDeque() : ChaseLevDeque(32u) {
  }

  ~ChaseLevDeque() {
    delete buffer_.load();
  }

  /// Get an approximate size for the deque.
  ///
  /// This is low cost but ignores the potential that steals might interpose
  /// between our read of the bottom and top indices.
  size_t size() const {
    auto bottom = bottom_.load(RELAXED);
    auto top = top_.load(RELAXED);
    return bottom - top;
  }

  /// Pop an item from the deque.
  T* pop() {
    auto bottom = bottom_.fetch_sub(1, SEQ_CST) - 1;
    topBound_ = top_.load(ACQUIRE);  // update bound

    // if the queue was empty, then we overshot (canonicalize empty)
    if (bottom < topBound_) {
      bottom_.store(topBound_, RELEASE);
      return nullptr;
    }

    // read the value
    T* value = buffer_.load(RELAXED)->get(bottom);
    if (bottom > topBound_) {
      return value;
    }

    // We're popping the last element, need to race with concurrent steal()s on
    // top.
    if (!tryIncTop(topBound_)) {
      value = nullptr;
    }

    // Either way, canonicalize the list after the CAS.
    bottom_.store(topBound_ + 1, RELEASE);
    return value;
  }

  /// Pop an item from the bottom of the deque.
  T* steal() {
    auto top = top_.load(ACQUIRE);
    auto bottom = bottom_.load(ACQUIRE);

    // if the dueque seems empty, fail
    if (bottom <= top) {
      return nullptr;
    }

    // Read the buffer and the value. Have to read the value before the CAS,
    // otherwise we could miss some push-pops and get the wrong value due to the
    // underlying cyclic array (see Chase-Lev 2.2).
    //
    // NB: it doesn't matter if the buffer grows a number of times between these
    //     two operations, because _buffer_get(top) will always return the same
    //     value---this is a result of the magic and beauty of this
    //     algorithm. If we want to shrink the buffer then we'll have to pay
    //     more attention.
    T* value = buffer_.load(ACQUIRE)->get(top);

    // if we update the top, return the stolen value, otherwise retry
    return (tryIncTop(top)) ? value : nullptr;
  }

  /// Push an item into the deque.
  size_t push(T* value) {
    // read bottom and buffer, using Chase-Lev 2.3 for top upper bound
    auto bottom = bottom_.load(RELAXED);

    // If the deque seems to be full then update its top bound, if the deque is
    // *really* full then expand its capacity---no underflow potential here
    // because pop() and steal() leave the queue canonical.
    if (bottom - topBound_ >= capacity_) {
      topBound_ = top_.load(ACQUIRE);
      if (bottom - topBound_ >= capacity_) {
        grow(bottom, topBound_);
      }
    }

    buffer_.load(RELAXED)->set(bottom++, value);
    bottom_.store(bottom, RELEASE);
    return bottom - topBound_;
  }

 private:
  class Buffer {
   public:
    Buffer(Buffer* parent, unsigned capacity)
        : parent_(parent), mask_(capacity - 1) {
      assert(ceil2(capacity) == capacity);
    }

    ~Buffer() {
      delete parent_;
    }

    static void* operator new(size_t bytes, uint32_t capacity) {
      return new char[bytes + capacity * sizeof(T*)];
    }

    static void operator delete(void* ptr) {
      delete [] reinterpret_cast<char*>(ptr);
    }

    void set(Index i, T* value) {
      buffer_[i & mask_] = value;
    }

    T* get(Index i) {
      return buffer_[i & mask_];
    }

   private:
    Buffer* const parent_;
    const Index     mask_;
    T*            buffer_[];
  };

  [[ gnu::noinline ]] void grow(const Index bottom, const Index top) {
    capacity_ = capacity_ * 2;
    Buffer* old = buffer_.load(RELAXED);
    Buffer* buffer = new(capacity_) Buffer(old, capacity_);
    for (auto i = top, e = bottom; i < e; ++i) {
      buffer->set(i, old->get(i));
    }
    buffer_.store(buffer, RELEASE);
  }

  bool tryIncTop(Index top) {
    return top_.compare_exchange_weak(top, top + 1, RELEASE, RELAXED);
  }

  std::atomic<Index>   bottom_;
  Index              topBound_;
  unsigned           capacity_;
  std::atomic<Buffer*> buffer_;

  alignas(HPX_CACHELINE_SIZE)
  std::atomic<Index>      top_;
};

} // namespace util
} // namespace libhpx

#endif // LIBHPX_UTIL_CHASE_LEV_DEQUE_H
