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

// Functions to perform reduction

#ifndef __REDUCTION_HELPER_H_
#define __REDUCTION_HELPER_H_

#include "CoMDTypes.h"
#include "parallel_hpx.h"
void CoMD_hpx_int_reduction(local_int_t local, global_int_t * global);
void CoMD_hpx_real_reduction(local_real_t local, global_real_t * global);
void CoMD_hpx_int_reduction_max(local_int_t local, global_int_t * global);
void CoMD_hpx_Rank_reduction_min(RankReduceData local, RankReduceData *global);
void CoMD_hpx_Rank_reduction_max(RankReduceData local, RankReduceData *global);


#endif
