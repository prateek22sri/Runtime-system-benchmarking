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

#ifndef LIBHPX_MEMORY_OPS_H
#define LIBHPX_MEMORY_OPS_H

/// The abstract base class for a provider for memory operations.

namespace libhpx {
class MemoryOps {
 public:
  virtual ~MemoryOps();

  /// Register a memory region for dma access.
  ///
  /// Network registration is a limited resource. Currently, we handle
  /// registration failures as unrecoverable. In the future it will make sense
  /// to implement a registration cache or other mechanism for resource
  /// management.
  ///
  /// @param         base The beginning of the region to register.
  /// @param        bytes The number of bytes to register.
  /// @param          key The key to use when registering dma.
  virtual void pin(const void *base, size_t bytes, void *key) = 0;

  /// Release a registered memory region.
  ///
  /// The region denotated by @p segment, @p bytes must correspond to a region
  /// previously registered.
  ///
  /// @param         base The beginning of the region to release.
  /// @param        bytes The number of bytes to release.
  virtual void unpin(const void *base, size_t bytes) = 0;
};
}

#endif // LIBHPX_MEMORY_OPS_H
