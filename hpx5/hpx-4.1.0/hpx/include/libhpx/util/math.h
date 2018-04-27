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

#ifndef HPX_UTIL_MATH_H
#define HPX_UTIL_MATH_H

#include <type_traits>

namespace libhpx {
namespace util {
namespace detail {
template <typename T, size_t Bytes = sizeof(T)>
struct CountLeadingZeros;

template <typename T>
struct CountLeadingZeros<T, 4> {
  static constexpr int op(T val) {
    return __builtin_clz(val);
  }
};

template <typename T>
struct CountLeadingZeros<T, 8> {
  static constexpr int op(T val) {
    return __builtin_clzll(val);
  }
};

template <>
struct CountLeadingZeros<unsigned long, sizeof(unsigned long)> {
  static constexpr int op(unsigned long val) {
    return __builtin_clzl(val);
  }
};
}

template <typename T>
inline constexpr char clz(T val) {
  return detail::CountLeadingZeros<T>::op(val);
}

template <typename T>
inline constexpr T ceil_log2(T val) {
  static_assert(std::is_unsigned<T>::value, "log2 of signed type undefined\n");
  return (T(sizeof(val) * 8 - 1) - clz(val)) + (!!(val & (val - 1)));
}

template <typename T>
inline constexpr T ceil2(T val) {
  return T(1) << ceil_log2(val);
}

template <typename T>
static inline T ceil_div(T num, T denom) {
  return (num / denom) + ((num % denom) ? 1 : 0);
}
} // namespace util
} // namespace libhpx

#endif // HPX_UTIL_MATH_H
