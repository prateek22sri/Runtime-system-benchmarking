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

#ifndef LIBHPX_NETWORK_WRAPPERS_H
#define LIBHPX_NETWORK_WRAPPERS_H

#include "libhpx/Network.h"
#include "libhpx/util/Aligned.h"
#include "libhpx/util/TwoLockQueue.h"
#include <atomic>

namespace libhpx {
namespace network {
class NetworkWrapper : public Network
{
 public:
  virtual ~NetworkWrapper();

  int type() const
  {
    return impl_->type();
  }

  void progress(int n) {
    impl_->progress(n);
  }

  hpx_parcel_t* probe(int n) {
    return impl_->probe(n);
  }

  void flush() {
    impl_->flush();
  }

  void deallocate(const hpx_parcel_t* p) {
    impl_->deallocate(p);
  }

  int send(hpx_parcel_t* p, hpx_parcel_t* ssync) {
    return impl_->send(p, ssync);
  }

  void pin(const void *base, size_t bytes, void *key) {
    impl_->pin(base, bytes, key);
  }

  void unpin(const void *base, size_t bytes) {
    impl_->unpin(base, bytes);
  }

  int init(void **collective) {
    return impl_->init(collective);
  }

  int sync(void *in, size_t in_size, void* out, void *collective) {
    return impl_->sync(in, in_size, out, collective);
  }

  int wait(hpx_addr_t lco, int reset) {
    return impl_->wait(lco, reset);
  }

  int get(hpx_addr_t lco, size_t n, void *to, int reset) {
    return impl_->get(lco, n, to, reset);
  }

  void memget(void *to, hpx_addr_t from, size_t size, hpx_addr_t lsync,
              hpx_addr_t rsync) {
    impl_->memget(to, from, size, lsync, rsync);
  }

  void memget(void *to, hpx_addr_t from, size_t size, hpx_addr_t lsync){
    impl_->memget(to, from, size, lsync);
  }

  void memget(void *to, hpx_addr_t from, size_t size) {
    impl_->memget(to, from, size);
  }

  void memput(hpx_addr_t to, const void *from, size_t size, hpx_addr_t lsync,
              hpx_addr_t rsync) {
    impl_->memput(to, from, size, lsync, rsync);
  }

  void memput(hpx_addr_t to, const void *from, size_t size, hpx_addr_t rsync) {
    impl_->memput(to, from, size, rsync);
  }

  void memput(hpx_addr_t to, const void *from, size_t size) {
    impl_->memput(to, from, size);
  }

  void memcpy(hpx_addr_t to, hpx_addr_t from, size_t size, hpx_addr_t sync) {
    impl_->memcpy(to, from, size, sync);
  }

  void memcpy(hpx_addr_t to, hpx_addr_t from, size_t size) {
    impl_->memcpy(to, from, size);
  }

 protected:
  NetworkWrapper(Network* impl);

  Network* impl_;
};

class InstrumentationWrapper final : public NetworkWrapper {
 public:
  InstrumentationWrapper(Network* impl);

  void progress(int n);
  hpx_parcel_t* probe(int);
  int send(hpx_parcel_t* p, hpx_parcel_t* ssync);
};

class CompressionWrapper final : public NetworkWrapper {
 public:
  CompressionWrapper(Network* impl);
  int send(hpx_parcel_t* p, hpx_parcel_t* ssync);
};

class CoalescingWrapper final : public NetworkWrapper,
                                public util::Aligned<HPX_CACHELINE_SIZE>
{
 public:
  CoalescingWrapper(Network* impl, const config_t *cfg, GAS *gas);
  void progress(int n);
  void flush();
  int send(hpx_parcel_t* p, hpx_parcel_t* ssync);

 private:
  void send(unsigned n);

  GAS& gas_;
  const unsigned size_;
  std::atomic<unsigned> prev_;
  std::atomic<unsigned> count_;
  std::atomic<unsigned> coalescing_;
  util::TwoLockQueue<hpx_parcel_t*> sends_;
};

} // namespace network
} // namespace libhpx

#endif //  LIBHPX_NETWORK_WRAPPERS_H
