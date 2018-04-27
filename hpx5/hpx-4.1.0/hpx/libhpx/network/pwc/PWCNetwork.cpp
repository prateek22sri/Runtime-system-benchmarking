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

#include "PWCNetwork.h"
#include "libhpx/collective.h"
#include "libhpx/gpa.h"
#include "libhpx/libhpx.h"
#include <exception>

namespace {
using libhpx::MemoryOps;
using libhpx::network::pwc::PWCNetwork;
using libhpx::network::pwc::AGASNetwork;
using libhpx::network::pwc::PGASNetwork;
using libhpx::network::pwc::PhotonTransport;
using Op = PhotonTransport::Op;
using Key = PhotonTransport::Key;
constexpr int ANY_SOURCE = PhotonTransport::ANY_SOURCE;
}

PWCNetwork* PWCNetwork::Instance_ = nullptr;

PWCNetwork& PWCNetwork::Instance()
{
  assert(Instance_);
  return *Instance_;
}

PWCNetwork*
PWCNetwork::Create(const config_t *cfg, const boot::Network& boot, GAS *gas)
{
  PhotonTransport::Initialize(cfg, boot.getRank(), boot.getNRanks());
  if (gas->type() == HPX_GAS_AGAS) {
    return new libhpx::network::pwc::AGASNetwork(cfg, boot, gas);
  }
  else {
    return new libhpx::network::pwc::PGASNetwork(cfg, boot, gas);
  }
}

PWCNetwork::PWCNetwork(const config_t *cfg, const boot::Network& boot, GAS *gas)
    : Network(),
      rank_(boot.getRank()),
      ranks_(boot.getNRanks()),
      eagerSize_(cfg->pwc_parcelbuffersize),
      gas_(*gas),
      boot_(boot),
      peers_(new Peer[ranks_]()),
      progressLock_(),
      probeLock_()
{
  assert(!Instance_);
  Instance_ = this;

  // Validate configuration.
  if (popcountl(cfg->pwc_parcelbuffersize) != 1) {
    dbg_error("--hpx-pwc-parcelbuffersize must 2^k (given %zu)\n",
              cfg->pwc_parcelbuffersize);
  }

  // Validate eager parcel limits.
  if (cfg->pwc_parceleagerlimit > cfg->pwc_parcelbuffersize) {
    dbg_error(" --hpx-pwc-parceleagerlimit (%zu) must be less than "
              "--hpx-pwc-parcelbuffersize (%zu)\n",
              cfg->pwc_parceleagerlimit, cfg->pwc_parcelbuffersize);
  }

  // Exchange the local Peer and Heap information.
  struct Exchange {
    Remote<Peer> peer;
    Remote<char> heap;
  } local;

  local.peer.addr = peers_.get();
  local.peer.key = PhotonTransport::FindKey(peers_.get(), ranks_*sizeof(Peer));
  local.heap.addr = static_cast<char*>(gas_.pinHeap(*this, &local.heap.key));

  std::unique_ptr<Exchange[]> remotes(new Exchange[ranks_]);
  boot.allgather(&local, &remotes[0], sizeof(Exchange));

  // And initialize the peers.
  for (int i = 0, e = ranks_; i < e; ++i) {
    peers_[i].init(i, rank_, remotes[i].peer, remotes[i].heap);
  }
}

PWCNetwork::~PWCNetwork()
{
  // Cleanup any remaining local work---this can leak memory and stuff, because
  // we aren't actually running the commands that we cleanup.
  {
    std::lock_guard<std::mutex> _(progressLock_);
    int remaining, src;
    Command command;
    do {
      PhotonTransport::Test(&command, &remaining, ANY_SOURCE, &src);
      // @todo: what do we do with these commands, other than ignoring them?
    } while (remaining > 0);
  }

  // Network deletion is effectively a collective, so this enforces that
  // everyone is done with rdma before we go and deregister anything.
  boot_.barrier();

  // Unpin the heap segment.
  gas_.unpinHeap(*this);
  Instance_ = nullptr;
}

int
PWCNetwork::type() const
{
  return HPX_NETWORK_PWC;
}

void
PWCNetwork::progress(int n)
{
  hpx_parcel_t* stack = nullptr;
  if (auto _ = std::unique_lock<std::mutex>(progressLock_, std::try_to_lock)) {
    Command command;
    int src;
    while (PhotonTransport::Test(&command, nullptr, ANY_SOURCE, &src)) {
      if (hpx_parcel_t* p = command(rank_)) {
        parcel_stack_push(&stack, p);
      }
    }
  }
  parcel_launch_all(stack);
}

void
PWCNetwork::flush()
{
}

hpx_parcel_t*
PWCNetwork::probe(int n)
{
  hpx_parcel_t* stack = nullptr;
  if (auto _ = std::unique_lock<std::mutex>(probeLock_, std::try_to_lock)) {
    Command command;
    int src;
    while (PhotonTransport::Probe(&command, nullptr, ANY_SOURCE, &src)) {
      if (hpx_parcel_t* p = command(src)) {
        parcel_stack_push(&stack, p);
      }
    }
  }
  return stack;
}

void
PWCNetwork::deallocate(const hpx_parcel_t* p)
{
  InplaceBlock::DeleteParcel(p);
}

int
PWCNetwork::send(hpx_parcel_t *p, hpx_parcel_t *ssync)
{
  // This is a blatant hack to keep track of the ssync parcel using p's next
  // pointer. It will allow us to both delete p and run ssync once the
  // underlying network operation is serviced. It works in conjunction with the
  // handle_delete_parcel command.
  dbg_assert(p->next == NULL);
  p->next = ssync;

  if (parcel_size(p) >= here->config->pwc_parceleagerlimit) {
    return rendezvousSend(p);
  }
  else {
    int rank = gas_.ownerOf(p->target);
    peers_[rank].send(p);
    return HPX_SUCCESS;
  }
}

void
PWCNetwork::progressSends(unsigned rank)
{
  peers_[rank].progress();
}

void
PWCNetwork::pin(const void *base, size_t bytes, void *key)
{
  PhotonTransport::Pin(base, bytes, static_cast<Key*>(key));
}

void
PWCNetwork::unpin(const void *base, size_t bytes)
{
  PhotonTransport::Unpin(base, bytes);
}

int
PWCNetwork::init(void **collective)
{
  return LIBHPX_OK;
}

int
PWCNetwork::sync(void *in, size_t in_size, void* out, void *collective)
{
  return LIBHPX_OK;
}

void
PWCNetwork::put(hpx_addr_t dest, const void *src, size_t n, const Command& lcmd,
                const Command& rcmd)
{
  int rank = gpa_to_rank(dest);
  peers_[rank].put(dest, src, n, lcmd, rcmd);
}

void
PWCNetwork::get(void *dest, hpx_addr_t src, size_t n, const Command& lcmd,
                const Command& rcmd)
{
  int rank = gpa_to_rank(src);
  peers_[rank].get(dest, src, n, lcmd, rcmd);
}

void
PWCNetwork::reload(unsigned src, size_t n)
{
  peers_[src].reload(n, eagerSize_);
}
