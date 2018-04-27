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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <assert.h>

#include "CoMD_hpx_actions.h"
#include <float.h>

extern Reductions reduce;

void initint_action(global_int_t *input, const size_t size) {
   assert(sizeof(global_int_t) == size);
   *input = 0;
}

void sumint_action(global_int_t *output, const global_int_t *input,
      const size_t size) {
   assert(sizeof(global_int_t) == size);
   *output += *input;
}

void initreal_action(global_real_t *input, const size_t size) {
   assert(sizeof(global_real_t) == size);
   *input = 0;
}

void sumreal_action(global_real_t *output, const global_real_t *input,
      const size_t size) {
   assert(sizeof(global_real_t) == size);
   *output += *input;
}

void maxint_action(global_int_t *output, const global_int_t *input,
      const size_t size) {
   assert(sizeof(global_int_t) == size);
   if (*output < *input)
      *output = *input;
}

void initminRank_action(RankReduceData *input, const size_t size) {
   assert(sizeof(RankReduceData) == size);
   input->rank = 0;
   input->val = DBL_MAX;
}

void minRank_action(RankReduceData *output, const RankReduceData *input,
      const size_t size) {
   assert(sizeof(RankReduceData) == size);
   if (output->val > input->val) {
      output->val = input->val;
      output->rank = input->rank;
   }
}

void initmaxRank_action(RankReduceData *input, const size_t size) {
   assert(sizeof(RankReduceData) == size);
   input->rank = 0;
   input->val = 0;
}

void maxRank_action(RankReduceData *output, const RankReduceData *input,
      const size_t size) {
   assert(sizeof(RankReduceData) == size);
   if (output->val < input->val) {
      output->val = input->val;
      output->rank = input->rank;
   }
}

int initCommunication_action(Communication *ld, size_t size) {

   reduce.allreduce = hpx_lco_future_new(sizeof(global_int_t));
   reduce.reduction_sum = ld->reduction_sum;
   reduce.reduceid = hpx_process_collective_allreduce_subscribe(
         reduce.reduction_sum, hpx_lco_set_action, reduce.allreduce);

   reduce.dallreduce = hpx_lco_future_new(sizeof(global_real_t));
   reduce.dreduction_sum = ld->dreduction_sum;
   reduce.dreduceid = hpx_process_collective_allreduce_subscribe(
         reduce.dreduction_sum, hpx_lco_set_action, reduce.dallreduce);

   reduce.allreduce_max = hpx_lco_future_new(sizeof(global_int_t));
   reduce.reduction_max = ld->reduction_max;
   reduce.reducemaxid = hpx_process_collective_allreduce_subscribe(
         reduce.reduction_max, hpx_lco_set_action, reduce.allreduce_max);

   reduce.allreduce_minRank = hpx_lco_future_new(sizeof(RankReduceData));
   reduce.reduction_minRank = ld->reduction_minRank;
   reduce.reduceminRankid = hpx_process_collective_allreduce_subscribe(
         reduce.reduction_minRank, hpx_lco_set_action,
         reduce.allreduce_minRank);

   reduce.allreduce_maxRank = hpx_lco_future_new(sizeof(RankReduceData));
   reduce.reduction_maxRank = ld->reduction_maxRank;
   reduce.reducemaxRankid = hpx_process_collective_allreduce_subscribe(
         reduce.reduction_maxRank, hpx_lco_set_action,
         reduce.allreduce_maxRank);

   return HPX_SUCCESS;
}
