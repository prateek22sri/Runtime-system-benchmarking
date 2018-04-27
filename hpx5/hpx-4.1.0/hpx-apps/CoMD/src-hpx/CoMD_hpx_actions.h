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

#ifndef COMD_HPX_ACTIONS_H_
#define COMD_HPX_ACTIONS_H_

#include "CoMDTypes.h"
#include "parallel_hpx.h"

void initint_action(global_int_t *input, const size_t size);
void sumint_action(global_int_t *output, const global_int_t *input,
      const size_t size);
void initreal_action(global_real_t *input, const size_t size);
void sumreal_action(global_real_t *output, const global_real_t *input,
      const size_t size);
void maxint_action(global_int_t *output, const global_int_t *input,
      const size_t size);
void initminRank_action(RankReduceData *input, const size_t size);
void minRank_action(RankReduceData *output, const RankReduceData *input,
      const size_t size);
void initmaxRank_action(RankReduceData *input, const size_t size);

void maxRank_action(RankReduceData *output, const RankReduceData *input,
      const size_t size);

int initCommunication_action(Communication *ld, size_t size);

#endif /* COMD_HPX_ACTIONS_H_ */
