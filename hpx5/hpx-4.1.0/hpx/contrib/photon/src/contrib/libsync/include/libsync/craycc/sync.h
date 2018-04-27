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
#ifndef HPX_SYNC_CRAYCC_SYNC_H_
#define HPX_SYNC_CRAYCC_SYNC_H_

/*
  ====================================================================
  Defines our synchronization interface in terms of the craycc
  __sync builtins defined in "Cray C and C++ Reference Manual."

  http://docs.cray.com/books/S-2179-81/S-2179-81.pdf
  ====================================================================
*/

#include <intrinsics.h>

/* no memory model */
#define SYNC_RELAXED 0
#define SYNC_CONSUME 0
#define SYNC_ACQ_REL 0
#define SYNC_SEQ_CST 0
#define SYNC_ACQUIRE 0
#define SYNC_RELEASE 0


#define sync_load(addr, mm) *addr; __builtin_ia32_lfence()


#define sync_store(addr, val, mm) do {          \
    *addr = val;                                \
    __builtin_ia32_sfence();                    \
  } while (0)

#define sync_swap(addr, val, mm)                \
  __sync_lock_test_and_set(addr, val)

#define sync_cas(addr, from, to, onsuccess, onfailure) \
  (__sync_val_compare_and_swap(addr, from, to) == from)

#define sync_cas_val(addr, from, to, onsuccess, onfailure)  \
  __sync_val_compare_and_swap(addr, from, to)

#define sync_fadd(addr, val, mm) __sync_fetch_and_add(addr, val)
#define sync_addf(addr, val, mm) __sync_add_and_fetch(addr, val)

#define sync_fand(addr, val, mm) __sync_fetch_and_and(addr, val)
#define sync_andf(addr, val, mm) __sync_and_and_fetch(addr, val)

#define sync_for(addr, val, mm) __sync_fetch_and_or(addr, val)
#define sync_orf(addr, val, mm) __sync_or_and_fetch(addr, val)

#define sync_fxor(addr, val, mm) __sync_fetch_and_xor(addr, val)
#define sync_xorf(addr, val, mm) __sync_xor_and_fetch(addr, val)

#define sync_fence(mm) __builtin_ia32_mfence()

/* ../generic.h implements all of the strongly-typed versions in
 * terms of the above generic versions.
 */
#include "../generic.h"

#endif /* HPX_SYNC_CRAYCC_SYNC_H_ */
