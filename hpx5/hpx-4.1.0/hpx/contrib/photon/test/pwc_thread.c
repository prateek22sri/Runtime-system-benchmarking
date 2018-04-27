#include "common.h"
#include "test_cfg.h"

#define PHOTON_BUF_SIZE (1024*1024*1) // 1M
#define MAX_SIZE        16384
#define PHOTON_TAG      UINT32_MAX
#define SQ_SIZE         1000

extern int DONE;

static int ITERS = 10000;
static char *send, **recv;
static int *recvCompT;
static int *gwcCompT;
static int myrank;
static int nranks;
static sem_t sem;
static int rthreads = 1;

#ifdef HAVE_SETAFFINITY
static int ncores = 1;                 /* number of CPU cores */
static cpu_set_t cpu_set;              /* processor CPU set */
static cpu_set_t def_set;
#endif

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
    if (flag && (cid.u64 == PHOTON_TAG)) {
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
  int check = 1;

  do {
    photon_probe_completion(proc, &flag, NULL, &cid, &src, NULL, PHOTON_PROBE_LEDGER);
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
  int rank, nproc;
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
  
  CHECK(photon_init(&cfg));
  
  aff_main = aff_evq = aff_ledg = -1;
  if (argc > 1)
    aff_main = atoi(argv[1]);
  if (argc > 2)
    aff_evq = atoi(argv[2]);
  if (argc > 3)
    aff_ledg = atoi(argv[3]);

  struct photon_buffer_t lbuf;
  struct photon_buffer_t rbuf[nproc];
  photon_rid recvReq[nproc], sendReq[nproc];
  photon_cid lid, rid;
  pthread_t th, recv_threads[rthreads];
  
  lid.u64 = PHOTON_TAG;
  lid.size = 0;
  rid.size = 0;

  recvCompT = calloc(nproc, sizeof(int));
  gwcCompT = calloc(nproc, sizeof(int));

  // only need one send buffer
  //posix_memalign((void **) &send, 8, PHOTON_BUF_SIZE*sizeof(uint8_t));
  send = malloc(PHOTON_BUF_SIZE);
  CHECK(photon_register_buffer(send, PHOTON_BUF_SIZE));
  
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
  
  // set affinity as requested
#ifdef HAVE_SETAFFINITY
  if ((ncores = sysconf(_SC_NPROCESSORS_CONF)) <= 0)
    err(1, "sysconf: couldn't get _SC_NPROCESSORS_CONF");
  CPU_ZERO(&def_set);
  for (i=0; i<ncores; i++)
    CPU_SET(i, &def_set);
  if (aff_main >= 0) {
    //printf("Setting main thread affinity to core %d\n", aff_main);
    CPU_ZERO(&cpu_set);
    CPU_SET(aff_main, &cpu_set);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set) != 0)
      err(1, "couldn't change CPU affinity");
  }
  else
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &def_set);
  if (aff_evq >= 0) {
    //printf("Setting EVQ probe thread affinity to core %d\n", aff_evq);
    CPU_ZERO(&cpu_set);
    CPU_SET(aff_evq, &cpu_set);
    pthread_setaffinity_np(th, sizeof(cpu_set_t), &cpu_set);
  }
  else
    pthread_setaffinity_np(th, sizeof(cpu_set_t), &def_set);
  if (aff_evq >= 0) {
    //printf("Setting LEDGER probe thread affinity to core %d\n", aff_ledg);
    CPU_ZERO(&cpu_set);
    CPU_SET(aff_ledg, &cpu_set);
    for (i=0; i<rthreads; i++)
      pthread_setaffinity_np(recv_threads[i], sizeof(cpu_set_t), &cpu_set);
  }
  else {
    for (i=0; i<rthreads; i++)
      pthread_setaffinity_np(recv_threads[i], sizeof(cpu_set_t), &def_set);
  }
#endif

  // now we can proceed with our benchmark
  if (rank == 0)
    printf("%-7s%-9s%-7s%-11s%-12s\n", "Ranks", "Senders", "Bytes", "Sync PUT", "Sync GET");

  float a;
  struct timespec time_s, time_e;
  for (ns = 0; ns < nranks; ns++) {
    for (a=0; a<=MAX_SIZE; a+=a) {
      
      i = (int) a;
      if (!a) a = 0.5;

      if (rank == 0) {
        printf("%-7d", nranks);
        printf("%-9u", ns + 1);
        printf("%-7u", i);
        fflush(stdout);
      }
      
      j = pick_rand_rank(myrank, nranks);

      // PUT
      if (rank <= ns) {
        clock_gettime(CLOCK_MONOTONIC, &time_s);
        for (k=0; k<ITERS; k++) {
	  if (sem_wait(&sem) == 0) {
	    struct photon_buffer_t tbuf;
	    tbuf.addr = rbuf[j].addr;
	    tbuf.priv = rbuf[j].priv; 
	    
	    lbuf.addr = (uintptr_t)send;
	    lbuf.size = i;
	    lbuf.priv = (struct photon_buffer_priv_t){0,0};
	    
	    rid.u64 = 0xcafebabe;
	    CHECK(photon_put_with_completion(j, i, &lbuf, &tbuf, lid, rid, 0));
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
        double latency = time_us/ITERS;
        printf("%1.4f     ", latency);
        fflush(stdout);
      }

      j = pick_rand_rank(myrank, nranks);

      if (rank <= ns) {
	clock_gettime(CLOCK_MONOTONIC, &time_s);
	for (k=0; k<ITERS; k++) {
	  if (sem_wait(&sem) == 0) {
	    lbuf.addr = (uintptr_t)send;
	    lbuf.size = i;
	    lbuf.priv = (struct photon_buffer_priv_t){0,0};
	    rid.u64 = 0xfacefeed;
	    CHECK(photon_get_with_completion(j, i, &lbuf, &rbuf[j], lid, rid, 0));
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
        double latency = time_us/ITERS;
        printf("%1.4f\n", latency);
        fflush(stdout);
      }
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
  
  for (i=0; i<nproc; i++) {
    printf("%d received from %d: pwc_comp: %d, gwc_comp: %d\n",
	   rank, i, recvCompT[i], gwcCompT[i]);
  }
  
  // Clean up and destroy the mutex
  CHECK(photon_unregister_buffer(send, PHOTON_BUF_SIZE));
  free(send);

  for (i=0; i<nproc; i++) {
    CHECK(photon_unregister_buffer(recv[i], PHOTON_BUF_SIZE));
    free(recv[i]);
  }

  CHECK(photon_finalize());
  MPI_Finalize();

  return 0;
}

