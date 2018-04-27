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

#ifndef LIBHPX_NETWORK_PARCEL_STRING_OPS_H
#define LIBHPX_NETWORK_PARCEL_STRING_OPS_H

#include "libhpx/StringOps.h"

namespace libhpx {
namespace network {
class ParcelStringOps : public virtual StringOps {
 public:
  void memget(void *dest, hpx_addr_t src, size_t n, hpx_addr_t lsync, hpx_addr_t rsync);
  void memget(void *dest, hpx_addr_t src, size_t n, hpx_addr_t lsync);
  void memget(void *dest, hpx_addr_t src, size_t n);
  void memput(hpx_addr_t dest, const void *src, size_t n, hpx_addr_t lsync, hpx_addr_t rsync);
  void memput(hpx_addr_t dest, const void *src, size_t n, hpx_addr_t rsync);
  void memput(hpx_addr_t dest, const void *src, size_t n);
  void memcpy(hpx_addr_t dest, hpx_addr_t src, size_t n, hpx_addr_t sync);
  void memcpy(hpx_addr_t dest, hpx_addr_t src, size_t n);
};
} // namespace network
} // namespace libhpx

#endif // LIBHPX_NETWORK_PARCEL_STRING_OPS_H
