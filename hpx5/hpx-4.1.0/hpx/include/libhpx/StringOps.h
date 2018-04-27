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

#ifndef LIBHPX_STRING_OPS_H
#define LIBHPX_STRING_OPS_H

/// The abstract class that provide the string operation interface.

#include "hpx/hpx.h"
#include <cstddef>

namespace libhpx {
class StringOps {
 public:
  virtual ~StringOps();

  /// The asynchronous memget operation.
  ///
  /// This operation will return before either the remote or the local operations
  /// have completed. The user may specify either an @p lsync or @p rsync LCO to
  /// detect the completion of the operations.
  ///
  /// @param         dest The local address to memget into.
  /// @param          src The global address we're memget-ing from
  /// @param            n The number of bytes to get.
  /// @param        lsync An LCO to set when @p to has been written.
  /// @param        rsync An LCO to set when @p src has been read.
  virtual void memget(void *dest, hpx_addr_t src, size_t n, hpx_addr_t lsync,
                      hpx_addr_t rsync) = 0;

  /// The rsync memget operation.
  ///
  /// This operation will not return until the remote read operation has
  /// completed. The @p lsync LCO will be set once the local write operation has
  /// completed.
  ///
  /// @param         dest The local address to memget into.
  /// @param          src The global address we're memget-ing from
  /// @param            n The number of bytes to get.
  /// @param        lsync An LCO to set when @p to has been written.
  virtual void memget(void *dest, hpx_addr_t src, size_t n,
                      hpx_addr_t lsync) = 0;

  /// The rsync memget operation.
  ///
  /// This operation will not return until the @p to buffer has been written,
  /// which also implies that the remote read has completed.
  ///
  /// @param         dest The local address to memget into.
  /// @param          src The global address we're memget-ing from
  /// @param            n The number of bytes to get.
  virtual void memget(void *dest, hpx_addr_t src, size_t n) = 0;

  /// The asynchronous memput operation.
  ///
  /// The @p lsync LCO will be set when it is safe to reuse or free the @p from
  /// buffer. The @p rsync LCO will be set when the remote buffer has been
  /// written.
  ///
  /// @param         dest The global address to put into.
  /// @param          src The local address we're putting from.
  /// @param            n The number of bytes to put.
  /// @param        lsync An LCO to set when @p src has been read.
  /// @param        rsync An LCO to set when @p to has been written.
  virtual void memput(hpx_addr_t dest, const void *src, size_t n,
                      hpx_addr_t lsync, hpx_addr_t rsync) = 0;

  /// The locally synchronous memput operation.
  ///
  /// This will not return until it is safe to modify or free the @p from
  /// buffer. The @p rsync LCO will be set when the remote buffer has been
  /// written.
  ///
  /// @param         dest The global address to put into.
  /// @param          src The local address we're putting from.
  /// @param            n The number of bytes to put.
  /// @param        rsync An LCO to set when @p to has been written.
  virtual void memput(hpx_addr_t dest, const void *src, size_t n,
                      hpx_addr_t rsync) = 0;

  /// The fully synchronous memput operation.
  ///
  /// This will not return until the buffer has been written and is visible at the
  /// remote size.
  ///
  /// @param         dest The global address to put into.
  /// @param          src The local address we're putting from.
  /// @param            n The number of bytes to put.
  virtual void memput(hpx_addr_t dest, const void *src, size_t n) = 0;

  /// The asynchronous memcpy operation.
  ///
  /// This will return immediately, and set the @p sync lco when the operation has
  /// completed.
  ///
  /// @param         dest The global address to write into.
  /// @param          src The global address to read (const).
  /// @param            n The number of bytes to write.
  /// @param         sync An optional LCO to signal remote completion.
  virtual void memcpy(hpx_addr_t dest, hpx_addr_t src, size_t n,
                      hpx_addr_t sync) = 0;

  /// The asynchronous memcpy operation.
  ///
  /// This will not return until the operation has completed.
  ///
  /// @param         dest The global address to write into.
  /// @param          src The global address to read (const).
  /// @param            n The number of bytes to write.
  virtual void memcpy(hpx_addr_t dest, hpx_addr_t src, size_t n) = 0;
};
} // namespace libhpx

#endif // LIBHPX_STRING_OPS_H
