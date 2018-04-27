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

#ifndef LIBHPX_PARCEL_OPS_H
#define LIBHPX_PARCEL_OPS_H

/// The abstract base class for a provider for parcel operations.

namespace libhpx {
class ParcelOps {
 public:
  virtual ~ParcelOps();

  /// Initiate a parcel send over the network.
  ///
  /// This is an asynchronous interface, the parcel will be sent at some point in
  /// the future, and is guaranteed to be delivered. Delivery order is not
  /// guaranteed. This transfers ownership of the parcel @p p, the client should
  /// not access the sent parcel again---in particular, the client should not
  /// release the parcel.
  ///
  /// This call will block if there are not enough resources available to satisfy
  /// the enqueue. It may progress the network as well, though that behavior is
  /// not guaranteed.
  ///
  /// @todo There is currently no way to test for send completion. We should add a
  ///       future parameter so that the sender can wait if necessary.
  ///
  /// @param            p The parcel to send.
  /// @param        ssync The local synchronization continuation.
  ///
  /// @returns  LIBHPX_OK The send was buffered successfully
  virtual int send(hpx_parcel_t* p, hpx_parcel_t* ssync) = 0;

  virtual void deallocate(const hpx_parcel_t* p) = 0;
};
}

#endif // LIBHPX_PARCEL_OPS_H
