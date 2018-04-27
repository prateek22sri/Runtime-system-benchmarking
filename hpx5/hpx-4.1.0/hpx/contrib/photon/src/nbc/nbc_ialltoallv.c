/*
 * Copyright (c) 2006 The Trustees of Indiana University and Indiana
 *                    University Research and Technology
 *                    Corporation.  All rights reserved.
 * Copyright (c) 2006 The Technical University of Chemnitz. All
 *                    rights reserved.
 *
 * Author(s): Torsten Hoefler <htor@cs.indiana.edu>
 *
 */
#include "nbc_internal.h"

/* an alltoallv schedule can not be cached easily because the contents
 * ot the recvcounts array may change, so a comparison of the address
 * would not be sufficient ... we simply do not cache it */

#ifdef HAVE_SYS_WEAK_ALIAS_PRAGMA
#pragma weak NBC_Ialltoallv = PNBC_Ialltoallv
#define NBC_Ialltoallv PNBC_Ialltoallv
#endif

/* simple linear Alltoallv */
int NBC_Ialltoallv(void *sendbuf, int *sendcounts, int *sdispls,
                   MPI_Datatype sendtype, void *recvbuf, int *recvcounts,
                   int *rdispls, MPI_Datatype recvtype, NBC_Comminfo *comm,
                   NBC_Handle *handle) {

  int rank, p, res, i;
  MPI_Aint sndext, rcvext;
  NBC_Schedule *schedule;
  char *rbuf, *sbuf, inplace;

  NBC_IN_PLACE(sendbuf, recvbuf, inplace);

  res = NBC_Init_handle(handle, comm);
  if (res != NBC_OK) {
    printf("Error in NBC_Init_handle(%i)\n", res);
    return res;
  }
  res = NBC_Comm_rank(comm, &rank);
  if (NBC_SUCCESS != res) {
    printf("MPI Error in NBC_Comm_rank() (%i)\n", res);
    return res;
  }
  res = NBC_Comm_size(comm, &p);
  if (NBC_SUCCESS != res) {
    printf("MPI Error in NBC_Comm_size() (%i)\n", res);
    return res;
  }
  res = MPI_Type_extent(sendtype, &sndext);
  if (MPI_SUCCESS != res) {
    printf("MPI Error in MPI_Type_extent() (%i)\n", res);
    return res;
  }
  res = MPI_Type_extent(recvtype, &rcvext);
  if (MPI_SUCCESS != res) {
    printf("MPI Error in MPI_Type_extent() (%i)\n", res);
    return res;
  }

  schedule = (NBC_Schedule *)malloc(sizeof(NBC_Schedule));
  if (NULL == schedule) {
    printf("Error in malloc() (%i)\n", res);
    return res;
  }

  handle->tmpbuf = NULL;

  res = NBC_Sched_create(schedule);
  if (res != NBC_OK) {
    printf("Error in NBC_Sched_create (%i)\n", res);
    return res;
  }

  /* copy data to receivbuffer */
  if ((sendcounts[rank] != 0) && !inplace) {
    rbuf = ((char *)recvbuf) + (rdispls[rank] * rcvext);
    sbuf = ((char *)sendbuf) + (sdispls[rank] * sndext);
    res = NBC_Copy(sbuf, sendcounts[rank], sendtype, rbuf, recvcounts[rank],
                   recvtype);
    if (NBC_OK != res) {
      printf("Error in NBC_Copy() (%i)\n", res);
      return res;
    }
  }

  for (i = 0; i < p; i++) {
    if (i == rank) {
      continue;
    }
    /* post all sends */
    if (sendcounts[i] != 0) {
      sbuf = ((char *)sendbuf) + (sdispls[i] * sndext);
      res = NBC_Sched_send(sbuf, false, sendcounts[i], sendtype, i, schedule,
                           comm);
      if (NBC_OK != res) {
        printf("Error in NBC_Sched_send() (%i)\n", res);
        return res;
      }
    }
    /* post all receives */
    if (recvcounts[i] != 0) {
      rbuf = ((char *)recvbuf) + (rdispls[i] * rcvext);
      res = NBC_Sched_recv(rbuf, false, recvcounts[i], recvtype, i, schedule,
                           comm);
      if (NBC_OK != res) {
        printf("Error in NBC_Sched_recv() (%i)\n", res);
        return res;
      }
    }
  }

  /*NBC_PRINT_SCHED(*schedule);*/

  res = NBC_Sched_commit(schedule);
  if (NBC_OK != res) {
    printf("Error in NBC_Sched_commit() (%i)\n", res);
    return res;
  }

  res = NBC_Start(handle, schedule);
  if (NBC_OK != res) {
    printf("Error in NBC_Start() (%i)\n", res);
    return res;
  }

  return NBC_OK;
}
