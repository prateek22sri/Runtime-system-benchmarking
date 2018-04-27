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

#ifndef LIBHPX_UTIL_LRU_CACHE_H
#define LIBHPX_UTIL_LRU_CACHE_H

#include <cassert>
#include <cstddef>
#include <mutex>

namespace libhpx {
namespace util {

class LRUCache {
 public:
  LRUCache(unsigned capacity) : lock_(), capacity_(capacity), size_(), chunks_()
  {
  }

  template <typename Lambda>
  void* get(size_t bytes, Lambda&& lambda) {
    std::lock_guard<std::mutex> _(lock_);
    for (Node* n = chunks_.next; n != &chunks_; n = n->next) {
      if (n->bytes == bytes) {
        --size_;
        return n->remove();
      }
    }
    return lambda();
  }

  template <typename Lambda>
  void put(void* obj, size_t bytes, Lambda&& lambda) {
    assert(bytes > sizeof(Node));
    std::lock_guard<std::mutex> _(lock_);
    chunks_.next = new(obj) Node(bytes, &chunks_);
    if (size_++ == capacity_) {
      Node* n = chunks_.prev->remove();
      lambda(n, n->bytes);
      --size_;
    }
  }

 private:
  struct Node {
    Node() : bytes(0), prev(this), next(this) {
    }

    Node(size_t b, Node* p) : bytes(b), prev(p), next(prev->next) {
      next->prev = this;
      prev->next = this;
    }

    Node* remove() {
      prev->next = next;
      next->prev = prev;
      return this;
    }

    size_t bytes;
    Node* prev;
    Node* next;
  };

  std::mutex lock_;
  const unsigned capacity_;
  unsigned size_;
  Node chunks_;
};

} // namespace util
} // namespace libhpx

#endif // LIBHPX_UTIL_LRU_CACHE_H
