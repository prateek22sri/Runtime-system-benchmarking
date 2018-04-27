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

#include "Wrappers.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/libhpx.h"
#include "libhpx/parcel.h"
#include <memory>
#include <cstring>

namespace {
using libhpx::network::NetworkWrapper;
using libhpx::network::CoalescingWrapper;

/// Demultiplex coalesced parcels on the receiver side.
///
/// @param       buffer The buffer of coalesced parcels.
/// @param            n The number of coalesced bytes.
int
DemultiplexHandler(char* buffer, size_t n) {
  auto end = buffer + n;
  while (buffer < end) {
    auto p = reinterpret_cast<hpx_parcel_t*>(buffer);
    auto q = parcel_clone(p);
    parcel_launch(q);
    buffer += parcel_size(p);
  }
  return HPX_SUCCESS;
}
LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, Demultiplex, DemultiplexHandler,
              HPX_POINTER, HPX_SIZE_T);

class CoalescingBuffer {
 public:
  CoalescingBuffer() : parcels_(), ssync_(), bytes_() {
  }

  void record(hpx_parcel_t *p) {
    while (auto s = parcel_stack_pop(&p->next)) {
      parcel_stack_push(&ssync_, s);
    }
    parcel_stack_push(&parcels_, p);
    bytes_ += parcel_size(p);
  }

  void finish(hpx_addr_t target, Network* net) {
    if (!bytes_) return;
    auto fat = action_new_parcel(Demultiplex, target, 0, 0, 2, NULL, bytes_);
    char *next = static_cast<char*>(hpx_parcel_get_data(fat));
    while (auto p = parcel_stack_pop(&parcels_)) {
      auto n = parcel_size(p);
      memcpy(next, p, n);
      parcel_delete(p);
      next += n;
    }
    if (net->send(fat, ssync_)) {
      throw std::exception();
    }
  }

 private:
  hpx_parcel_t *parcels_;                       //!< parcels for this rank
  hpx_parcel_t   *ssync_;                       //!< the continuations
  unsigned        bytes_;                       //!< the number of bytes
};

class Coalescing {
 public:
  Coalescing(std::atomic<unsigned>& c) : c_(c) {
    c_++;
  }
  ~Coalescing() {
    c_--;
  }
 private:
  std::atomic<unsigned>& c_;
};
}

/// Send parcels from the coalesced network.
void
CoalescingWrapper::send(unsigned n) {
  if (!n)
    return;

  std::unique_ptr<CoalescingBuffer[]> buffers(new CoalescingBuffer[HPX_LOCALITIES]);

  for (auto i = 0u, e = n; i < e; ++i) {
    auto p = sends_.dequeue();
    dbg_assert(p);
    auto target = gas_.ownerOf(p->target);
    buffers[target].record(p);
  }

  for (auto i = 0, e = HPX_LOCALITIES; i < e; ++i) {
    buffers[i].finish(HPX_THERE(i), impl_);
  }
}

int
CoalescingWrapper::send(hpx_parcel_t *p, hpx_parcel_t *ssync)
{
  if (!action_is_coalesced(p->action)) {
    return impl_->send(p, ssync);
  }

  // Prepare the parcel now, 1) to serialize it while its data is probably in
  // our cache and 2) to make sure it gets a pid from the right parent.
  parcel_prepare(p);
  sends_.enqueue(p);

  // Notify the world that there's another parcel in the queue.
  auto n = 1 + count_++;

  // Coalesce on demand as long as we have enough parcels available.
  while (size_ < n) {
    Coalescing _(coalescing_);
    if (count_.compare_exchange_strong(n, n - size_)) {
      send(size_);
      n -= size_;
    }
  }

  return LIBHPX_OK;
}

void
CoalescingWrapper::progress(int n) {
  // If the number of buffered parcels is the same as the previous time we
  // progressed, we'll do an eager send operation to reduce latency and make
  // sure we're not inducing deadlock.
  unsigned count = count_;
  if (count == prev_) {
    Coalescing _(coalescing_);
    send(count_.exchange(0));
    count = 0;
  }
  prev_ = count;
  NetworkWrapper::progress(n);
}

void
CoalescingWrapper::flush() {
  // coalesce the rest of the buffered sends
  send(count_.exchange(0));

  // wait for any concurrent operations to complete
  while (coalescing_) { /* spin */; }

  // and flush the underlying network
  NetworkWrapper::flush();
}

CoalescingWrapper::CoalescingWrapper(Network* impl, const config_t *cfg,
                                     GAS *gas)
    : NetworkWrapper(impl),
      util::Aligned<HPX_CACHELINE_SIZE>(),
      gas_(*gas),
      size_(cfg->coalescing_buffersize),
      prev_(0),
      count_(0),
      coalescing_(0),
      sends_()
{
  log_net("Created coalescing network\n");
}
