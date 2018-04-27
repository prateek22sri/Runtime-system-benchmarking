#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <semaphore.h>
#include <pthread.h>

#include "test_cfg.h"

static void __attribute__((used)) dbg_wait(void) {
  int i = 0;
  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  printf("PID %d on %s ready for attach\n", getpid(), hostname);
  fflush(stdout);
  while (0 == i)
    sleep(12);
}

#define PHOTON_TAG       UINT32_MAX
#define SQ_SIZE          30

int send_comp = 0;
int recv_comp = 0;
int myrank;

static sem_t sem;
static int ITERS = 10000;

static int tests[] = {
  PHOTON_COLL_BARRIER,
  PHOTON_COLL_IBARRIER
};

static char *test_str[] = {
  "BARRIER",
  "IBARRIER"
};

int send_done(int r) {
  int i;
  photon_cid lid = {
    .u64 = PHOTON_TAG,
    .size = 0
  };
  photon_cid rid = {
    .u64 = 0xdeadbeef,
    .size =0
  };
  photon_put_with_completion(r, 0, NULL, NULL, lid, rid, 0);
  return 0;
}

void *progress_thread(void *args) {
  photon_cid request;
  int flag, src;
  do {
    photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &request, &src, NULL, PHOTON_PROBE_ANY);
    if ((flag > 0) && (request.u64 == PHOTON_TAG)) {
      // Increments the counter
      sem_post(&sem);
    }
    if (flag && request.u64 == 0xcafebabe) {
      recv_comp++;
    }
  } while (request.u64 != 0xdeadbeef);
  
  return 0;
}

int main(int argc, char *argv[]) {
  int i, j, k, ns;
  int rank, nproc, val;
  photon_rid req;
  photon_cid lid;
  pthread_t th;
  struct timespec time_s, time_e;

  int rdata;
  int ur = open("/dev/urandom", O_RDONLY);
  int rc = read(ur, &rdata, sizeof rdata);
  srand(rdata);
  close(ur);
  
  if (argc > 1) {
    ITERS = atoi(argv[1]);
    if (rank == 0)
      printf("ITERS: %d\n", ITERS);
  }
  
  sem_init(&sem, 0, SQ_SIZE);
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  cfg.nproc = nproc;
  cfg.address = rank;

  myrank = rank;

  if (myrank == 0) {
    //dbg_wait();
  }

  if (photon_init(&cfg) != PHOTON_OK) {
    exit(1);
  }

  lid.u64 = PHOTON_TAG;
  lid.size = 0;

  // create progress thread
  pthread_create(&th, NULL, progress_thread, NULL);
  
  clock_gettime(CLOCK_MONOTONIC, &time_s);

  for (k=0; k<(sizeof(tests)/sizeof(tests[0])); k++) {    
    for (i=0; i<ITERS; i++) {
      if (sem_wait(&sem) == 0) {
	//MPI_Barrier(MPI_COMM_WORLD);
	rc = photon_collective_init(tests[k], lid, &req, PHOTON_REQ_NIL);
	if (rc != PHOTON_OK) {
	  fprintf(stderr, "Could not initialize collective\n");
	  goto err_exit;
	}
	
	rc = photon_collective_join(req, NULL, NULL, 0, PHOTON_UINT8, NULL);
	if (rc != PHOTON_OK) {
	  fprintf(stderr, "Could not join collective\n");
	  goto err_exit;
	}
      }
    }
    
    // clear remaining local completions
    do {
      if (sem_getvalue(&sem, &val)) continue;
    } while (val < SQ_SIZE);
    
    clock_gettime(CLOCK_MONOTONIC, &time_e);
    
    if (rank == 0) {
      double time_ns = (double)(((time_e.tv_sec - time_s.tv_sec) * 1e9) + (time_e.tv_nsec - time_s.tv_nsec));
      double time_us = time_ns/1e3;
      double latency = time_us/ITERS;
      printf("Avg time in %s: %1.6f\n", test_str[k], latency);
      fflush(stdout);
    }
  }

  send_done(rank);
  pthread_join(th, NULL);

 err_exit:
  photon_finalize();
  MPI_Finalize();
  return 0;
}
