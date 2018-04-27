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

#ifndef LIBHPX_LCO_H
#define LIBHPX_LCO_H

#include "hpx/hpx.h"

typedef struct lco {
  tatas_lock_t lock;
  uint8_t      type;
  uint8_t     state;
} HPX_ALIGNED(16) lco_t;

#endif
