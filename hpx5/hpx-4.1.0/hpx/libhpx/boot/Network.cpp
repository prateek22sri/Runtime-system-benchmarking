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
#include <cstdlib>

namespace {
using BootNetwork = libhpx::boot::Network;
using libhpx::boot::SMP;
}

BootNetwork::Network() : rank_(-1), nRanks_(-1)
{
}

BootNetwork::~Network()
{
}

void
BootNetwork::abort() const
{
  std::abort();
  unreachable();
}

static BootNetwork*
_Default()
{
#ifdef HAVE_PMI
  return new libhpx::boot::PMI();
#endif

#ifdef HAVE_MPI
  return new libhpx::boot::MPI();
#endif

  return new SMP();
}

BootNetwork*
BootNetwork::Create(libhpx_boot_t type)
{
  BootNetwork* boot = nullptr;
  switch (type) {
   case (HPX_BOOT_PMI):
#ifdef HAVE_PMI
    boot = new libhpx::boot::PMI();
    log_boot("initialized PMI bootstrapper.\n");
#else
    dbg_error("PMI bootstrap not supported in current configuration.\n");
#endif
    break;

   case (HPX_BOOT_MPI):
#ifdef HAVE_MPI
    boot = new libhpx::boot::MPI();
    log_boot("initialized mpirun bootstrapper.\n");
#else
    dbg_error("MPI bootstrap not supported in current configuration.\n");
#endif
    break;

   case (HPX_BOOT_SMP):
    boot = new libhpx::boot::SMP();
    log_boot("initialized the SMP bootstrapper.\n");
    break;

   case HPX_BOOT_DEFAULT:
   default:
    boot = _Default();
    break;
  }

  if (!boot) {
    boot = _Default();
  }

  if (!boot) {
    dbg_error("failed to initialize the bootstrapper.\n");
  }
  else {
    log_dflt("bootstrapped using %s.\n", HPX_BOOT_TO_STRING[boot->type()]);
  }

  return boot;
}

