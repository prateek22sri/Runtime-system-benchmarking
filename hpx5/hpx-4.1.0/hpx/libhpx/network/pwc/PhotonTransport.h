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

#ifndef LIBHPX_NETWORK_PWC_PHOTON_TRANSPORT_H
#define LIBHPX_NETWORK_PWC_PHOTON_TRANSPORT_H

#include "Commands.h"
#include "libhpx/config.h"
#include <photon.h>

namespace libhpx {
namespace network {
namespace pwc {

class PhotonTransport {
 public:
  static constexpr int ANY_SOURCE = -1;

  typedef struct photon_buffer_priv_t Key;

  struct alignas(HPX_CACHELINE_SIZE) Op {
    unsigned                        rank;
    const unsigned               PADDING;
    Command                          lop;
    Command                          rop;
    size_t                             n;
    void*                           dest;
    const PhotonTransport::Key* dest_key;
    const void*                      src;
    const PhotonTransport::Key*  src_key;

    Op() : rank(), PADDING(), lop(), rop(), n(), dest(nullptr),
           dest_key(nullptr), src(nullptr), src_key(nullptr) {
    }

    int cmd();
    int put();
    int get();
  };

  static void Initialize(const config_t *config, int rank, int ranks);

  static int Test(Command *op, int *remaining, int id, int *src);
  static int Probe(Command *op, int *remaining, int rank, int *src);

  static void Pin(const void *base, size_t bytes, Key *key);
  static void Unpin(const void *base, size_t bytes);

  static const Key* FindKeyRef(const void *addr, size_t n);
  static void FindKey(const void *addr, size_t n, Key *key);
  static Key FindKey(const void *addr, size_t n);

 private:
  static int Poll(Command* op, int* remaining, int rank, int* src, int type);
};

} // namespace pwc
} // namespace network
} // namespace libhpx

#endif // LIBHPX_NETWORK_PWC_XPORT_H
