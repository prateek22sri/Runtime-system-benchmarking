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

#ifndef LIBHPX_BOOTSTRAP_NETWORK_H
#define LIBHPX_BOOTSTRAP_NETWORK_H

#include "libhpx/config.h"

namespace libhpx {
namespace boot {
class Network {
 public:
  virtual ~Network();

  int getRank() const {
    return rank_;
  }

  int getNRanks() const {
    return nRanks_;
  }

  [[ noreturn ]] virtual void abort() const;

  virtual libhpx_boot_t type() const = 0;
  virtual void barrier() const = 0;
  virtual void allgather(const void* src, void* dest, int n) const = 0;
  virtual void alltoall(void* dest, const void* src, int n, int stride) const = 0;

  static Network* Create(libhpx_boot_t type);

 protected:
  Network();

  int rank_;
  int nRanks_;
};
}
}

#endif // LIBHPX_BOOTSTRAP_NETWORK_H
