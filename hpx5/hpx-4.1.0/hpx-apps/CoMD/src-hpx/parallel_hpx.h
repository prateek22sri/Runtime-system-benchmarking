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

// Wrappers for MPI-kind of functionality.

#ifndef _PARALLEL_HPX_H_
#define _PARALLEL_HPX_H_

#include "mytype.h"

/// Structure for use for MPI_MINLOC and MPI_MAXLOC like operations.
typedef struct RankReduceDataSt {
  double val;
  int rank;
} RankReduceData;

/// Return total number of processors.
int getNRanks(void);

/// Return local rank.
int getMyRank(void);

/// Return non-zero if printing occurs from this rank.
int printRank(void);

/// Print a timestamp and message when all tasks arrive.
void timestampBarrier(const char* msg);

/// Wrapper for Barrier operation.
void barrierParallel(void);

/// Wrapper for Allreduce integer sum.
void addIntParallel(int* sendBuf, int* recvBuf, int count);

/// Wrapper for Allreduce real sum.
void addRealParallel(real_t* sendBuf, real_t* recvBuf, int count);

/// Wrapper for Allreduce double sum.
void addDoubleParallel(double* sendBuf, double* recvBuf, int count);

/// Wrapper for Allreduce integer max.
void maxIntParallel(int* sendBuf, int* recvBuf, int count);

/// Wrapper for Allreduce double min with rank.
void minRankDoubleParallel(RankReduceData* sendBuf, RankReduceData* recvBuf,
                           int count);

/// Wrapper for Allreduce double max with rank.
void maxRankDoubleParallel(RankReduceData* sendBuf, RankReduceData* recvBuf,
                           int count);

/// Wrapper for Bcast
void bcastParallel(void* buf, int len, int root);

#endif

