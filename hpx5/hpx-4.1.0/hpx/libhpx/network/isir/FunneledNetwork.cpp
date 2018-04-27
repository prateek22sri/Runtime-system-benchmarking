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

#include "FunneledNetwork.h"
#include "libhpx/collective.h"
#include "libhpx/debug.h"
#include "libhpx/parcel.h"

namespace {
using libhpx::Network;
using libhpx::network::ParcelStringOps;
using libhpx::network::isir::FunneledNetwork;
}

FunneledNetwork::FunneledNetwork(const config_t *cfg, GAS *gas)
    : Network(),
      ParcelStringOps(),
      util::Aligned<HPX_CACHELINE_SIZE>(),
      sends_(),
      recvs_(),
      xport_(),
      isends_(cfg, gas, xport_),
      irecvs_(cfg, xport_),
      lock_()
{
}

FunneledNetwork::~FunneledNetwork()
{
  while (hpx_parcel_t *p = sends_.dequeue()) {
    parcel_delete(p);
  }
  while (hpx_parcel_t *p = recvs_.dequeue()) {
    parcel_delete(p);
  }
}

int
FunneledNetwork::type() const {
  return HPX_NETWORK_ISIR;
}

void
FunneledNetwork::sendAll() {
  while (hpx_parcel_t *p = sends_.dequeue()) {
    hpx_parcel_t *ssync = p->next;
    p->next = NULL;
    isends_.append(p, ssync);
  }
}

int
FunneledNetwork::init(void **ctx)
{
  flush();

  auto coll = static_cast<coll_t*>(*ctx);
  int num_active = coll->group_sz;
  log_net("ISIR network collective being initialized."
          "Total active ranks: %d\n", num_active);
  int32_t *ranks = reinterpret_cast<int32_t*>(coll->data);

  if (coll->comm_bytes == 0) {
    // we have not yet allocated a communicator
    coll->comm_bytes = sizeof(Transport::Communicator);
    auto bytes = sizeof(coll_t) + coll->group_bytes + coll->comm_bytes;
    coll = static_cast<coll_t*>(realloc(coll, bytes));
    *ctx = coll;
  }

  // setup communicator
  auto offset = coll->data + coll->group_bytes;
  auto comm = reinterpret_cast<Transport::Communicator*>(offset);
  std::lock_guard<std::mutex> _(lock_);
  xport_.createComm(comm, num_active, ranks);
  return 0;
}

int
FunneledNetwork::sync(void *in, size_t count, void *out, void *ctx)
{
  // flushing network is necessary (sufficient?) to execute any
  // packets destined for collective operation
  flush();

  auto coll = static_cast<coll_t *>(ctx);
  auto offset = coll->data + coll->group_bytes;
  auto comm = reinterpret_cast<Transport::Communicator*>(offset);
  std::lock_guard<std::mutex> _(lock_);
  switch (coll->type) {
   case ALL_REDUCE:
    xport_.allreduce(in, out, count, NULL, &coll->op, comm);
    break;
   default:
    log_dflt("Collective type descriptor: %d is invalid!\n", coll->type);
    break;
  }
  return 0;
}

void
FunneledNetwork::deallocate(const hpx_parcel_t* p)
{
  dbg_error("ISIR network has not network-managed parcels\n");
}

int
FunneledNetwork::send(hpx_parcel_t *p, hpx_parcel_t *ssync) {
  // Use the unused parcel-next pointer to get the ssync continuation parcels
  // through the concurrent queue, along with the primary parcel.
  p->next = ssync;
  sends_.enqueue(p);
  return 0;
}

hpx_parcel_t *
FunneledNetwork::probe(int) {
  return recvs_.dequeue();
}

void
FunneledNetwork::flush()
{
  std::lock_guard<std::mutex> _(lock_);
  sendAll();
  hpx_parcel_t *ssync = NULL;
  isends_.flush(&ssync);
  if (ssync) {
    recvs_.enqueue(ssync);
  }
}

void
FunneledNetwork::pin(const void *base, size_t n, void *key)
{
  xport_.pin(base, n, key);
}

void
FunneledNetwork::unpin(const void* base, size_t n)
{
  xport_.unpin(base, n);
}

void
FunneledNetwork::progress(int)
{
  if (auto _ = std::unique_lock<std::mutex>(lock_, std::try_to_lock)) {
    hpx_parcel_t *chain = NULL;
    if (int n = irecvs_.progress(&chain)) {
      log_net("completed %d recvs\n", n);
      recvs_.enqueue(chain);
    }
    chain = NULL;
    if (int n = isends_.progress(&chain)) {
      log_net("completed %d sends\n", n);
      recvs_.enqueue(chain);
    }
    sendAll();
  }
}
