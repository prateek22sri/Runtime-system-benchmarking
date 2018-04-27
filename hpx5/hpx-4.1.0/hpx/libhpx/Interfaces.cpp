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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/// Provide a home for vtables for abstract interfaces.

#include "libhpx/CollectiveOps.h"
#include "libhpx/LCOOps.h"
#include "libhpx/MemoryOps.h"
#include "libhpx/ParcelOps.h"
#include "libhpx/StringOps.h"

libhpx::CollectiveOps::~CollectiveOps()
{
}

libhpx::LCOOps::~LCOOps()
{
}

libhpx::MemoryOps::~MemoryOps()
{
}

libhpx::ParcelOps::~ParcelOps()
{
}

libhpx::StringOps::~StringOps()
{
}
