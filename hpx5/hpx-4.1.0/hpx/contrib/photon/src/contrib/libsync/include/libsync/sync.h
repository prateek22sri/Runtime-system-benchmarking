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
#ifndef HPX_SYNC_SYNC_H_
#define HPX_SYNC_SYNC_H_

/*
  ====================================================================
  This file defines a basic, low-level interface to the
  synchronization primitives used in libhpx. This accounts for
  architecture and compiler-specific differences. It is designed based
  on gcc's builtin atomic interface.

  Wherever possible, we prefer compiler builtins to library-based, or
  inline asm-based implementations of this interface. These
  implementations are done in platform-specific headers, which we
  selectively include based on ifdefs.

  If that fails, this file provides a library-based interface to the
  synchronization that we desire.
  ====================================================================
*/

#ifndef __cplusplus
#include <stdint.h>
#include <stdbool.h>
#endif

#if defined(__ATOMIC_ACQUIRE)
#include "gcc/atomic.h"
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1)
#include "gcc/sync.h"
#elif defined(__INTEL_COMPILER)
#include "gcc/sync.h"
#elif defined(_CRAYC)
#include "craycc/sync.h"
#elif GCC_VERSION >= 40100 && GCC_VERSION < 40201
#include "gcc/sync.h"
#else

#if defined(__aarch64__)
#error "sync.h is not supported for AARCH64"
#endif

#define SYNC_RELAXED 0
#define SYNC_CONSUME 1
#define SYNC_ACQUIRE 2
#define SYNC_RELEASE 3
#define SYNC_ACQ_REL 4
#define SYNC_SEQ_CST 5

/*
  ====================================================================
  No builtin support for atomics, use a library-based implementation.
  ====================================================================
*/

#include <stdbool.h>
#include <stdint.h>
#include <hpx/attributes.h>

/*
  ====================================================================
  Load a value atomically. Where relaxed consistency is available,
  this is performed using "acquire" semantics.
  ====================================================================
*/

#define SYNC_INT_TY(T, s) T sync_load_##s(T *addr, int mm);
#include "types.def"
#undef SYNC_INT_TY

#if __STDC_VERSION__ == 201112L
#define sync_load(addr, mm)                     \
    _Generic((addr),                                \
        int8_t*      :sync_load_i8,             \
        int16_t*     :sync_load_i16,            \
        int32_t*     :sync_load_i32,            \
        int64_t*     :sync_load_i64,            \
        __int128_t*  :sync_load_i128,           \
        intptr_t*    :sync_load_ip,             \
        uint8_t*     :sync_load_ui8,            \
        uint16_t*    :sync_load_ui16,           \
        uint32_t*    :sync_load_ui32,           \
        uint64_t*    :sync_load_ui64,           \
        __uint128_t* :sync_load_ui128,          \
        uintptr_t*   :sync_load_uip,            \
        float*       :sync_load_f,              \
        double*      :sync_load_d,              \
        void**       :sync_load_p)(addr, mm)
#else
#warning No support for generic synchronization for your platform.
#endif

/*
  ====================================================================
  Store a value atomically. Where relaxed consistency is available,
  this is performed using "release" semantics.
  ====================================================================
*/

#define SYNC_INT_TY(T, s) void sync_store_##s(T *addr, T val, int mm) ;
#include "types.def"
#undef SYNC_INT_TY

#if __STDC_VERSION__ == 201112L
#define sync_store(addr, val, mm)                   \
    _Generic((addr),                                    \
        int8_t*      :sync_store_i8,                \
        int16_t*     :sync_store_i16,               \
        int32_t*     :sync_store_i32,               \
        int64_t*     :sync_store_i64,               \
        __int128_t*  :sync_store_i128,              \
        intptr_t*    :sync_store_ip,                \
        uint8_t*     :sync_store_ui8,               \
        uint16_t*    :sync_store_ui16,              \
        uint32_t*    :sync_store_ui32,              \
        uint64_t*    :sync_store_ui64,              \
        __uint128_t* :sync_store_ui128,             \
        uintptr_t*   :sync_store_uip,               \
        float*       :sync_store_f,                 \
        double*      :sync_store_d,                 \
        void**       :sync_store_p)(addr, val, mm)
#else
#warning No support for generic synchronization for your platform.
#endif

/*
  ====================================================================
  Atomically swap a value with memory.
  ====================================================================
*/

#define SYNC_INT_TY(T, s) T sync_swap_##s(T *addr, T val, int mm) ;
#include "types.def"
#undef SYNC_INT_TY

#if __STDC_VERSION__ == 201112L
#define sync_swap(addr, val, mm)                    \
    _Generic((addr),                                    \
        int8_t*      :sync_swap_i8,                 \
        int16_t*     :sync_swap_i16,                \
        int32_t*     :sync_swap_i32,                \
        int64_t*     :sync_swap_i64,                \
        __int128_t*  :sync_swap_i128,               \
        intptr_t*    :sync_swap_ip,                 \
        uint8_t*     :sync_swap_ui8,                \
        uint16_t*    :sync_swap_ui16,               \
        uint32_t*    :sync_swap_ui32,               \
        uint64_t*    :sync_swap_ui64,               \
        __uint128_t* :sync_swap_ui128,              \
        uintptr_t*   :sync_swap_uip,                \
        float*       :sync_swap_f,                  \
        double*      :sync_swap_d,                  \
        void**       :sync_swap_p)(addr, val, mm)
