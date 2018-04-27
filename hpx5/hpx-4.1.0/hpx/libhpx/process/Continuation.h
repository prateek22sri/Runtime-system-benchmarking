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

#ifndef LIBHPX_PROCESS_CONTINUATION_H
#define LIBHPX_PROCESS_CONTINUATION_H

#include "libhpx/parcel.h"
#include "hpx/hpx.h"
#include <vector>

namespace libhpx {
namespace process {
class Continuation {
 public:
  Continuation(size_t bytes);
  ~Continuation();

  unsigned insert(hpx_action_t op, hpx_addr_t addr);
  void remove(unsigned id);
  void trigger(const void *value) const;


 private:
  const size_t bytes_;
  std::vector<hpx_parcel_t*> parcels_;
};
} // namespace process
} // namespace libhpx

#endif // LIBHPX_PROCESS_CONTINUATION_H
