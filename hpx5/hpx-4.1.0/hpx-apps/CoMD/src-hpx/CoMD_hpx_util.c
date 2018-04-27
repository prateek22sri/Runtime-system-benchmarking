// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2016-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

#include <hpx/hpx.h>
#include "CoMDTypes.h"

hpx_addr_t sim_element_at(hpx_addr_t sim, int i) {
   return hpx_addr_add(sim, sizeof(SimFlat) * i, sizeof(SimFlat));
}
