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

#ifndef HPX_BUILTINS_H
#define HPX_BUILTINS_H

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
/// @file builtins.h
///
/// This file encapsulates some operations that are builtin compiler
/// functions in order to support multiple compilers.
///
/// @todo Deal with non-gcc compatible compilers.
/// @todo Deal with this during configuration.
// ----------------------------------------------------------------------------

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

#ifdef HAVE___BUILTIN_EXPECT
# define likely(S) (__builtin_expect(S, 1))
# define unlikely(S) (__builtin_expect(S, 0))
#else
# define likely(S) (S)
# define unlikely(S) (S)
#endif

#ifdef HAVE___BUILTIN_UNREACHABLE
# define unreachable() __builtin_unreachable()
#else
# define unreachable()
#endif

#define ctzl(N) __builtin_ctzl(N)
#define clzl(N) __builtin_clzl(N)
#define clz(N) __builtin_clz(N)

#define popcountl(N) __builtin_popcountl(N)

#if (GCC_VERSION > 30500)
#define _HELPER0(x) #x
#define _HELPER1(x) _HELPER0(GCC diagnostic ignored x)
#define _HELPER2(y) _HELPER1(#y)
#define HPX_PUSH_IGNORE(S)                      \
  _Pragma("GCC diagnostic push")                \
  _Pragma(_HELPER2(S))
#define HPX_POP_IGNORE                          \
  _Pragma("GCC diagnostic pop")
#else
#define HPX_PUSH_IGNORE(S)
#define HPX_POP_IGNORE
#endif

#include <stdint.h>

/// From stack overflow.
///
/// http://stackoverflow.com/questions/3272424/compute-fast-log-base-2-ceiling
static inline uint32_t ceil_log2_32(uint32_t val) {
  return ((sizeof(val) * 8 - 1) - clz(val)) + (!!(val & (val - 1)));
}

static inline uint32_t ceil_log2_64(uint64_t val) {
  return ((sizeof(val) * 8 - 1) - clzl(val)) + (!!(val & (val - 1)));
}

static inline uint32_t ceil_div_32(uint32_t num, uint32_t denom) {
  return (num / denom) + ((num % denom) ? 1 : 0);
}

static inline uint64_t ceil_div_64(uint64_t num, uint64_t denom) {
  return (num / denom) + ((num % denom) ? 1 : 0);
}

static inline int min_int(int lhs, int rhs) {
  return (lhs < rhs) ? lhs : rhs;
}

static inline int32_t min_i32(int32_t lhs, int32_t rhs) {
  return (lhs < rhs) ? lhs : rhs;
}

static inline uint32_t min_u32(uint32_t lhs, uint32_t rhs) {
  return (lhs < rhs) ? lhs : rhs;
}

static inline uint64_t min_u64(uint64_t lhs, uint64_t rhs) {
  return (lhs < rhs) ? lhs : rhs;
}

static inline int32_t max_i32(int32_t lhs, int32_t rhs) {
  return (lhs < rhs) ? rhs : lhs;
}

static inline uint32_t max_u32(uint32_t lhs, uint32_t rhs) {
  return (lhs < rhs) ? rhs : lhs;
}

static inline uint64_t max_u64(uint64_t lhs, uint64_t rhs) {
  return (lhs < rhs) ? rhs : lhs;
}

#define _HPX_CAT(l,r) l##r
#define _HPX_CAT1(l,r) _HPX_CAT(l,r)
#define _HPX_CAT2(l,r) _HPX_CAT1(l,r)

#ifdef __LP64__
# define _HPX_BITS 64
#else
# define _HPX_BITS 32
#endif

#define ceil_log2_uintptr_t _HPX_CAT1(ceil_log2_, _HPX_BITS)
#define ceil_div_uintptr_t  _HPX_CAT1(ceil_div_, _HPX_BITS)
#define min_uintptr_t       _HPX_CAT1(min_u, _HPX_BITS)
#define max_uintptr_t       _HPX_CAT1(max_u, _HPX_BITS)

#define ceil_log2_size_t    _HPX_CAT1(ceil_log2_, _HPX_BITS)
#define ceil_div_size_t     _HPX_CAT1(ceil_div_, _HPX_BITS)
#define min_size_t          _HPX_CAT1(min_u, _HPX_BITS)
#define max_size_t          _HPX_CAT1(max_u, _HPX_BITS)

/// Miscellaneous utility macros.

/// Macros for static assertions
/// Source: http://stackoverflow.com/questions/3385515/static-assert-in-c
#define _HPX_ASSERT(COND,MSG) \
  typedef char static_assertion_##MSG[(!!(COND))*2-1]

