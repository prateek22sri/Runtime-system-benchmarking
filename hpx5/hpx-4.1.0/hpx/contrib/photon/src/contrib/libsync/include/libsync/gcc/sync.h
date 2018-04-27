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
#ifndef HPX_SYNC_GCC_SYNC_H_
#define HPX_SYNC_GCC_SYNC_H_

/*
  ====================================================================
  Defines our synchronization interface in terms of the older gcc
  __sync builtins defined in
  http://gcc.gnu.org/onlinedocs/gcc/_005f_005fsync-Builtins.html#g_t_005f_005fsync-Builtins
  ====================================================================
*/

/* only pay attention in sync_load/sync_store */
#define SYNC_RELAXED 0
#define SYNC_CONSUME 1
#define SYNC_ACQ_REL 2
#define SYNC_SEQ_CST 3
#define SYNC_ACQUIRE 4
#define SYNC_RELEASE 5

/*
 * Extremely annoying that val is required here, but without using GNU
 * extensions I can't figure out a good way to deal with the compiler barrier.
 */
#if defined(__x86_64__) || defined(__ARMEL__)
# define sync_load(addr, mm) ({ __typeof(*(addr)) a = *(addr); __asm volatile ("":::"memory"); a; })
#else
#error sync_load not implemented for your processor
#endif

/*
 * Synchronizing a store requires that all previous operations complete before
 * the store occurs. Normal TSO (and x86-TSO) provides this in hardware (the
 * store won't bypass previous loads or stores), so we just need to make sure
 * that the compiler understands not to reorder the store with previous
 * operations.
 */
#if defined(__x86_64__) || defined(__ARMEL__)
#define sync_store(addr, val, mm) do {              \
    __asm volatile ("":::"memory");                 \
    *(addr) = val;                                  \
  } while (0)
#else
#error sync store not implemented for your processor
#endif

#define sync_swap(addr, val, mm) __sync_lock_test_and_set (addr, val)

#define sync_cas(addr, from, to, onsuccess, onfailure) ({               \
      __typeof(to) expected = *(from);                                  \
      __typeof(to) actual = __sync_val_compare_and_swap(addr, expected, to); \
      bool success = (actual == expected);                              \
      if (!success) {                                                   \
        *(from) = actual;                                               \
      }                                                                 \
      success;                                                          \
    })

#define sync_fadd(addr, val, mm) __sync_fetch_and_add(addr, val)
#define sync_addf(addr, val, mm) __sync_add_and_fetch(addr, val)

#define sync_fand(addr, val, mm) __sync_fetch_and_and(addr, val)
#define sync_andf(addr, val, mm) __sync_and_and_fetch(addr, val)

#define sync_for(addr, val, mm) __sync_fetch_and_or(addr, val)
#define sync_orf(addr, val, mm) __sync_or_and_fetch(addr, val)

#define sync_fxor(addr, val, mm) __sync_fetch_and_xor(addr, val)
#define sync_xorf(addr, val, mm) __sync_xor_and_fetch(addr, val)

#define sync_fence(mm) __sync_synchronize()

/* ../generic.h implements all of the strongly-typed versions in
 * terms of the above generic versions.
 */
#include "../generic.h"

#endif /* HPX_SYNC_GCC_SYNC_H_ */
