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

#ifndef LIBHPX_PROCESS_H
#define LIBHPX_PROCESS_H

#include <hpx/hpx.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Recover any credit associated with a parcel.
int process_recover_credit(hpx_parcel_t *p)
  HPX_NON_NULL(1);

#ifdef __cplusplus
}
#endif

#endif // LIBHPX_PROCESS_H
