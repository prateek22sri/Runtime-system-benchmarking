// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

#ifndef LULESH_ATOMICS_H
#define LULESH_ATOMICS_H

#ifdef __INTEL_COMPILER
#define _Atomic volatile
#undef atomic_fetch_add_explicit
#define atomic_fetch_add_explicit __atomic_fetch_add_explicit
#endif

#endif // LULESH_ATOMICS_H
