#include "common.h"
#include "test_cfg.h"

#define PHOTON_BUF_SIZE (1024*1024*1024*2UL) // 256M
#define PHOTON_TAG UINT32_MAX
#define SQ_SIZE 1000

static int ITERS = 100000;
static char *send, **recv;

#define RVAL 3

static int sizes[] = {
  0,
  1,
  8,
  64,
  128,
  192,
  256,
  2048,
  4096,
  8192,
  12288,
  16384
};

extern int DONE;

static int *recvCompT;
static int *gwcCompT;
static int myrank;
static int nranks;
static sem_t sem;
static sem_t rsem;
static int rthreads = 1;

#ifdef HAVE_SETAFFINITY
static int ncores = 1;                 /* number of CPU cores */
static cpu_set_t cpu_set;              /* processor CPU set */
static cpu_set_t def_set;
#endif

int do_sync() {
  int i, val;
  photon_cid lid = {
    .u64 = 0x0,
    .size = 0
  };
  photon_cid rid = {
    .u64 = 0xdeadbeef,
    .size =0
  };
  
  for (i=0; i<nranks; i++) {
    sem_wait(&rsem);
    photon_put_with_completion(i, 0, NULL, NULL, lid, rid, 0);
  }
  
  do {
    if (sem_getvalue(&rsem, &val)) continue;
  } while (val < nranks);

  return 0;
}

// Have one thread poll local completion only, PROTON_PROBE_EVQ
void *wait_local_completion_thread(void *arg) {
  photon_cid request;
  int flag, rc, src;

  do {
    rc = photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &request, &src, NULL, PHOTON_PROBE_EVQ);
    if (rc < 0) {
      exit(1);
    }
    if ((flag > 0) && (request.u64 == PHOTON_TAG)) {
      // Increments the counter
      sem_post(&sem);
    }
  } while (!DONE);
  
  pthread_exit(NULL);
}

