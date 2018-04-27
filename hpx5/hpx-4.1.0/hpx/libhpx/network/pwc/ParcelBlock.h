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

#ifndef LIBHPX_NETWORK_PWC_PARCEL_BLOCK_H
#define LIBHPX_NETWORK_PWC_PARCEL_BLOCK_H

#include "PhotonTransport.h"
#include "libhpx/padding.h"
#include "libhpx/parcel.h"
#include <cstddef>
#include <atomic>

struct hpx_parcel;

namespace libhpx {
namespace network {
namespace pwc {

/// An eager block is a chunk of memory that provides parcel allocation.
///
/// Eager buffers are used in the PWC network to implement parcel send
/// operations using rdma operations. The full functionality for an eager buffer
/// requires two objects.
///
/// 1) The send-side metadata for the buffer. This metadata is contained in this
///    EagerBlock class, and consists of the data necessary to allocate parcels
///    and to initiate rdma put operations.
///
/// 2) The receive-side metadata and storage for the buffer. This metatdata
///    allows parcels to be concurrently deallocated from the block This data
///    and functionality is implemented in the InplaceBuffer.
///
/// We are sensitive to the amount of data stored in the EagerBlock because we
/// allocate N^2 blocks throughout the system. The EagerBlock is also the unit
/// of data exchange during reload, where the receiver allocates a new
/// InplaceBlock and copies the EagerBlock data back to the sender.
class EagerBlock {
 public:
  /// Allocate an empty eager block.
  ///
  /// This constructor is only useful (and used) during allocate of a send block
  /// array, where the EagerBlock data will ultimately be written back via
  /// rdma. The EagerBlock does not own the pointed-to buffer space and does not
  /// declare a destructor. It is designed to be inherited, but does not contain
  /// a virtual destructor or virtual methods, so clients should take care not
  /// to delete an object of subclass type through a pointer to the EagerBlock
  /// itself.
  ///
  /// @note The rationale for not declaring a virtual destructor is to minimize
  ///       the size of objects of the EagerBlock class, and to allow rdma
  ///       operations without overwriting runtime data (e.g., vtable
  ///       pointers).
  EagerBlock();

  /// Allocate an array of send blocks from registered memory.
  ///
  /// The current internal use of the EagerBlocks are as an array of send
  /// buffers, which are allocated by the ReloadParcelEmulator using the array
  /// allocator. This operator new overload will handle that allocation, making
  /// sure that the EagerBlock array comes from registered memory and is
  /// well-aligned.
  static void* operator new [](size_t bytes);

  /// Delete an array of send blocks.
  static void operator delete [](void* ptr);

  /// Put the parcel to the associated rank.
  ///
  /// We could save this rank but we are sensitive to the size of the
  /// structure. The ReloadParcelEmulator, which is our only client, knows the
  /// rank and can just give it to us for this operation.
  ///
  /// @param       rank The target rank.
  /// @param     buffer The serialized parcel to put.
  /// @param[in/out]  n The size of the buffer.
  ///
  /// @returns          True on success. If the put fails because there isn't
  ///                   enough space then the @p n parameter will be updated
  ///                   with the number of remaining bytes in the block.
  bool put(unsigned rank, const hpx_parcel_t* buffer, size_t& n);

 protected:
  /// Use the passed buffer as the block buffer.
  ///
  /// The @p buffer should point to a @p capacity sized buffer that has been
  /// allocate from registered memory.
  ///
  /// @param   capacity The size of the @p buffer.
  /// @param     buffer The base of the buffer.
  EagerBlock(size_t capacity, char* buffer);

  /// Allocate a buffer from the block.
  ///
  /// The returned buffer will be cacheline aligned. If there is not enough
  /// space left in the buffer this will return nullptr, and update the @p n
  /// parameter with the number of bytes remaining in the block.
  ///
  /// @param[in/out]  n The number of bytes to allocate.
  ///
  /// @returns          The cacheline-aligned buffer, or nullptr.
  void* allocate(size_t& n, int rank);

  const char* end_;                        //<! The end of the buffer
  char* next_;                             //<! The next pointer
  PhotonTransport::Key key_;               //<! The rdma key covering the buffer
};

/// An eager buffer where the buffer is allocated adjacent to the buffer.
///
/// This optimizes accesses between the EagerBlock metadata and the managed
/// buffer space. The only reason that this class inherits from the EagerBlock
/// is in order to facilitate the reload functionality where we copy the
/// EagerBlock metadata for an InplaceBlock back to the sender, such that the
/// sender can manage allocation.
class InplaceBlock final : public EagerBlock {
  /// Destruct the eager block.
  ///
  /// This is private to prevent accidental double frees. All block deletion
  /// occurs through the reference-counted deallocate() or shutdown-time
  /// finalize() routines.
  ~InplaceBlock();

 public:
  /// Construct a recv block with the given size.
  ///
  /// The size is the total number of bytes associated with the receive
  /// block. Some of this space is used for the block data itself.
  ///
  /// @param       size The total size of the Recv block.
  InplaceBlock(size_t size);

  /// Allocate registered memory for a new RecvBlock.
  ///
  /// This allocator deals with the variable-length array of bytes required, as
  /// well as the alignment requirements and the memory registration.
  ///
  /// @param      bytes sizeof(EagerBuffer) from C++ compiler.
  /// @param          n The size of the buffer.
  ///
  /// @returns          The allocated, registered memory for this buffer.
  static void* operator new(size_t bytes, size_t n);

  /// Return the block memory to the registered memory allocator.
  static void operator delete(void* block);

  /// Return a number of bytes to the block.
  ///
  /// This is properly synchronized and can be called from multiple threads. If
  /// this represents the last outstanding bytes from the buffer then it is
  /// deleted.
  ///
  /// @param          n The number of bytes to return.
  void deallocate(size_t n);

  /// Clean up a block at shutdown.
  ///
  /// This is called by a Peer to finalize its current inplace block at
  /// shutdown. If there are bytes remaining then it should deallocate them,
  /// otherwise if there are no bytes remaining then it has no impact (because
  /// the fact that there are 0 remaining bytes implies delete was already
  /// called).
  void finalize();

  /// A utility to help delete a block-allocated parcel.
  ///
  /// This uses the current configuration to find the block associated with the
  /// parcel, and forwards to its deallocate() routine. If the @p p parcel was
  /// not allocated from an InplaceBlock then this results in undefined
  /// behavior.
  ///
  /// @param          p The parcel to delete.
  static void DeleteParcel(const hpx_parcel_t* p);

 private:
  std::atomic<size_t> remaining_;             //<! The number of bytes remaining
  const char padding_[util::PadToCacheline(sizeof(EagerBlock) + sizeof(remaining_))];
  char bytes_[];                              //<! The in-place buffer data
};

} // namespace pwc
} // namespace network
} // namespace libhpx

#endif // LIBHPX_NETWORK_PWC_PARCEL_BLOCK_H
