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

#ifndef LIBPXGL_DEFS_H
#define LIBPXGL_DEFS_H

#include <inttypes.h>
#include <config.h>

#ifdef SSSP_64BIT
#define SSSP_UINT_T uint64_t
#define SSSP_UINT_PRI PRIu64
#define SSSP_UINT_MAX UINT64_MAX
#define SSSP_INT_T int64_t
#define SSSP_INT_PRI PRId64
#define SSSP_INT_MAX INT64_MAX
#elif defined(SSSP_32BIT)
#define SSSP_UINT_T uint32_t
#define SSSP_UINT_PRI PRIu32
#define SSSP_UINT_MAX UINT32_MAX
#define SSSP_INT_T int32_t
#define SSSP_INT_PRI PRId32
#define SSSP_INT_MAX INT32_MAX
#endif


typedef SSSP_UINT_T sssp_uint_t;
typedef SSSP_INT_T sssp_int_t;

#endif // LIBPXGL_DEFS_H
