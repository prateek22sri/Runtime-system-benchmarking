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

#ifndef LIBHPX_NETWORK_SMP_H
#define LIBHPX_NETWORK_SMP_H

#include "libhpx/Network.h"

namespace libhpx {
namespace network {

class SMPNetwork final : public Network
{
 public:
  SMPNetwork(const boot::Network& boot);

  int type() const;
  void progress(int n);
  hpx_parcel_t* probe(int);
  void flush();

  void deallocate(const hpx_parcel_t* p);
  int send(hpx_parcel_t* p, hpx_parcel_t* ssync);

  void pin(const void *base, size_t bytes, void *key);
  void unpin(const void *base, size_t bytes);

  int wait(hpx_addr_t lco, int reset);
  int get(hpx_addr_t lco, size_t n, void *to, int reset);

  int init(void **collective);
  int sync(void *in, size_t in_size, void* out, void *collective);

  void memget(void *to, hpx_addr_t from, size_t size, hpx_addr_t lsync, hpx_addr_t rsync);
  void memget(void *to, hpx_addr_t from, size_t size, hpx_addr_t lsync);
  void memget(void *to, hpx_addr_t from, size_t size);

  void memput(hpx_addr_t to, const void *from, size_t size, hpx_addr_t lsync, hpx_addr_t rsync);
  void memput(hpx_addr_t to, const void *from, size_t size, hpx_addr_t rsync);
  void memput(hpx_addr_t to, const void *from, size_t size);

  void memcpy(hpx_addr_t to, hpx_addr_t from, size_t size, hpx_addr_t sync);
  void memcpy(hpx_addr_t to, hpx_addr_t from, size_t size);
};

} // namespace network
} // namespace libhpx

#endif
