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

#ifndef LIBHPX_COLLECTIVE_OPS_H
#define LIBHPX_COLLECTIVE_OPS_H

/// The abstract base class for a provider for COLLECTIVE operations.

#include <cstddef>

namespace libhpx {
class CollectiveOps {
 public:
  virtual ~CollectiveOps();
  virtual int init(void **collective) = 0;
  virtual int sync(void *in, size_t in_size, void* out, void *collective) = 0;
};
}

#endif // LIBHPX_COLLECTIVE_OPS_H
