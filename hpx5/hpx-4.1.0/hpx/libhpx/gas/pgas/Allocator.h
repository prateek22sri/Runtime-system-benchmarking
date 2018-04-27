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

#ifndef LIBHPX_GAS_PGAS_ALLOCATOR_H
#define LIBHPX_GAS_PGAS_ALLOCATOR_H

namespace libhpx {
namespace gas {
namespace pgas {
class GlobalAllocator {
 public:
  GlobalAllocator(int rank);
  ~GlobalAllocator();

 private:
  void* hpx143Fix_;
};

class CyclicAllocator {
 public:
  CyclicAllocator(int rank);
  ~CyclicAllocator();
};
} // namespace pgas
} // namespace gas
} // namespace libhpx

#endif // LIBHPX_GAS_PGAS_ALLOCATOR_H
