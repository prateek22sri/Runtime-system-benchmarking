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

#ifndef LIBHPX_LCO_OPS_H
#define LIBHPX_LCO_OPS_H

/// The abstract base class for a provider for LCO operations.
#include "hpx/hpx.h"

namespace libhpx {
class LCOOps {
 public:
  virtual ~LCOOps();

  /// Perform an LCO wait operation through the PWC network.
  ///
  /// This performs a wait operation on a remote LCO. It allows the calling thread
  /// to wait without an intermediate "proxy" future allocation.
  ///
  /// @param          lco The global address of the LCO to wait for.
  /// @param        reset True if the wait should also reset the LCO.
  ///
  /// @returns            The status set in the LCO.
  virtual int wait(hpx_addr_t lco, int reset) = 0;

  /// Perform an LCO get operation through the PWC network.
  ///
  /// This performs an LCO get operation on a remote LCO. It allows the calling
  /// thread to get an LCO value without having to allocate an intermediate
  /// "proxy" future and its associated redundant copies.
  ///
  /// @param          lco The global address of the LCO to wait for.
  /// @param            n The size of the output buffer, in bytes.
  /// @param          out The output buffer---must be from registered memory.
  /// @param        reset True if the wait should also reset the LCO.
  ///
  /// @returns            The status set in the LCO.
  virtual int get(hpx_addr_t lco, size_t n, void *to, int reset) = 0;
};
}

#endif // LIBHPX_LCO_OPS_H
