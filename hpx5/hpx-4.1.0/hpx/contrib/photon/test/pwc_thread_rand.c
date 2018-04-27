#include "common.h"
#include "test_cfg.h"

#define PHOTON_BUF_SIZE (1024*1024*1)
#define PHOTON_TAG UINT32_MAX
#define SQ_SIZE 1000

#define LARGE_LIMIT      16384
#define LARGE_ITERS      10000

extern int DONE;

static int ITERS       = 100000;
static char *send, **recv;

static int *recvCompT;
static int *gwcCompT;
static int myrank;
static int nranks;
static sem_t sem;
static int rthreads = 1;
static int athreads = 0;

// Have one thread poll local completion only, PROTON_PROBE_EVQ
void *wait_local_completion_thread(void *arg) {
  photon_cid cid;
  int flag, rc, src;

  do {
    rc = photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &cid,
				 &src, NULL, PHOTON_PROBE_EVQ);
    if (rc < 0) {
      exit(1);
    }
    if ((flag > 0) && (cid.u64 == PHOTON_TAG)) {
      // Increments the counter
      sem_post(&sem);
    }
  } while (!DONE);
  
  pthread_exit(NULL);
}

// Have multiple threads each polling for ledger completions (i.e., revs) 
// PHOTON_PROBE_LEDGER from a given rank instead of any source
void *wait_ledger_completions_thread(void *arg) {
  photon_cid cid;
  long inputrank = (long)arg;
  int flag, src;
  int proc = (nranks > rthreads) ? PHOTON_ANY_SOURCE : inputrank;
  int check = 0;

  do {
    photon_probe_completion(proc, &flag, NULL, &cid,
			    &src, NULL, PHOTON_PROBE_LEDGER);
    if (flag) {
      if (cid.u64 == 0xcafebabe) {
	recvCompT[src]++;
      }
      if (cid.u64 == 0xfacefeed) {
	gwcCompT[src]++;
      }
	}
  } while (!DONE);
  
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  int i, j, k, ns, val;
  int rank, nproc, iters;
  int aff_main, aff_evq, aff_ledg;
  long t;

  setup_rand_seed();

  MPI_Init(&argc,&argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);
  myrank = rank;
  nranks = nproc;
  
  if (rthreads == 0)
    rthreads = nproc;
  
  cfg.nproc = nproc;
  cfg.address = rank;

  //if (myrank == 0) {
  //  dbg_wait();
  //}
  
  if (photon_init(&cfg))
    exit(1);

  aff_main = aff_evq = aff_ledg = -1;
  if (argc > 1)
    aff_main = atoi(argv[1]);
  if (argc > 2)
    aff_evq = atoi(argv[2]);
  if (argc > 3)
    aff_ledg = atoi(argv[3]);

  struct photon_buffer_t lbuf;
  const struct photon_buffer_priv_t *tmp;
  struct photon_buffer_t rbuf[nproc];
  photon_rid recvReq[nproc], sendReq[nproc];
  photon_cid lid, rid;
  pthread_t th, recv_threads[rthreads];
  pthread_t th2[athreads];

  lid.u64 = PHOTON_TAG;
  lid.size = 0;
  rid.size = 0;

  recvCompT = calloc(nproc, sizeof(int));
  gwcCompT = calloc(nproc, sizeof(int));

  // only need one send buffer
  //posix_memalign((void **) &send, 8, PHOTON_BUF_SIZE*sizeof(uint8_t));
  send = malloc(PHOTON_BUF_SIZE);
  CHECK(photon_register_buffer(send, PHOTON_BUF_SIZE));
  CHECK(photon_get_buffer_private(send, PHOTON_BUF_SIZE, &tmp));
  lbuf.priv = *tmp;

  recv = malloc(nproc*sizeof(char*));
  // ... but recv buffers for each potential sender
  for (i=0; i<nproc; i++) {
    //posix_memalign((void **) &recv[i], 8, PHOTON_BUF_SIZE*sizeof(uint8_t));
    recv[i] = calloc(1, PHOTON_BUF_SIZE);
    CHECK(photon_register_buffer(recv[i], PHOTON_BUF_SIZE));
  }

  for (i=0; i<nproc; i++) {
    // everyone posts their recv buffers
    CHECK(photon_post_recv_buffer_rdma(i, recv[i], PHOTON_BUF_SIZE, PHOTON_TAG, &recvReq[i]));
  }

  for (i=0; i<nproc; i++) {
    // wait for a recv buffer that was posted
    CHECK(photon_wait_recv_buffer_rdma(i, PHOTON_ANY_SIZE, PHOTON_TAG, &sendReq[i]));
    // get the remote buffer info so we can do our own put
    CHECK(photon_get_buffer_remote(sendReq[i], &rbuf[i]));
    CHECK(photon_send_FIN(sendReq[i], i, PHOTON_REQ_COMPLETED));
    CHECK(photon_wait(recvReq[i]));
  }

  sem_init(&sem, 0, SQ_SIZE);

  // Create a thread to wait for local completions 
  pthread_create(&th, NULL, wait_local_completion_thread, NULL);

  for (t=0; t<rthreads; t++) {
    pthread_create(&recv_threads[t], NULL, wait_ledger_completions_thread, (void*)t);
  }
  
  // Create some threads that aggressively allocate and register memory
  for (t=0; t<athreads; t++) {
    pthread_create(&th2[t], NULL, alloc_thread, NULL);
  }
  
  // now we can proceed with our benchmark
  if (rank == 0)
    printf("%-7s%-9s%-12s%-12s%-12s\n", "Ranks", "Senders", "Bytes", "Sync PUT", "Sync GET");

  float a;
  struct timespec time_s, time_e;
  ns = nproc-1;
  for (a=0; a<=PHOTON_BUF_SIZE; a+=a) {

    i = (int) a;
    if (!a) a = 0.5;

    iters = (i > LARGE_LIMIT) ? LARGE_ITERS : ITERS;

    if (rank == 0) {
      printf("%-7d", nproc);
      printf("%-9u", ns + 1);
      printf("%-12u", i);
      fflush(stdout);
    }
    
    // PUT
    if (rank <= ns) {
      clock_gettime(CLOCK_MONOTONIC, &time_s);
      for (k=0; k<iters; k++) {
	
	j = pick_rand_rank(myrank, nranks);
	
	if (sem_wait(&sem) == 0) {
	  int rc;
	  struct photon_buffer_t tbuf;
	  tbuf.addr = rbuf[j].addr;
	  tbuf.priv = rbuf[j].priv; 
	  
	  int ssize = 0;
	  if (i) {
	    ssize = rand() % i;
	  }
	  
	  lbuf.addr = (uintptr_t)send;
	  lbuf.size = ssize;
	  rid.u64 = 0xcafebabe;
	  CHECK(photon_put_with_completion(j, ssize, &lbuf, &tbuf, lid, rid, 0));
	}
      }
    }
    
    // clear remaining local completions
    do {
      if (sem_getvalue(&sem, &val)) continue;
    } while (val < SQ_SIZE);
    
    clock_gettime(CLOCK_MONOTONIC, &time_e);
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 0) {
      double time_ns = (double)(((time_e.tv_sec - time_s.tv_sec) * 1e9) + (time_e.tv_nsec - time_s.tv_nsec));
      double time_us = time_ns/1e3;
      double latency = time_us/iters;
      printf("%-12.2f", latency);
      fflush(stdout);
    }

    if (rank <= ns) {
      clock_gettime(CLOCK_MONOTONIC, &time_s);
      for (k=0; k<iters; k++) {

	j = pick_rand_rank(myrank, nranks);
	
	if (sem_wait(&sem) == 0) {
	  int ssize = 0;
	  if (i) {
	    ssize = rand() % i;
	  }
	  
	  lbuf.addr = (uintptr_t)send;
	  lbuf.size = ssize;
	  rid.u64 = 0xfacefeed;
	  CHECK(photon_get_with_completion(j, ssize, &lbuf, &rbuf[j], lid, rid, 0));
	}
      }
    }
    
    // clear remaining local completions
    do {
      if (sem_getvalue(&sem, &val)) continue;
    } while (val < SQ_SIZE);
    
    clock_gettime(CLOCK_MONOTONIC, &time_e);
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 0) {
      double time_ns = (double)(((time_e.tv_sec - time_s.tv_sec) * 1e9) + (time_e.tv_nsec - time_s.tv_nsec));
      double time_us = time_ns/1e3;
      double latency = time_us/iters;
      printf("%-12.2f\n", latency);
      fflush(stdout);
    }
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  
  // give recv probing some extra time
  sleep(1);

  DONE = 1;
  // Wait for all threads to complete
  pthread_join(th, NULL);
  for (t=0; t<rthreads; t++) {
    pthread_join(recv_threads[t], NULL);
  }

  // Clean up and destroy the mutex
  photon_unregister_buffer(send, PHOTON_BUF_SIZE);
  free(send);

  for (i=0; i<nproc; i++) {
    photon_unregister_buffer(recv[i], PHOTON_BUF_SIZE);
    free(recv[i]);
  }
  
  if (rank == 0) {
    for (i=0; i<nproc; i++) {
      printf("%d received from %d: pwc_comp: %d, gwc_comp: %d\n", rank, i, recvCompT[i], gwcCompT[i]);
    }
  }
  
  photon_finalize();
  MPI_Finalize();

  return 0;
}

