// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2016, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#pragma once
#ifndef HPX_SYNC_GCC_ATOMIC_H_
#define HPX_SYNC_GCC_ATOMIC_H_

/*
  ====================================================================
  The gcc-builtin atomic mapping. Just uses the builtins from
  http://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
  directly.
  ====================================================================
*/

#define SYNC_RELAXED __ATOMIC_RELAXED
#define SYNC_CONSUME __ATOMIC_CONSUME
#define SYNC_ACQ_REL __ATOMIC_ACQ_REL
#define SYNC_SEQ_CST __ATOMIC_SEQ_CST

#ifdef __ATOMIC_HLE_ACQUIRE
#define SYNC_ACQUIRE __ATOMIC_ACQUIRE | __ATOMIC_HLE_ACQUIRE
#else
#define SYNC_ACQUIRE __ATOMIC_ACQUIRE
#endif

#ifdef __ATOMIC_HLE_RELEASE
#define SYNC_RELEASE __ATOMIC_RELEASE | __ATOMIC_HLE_RELEASE
#else
#define SYNC_RELEASE __ATOMIC_RELEASE
#endif

#define sync_load(addr, mm) __atomic_load_n(addr, mm)
#define sync_store(addr, val, mm) __atomic_store_n(addr, val, mm)
#define sync_swap(addr, val, mm) __atomic_exchange_n(addr, val, mm)
#define sync_cas(addr, from, to, onsuccess, onfailure)              \
    __atomic_compare_exchange_n(addr, from, to, false, onsuccess, onfailure)

#define sync_fadd(addr, val, mm) __atomic_fetch_add(addr, val, mm)
#define sync_addf(addr, val, mm) __atomic_add_fetch(addr, val, mm)

#define sync_fand(addr, val, mm) __atomic_fetch_and(addr, val, mm)
#define sync_andf(addr, val, mm) __atomic_and_fetch(addr, val, mm)


#define sync_for(addr, val, mm) __atomic_fetch_or(addr, val, mm)
#define sync_orf(addr, val, mm) __atomic_or_fetch(addr, val, mm)

#define sync_fxor(addr, val, mm) __atomic_fetch_xor(addr, val, mm)
#define sync_xorf(addr, val, mm) __atomic_xor_fetch(addr, val, mm)

#define sync_fence(mm) __atomic_thread_fence(mm)

/* ../generic.h implements all of the strongly-typed versions in
 * terms of the above generic versions.
 */
#include "../generic.h"

#endif /* HPX_SYNC_GCC_ATOMIC_H_ */
