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

/// @file libhpx/network/network.c
#include "libhpx/Network.h"
#include "Wrappers.h"
#include "SMPNetwork.h"
#ifdef HAVE_MPI
#include "isir/FunneledNetwork.h"
#endif
#ifdef HAVE_PHOTON
#include "pwc/PWCNetwork.h"
#endif
#include "libhpx/debug.h"
#include "libhpx/instrumentation.h"

static const int LEVEL = HPX_LOG_CONFIG | HPX_LOG_NET | HPX_LOG_DEFAULT;

namespace {
using namespace libhpx;
using namespace libhpx::network;
using BootNetwork = libhpx::boot::Network;
}

Network::Network()
    : StringOps(),
      CollectiveOps(),
      LCOOps(),
      MemoryOps(),
      ParcelOps()
{
}

Network::~Network()
{
}

Network*
Network::Create(config_t *cfg, const BootNetwork& boot, GAS *gas)
{
#ifndef HAVE_NETWORK
  // if we didn't build a network we need to default to SMP
  cfg->network = HPX_NETWORK_SMP;
#endif

  libhpx_network_t type = cfg->network;
  int ranks = boot.getNRanks();
  Network* network = nullptr;

  // default to HPX_NETWORK_SMP for SMP execution
  if (ranks == 1 && cfg->opt_smp) {
    if (type != HPX_NETWORK_SMP && type != HPX_NETWORK_DEFAULT) {
      log_level(LEVEL, "%s overridden to SMP.\n", HPX_NETWORK_TO_STRING[type]);
      cfg->network = HPX_NETWORK_SMP;
    }
    type = HPX_NETWORK_SMP;
  }

  if (ranks > 1) {
#ifndef HAVE_NETWORK
    dbg_error("Launched on %d ranks but no network available\n", ranks);
#endif
  }

  if (ranks > 1 && type == HPX_NETWORK_SMP) {
    dbg_error("SMP network selection fails for %d ranks\n", ranks);
  }

  if (type == HPX_NETWORK_PWC) {
#ifndef HAVE_PHOTON
    dbg_error("PWC network selection fails (photon disabled in config)\n");
#endif
  }

  // handle default
  if (type == HPX_NETWORK_DEFAULT) {
#ifdef HAVE_PHOTON
    type = HPX_NETWORK_PWC;
#else
    type = HPX_NETWORK_ISIR;
#endif
  }

  switch (type) {
   case HPX_NETWORK_PWC:
#ifdef HAVE_PHOTON
    network = libhpx::network::pwc::PWCNetwork::Create(cfg, boot, gas);
#else
    log_level(LEVEL, "PWC network unavailable (no network configured)\n");
#endif
    break;

   case HPX_NETWORK_ISIR:
#ifdef HAVE_MPI
    network = new libhpx::network::isir::FunneledNetwork(cfg, gas);
#else
    log_level(LEVEL, "ISIR network unavailable (no network configured)\n");
#endif
    break;

   case HPX_NETWORK_SMP:
    network = new SMPNetwork(boot);
    break;

   default:
    log_level(LEVEL, "unknown network type\n");
    break;
  }

  if (!network) {
    dbg_error("%s did not initialize\n", HPX_NETWORK_TO_STRING[cfg->network]);
  }
  else {
    log_level(LEVEL, "%s network initialized\n", HPX_NETWORK_TO_STRING[type]);
  }

  if (cfg->parcel_compression) {
    network = new CompressionWrapper(network);
  }

  if (cfg->coalescing_buffersize) {
    network = new CoalescingWrapper(network, cfg, gas);
  }

  if (!config_trace_at_isset(here->config, here->rank)) {
    return network;
  }

  if (!inst_trace_class(HPX_TRACE_NETWORK)) {
    return network;
  }

  return new InstrumentationWrapper(network);
}
