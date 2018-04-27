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

#ifndef LIBHPX_NETWORK_ISIR_IRECV_BUFFER_H
#define LIBHPX_NETWORK_ISIR_IRECV_BUFFER_H

#include "MPITransport.h"
#include "libhpx/config.h"
#include "libhpx/parcel.h"

namespace libhpx {
namespace network {
namespace isir {

class IRecvBuffer {
 public:
  using Transport = libhpx::network::isir::MPITransport;
  using Status = libhpx::network::isir::MPITransport::Status;

  IRecvBuffer(const config_t *config, Transport &xport);
  ~IRecvBuffer();

  int progress(hpx_parcel_t** recvs);

 private:
  using Request = libhpx::network::isir::MPITransport::Request;
  struct Record {
    int tag;
    hpx_parcel_t *p;
  };

  /// Resize an irecv buffer to the requested size.
  ///
  /// Buffer sizes can either be increased or decreased. The increase in size is
  /// saturated by the limit_, unless the limit is 0 in which case unbounded
  /// growth is permitted. A decrease in size could cancel active irecvs, which
  /// may in turn match real sends, so this routine returns successfully
  /// received parcels.
  ///
  /// @note Increasing the size of the buffer will never generate any returned
  ///       parcels.
  ///
  /// @param         size The new size for the buffer.
  /// @returns            A stack of parcels corresponding to completed
  ///                     requests that are canceled.
  hpx_parcel_t* reserve(unsigned capacity);

  /// Start an irecv operation.
  ///
  /// This is used both to start new irecvs, and to reincarnate irecvs during
  /// normal operation.
  void start(unsigned i);

  /// Probe for unexpected messages.
  ///
  /// We do not have prior knowledge of the message sizes that we will receive. In
  /// order to detect messages that have not yet matched an active irecv, we use
  /// the MPI_Iprobe operation.
  ///
  /// The side effect of this operation is to start a new irecv operation.
  void probe();

  /// Cancel an irecv.
  ///
  /// It might not be possible to cancel an irecv because it has already
  /// completed. In this case, this operation behaves like a finish() without
  /// regenerating the irecv, i.e., we'll return the received parcel. If the
  /// request is canceled successfully then we delete the posted buffer.
  ///
  /// @param
  hpx_parcel_t* cancel(unsigned i);

  /// Finish an irecv.
  ///
  /// This will use the status to finish up the irecv, turning it into a usable
  /// parcel.
  hpx_parcel_t* finish(unsigned i, const Status& status);

  Transport              &xport_;
  const unsigned          limit_;
  unsigned             capacity_;
  unsigned                 size_;
  Request* requests_;
  Record*   records_;
};

} // namespace isir
} // namespace network
} // namespace libhpx

#endif // LIBHPX_NETWORK_ISIR_IRECV_BUFFER_H
