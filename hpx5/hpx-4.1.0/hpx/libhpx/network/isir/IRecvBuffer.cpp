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

#include "IRecvBuffer.h"
#include "parcel_utils.h"
#include "libhpx/events.h"
#include "libhpx/parcel.h"
#include <memory>
#include <libhpx/Topology.h>
#ifdef HAVE_APEX
#include "apex.h"
#endif

namespace {
using libhpx::network::isir::IRecvBuffer;
}

IRecvBuffer::IRecvBuffer(const config_t *cfg, Transport &xport)
    : xport_(xport),
      limit_(cfg->isir_recvlimit),
      capacity_(0),
      size_(0),
      requests_(nullptr),
      records_(nullptr)
{
  reserve((limit_) ? std::min(64u, limit_) : 64u);
}

IRecvBuffer::~IRecvBuffer()
{
  hpx_parcel_t *chain = reserve(0);
  while (hpx_parcel_t *p = parcel_stack_pop(&chain)) {
    parcel_delete(p);
  }
}

int
IRecvBuffer::progress(hpx_parcel_t** stack)
{
  assert(stack);
  probe();
  std::unique_ptr<int[]> out(new int[size_]);
  std::unique_ptr<Status[]> statuses(new Status[size_]);
  int e = xport_.Testsome(size_, &requests_[0], &out[0], &statuses[0]);
  if (e) log_net("detected completed irecvs: %u\n", e);
  for (int i = 0; i < e; ++i) {
    auto p = finish(out[i], statuses[i]);
    EVENT_PARCEL_RECV(p->id, p->action, p->size, p->src, p->target);
    parcel_stack_push(stack, p);
    start(out[i]);
  }
  return e;
}

hpx_parcel_t *
IRecvBuffer::reserve(unsigned capacity)
{
  hpx_parcel_t *out = nullptr;

  if (capacity == capacity_) {
    return out;
  }

  // saturate the capacity
  capacity_ = (limit_) ? std::min(capacity, limit_) : capacity;

  // cancel any excess irecvs
  for (auto i = size_ - 1; size_ > capacity_; --i, --size_) {
    if (auto* p = cancel(i)) {
      parcel_stack_push(&out, p);
    }
  }

  // resize the vectors
  unsigned n = capacity_;
  requests_ = static_cast<Request*>(realloc(requests_, n * sizeof(Request)));
  records_ = static_cast<Record*>(realloc(records_, n* sizeof(Record)));
  return out;
}

void
IRecvBuffer::start(unsigned i)
{
  auto& record = records_[i];
  auto tag = record.tag;
  auto size = tag_to_payload_size(tag);
  record.p = parcel_alloc(size);
  record.p->thread = nullptr;
  record.p->next = nullptr;
  record.p->state = PARCEL_SERIALIZED;

  auto buffer = isir_network_offset(record.p);
  auto bytes = payload_size_to_isir_bytes(size);
  requests_[i] = xport_.irecv(buffer, bytes, tag);

  log_net("started an MPI_Irecv operation: tag %d , %u bytes\n", tag, bytes);
}

void
IRecvBuffer::probe()
{
  int tag = xport_.iprobe();
  if (tag < 0) {
    return;
  }

  auto i = size_++;
  log_net("detected a new recv (%u) with tag: %u\n", i, tag);
  records_[i].tag = tag;
  start(i);
  if (size_ == capacity_) {
    reserve(2 * capacity_);
  }
}

hpx_parcel_t *
IRecvBuffer::cancel(unsigned i)
{
  assert(i < size_);
  auto& record = records_[i];
  auto* p = record.p;
  record.p = NULL;
  record.tag = -1;

  if (xport_.cancel(requests_[i])) {
    parcel_delete(p);
    p = NULL;
  }

  return p;
}

hpx_parcel_t*
IRecvBuffer::finish(unsigned i, const Status& status)
{
  assert(i < size_);
  assert(xport_.bytes(status) > 0);
  Record& record = records_[i];
  auto p = record.p;
  record.p = nullptr;
  p->size = isir_bytes_to_payload_size(xport_.bytes(status));
  p->src = xport_.source(status);
  log_net("finished a recv for a %u-byte payload\n", p->size);
  EVENT_NETWORK_RECV();
#ifdef HAVE_APEX
  apex_recv(p->id, p->size, p->src, topo_value_to_worker(p->id)+1);
#endif
  return p;
}