#else
#warning No support for generic synchronization for your platform.
#endif

/*
  ====================================================================
  Boolean compare-and-exchange.
  ====================================================================
*/

#define SYNC_INT_TY(T, s) T sync_cas_##s(T *addr, T from, T to, int onsuccess, \
                                  int onfailure);
#include "types.def"
#undef SYNC_INT_TY

#if __STDC_VERSION__ == 201112L
#define sync_cas(addr, from, to, onsucces, onfailure)               \
    _Generic((addr),                                                \
        int8_t*      :sync_cas_i8,                                  \
        int16_t*     :sync_cas_i16,                                 \
        int32_t*     :sync_cas_i32,                                 \
        int64_t*     :sync_cas_i64,                                 \
        __int128_t*  :sync_cas_i128,                                \
        intptr_t*    :sync_cas_ip,                                  \
        uint8_t*     :sync_cas_ui8,                                 \
        uint16_t*    :sync_cas_ui16,                                \
        uint32_t*    :sync_cas_ui32,                                \
        uint64_t*    :sync_cas_ui64,                                \
        __uint128_t* :sync_cas_ui128,                               \
        uintptr_t*   :sync_cas_uip,                                 \
        float*       :sync_cas_f,                                   \
        double*      :sync_cas_d,                                   \
        void**       :sync_cas_p)(addr, from, to, onsuccess, onfailure)
#else
#warning No support for generic synchronization for your platform.
#endif

/*
  ====================================================================
  Value-based compare and exchange, returns the actual value that was
  seen at the address.
  ====================================================================
*/
#define SYNC_INT_TY(T, s) T sync_cas_val_##s(T *addr, T from, T to, int onsuccess, \
                                      int onfailure);
#include "types.def"
#undef SYNC_INT_TY

#if __STDC_VERSION__ == 201112L
#define sync_cas_val(addr, from, to, onsucces, onfailure)           \
    _Generic((addr),                                                    \
        int8_t*      :sync_cas_val_i8,                              \
        int16_t*     :sync_cas_val_i16,                             \
        int32_t*     :sync_cas_val_i32,                             \
        int64_t*     :sync_cas_val_i64,                             \
        __int128_t*  :sync_cas_val_i128,                            \
        intptr_t*    :sync_cas_val_ip,                              \
        uint8_t*     :sync_cas_val_ui8,                             \
        uint16_t*    :sync_cas_val_ui16,                            \
        uint32_t*    :sync_cas_val_ui32,                            \
        uint64_t*    :sync_cas_val_ui64,                            \
        __uint128_t* :sync_cas_val_ui128,                           \
        uintptr_t*   :sync_cas_val_uip,                             \
        float*       :sync_cas_val_f,                               \
        double*      :sync_cas_val_d,                               \
        void**       :sync_cas_val_p)(addr, from, to, onsuccess, onfailure)
#else
#warning No support for generic synchronization for your platform.
#endif

/*
  ====================================================================
  Atomic fetch-and-add for integer types.
  ====================================================================
*/

#define SYNC_INT_TY(T, s) T sync_fadd_##s(T *addr, T val, int mm);
#include "types.def"
#undef SYNC_INT_TY

#if __STDC_VERSION__ == 201112L
#define sync_fadd(addr, val, mm)                        \
    _Generic((addr),                                        \
        int8_t*      :sync_fadd_i8,                     \
        int16_t*     :sync_fadd_i16,                    \
        int32_t*     :sync_fadd_i32,                    \
        int64_t*     :sync_fadd_i64,                    \
        __int128_t*  :sync_fadd_i128,                   \
        uint8_t*     :sync_fadd_ui8,                    \
        uint16_t*    :sync_fadd_ui16,                   \
        uint32_t*    :sync_fadd_ui32,                   \
        uint64_t*    :sync_fadd_ui64,                   \
        __uint128_t* :sync_fadd_ui128)(addr, val, mm)
#else
#warning No support for generic synchronization for your platform.
#endif

/*
  ====================================================================
  Atomic add-and-fetch for integer types.
  ====================================================================
*/

#define SYNC_INT_TY(T, s) T sync_addf_##s(T *addr, T val, int mm);
#include "types.def"
#undef SYNC_INT_TY

#if __STDC_VERSION__ == 201112L
#define sync_addf(addr, val, mm)                        \
    _Generic((addr),                                        \
        int8_t*      :sync_addf_i8,                     \
        int16_t*     :sync_addf_i16,                    \
        int32_t*     :sync_addf_i32,                    \
        int64_t*     :sync_addf_i64,                    \
        __int128_t*  :sync_addf_i128,                   \
        uint8_t*     :sync_addf_ui8,                    \
        uint16_t*    :sync_addf_ui16,                   \
        uint32_t*    :sync_addf_ui32,                   \
        uint64_t*    :sync_addf_ui64,                   \
        __uint128_t* :sync_addf_ui128)(addr, val, mm)
#else
#warning No support for generic synchronization for your platform.
#endif

/*
  ====================================================================
  Implements a fence based on the memory model.
  ====================================================================
*/

void sync_fence(int) HPX_PUBLIC;

#endif

#endif /* HPX_SYNC_SYNC_H_ */
