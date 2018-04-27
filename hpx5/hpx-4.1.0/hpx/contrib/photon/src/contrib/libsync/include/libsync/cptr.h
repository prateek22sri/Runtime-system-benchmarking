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

#ifndef HPX_SYNC_CPTR_H
#define HPX_SYNC_CPTR_H

#include <stdint.h>
#include <hpx/attributes.h>
#include "sync.h"

/// @struct cptr_t
/// ----------------------------------------------------------------------------
/// A "counted pointer" structure.
///
/// This common pattern is used in non-blocking algorithms where CAS is the
/// primitive atomic operation. It represents a normal pointer, where each CAS
/// updates the "version" of the pointer, and avoids the ABA problem for up to
/// 64-bits worth of CAS operations.
///
/// It relies on having access to a CAS that is twice as big as a pointer.
///
/// @var cptr_t::p
/// the actual pointer that we're protecting
/// @var cptr_t::c
/// the count of the number of times this pointer has been CASed
/// ----------------------------------------------------------------------------
#ifdef __LP64__
typedef struct {
  void *p;
  uint64_t c;
} cptr_t __attribute__((aligned(16)));
#else
typedef struct {
  void *p;
  uint32_t c;
} cptr_t __attribute__((aligned(8)));
#endif

#define SYNC_CPTR_INITIALIZER { .p = NULL, .c = 0 }

/// ----------------------------------------------------------------------------
/// CAS a counter pointer.
///
/// This performs a compare-and-swap of the counted pointer, attempting to
/// atomically update @p ptr ->p from @p from ->p to @p to while updating the
/// count from @p from ->c to @p from ->p + 1. If either the pointer or count is
/// not correct, it will fail and return the actual "seen" value in *from.
/// This does not update @p from and thus isn't really a CAS.
///
/// @param  ptr the memory location we are going to cas
/// @param from the value we're expecting to see at ptr
/// @param   to the address that we're trying to update ptr to
void sync_cptr_cas_val(volatile cptr_t *ptr, const cptr_t *from, const void *to)
  HPX_PUBLIC;

/// ----------------------------------------------------------------------------
/// CAS a counted pointer.
///
/// This performs a compare-and-swap of the counted pointer, returning true if
/// it succeeds and false if it fails. Pass the from by-value, and don't tell
/// gcc about it, because it doesn't make any
/// ----------------------------------------------------------------------------
bool sync_cptr_cas(volatile cptr_t *ptr, const cptr_t *from, const void *to)
  HPX_PUBLIC;

/// ----------------------------------------------------------------------------
/// Checks to see if a counted pointer has changed.
///
/// Because we need to use an atomic cmpchg16b to read a location anyway, we can
/// find out if it has changed by supplying an expected value.
/// ----------------------------------------------------------------------------
bool sync_cptr_is_consistent(volatile cptr_t *ptr, const cptr_t *val)
  HPX_PUBLIC;

/// ----------------------------------------------------------------------------
/// Atomically load a counted pointer.
///
/// For x86_64, which we've implemented here, the only valid way to read a 16
/// byte memory address atomically is with the cmpxch16b instruction.
/// ----------------------------------------------------------------------------
void sync_cptr_load(volatile cptr_t *ptr, cptr_t *out) HPX_PUBLIC;

#endif // HPX_SYNC_CPTR_H
