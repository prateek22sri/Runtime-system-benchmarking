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
#include "config.h"
#endif

#include "Networks.h"
#include "libhpx/debug.h"
#include "libhpx/libhpx.h"
#include <cstring>

namespace {
using libhpx::boot::Network;
using libhpx::boot::SMP;
}

SMP::SMP() : Network()
{
  rank_ = 0;
  nRanks_ = 1;
}

SMP::~SMP()
{
}

void
SMP::allgather(const void* src, void* dest, int n) const
{
  std::memcpy(dest, src, n);
}

void
SMP::alltoall(void* dest, const void* src, int n, int) const
{
  std::memcpy(dest, src, n);
}
