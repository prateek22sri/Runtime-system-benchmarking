// =============================================================================
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/// @file libhpx/scheduler/lco/monoid.c
/// @brief Implements reductions for "reduce" LCOs.

#include <cstdlib>
#include <float.h>
#include <climits>

#define _HPX_REDUCTION_SUM_DEF(TYPE, dtype, initializer)  \
  void HPX_##TYPE##_SUM_ID(dtype *i, size_t UNUSED) {     \
    *i = initializer;                                     \
  }                                                       \
  void HPX_##TYPE##_SUM_OP(dtype *i, const dtype *j,      \
                           size_t UNUSED) {               \
    *i += *j;                                             \
  }

_HPX_REDUCTION_SUM_DEF(INT, int, 0);
_HPX_REDUCTION_SUM_DEF(DOUBLE, double, 0.0);
_HPX_REDUCTION_SUM_DEF(FLOAT, float, 0.0);

#define _HPX_REDUCTION_PROD_DEF(TYPE, dtype, initializer) \
  void HPX_##TYPE##_PROD_ID(dtype *i, size_t UNUSED) {    \
    *i = initializer;                                     \
  }                                                       \
  void HPX_##TYPE##_PROD_OP(dtype *i, const dtype *j,     \
                            size_t UNUSED) {              \
    *i *= *j;                                             \
  }

_HPX_REDUCTION_PROD_DEF(INT, int, 1);
_HPX_REDUCTION_PROD_DEF(DOUBLE, double, 1.0);
_HPX_REDUCTION_PROD_DEF(FLOAT, float, 1.0);

#define _HPX_REDUCTION_MAX_DEF(TYPE, dtype, initializer)  \
  void HPX_##TYPE##_MAX_ID(dtype *i, size_t UNUSED) {     \
    *i = initializer;                                     \
  }                                                       \
  void HPX_##TYPE##_MAX_OP(dtype *i, const dtype *j,      \
                           size_t UNUSED) {               \
    *i = (*i > *j) ? *i : *j;                             \
  }

_HPX_REDUCTION_MAX_DEF(INT, int, -INT_MAX);
_HPX_REDUCTION_MAX_DEF(DOUBLE, double, -DBL_MAX);
_HPX_REDUCTION_MAX_DEF(FLOAT, float, -FLT_MAX);

#define _HPX_REDUCTION_MIN_DEF(TYPE, dtype, initializer)  \
  void HPX_##TYPE##_MIN_ID(dtype *i, size_t UNUSED) {     \
    *i = initializer;                                     \
  }                                                       \
  void HPX_##TYPE##_MIN_OP(dtype *i, const dtype *j,      \
                           size_t UNUSED) {               \
    *i = (*i < *j) ? *i : *j;                             \
  }

_HPX_REDUCTION_MIN_DEF(INT, int, INT_MAX);
_HPX_REDUCTION_MIN_DEF(DOUBLE, double, DBL_MAX);
_HPX_REDUCTION_MIN_DEF(FLOAT, float, FLT_MAX);
