#include "common.h"
#include "test_cfg.h"

#define PHOTON_BUF_SIZE (1024*1024*1024*2UL) // 2GB
#define MAX_SIZE         16384
#define PHOTON_TAG       UINT32_MAX
#define SQ_SIZE          1000
#define RVAL             3

#define COMP_CHECK       (uint64_t)0xf0<<56
#define COMP_SYNC_PUT    (uint64_t)0xf1<<56
#define COMP_SYNC_GET    (uint64_t)0xf2<<56
#define COMP_SYNC_REP    (uint64_t)0xf3<<56

extern int DONE;

static int scount = 0;
static int rcount = 0;

static int ITERS = 100000;
static char *send, **recv;
static int *recvCompT;
static int *gwcCompT;
static int myrank;
static int nranks;
static sem_t sem;
static sem_t rsem;
static int rthreads = 1;

void send_ack(int dst) {
  photon_cid lid = {
    .u64 = 0x0,
    .size = 0
  };
  photon_cid rid = {
    .u64 = COMP_SYNC_REP,
    .size =0
  };
  photon_put_with_completion(dst, 0, NULL, NULL, lid, rid,
			     PHOTON_REQ_PWC_NO_LCE);
}

int do_sync(uint64_t r) {
  int i, val;
  photon_cid lid = {
    .u64 = 0x0,
    .size = 0
  };
  photon_cid rid = {
    .u64 = r,
    .size =0
  };
  
  for (i=0; i<nranks; i++) {
    sem_wait(&rsem);
    if (i==myrank) continue;
    photon_put_with_completion(i, 0, NULL, NULL, lid, rid,
			       PHOTON_REQ_PWC_NO_LCE);
  }
  
  do {
    if (sem_getvalue(&rsem, &val)) continue;
  } while (val < nranks);

  return 0;
}

void check_recv_buffer(int src, int stride, int iter) {
  for (int i=0; i<stride; i++) {
    if (recv[src][iter*stride+i] != RVAL) {
      printf("\n\n%d: invalid entry for stride %d from src: %d at iter: %d position: %i, value found: %d\n\n",
	     myrank, stride, src, iter, i, recv[src][iter*stride+i]);
      exit(1);
    }
  }
}
     
// Have one thread poll local completion only, PROTON_PROBE_EVQ
void *wait_local_completion_thread(void *arg) {
  photon_cid cid;
  int flag, rc, src;

  do {
    CHECK(photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &cid,
				  &src, NULL, PHOTON_PROBE_EVQ));
    if (flag) {
      uint64_t prefix = cid.u64>>56<<56;
      uint32_t stride = cid.u64<<8>>40;
      uint32_t iter   = cid.u64<<32>>32;
      if (prefix == COMP_CHECK) {
	check_recv_buffer(myrank, stride, iter);
	rcount++;
	sem_post(&sem);
      }
      else if (cid.u64 == PHOTON_TAG) {
	rcount++;
	sem_post(&sem);	
      }
    }
  } while (!DONE);
  
  pthread_exit(NULL);
}

