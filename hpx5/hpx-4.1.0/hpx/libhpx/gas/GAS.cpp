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

#include "libhpx/GAS.h"
#include "smp/SMP.h"
#include "pgas/PGAS.h"
#include "agas/AGAS.h"
#include "libhpx/debug.h"
#include "libhpx/gas/Affinity.h"
#include "libhpx/util/Aligned.h"

namespace {
const int LEVEL = HPX_LOG_CONFIG | HPX_LOG_GAS;

using libhpx::gas::SMP;
using libhpx::gas::pgas::PGAS;
using libhpx::gas::agas::AGAS;
using libhpx::gas::affinity::None;
using libhpx::gas::affinity::CuckooHash;
using libhpx::gas::affinity::URCU;
using namespace libhpx::util;

/// The Hosted class mixes a GAS implementation with an Affinity policy to
/// create a complete GAS class. The associated make_hosted template allows us
/// to dynamically dispatch to a static class based on the current
/// configuration.
template <class GAS, class Affinity>
class Hosted : public Affinity, public GAS, public Aligned<HPX_CACHELINE_SIZE>
{
 public:
  template <typename... Args>
  Hosted(Args... args) : Affinity(), GAS(args...), Aligned<HPX_CACHELINE_SIZE>()
  {
  }
};

template <class GAS, typename... Args>
GAS*
make_hosted(config_t* cfg, Args... args)
{
  switch (cfg->gas_affinity) {
   default:
    return new Hosted<GAS, None>(args...);
   case (HPX_GAS_AFFINITY_NONE):
    return new Hosted<GAS, None>(args...);
   case (HPX_GAS_AFFINITY_CUCKOO):
    return new Hosted<GAS, CuckooHash>(args...);
   case (HPX_GAS_AFFINITY_URCU):
#ifdef HAVE_URCU
    return new Hosted<GAS, URCU>(args...);
#else
    // log_error("URCU not supported on the current platform, "
    //           "using --hpx-gas-affinity=none\n");
    return new Hosted<GAS, None>(args...);
#endif
  }
}

GAS*
make_hosted(config_t* cfg, const libhpx::boot::Network* boot)
{
  switch (cfg->gas) {
   case HPX_GAS_SMP:
    return make_hosted<SMP>(cfg);

   case HPX_GAS_AGAS:
#if defined(HAVE_AGAS) && defined(HAVE_NETWORK)
    return make_hosted<AGAS>(cfg, cfg, boot);
#endif

   case HPX_GAS_PGAS:
#ifdef HAVE_NETWORK
    return make_hosted<PGAS>(cfg, cfg);
#endif

   default:
    dbg_error("unexpected configuration value for --hpx-gas\n");
  }
}
}

GAS::~GAS()
{
}

GAS*
GAS::Create(config_t *cfg, const boot::Network* const boot) {
#ifndef HAVE_NETWORK
  // if we didn't build a network we need to default to SMP
  cfg->gas = HPX_GAS_SMP;
#endif

  int ranks = boot->getNRanks();

  // if we built a network, we might want to optimize for SMP
  if (ranks == 1 && cfg->opt_smp) {
    if (cfg->gas != HPX_GAS_SMP && cfg->gas != HPX_GAS_DEFAULT) {
      // log_level(LEVEL, "GAS %s overridden to SMP.\n", HPX_GAS_TO_STRING[type]);
      cfg->gas = HPX_GAS_SMP;
    }
    cfg->gas = HPX_GAS_SMP;
  }

  if (ranks > 1 && cfg->gas == HPX_GAS_SMP) {
    dbg_error("SMP GAS selection fails for %d ranks\n", ranks);
  }

  GAS* gas = make_hosted(cfg, boot);

  if (!gas) {
    // log_error("GAS %s failed to initialize\n", HPX_GAS_TO_STRING[type]);
  }
  else {
    // log_gas("GAS %s initialized\n", HPX_GAS_TO_STRING[type]);
  }

  // NB: HPX_GAS_BLOCK_BYTES_MAX is declared const in the interface... this does
  //     a C-style const cast during initialization to set it up.
  dbg_assert(gas);
  HPX_GAS_BLOCK_BYTES_MAX = gas->maxBlockSize();
  return gas;
}
