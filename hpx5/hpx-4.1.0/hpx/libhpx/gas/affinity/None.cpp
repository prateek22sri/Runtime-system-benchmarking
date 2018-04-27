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

/// Provide a home for the None vtable.

#include "libhpx/gas/Affinity.h"

namespace {
using libhpx::gas::Affinity;
using libhpx::gas::affinity::None;
}

None::None() : Affinity() {
}

None::~None()
{
}
