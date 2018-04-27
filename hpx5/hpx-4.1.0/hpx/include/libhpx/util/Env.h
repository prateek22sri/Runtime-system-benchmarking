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

#ifndef HPX_UTIL_ENV_H
#define HPX_UTIL_ENV_H

#include <algorithm>
#include <string>
#include <sstream>

namespace libhpx {
namespace util {

class NotFound : public std::exception {
};

/// Get a value of type T from the environment associated with a
/// case-insensitive key @p key.
template <typename T>
inline T getEnv(const char * const key) {
  char* env = std::getenv(key);
  if (!env) {
    std::string upper(key);
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    env = std::getenv(upper.c_str());
  }

  if (!env) {
    throw NotFound();
  }

  std::istringstream is(env);
  T value;
  is >> value;
  return value;
}

} // namespace util
} // namespace libhpx

#endif // HPX_UTIL_ENV_H