// Have multiple threads each polling for ledger completions (i.e., revs) 
// PHOTON_PROBE_LEDGER from a given rank instead of any source
void *wait_ledger_completions_thread(void *arg) {
  photon_cid request;
  long inputrank = (long)arg;
  int flag, src;
  int proc = (nranks > rthreads) ? PHOTON_ANY_SOURCE : inputrank;
  int check = 1;

  do {
    photon_probe_completion(proc, &flag, NULL, &request, &src, NULL, PHOTON_PROBE_LEDGER);
    uint16_t prefix = request.u64>>48;
    uint16_t sind = request.u64<<16>>48;
    uint32_t iter = request.u64<<32>>32;
    if (check && flag && prefix == 0x00ff) {
      int stride = sizes[sind];
      // check recv buffer
      for (int i=0; i<stride; i++) {
	if (recv[src][iter*stride+i] != RVAL) {
	  printf("\n\n%d: invalid entry for stride %d from src: %d at iter: %d position: %i, value found: %d\n\n",
		 myrank, stride, src, iter, i, recv[src][iter*stride+i]);
	  exit(1);
	}
      }
      recvCompT[src]++;
    }
    if (flag && request.u64 == 0xfacefeed)
      gwcCompT[src]++;
    else if (flag && request.u64 == 0xdeadbeef) {
      // clear all the relevant recv buffer
      memset(recv[src], 0, sizeof(PHOTON_BUF_SIZE));
      photon_cid lid = {
	.u64 = 0x0,
	.size = 0
      };
      photon_cid rid = {
	.u64 = 0xcafebabe,
	.size =0
      };
      // send the ack
      photon_put_with_completion(src, 0, NULL, NULL, lid, rid, 0);
    }
    else if (flag && request.u64 == 0xcafebabe) {
      // signal reception of the ack
      sem_post(&rsem);
    }
    
  } while (!DONE);
  
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  int i, j, k, ns, val;
  int rank, nproc;
  int aff_main, aff_evq, aff_ledg;
  long t;

  int rdata;
  int ur = open("/dev/urandom", O_RDONLY);
  int rc = read(ur, &rdata, sizeof rdata);
  srand(rdata);
  close(ur);

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
  struct photon_buffer_t rbuf[nproc];
  photon_rid recvReq[nproc], sendReq[nproc];
  photon_cid lid, rid;
  pthread_t th, recv_threads[rthreads];
  pthread_t th2;

  lid.u64 = PHOTON_TAG;
  lid.size = 0;
  rid.size = 0;

  recvCompT = calloc(nproc, sizeof(int));
  gwcCompT = calloc(nproc, sizeof(int));

  // only need one send buffer
  //posix_memalign((void **) &send, 8, PHOTON_BUF_SIZE*sizeof(uint8_t));
  send = malloc(PHOTON_BUF_SIZE);
  memset(send, RVAL, PHOTON_BUF_SIZE);
  photon_register_buffer(send, PHOTON_BUF_SIZE);

  recv = malloc(nproc*sizeof(char*));
  // ... but recv buffers for each potential sender
  for (i=0; i<nproc; i++) {
    //posix_memalign((void **) &recv[i], 8, PHOTON_BUF_SIZE*sizeof(uint8_t));
    recv[i] = calloc(1, PHOTON_BUF_SIZE);
    photon_register_buffer(recv[i], PHOTON_BUF_SIZE);
  }

  for (i=0; i<nproc; i++) {
    // everyone posts their recv buffers
    photon_post_recv_buffer_rdma(i, recv[i], PHOTON_BUF_SIZE, PHOTON_TAG, &recvReq[i]);
  }

  for (i=0; i<nproc; i++) {
    // wait for a recv buffer that was posted
    photon_wait_recv_buffer_rdma(i, PHOTON_ANY_SIZE, PHOTON_TAG, &sendReq[i]);
    // get the remote buffer info so we can do our own put
    photon_get_buffer_remote(sendReq[i], &rbuf[i]);
    photon_send_FIN(sendReq[i], i, PHOTON_REQ_COMPLETED);
    photon_wait(recvReq[i]);
  }

  sem_init(&sem, 0, SQ_SIZE);
  sem_init(&rsem, 0, nproc);

  // Create a thread to wait for local completions 
  pthread_create(&th, NULL, wait_local_completion_thread, NULL);

  // do allocations
  pthread_create(&th2, NULL, alloc_thread, NULL);
  
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

  //pthread_setaffinity_np(th2, sizeof(cpu_set_t), &def_set);
#endif

  // now we can proceed with our benchmark
  if (rank == 0)
    printf("%-7s%-9s%-7s%-11s%-12s\n", "Ranks", "Senders", "Bytes", "Sync PUT", "Sync GET");

  struct timespec time_s, time_e;
  for (ns = 0; ns < nranks; ns++) {
    for (i=0; i<sizeof(sizes)/sizeof(sizes[0]); i++) {
      if (rank == 0) {
        printf("%-7d", nproc);
        printf("%-9u", ns + 1);
        printf("%-7u", sizes[i]);
        fflush(stdout);
      }
      
      j = rand() % nproc;
      // send to random rank, excluding self
      while (j == rank)
        j = rand() % nproc;

      // PUT
      if (rank <= ns) {
        clock_gettime(CLOCK_MONOTONIC, &time_s);
        for (k=0; k<ITERS; k++) {
	  if (sem_wait(&sem) == 0) {
	    int rc;
	    struct photon_buffer_t tbuf;
	    const struct photon_buffer_priv_t *priv;
	    tbuf.addr = rbuf[j].addr + sizes[i]*k;
	    tbuf.priv = rbuf[j].priv;
	    
	    lbuf.addr = (uintptr_t)send;
	    lbuf.size = sizes[i];
	    photon_get_buffer_private((void*)lbuf.addr, sizes[i], &priv);
	    lbuf.priv = *priv;
	    
	    rid.u64 = (uint64_t)0x00ff<<48 | (uint64_t)i<<32 | k;
	    rc = photon_put_with_completion(j, sizes[i], &lbuf, &tbuf, lid, rid, 0);
	    if (rc == PHOTON_ERROR) {
	      fprintf(stderr, "Error doing PWC\n");
	      exit(1);
	    }
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

/*

      if (rank <= ns) {
	clock_gettime(CLOCK_MONOTONIC, &time_s);
	for (k=0; k<ITERS; k++) {
	  if (sem_wait(&sem) == 0) {
	    lbuf.addr = (uintptr_t)send;
	    lbuf.size = sizes[i];
	    lbuf.priv = (struct photon_buffer_priv_t){0,0};
	    rid.u64 = 0xfacefeed;
	    if (photon_get_with_completion(j, sizes[i], &lbuf, &rbuf[j], lid, rid, 0)) {
	      fprintf(stderr, "Error doing GWC\n");
	      exit(1);
	    }
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
*/    
      // make sure remote completions are caught up and re-initialized after each size
      do_sync();
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
    printf("%d received from %d: pwc_comp: %d, gwc_comp: %d\n", rank, i, recvCompT[i], gwcCompT[i]);
  }
  
  // Clean up and destroy the mutex
  photon_unregister_buffer(send, PHOTON_BUF_SIZE);
  free(send);

  for (i=0; i<nproc; i++) {
    photon_unregister_buffer(recv[i], PHOTON_BUF_SIZE);
    free(recv[i]);
  }

  photon_finalize();
  MPI_Finalize();

  return 0;
}

