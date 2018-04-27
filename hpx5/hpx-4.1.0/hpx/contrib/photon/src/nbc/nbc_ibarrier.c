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

/* Dissemination implementation of MPI_Ibarrier */
#ifdef HAVE_SYS_WEAK_ALIAS_PRAGMA
#pragma weak NBC_Ibarrier = PNBC_Ibarrier
#define NBC_Ibarrier PNBC_Ibarrier
#endif

int NBC_Ibarrier(NBC_Comminfo *comm, NBC_Handle *handle) {
  int round, rank, p, maxround, res, recvpeer, sendpeer;
  NBC_Schedule *schedule;

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

  handle->tmpbuf = (void *)malloc(2 * sizeof(char));

#ifdef NBC_CACHE_SCHEDULE
  /* there only one argument set per communicator -> hang it directly at
   * the tree-position, NBC_Dict_size[...] is 0 for not initialized and
   * 1 for initialized. NBC_Dict[...] is a pointer to the schedule in
   * this case */
  if (handle->comminfo->NBC_Dict_size[NBC_BARRIER] == 0) {
/* we did not init it yet */
#endif
    schedule = (NBC_Schedule *)malloc(sizeof(NBC_Schedule));
    if (NULL == schedule) {
      printf("Error in malloc()\n");
      return res;
    }

    round = -1;
    res = NBC_Sched_create(schedule);
    if (res != NBC_OK) {
      printf("Error in NBC_Sched_create (%i)\n", res);
      return res;
    }

    maxround = (int)ceil((log(p) / LOG2) - 1);

    do {
      round++;
      sendpeer = (rank + (1 << round)) % p;
      /* add p because modulo does not work with negative values */
      recvpeer = ((rank - (1 << round)) + p) % p;

      /* send msg to sendpeer */
      res = NBC_Sched_send((void *)0, true, 1, MPI_BYTE, sendpeer, schedule,
                           comm);
      if (NBC_OK != res) {
        printf("Error in NBC_Sched_send() (%i)\n", res);
        return res;
      }

      /* recv msg from recvpeer */
      res = NBC_Sched_recv((void *)1, true, 1, MPI_BYTE, recvpeer, schedule,
                           comm);
      if (NBC_OK != res) {
        printf("Error in NBC_Sched_recv() (%i)\n", res);
        return res;
      }

      /* end communication round */
      if (round < maxround) {
        res = NBC_Sched_barrier(schedule);
        if (NBC_OK != res) {
          printf("Error in NBC_Sched_barrier() (%i)\n", res);
          return res;
        }
      }
    } while (round < maxround);

    res = NBC_Sched_commit(schedule);
    if (NBC_OK != res) {
      printf("Error in NBC_Sched_commit() (%i)\n", res);
      return res;
    }

#ifdef NBC_CACHE_SCHEDULE
    /* add it */
    handle->comminfo->NBC_Dict[NBC_BARRIER] = (hb_tree *)schedule;
    handle->comminfo->NBC_Dict_size[NBC_BARRIER] = 1;
  } else {
    /* we found it */
    schedule = (NBC_Schedule *)handle->comminfo->NBC_Dict[NBC_BARRIER];
  }
#endif

  res = NBC_Start(handle, schedule);
  if (NBC_OK != res) {
    printf("Error in NBC_Start() (%i)\n", res);
    return res;
  }

  return NBC_OK;
}
