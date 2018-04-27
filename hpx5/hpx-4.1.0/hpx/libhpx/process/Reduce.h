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

#ifndef LIBHPX_PROCESS_REDUCE_H
#define LIBHPX_PROCESS_REDUCE_H

#include "hpx/hpx.h"
#include <atomic>

namespace libhpx {
namespace process {
class Reduce {
 public:
  Reduce(size_t bytes, hpx_monoid_id_t id, hpx_monoid_op_t op);
  ~Reduce();

  static void* operator new(size_t, size_t bytes);
  static void operator delete(void* ptr);

  bool add();
  bool remove();
  bool join(const void *in);

  void reset(void *out);

 private:
  const size_t bytes_;
  const size_t padded_;
  const hpx_monoid_id_t id_;
  const hpx_monoid_op_t op_;
  const int slots_;
  std::atomic<int> i_;
  int n_;
  alignas(HPX_CACHELINE_SIZE) char values_[];
};

} // namespace process
} // namespace libhpx

#endif // LIBHPX_PROCESS_REDUCE_H
