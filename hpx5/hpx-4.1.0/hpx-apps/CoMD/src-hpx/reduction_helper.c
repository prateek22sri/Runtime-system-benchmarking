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

#include "reduction_helper.h"

#include "parallel_hpx.h"

//extern HPCG_Params params;
extern Reductions reduce;
//typedef int local_int_t;
//typedef long long global_int_t;

void CoMD_hpx_int_reduction(local_int_t local, global_int_t *global) {
  global_int_t lnnz = local;

  hpx_process_collective_allreduce_join(reduce.reduction_sum, reduce.reduceid,
      sizeof(global_int_t), &lnnz);

  hpx_status_t e = hpx_lco_get_reset(reduce.allreduce, sizeof(global_int_t),
      global);
  if (e != HPX_SUCCESS)
    printf("reset not success!\n");

}

void CoMD_hpx_real_reduction(local_real_t local, global_real_t *global) {
  global_real_t lnnz = local;
  hpx_process_collective_allreduce_join(reduce.dreduction_sum, reduce.dreduceid,
      sizeof(global_real_t), &lnnz);

  hpx_status_t e = hpx_lco_get_reset(reduce.dallreduce, sizeof(global_real_t),
      global);
  if (e != HPX_SUCCESS)
    printf("reset not success!\n");

  //printf("called -> hpx_lco_get_reset on real\n");
}

void CoMD_hpx_int_reduction_max(local_int_t local, global_int_t *global) {
  global_int_t lnnz = local;

  hpx_process_collective_allreduce_join(reduce.reduction_max,
      reduce.reducemaxid, sizeof(global_int_t), &lnnz);

  hpx_status_t e = hpx_lco_get_reset(reduce.allreduce_max, sizeof(global_int_t),
      global);
  if (e != HPX_SUCCESS)
    printf("reset not success!\n");
  //printf("called -> hpx_lco_get_reset\n");

}

void CoMD_hpx_Rank_reduction_min(RankReduceData local, RankReduceData *global) {
  RankReduceData l;
  l.rank = local.rank;
  l.val = local.val;

  hpx_process_collective_allreduce_join(reduce.reduction_minRank,
      reduce.reduceminRankid, sizeof(RankReduceData), &l);

  hpx_status_t e = hpx_lco_get_reset(reduce.allreduce_minRank,
      sizeof(RankReduceData), global);
  if (e != HPX_SUCCESS)
    printf("CoMD_hpx_Rank_reduction_min reset not success!\n");

}

void CoMD_hpx_Rank_reduction_max(RankReduceData local, RankReduceData *global) {
  RankReduceData l;
  l.rank = local.rank;
  l.val = local.val;

  hpx_process_collective_allreduce_join(reduce.reduction_maxRank,
      reduce.reducemaxRankid, sizeof(RankReduceData), &l);

  hpx_status_t e = hpx_lco_get_reset(reduce.allreduce_maxRank,
      sizeof(RankReduceData), global);
  if (e != HPX_SUCCESS)
    printf("CoMD_hpx_Rank_reduction_min reset not success!\n");

}

