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

#ifndef LIBHPX_UTIL_ALIGNED_H
#define LIBHPX_UTIL_ALIGNED_H

#include <new>                                  // std::bad_alloc
#include <cstdlib>                              // posix_memalign, free

namespace libhpx {
namespace util {

/// A simple template to provide alignment to a class.
///
/// Used through public inheritance.
template <size_t Align>
class alignas(Align) Aligned {
 public:
  static void* operator new(size_t bytes) {
    void* addr;
    if (posix_memalign(&addr, Align, bytes)) {
      throw std::bad_alloc();
    }
    return addr;
  }

  /// The matching delete operator for operator new().
  static void operator delete(void* worker) {
    // delete can be called with a null pointer, but free can't be
    if (worker) {
      free(worker);
    }
  }
};

} // namespace util
} // namespace libhpx

#endif // LIBHPX_UTIL_ALIGNED_H
