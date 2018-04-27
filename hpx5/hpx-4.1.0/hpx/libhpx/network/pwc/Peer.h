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

#ifndef LIBHPX_NETWORK_PWC_PEER_H
#define LIBHPX_NETWORK_PWC_PEER_H

#include "CircularBuffer.h"
#include "ParcelBlock.h"
#include "PhotonTransport.h"
#include "libhpx/config.h"
#include "libhpx/padding.h"
#include "libhpx/parcel.h"
#include <memory>
#include <mutex>

namespace libhpx {
namespace network {
namespace pwc {

/// An rdma-able remote address.
///
/// We use an array of these remote addresses to keep track of the
/// backpointers that we need during the reload operation. During reload we
/// allocate a new InplaceBuffer and then rdma its data back to the remote
/// address that we have stored for the requesting rank.
template <typename T>
struct Remote {
  T                  *addr;
  PhotonTransport::Key key;
};

class Peer {
 public:
  Peer();
  ~Peer();

  /// Make sure that arrays of peers come from aligned, registered memory.
  static void* operator new[](size_t bytes);
  static void operator delete[](void* peer);

  /// Initialize the Peer data.
  ///
  /// @param       rank The rank this Peer data represents.
  /// @param       here The rank of this locality.
  /// @param     remote The remote buffer for the Peer data at @p rank.
  /// @param       heap The remote heap segment at @p rank.
  void init(unsigned rank, unsigned here, const Remote<Peer>& remote,
            const Remote<char>& heap);

  /// Perform progress on this point-to-point link.
  void progress();

  /// Send the designated parcel.
  void send(const hpx_parcel_t* p);

  /// Reload the eager buffer.
  void reload(size_t n, size_t eagerSize);

  /// Initiate an rDMA put operation with a remote continuation to the heap.
  ///
  /// This will copy @p n bytes between the @p lca and the @p to buffer, running
  /// the @p lcmd when the local buffer can be modified or deleted and the @p rcmd
  /// when the remote write has completed.
  ///
  /// @param           to The global target for the put.
  /// @param          lva The local source for the put.
  /// @param            n The number of bytes to put.
  /// @param         lcmd The local command, run when @p lva can be reused.
  /// @param         rcmd The remote command, run when @p to has be written.
  void put(hpx_addr_t to, const void *lva, size_t n, const Command& lcmd,
           const Command& rcmd);

  /// Initiate an rDMA get operation.
  ///
  /// This will copy @p n bytes between the @p from buffer and the @p lva, running
  /// the @p rcmd when the read completes remotely and running the @p lcmd when
  /// the local write is complete.
  ///
  /// @param          lva The local target for the get.
  /// @param         from The global source for the get.
  /// @param            n The number of bytes to get.
  /// @param         lcmd A local command, run when @p lva is written.
  /// @param         rcmd A remote command, run when @p from has been read.
  void get(void *lva, hpx_addr_t from, size_t n, const Command& lcmd,
           const Command& rcmd);

 private:
  typedef CircularBuffer<const hpx_parcel_t*> SendBuffer;
  typedef Remote<char> HeapSegment;
  typedef Remote<EagerBlock> SendRemote;

  /// Append a parcel send operation to the sendBuffer.
  void append(const hpx_parcel_t *p);

  /// Start a parcel send operation.
  int start(const hpx_parcel_t* p);

  std::mutex         lock_;  //<! Synchronizes send and progress.
  unsigned           rank_;  //<! The rank for this peer.
  HeapSegment heapSegment_;  //<! The heap segment address.
  SendRemote   remoteSend_;  //<! The send header this peer uses on sends to me.
  EagerBlock    sendEager_;  //<! The send header for this peer.
  SendBuffer   sendBuffer_;  //<! The send buffer for parcels during reload.
  InplaceBlock*      recv_;  //<! The receive block for parcels from this peer.
  const char      padding_[util::PadToCacheline(sizeof(lock_) + sizeof(rank_) +
                                                sizeof(heapSegment_) +
                                                sizeof(sendEager_) +
                                                sizeof(sendBuffer_) +
                                                sizeof(recv_))];
};

} // namespace pwc
} // namespace network
} // namespace libhpx

#endif // LIBHPX_NETWORK_PWC_PEER_H