// Have multiple threads each polling for ledger completions (i.e., revs) 
// PHOTON_PROBE_LEDGER from a given rank instead of any source
void *wait_ledger_completions_thread(void *arg) {
  photon_cid cid;
  long inputrank = (long)arg;
  int flag, src, scount = 0;
  int proc = (nranks > rthreads) ? PHOTON_ANY_SOURCE : inputrank;
  
  do {
    CHECK(photon_probe_completion(proc, &flag, NULL, &cid, &src,
				  NULL, PHOTON_PROBE_LEDGER));
    if (flag) {
      uint64_t prefix = cid.u64>>56<<56;;
      uint16_t stride = cid.u64<<8>>40;
      uint32_t iter   = cid.u64<<32>>32;

      switch (prefix) {
      case COMP_CHECK:
	{
	  check_recv_buffer(src, stride, iter);
	  recvCompT[src]++;
	}
	break;
      case COMP_SYNC_PUT:
	{
	  // clear all the relevant recv buffer
	  memset(recv[src], 0, PHOTON_BUF_SIZE);
	  if (++scount == nranks-1) {
	    // signal when we got the last request
	    scount = 0;
	    memset(recv[myrank], 0, PHOTON_BUF_SIZE);
	    sem_post(&rsem);
	  }
	  send_ack(src);
	}
	break;
      case COMP_SYNC_GET:
	{
	  memset(recv[src], RVAL, PHOTON_BUF_SIZE);
	  if (++scount == nranks-1) {
	    scount = 0;
	    memset(recv[myrank], 0, PHOTON_BUF_SIZE);
	    sem_post(&rsem);
	  }
	  
	  send_ack(src);
	}
	break;
      case COMP_SYNC_REP:
	// signal reception of the ack
	sem_post(&rsem);
	break;
      default:
	if (cid.u64 == 0xfacefeed)
	  gwcCompT[src]++;
	break;
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
  
  for (t=0; t<rthreads; t++) {
    pthread_create(&recv_threads[t], NULL, wait_ledger_completions_thread, (void*)t);
  }
  
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

      // PUT
      if (rank <= ns) {
        clock_gettime(CLOCK_MONOTONIC, &time_s);
        for (k=0; k<ITERS; k++) {
	  j = pick_rand_rank(myrank, nranks);
	  if (sem_wait(&sem) == 0) {
	    struct photon_buffer_t tbuf;
	    tbuf.addr = rbuf[j].addr + i*k;
	    tbuf.priv = rbuf[j].priv; 
	    
	    lbuf.addr = (uintptr_t)send;
	    lbuf.size = i;
	    lbuf.priv = (struct photon_buffer_priv_t){0,0};
	    
	    lid.u64 = PHOTON_TAG;
	    rid.u64 = COMP_CHECK | (uint64_t)i<<32 | k;
	    CHECK(photon_put_with_completion(j, i, &lbuf, &tbuf, lid, rid, 0));
	    scount++;
	  }
	}
      }

      // clear remaining local completions
      do {
	if (sem_getvalue(&sem, &val)) continue;
      } while (val < SQ_SIZE);

      clock_gettime(CLOCK_MONOTONIC, &time_e);

      assert(scount == rcount);
      scount = rcount = 0;

      MPI_Barrier(MPI_COMM_WORLD);
      
      if (rank == 0) {
        double time_ns = (double)(((time_e.tv_sec - time_s.tv_sec) * 1e9) + (time_e.tv_nsec - time_s.tv_nsec));
        double time_us = time_ns/1e3;
        double latency = time_us/ITERS;
        printf("%1.4f     ", latency);
        fflush(stdout);
      }

      // prepare for GETs
      do_sync(COMP_SYNC_GET);

      if (rank <= ns) {
	clock_gettime(CLOCK_MONOTONIC, &time_s);
	for (k=0; k<ITERS; k++) {
	  j = pick_rand_rank(myrank, nranks);
	  if (sem_wait(&sem) == 0) {
	    struct photon_buffer_t tbuf;
	    tbuf.addr = rbuf[myrank].addr + i*k;
	    tbuf.size = i;
	    tbuf.priv = rbuf[myrank].priv;

	    lid.u64 = COMP_CHECK | (uint64_t)i<<32 | k;
	    rid.u64 = 0xfacefeed;
	    CHECK(photon_get_with_completion(j, i, &tbuf, &rbuf[j], lid, rid, 0));
	    scount++;
	  }
	}
      }

      // clear remaining local completions
      do {
	if (sem_getvalue(&sem, &val)) continue;
      } while (val < SQ_SIZE);
      
      clock_gettime(CLOCK_MONOTONIC, &time_e);
      
      assert(scount == rcount);
      scount = rcount = 0;

      MPI_Barrier(MPI_COMM_WORLD);
      
      if (rank == 0) {
        double time_ns = (double)(((time_e.tv_sec - time_s.tv_sec) * 1e9) + (time_e.tv_nsec - time_s.tv_nsec));
        double time_us = time_ns/1e3;
        double latency = time_us/ITERS;
        printf("%1.4f\n", latency);
        fflush(stdout);
      }
      
      // make sure remote completions are caught up and re-initialized after each size
      do_sync(COMP_SYNC_PUT);
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

