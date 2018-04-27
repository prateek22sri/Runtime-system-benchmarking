// ==================================================================-*- C++ -*-
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifndef LIBHPX_UTIL_TWO_LOCK_QUEUE_H
#define LIBHPX_UTIL_TWO_LOCK_QUEUE_H

#include "libhpx/util/Aligned.h"             // template Align
#include "hpx/hpx.h"                         // HPX_CACHELINE_SIZE, hpx_parcel_t
#include <mutex>

namespace libhpx {
namespace util {
template <typename T>
class TwoLockQueue;

/// M&S two lock queue from
/// http://www.cs.rochester.edu/u/scott/papers/1996_PODC_queues.pdf, implemented
/// based on
/// https://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html#tlq
template <typename T>
class TwoLockQueue<T*> : public Aligned<HPX_CACHELINE_SIZE>
{
  struct Node {
    Node() : next(nullptr), value(nullptr) {
    }

    Node(T* val) : next(nullptr), value(val) {
    }

    Node* next;
    T* value;
  };

 public:
  TwoLockQueue() : headLock_(), head_(new Node()), tailLock_(), tail_(head_) {
  }

  ~TwoLockQueue() {
    while (Node* node = dequeueNode()) {
      delete node;
    }
  }

  T* dequeue() {
    T* value = nullptr;
    if (Node* node = dequeueNode()) {
      value = node->value;
      delete node;
    }
    return value;
  }

  void enqueue(T* t) {
    enqueueNode(new Node(t));
  }

 private:
  Node* dequeueNode() {
    std::lock_guard<std::mutex> _(headLock_);
    Node* head = head_;
    Node* next = head->next;
    if (!next) {
      return nullptr;
    }

    head->value = next->value;
    head_ = next;
    return head;
  }

  void enqueueNode(Node* node) {
    std::lock_guard<std::mutex> _(tailLock_);
    tail_->next = node;
    tail_ = node;
  }

  std::mutex headLock_;
  Node* head_;
  alignas(HPX_CACHELINE_SIZE) std::mutex tailLock_;
  Node* tail_;
};

} // namespace util
} // namespace libhpx

#endif // LIBHPX_UTIL_TWO_LOCK_QUEUE_H
