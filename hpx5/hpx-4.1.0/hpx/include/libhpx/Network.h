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

#ifndef LIBHPX_NETWORK_CXX_H
#define LIBHPX_NETWORK_CXX_H

#include "libhpx/CollectiveOps.h"
#include "libhpx/config.h"
#include "libhpx/GAS.h"
#include "libhpx/LCOOps.h"
#include "libhpx/MemoryOps.h"
#include "libhpx/ParcelOps.h"
#include "libhpx/StringOps.h"
#include "libhpx/boot/Network.h"

namespace libhpx {

class Network : public virtual StringOps, public CollectiveOps, public LCOOps,
                public MemoryOps, public ParcelOps {
 public:
  virtual ~Network();

  /// Create a new network.
  ///
  /// @param          cfg The current configuration.
  /// @param         boot The bootstrap network object.
  /// @param          gas The global address space.
  ///
  /// @returns            The network object, or NULL if there was an issue.
static Network* Create(config_t *config, const boot::Network& boot, GAS *gas);

  /// Get the HPX configuration type of the base network implementation.
  virtual int type() const = 0;

  /// Perform one network progress operation.
  ///
  /// @param       n The id to use when progressing the network.
  virtual void progress(int n) = 0;

  /// Flush the network.
  virtual void flush() = 0;

  /// Probe the network for received parcels.
  virtual hpx_parcel_t* probe(int rank) = 0;

 protected:
  Network();
};

} // namespace libhpx

#endif // LIBHPX_NETWORK_CXX_H
