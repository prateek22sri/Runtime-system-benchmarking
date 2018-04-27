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
#include "parallel_hpx.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "reduction_helper.h"

int getNRanks() {
  return hpx_get_num_ranks();
}

int getMyRank() {
  return hpx_get_my_rank();
}

/// \details
/// For now this is just a check for rank 0 but in principle it could be
/// more complex.  It is also possible to suppress practically all
/// output by causing this function to return 0 for all ranks.
int printRank() {
  if (hpx_get_my_rank() == 0)
    return 1;
  return 0;
}

void timestampBarrier(const char* msg) {
  barrierParallel();
  if (!printRank())
    return;
  time_t t = time(NULL);
  char* timeString = ctime(&t);
  timeString[24] = '\0'; // clobber newline
  fprintf(screenOut, "%s: %s\n", timeString, msg);
  fflush(screenOut);
}

void barrierParallel() {
}

void addIntParallel(int* sendBuf, int* recvBuf, int count) {
  //printf("count = %d\n",count);
  for (int i = 0; i < count; i++) {
    int send = sendBuf[i];
    global_int_t recv = 0; //recvBuf[i];
    CoMD_hpx_int_reduction((local_int_t) send, (global_int_t *) &recv);
    recvBuf[i] = recv;
  }
}

void addRealParallel(real_t* sendBuf, real_t* recvBuf, int count) {

  //printf("real comm count = %d\n",count);
  for (int i = 0; i < count; i++) {
    local_real_t send = sendBuf[i];
    global_real_t recv = 0.0; //recvBuf[i];
    CoMD_hpx_real_reduction((local_real_t) send, (global_real_t *) &recv);
    recvBuf[i] = recv;
    //printf("send = %lg and recv = %lg\n",send, recv);
  }

}

//From CoMD code there is an option to either go for single precision
// or double, here in this hpx port of CoMD we are only considering
//double precision so that redundancy comes from there.
void addDoubleParallel(double* sendBuf, double* recvBuf, int count) {
  for (int i = 0; i < count; i++) {
    local_real_t send = sendBuf[i];
    global_real_t recv = 0.0; //recvBuf[i];
    CoMD_hpx_real_reduction((local_real_t) send, (global_real_t *) &recv);
    recvBuf[i] = recv;

  }

}

void maxIntParallel(int* sendBuf, int* recvBuf, int count) {

  for (int i = 0; i < count; i++) {
    local_int_t send = sendBuf[i];
    global_int_t recv = 0; //recvBuf[i];
    CoMD_hpx_int_reduction_max((local_int_t) send, (global_int_t *) &recv);
    recvBuf[i] = recv;
    //printf("send = %d and recv = %d\n",send, recv);
  }

}

void minRankDoubleParallel(RankReduceData* sendBuf, RankReduceData* recvBuf,
                           int count) {

  //printf("count = %d\n",count);
  for (int i = 0; i < count; i++) {
    RankReduceData send;
    send.rank = sendBuf[i].rank;
    send.val = (double) sendBuf[i].val;
    RankReduceData recv; //recvBuf[i];
    recv.rank = 0;
    recv.val = (double) 0.0;
    CoMD_hpx_Rank_reduction_min((RankReduceData) send,
        (RankReduceData *) &recv);
    recvBuf[i].rank = recv.rank;
    recvBuf[i].val = (double) recv.val;

    //printf("send.rank = %d send.val = %lg and recv.rank = %d recv.val = %lg\n",send.rank, send.val,
    //		recv.rank, recv.val);
  }

}

void maxRankDoubleParallel(RankReduceData* sendBuf, RankReduceData* recvBuf,
                           int count) {

  //printf("count = %d\n",count);
  for (int i = 0; i < count; i++) {
    RankReduceData send;
    send.rank = sendBuf[i].rank;
    send.val = (double) sendBuf[i].val;
    RankReduceData recv;		//recvBuf[i];
    recv.rank = 0;
    recv.val = (double) 0.0;
    CoMD_hpx_Rank_reduction_max((RankReduceData) send,
        (RankReduceData *) &recv);
    recvBuf[i].rank = recv.rank;
    recvBuf[i].val = (double) recv.val;

    //printf("send.rank = %d send.val = %lg and recv.rank = %d recv.val = %lg\n",send.rank, send.val,
    //		recv.rank, recv.val);
  }

}

//TODO provide HPX implementation
/// \param [in] count Length of buf in bytes.
void bcastParallel(void* buf, int count, int root) {
}