#define _HPX_NELEM(e) (sizeof(e)/sizeof(e[0]))
#define _HPX_XSTR(s) _HPX_STR(s)
#define _HPX_STR(l) #l

/// Macro to count the number of variadic arguments
/// Source: https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
#define __HPX_NARGS(...) __VA_NARG__(__VA_ARGS__)
#define __VA_NARG__(...) __VA_NARG_(_0, ##__VA_ARGS__, __RSEQ_N())
#define __VA_NARG_(...) __VA_ARG_N(__VA_ARGS__)
#define __VA_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, \
                   _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, \
                   _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, \
                   _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
                   _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, \
                   _63, _,...) _

#define __RSEQ_N()  62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, \
    48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
    29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
    10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0

/// Make sure that the preprocessor will work for the empty NARGS macro. If the
/// user forces std=c99, then it will return 1 instead of 0. If you fail this
/// assertion you are probably trying to for strict c99 compliance.
#ifndef __cplusplus
_HPX_ASSERT(__HPX_NARGS() == 0, __c_preprocessor_is_not_gnu99_compatible);
#endif

#define _HPX_FOREACH_0(F)
#define _HPX_FOREACH_1(F, car) F(car)
#define _HPX_FOREACH_2(F, car, ...) F(car),_HPX_FOREACH_1(F, __VA_ARGS__)
#define _HPX_FOREACH_3(F, car, ...) F(car),_HPX_FOREACH_2(F, __VA_ARGS__)
#define _HPX_FOREACH_4(F, car, ...) F(car),_HPX_FOREACH_3(F, __VA_ARGS__)
#define _HPX_FOREACH_5(F, car, ...) F(car),_HPX_FOREACH_4(F, __VA_ARGS__)
#define _HPX_FOREACH_6(F, car, ...) F(car),_HPX_FOREACH_5(F, __VA_ARGS__)
#define _HPX_FOREACH_7(F, car, ...) F(car),_HPX_FOREACH_6(F, __VA_ARGS__)
#define _HPX_FOREACH_8(F, car, ...) F(car),_HPX_FOREACH_7(F, __VA_ARGS__)
#define _HPX_FOREACH_9(F, car, ...) F(car),_HPX_FOREACH_8(F, __VA_ARGS__)
#define _HPX_FOREACH_10(F, car, ...) F(car),_HPX_FOREACH_9(F, __VA_ARGS__)
#define _HPX_FOREACH_11(F, car, ...) F(car),_HPX_FOREACH_10(F, __VA_ARGS__)
#define _HPX_FOREACH_12(F, car, ...) F(car),_HPX_FOREACH_11(F, __VA_ARGS__)
#define _HPX_FOREACH_13(F, car, ...) F(car),_HPX_FOREACH_12(F, __VA_ARGS__)
#define _HPX_FOREACH_14(F, car, ...) F(car),_HPX_FOREACH_13(F, __VA_ARGS__)
#define _HPX_FOREACH_15(F, car, ...) F(car),_HPX_FOREACH_14(F, __VA_ARGS__)
#define _HPX_FOREACH_16(F, car, ...) F(car),_HPX_FOREACH_15(F, __VA_ARGS__)
#define _HPX_FOREACH_17(F, car, ...) F(car),_HPX_FOREACH_16(F, __VA_ARGS__)
#define _HPX_FOREACH_18(F, car, ...) F(car),_HPX_FOREACH_17(F, __VA_ARGS__)
#define _HPX_FOREACH_19(F, car, ...) F(car),_HPX_FOREACH_18(F, __VA_ARGS__)
#define _HPX_FOREACH_20(F, car, ...) F(car),_HPX_FOREACH_19(F, __VA_ARGS__)
#define _HPX_FOREACH_21(F, car, ...) F(car),_HPX_FOREACH_20(F, __VA_ARGS__)
#define _HPX_FOREACH_22(F, car, ...) F(car),_HPX_FOREACH_21(F, __VA_ARGS__)
#define _HPX_FOREACH_23(F, car, ...) F(car),_HPX_FOREACH_22(F, __VA_ARGS__)
#define _HPX_FOREACH_24(F, car, ...) F(car),_HPX_FOREACH_23(F, __VA_ARGS__)
#define _HPX_FOREACH_25(F, car, ...) F(car),_HPX_FOREACH_24(F, __VA_ARGS__)
#define _HPX_FOREACH_26(F, car, ...) F(car),_HPX_FOREACH_25(F, __VA_ARGS__)
#define _HPX_FOREACH_27(F, car, ...) F(car),_HPX_FOREACH_26(F, __VA_ARGS__)
#define _HPX_FOREACH_28(F, car, ...) F(car),_HPX_FOREACH_27(F, __VA_ARGS__)
#define _HPX_FOREACH_29(F, car, ...) F(car),_HPX_FOREACH_28(F, __VA_ARGS__)
#define _HPX_FOREACH_30(F, car, ...) F(car),_HPX_FOREACH_29(F, __VA_ARGS__)
#define _HPX_FOREACH_31(F, car, ...) F(car),_HPX_FOREACH_30(F, __VA_ARGS__)
#define _HPX_FOREACH_32(F, car, ...) F(car),_HPX_FOREACH_31(F, __VA_ARGS__)
#define _HPX_FOREACH_33(F, car, ...) F(car),_HPX_FOREACH_32(F, __VA_ARGS__)
#define _HPX_FOREACH_34(F, car, ...) F(car),_HPX_FOREACH_33(F, __VA_ARGS__)
#define _HPX_FOREACH_35(F, car, ...) F(car),_HPX_FOREACH_34(F, __VA_ARGS__)
#define _HPX_FOREACH_36(F, car, ...) F(car),_HPX_FOREACH_35(F, __VA_ARGS__)
#define _HPX_FOREACH_37(F, car, ...) F(car),_HPX_FOREACH_36(F, __VA_ARGS__)
#define _HPX_FOREACH_38(F, car, ...) F(car),_HPX_FOREACH_37(F, __VA_ARGS__)
#define _HPX_FOREACH_39(F, car, ...) F(car),_HPX_FOREACH_38(F, __VA_ARGS__)
#define _HPX_FOREACH_40(F, car, ...) F(car),_HPX_FOREACH_39(F, __VA_ARGS__)
#define _HPX_FOREACH_41(F, car, ...) F(car),_HPX_FOREACH_40(F, __VA_ARGS__)
#define _HPX_FOREACH_42(F, car, ...) F(car),_HPX_FOREACH_41(F, __VA_ARGS__)
#define _HPX_FOREACH_43(F, car, ...) F(car),_HPX_FOREACH_42(F, __VA_ARGS__)
#define _HPX_FOREACH_44(F, car, ...) F(car),_HPX_FOREACH_43(F, __VA_ARGS__)
#define _HPX_FOREACH_45(F, car, ...) F(car),_HPX_FOREACH_44(F, __VA_ARGS__)
#define _HPX_FOREACH_46(F, car, ...) F(car),_HPX_FOREACH_45(F, __VA_ARGS__)
#define _HPX_FOREACH_47(F, car, ...) F(car),_HPX_FOREACH_46(F, __VA_ARGS__)
#define _HPX_FOREACH_48(F, car, ...) F(car),_HPX_FOREACH_47(F, __VA_ARGS__)
#define _HPX_FOREACH_49(F, car, ...) F(car),_HPX_FOREACH_48(F, __VA_ARGS__)
#define _HPX_FOREACH_50(F, car, ...) F(car),_HPX_FOREACH_49(F, __VA_ARGS__)
#define _HPX_FOREACH_51(F, car, ...) F(car),_HPX_FOREACH_50(F, __VA_ARGS__)
#define _HPX_FOREACH_52(F, car, ...) F(car),_HPX_FOREACH_51(F, __VA_ARGS__)
#define _HPX_FOREACH_53(F, car, ...) F(car),_HPX_FOREACH_52(F, __VA_ARGS__)
#define _HPX_FOREACH_54(F, car, ...) F(car),_HPX_FOREACH_53(F, __VA_ARGS__)
#define _HPX_FOREACH_55(F, car, ...) F(car),_HPX_FOREACH_54(F, __VA_ARGS__)
#define _HPX_FOREACH_56(F, car, ...) F(car),_HPX_FOREACH_55(F, __VA_ARGS__)
#define _HPX_FOREACH_57(F, car, ...) F(car),_HPX_FOREACH_56(F, __VA_ARGS__)
#define _HPX_FOREACH_58(F, car, ...) F(car),_HPX_FOREACH_57(F, __VA_ARGS__)
#define _HPX_FOREACH_59(F, car, ...) F(car),_HPX_FOREACH_58(F, __VA_ARGS__)
#define _HPX_FOREACH_60(F, car, ...) F(car),_HPX_FOREACH_59(F, __VA_ARGS__)
#define _HPX_FOREACH_61(F, car, ...) F(car),_HPX_FOREACH_60(F, __VA_ARGS__)
#define _HPX_FOREACH_62(F, car, ...) F(car),_HPX_FOREACH_61(F, __VA_ARGS__)
#define _HPX_FOREACH_63(F, car, ...) F(car),_HPX_FOREACH_62(F, __VA_ARGS__)

#define __HPX_FOREACH(F, ...)                                       \
  _HPX_CAT1(_HPX_FOREACH_, __HPX_NARGS(__VA_ARGS__))(F, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // HPX_BUILTINS_H
