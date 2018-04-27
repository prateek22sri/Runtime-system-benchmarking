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

#ifndef LIBHPX_NETWORK_ISIR_FUNNELED_NETWORK_H
#define LIBHPX_NETWORK_ISIR_FUNNELED_NETWORK_H

#include "libhpx/Network.h"
#include "IRecvBuffer.h"
#include "ISendBuffer.h"
#include "MPITransport.h"
#include "libhpx/ParcelStringOps.h"
#include "libhpx/util/Aligned.h"
#include "libhpx/util/TwoLockQueue.h"
#include <mutex>

namespace libhpx {
namespace network {
namespace isir {
class FunneledNetwork : public Network, public ParcelStringOps,
                        public util::Aligned<HPX_CACHELINE_SIZE>
{
 public:
  FunneledNetwork(const config_t *cfg, GAS *gas);
  ~FunneledNetwork();

  int type() const;
  void progress(int);
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

 private:

  using Transport = libhpx::network::isir::MPITransport;
  using IRecvBuffer = libhpx::network::isir::IRecvBuffer;
  using ISendBuffer = libhpx::network::isir::ISendBuffer;
  using ParcelQueue = libhpx::util::TwoLockQueue<hpx_parcel_t*>;

  void sendAll();

  ParcelQueue  sends_;
  ParcelQueue  recvs_;
  Transport    xport_;
  ISendBuffer isends_;
  IRecvBuffer irecvs_;
  std::mutex    lock_;
};
} // namespace isir
} // namespace network
} // namespace libhpx

#endif // LIBHPX_NETWORK_ISIR_FUNNELED_NETWORK_H
