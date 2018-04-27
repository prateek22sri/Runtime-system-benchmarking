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

#ifndef LIBPXGL_ATOMICS_H
#define LIBPXGL_ATOMICS_H

#ifdef __INTEL_COMPILER
#define _Atomic volatile

#undef atomic_load_explicit
#define atomic_load_explicit __atomic_load_explicit

#undef atomic_fetch_add_explicit
#define atomic_fetch_add_explicit __atomic_fetch_add_explicit

#undef atomic_compare_exchange_strong_explicit
#define atomic_compare_exchange_strong_explicit __atomic_compare_exchange_strong_explicit

#undef atomic_thread_fence
#define atomic_thread_fence __atomic_thread_fence

#undef atomic_store_explicit
#define atomic_store_explicit __atomic_store_explicit

#endif //__INTEL_COMPILER

#endif // LIBPXGL_ATOMICS_H
