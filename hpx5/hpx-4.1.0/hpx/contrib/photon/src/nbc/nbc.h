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
#ifndef __NBC_H__
#define __NBC_H__

#include <mpi.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/queue.h>

#define USE_PHOTON // [> set by the buildsystem! <]

#ifdef USE_PHOTON
#include <ph/ph.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Function return codes  */
#define NBC_OK 0        /* everything went fine */
#define NBC_SUCCESS 0   /* everything went fine (MPI compliant :) */
#define NBC_OOR 1       /* out of resources */
#define NBC_BAD_SCHED 2 /* bad schedule */
#define NBC_CONTINUE 3  /* progress not done */
#define NBC_DATATYPE_NOT_SUPPORTED 4 /* datatype not supported or not valid */
#define NBC_OP_NOT_SUPPORTED 5       /* operation not supported or not valid */
#define NBC_NOT_IMPLEMENTED 6
#define NBC_INVALID_PARAM 7 /* invalid parameters */
#define NBC_INVALID_TOPOLOGY_COMM                                              \
  8 /* invalid topology attached to communicator */
#define NBC_ERROR 9
/* number of implemented collective functions */
#define NBC_NUM_COLL 19

#define NBC_CACHE_SCHEDULE
/* a schedule is basically a pointer to some memory location where the
 * schedule array resides */
typedef void *NBC_Schedule;

/*
 * a buffer pool definition
 * */

// define buffer pool size
/* used to hang off a communicator */
typedef struct {
  int tag;
  int vrank; // this is the virtual rank, physical rank can be calculated ->
             // mapping[vrank]
  int group_sz;
  int world_sz;
  int *group_mapping;
  buffer_pool_t bpool;
  void *NBC_HandleCache;
  uint64_t handles_sz;
#ifdef NBC_CACHE_SCHEDULE
  void *NBC_Dict[NBC_NUM_COLL]; /* this should point to a struct
                                      hb_tree, but since this is a
                                      public header-file, this would be
                                      an include mess :-(. So let's void
                                      it ...*/
  int NBC_Dict_size[NBC_NUM_COLL];
#endif
} NBC_Comminfo;

int NBC_Comm_rank(NBC_Comminfo *comminfo, int *rank) ;
int NBC_Comm_size(NBC_Comminfo *comminfo, int *grpsz);
int NBC_Comm_prank(NBC_Comminfo *comminfo, int vrank);
/*
 * check if this rank belongs to passed communicator group
 * */
int NBC_Comm_inrank(NBC_Comminfo *comm) ;

typedef struct CompEntry {
  TAILQ_ENTRY(CompEntry) nexte;
  void *event;
} CompEntry;

typedef TAILQ_HEAD(CompList, CompEntry) CompList;
/* thread specific data */
typedef struct {
  long row_offset;
  uint64_t tag;
  volatile int req_count;
  pthread_mutex_t lock;
  sem_t semid;
  photon_nbc_req *req_array;
  photon_rid reqid;
  recv_info_t recvs;
  bool completed;
  void *comp_event_q;
  // void *req_array;
  NBC_Comminfo *comminfo;
  volatile NBC_Schedule *schedule;
  void *tmpbuf; /* temporary buffer e.g. used for Reduce */
  /* TODO: we should make a handle pointer to a state later (that the user
   * can move request handles) */
} NBC_Handle;
typedef NBC_Handle NBC_Request;

/*******************************************************
 ****** external NBC functions are defined here *******
 *******************************************************/

/* external function prototypes */
int NBC_Init_comm(int rank, int *active_ranks, int group_size, int world_size,
                  NBC_Comminfo *comm);
int NBC_Init_handle(NBC_Handle *handle, NBC_Comminfo *comm);
int NBC_Ibarrier(NBC_Comminfo *comm, NBC_Handle *handle);
int NBC_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root,
               NBC_Comminfo *comm, NBC_Handle *handle);
int NBC_Ibcast_inter(void *buffer, int count, MPI_Datatype datatype, int root,
                     NBC_Comminfo *comm, NBC_Handle *handle);
int NBC_Ialltoallv(void *sendbuf, int *sendcounts, int *sdispls,
                   MPI_Datatype sendtype, void *recvbuf, int *recvcounts,
                   int *rdispls, MPI_Datatype recvtype, NBC_Comminfo *comm,
                   NBC_Handle *handle);
int NBC_Igather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
                NBC_Comminfo *comm, NBC_Handle *handle);
int NBC_Igatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int *recvcounts, int *displs,
                 MPI_Datatype recvtype, int root, NBC_Comminfo *comm,
                 NBC_Handle *handle);
int NBC_Iscatter(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
                 NBC_Comminfo *comm, NBC_Handle *handle);
int NBC_Iscatterv(void *sendbuf, int *sendcounts, int *displs,
                  MPI_Datatype sendtype, void *recvbuf, int recvcount,
                  MPI_Datatype recvtype, int root, NBC_Comminfo *comm,
                  NBC_Handle *handle);
int NBC_Iallgather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                   void *recvbuf, int recvcount, MPI_Datatype recvtype,
                   NBC_Comminfo *comm, NBC_Handle *handle);
int NBC_Iallgatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                    void *recvbuf, int *recvcounts, int *displs,
                    MPI_Datatype recvtype, NBC_Comminfo *comm,
                    NBC_Handle *handle);
int NBC_Ialltoall(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                  void *recvbuf, int recvcount, MPI_Datatype recvtype,
                  NBC_Comminfo *comm, NBC_Handle *handle);
int NBC_Ireduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
                MPI_Op op, int root, NBC_Comminfo *comm, NBC_Handle *handle);
int NBC_Iallreduce(void *sendbuf, void *recvbuf, int count,
                   MPI_Datatype datatype, MPI_Op op, NBC_Comminfo *comm,
                   NBC_Handle *handle);
int NBC_Iscan(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
              MPI_Op op, NBC_Comminfo *comm, NBC_Handle *handle);
int NBC_Ireduce_scatter(void *sendbuf, void *recvbuf, int *recvcounts,
                        MPI_Datatype datatype, MPI_Op op, NBC_Comminfo *comm,
                        NBC_Handle *handle);

int NBC_Wait(NBC_Handle *handle);
int NBC_Test(NBC_Handle *handle);

// optional API
int NBC_Cache_Handle(NBC_Handle *handle);
int NBC_Cache_Handle_remove(NBC_Handle *handle);
int NBC_Handle_put_event(NBC_Handle *handle, completion_info_t *e);
NBC_Handle *NBC_Handle_get(uint64_t tag, NBC_Comminfo *comm);
int NBC_Handle_done(NBC_Handle *handle);
/* TODO: some hacks */
int NBC_Operation(void *buf3, void *buf1, void *buf2, MPI_Op op,
                  MPI_Datatype type, int count);

void NBC_Reset_times();
void NBC_Print_times(double div);
void NBC_Finalize(NBC_Comminfo *comm);

#ifdef __cplusplus
}
#endif

#endif
