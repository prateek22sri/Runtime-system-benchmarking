// =============================================================================
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "Peer.h"
#include "libhpx/debug.h"
#include "libhpx/gpa.h"
#include "libhpx/libhpx.h"
#include "libhpx/memory.h"

namespace {
using libhpx::network::pwc::Command;
using libhpx::network::pwc::PhotonTransport;
using libhpx::network::pwc::Peer;
using Op = libhpx::network::pwc::PhotonTransport::Op;
template <typename T> using Remote = libhpx::network::pwc::Remote<T>;
}

Peer::Peer()
    : lock_(),
      rank_(),
      heapSegment_(),
      remoteSend_(),
      sendEager_(),
      sendBuffer_(8),
      recv_(nullptr),
      padding_()
{
}

Peer::~Peer()
{
  if (recv_)
    recv_->finalize();
}

void*
Peer::operator new[](size_t bytes)
{
  size_t align = 1lu << ceil_log2_size_t(bytes);
  return registered_memalign(align, bytes);
}

void
Peer::operator delete[](void* peer)
{
  if (peer) {
    registered_free(peer);
  }
}


void
Peer::init(unsigned rank, unsigned here, const Remote<Peer>& remote,
          const HeapSegment& heap)
{
  std::lock_guard<std::mutex> _(lock_);
  rank_ = rank;
  remoteSend_.addr = &remote.addr[here].sendEager_;
  remoteSend_.key = remote.key;
  heapSegment_.addr = heap.addr;
  heapSegment_.key = heap.key;
}

int
Peer::start(const hpx_parcel_t* p)
{
  size_t n = parcel_size(p);
  if (sendEager_.put(rank_, p, n)) {
    return LIBHPX_OK;
  }

  Op op;
  op.rank = rank_;
  op.lop = Command::Nop();
  op.rop = Command::ReloadRequest(n);
  dbg_check( op.cmd() );
  return LIBHPX_RETRY;
}

void
Peer::progress()
{
  std::lock_guard<std::mutex> _(lock_);
  int i = sendBuffer_.progress([&](const hpx_parcel_t* p) {
      return start(p);
    });

  if (i < 0) {
    dbg_error("failed to progress the send buffer\n");
  }
}

void
Peer::send(const hpx_parcel_t *p)
{
  std::lock_guard<std::mutex> _(lock_);
  if (sendBuffer_.size() || start(p) == LIBHPX_RETRY) {
    sendBuffer_.push(p);
  }
}

void
Peer::reload(size_t n, size_t eagerSize)
{
  if (recv_ && n) {
    recv_->deallocate(n);
  }

  recv_ = new(eagerSize) InplaceBlock(eagerSize);

  Op op;
  op.rank = rank_;
  op.n = sizeof(EagerBlock);                    // only copy EeagerBlock data
  op.dest = remoteSend_.addr;
  op.dest_key = &remoteSend_.key;
  op.src = recv_;
  op.src_key = PhotonTransport::FindKeyRef(recv_, eagerSize);
  op.lop = Command::Nop();
  op.rop = Command::ReloadReply();

  dbg_check( op.put() );
}

void
Peer::put(hpx_addr_t to, const void *lva, size_t n, const Command& lcmd,
          const Command& rcmd)
{
  Op op;
  op.rank = rank_;
  op.n = n;
  op.dest = heapSegment_.addr + gpa_to_offset(to);
  op.dest_key = &heapSegment_.key;
  op.src = lva;
  op.src_key = PhotonTransport::FindKeyRef(lva, n);
  op.lop = lcmd;
  op.rop = rcmd;
  dbg_check( op.put() );
}

void
Peer::get(void *lva, hpx_addr_t from, size_t n, const Command& lcmd,
          const Command& rcmd)
{
  Op op;
  op.rank = rank_;
  op.n = n;
  op.dest = lva;
  op.dest_key = PhotonTransport::FindKeyRef(lva, n);
  op.src = heapSegment_.addr + gpa_to_offset(from);
  op.src_key = &heapSegment_.key;
  op.lop = lcmd;
  op.rop = rcmd;
  dbg_check( op.get() );
}
