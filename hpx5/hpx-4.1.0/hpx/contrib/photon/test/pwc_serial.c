#include "common.h"
#include "test_cfg.h"

#define PHOTON_BUF_SIZE (1024*1024*4) // 4M
#define PHOTON_TAG       UINT32_MAX

#define LARGE_LIMIT      524288
#define LARGE_ITERS      500
#define LARGE_SKIP       100
#define SKIP             1000

static int ITERS       = 10000;
static int iters;
static int skip;
static int send_comp = 0;
static int recv_comp = 0;
static int myrank;

// could be a thread
void *wait_local(void *arg) {
  photon_cid request;
  int flag, rc, src;

  while (send_comp) {
    rc = photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &request, &src, NULL, PHOTON_PROBE_ANY);
    if (rc != PHOTON_OK)
      continue;  // no events
    if (flag) {
      if (request.u64 == PHOTON_TAG)
        send_comp--;
    }
  }
  return NULL;
}

int send_done(int n, int r) {
  int i;
  photon_cid lid = {
    .u64 = PHOTON_TAG,
    .size = 0
  };
  photon_cid rid = {
    .u64 = 0xdeadbeef,
    .size =0
  };
  for (i=0; i<n; i++) {
    if (i==r)
      continue;
    photon_put_with_completion(i, 0, NULL, NULL, lid, rid, 0);
    send_comp++;
  }
  return 0;
}

int wait_done() {
  photon_cid request;
  int flag, src;
  do {
    photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &request, &src, NULL, PHOTON_PROBE_ANY);
  } while (request.u64 != 0xdeadbeef);
  
  return 0;
}

int main(int argc, char *argv[]) {
  float a;
  int i, j, k, ns;
  int rank, nproc;

  setup_rand_seed();

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  cfg.nproc = nproc;
  cfg.address = rank;

  myrank = rank;

  if (argc > 1) {
    ITERS = atoi(argv[1]);
    if (!myrank)
      printf("ITERS: %d\n", ITERS);
  }

  if (myrank == 1) {
    //dbg_wait();
  }

  if (photon_init(&cfg) != PHOTON_OK) {
    exit(1);
  }

  struct photon_buffer_t lbuf;
  struct photon_buffer_t rbuf[nproc];
  photon_rid recvReq[nproc], sendReq[nproc];
  photon_cid lid, rid;
  char *send = NULL, *recv[nproc];

  lid.u64 = PHOTON_TAG;
  lid.size = 0;
  rid.u64 = 0xcafebabe;
  rid.size = 0;

  // only need one send buffer
  posix_memalign((void **) &send, 8, PHOTON_BUF_SIZE*sizeof(uint8_t));
  photon_register_buffer(send, PHOTON_BUF_SIZE);

  // ... but recv buffers for each potential sender
  for (i=0; i<nproc; i++) {
    posix_memalign((void **) &recv[i], 8, PHOTON_BUF_SIZE*sizeof(uint8_t));
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

  // now we can proceed with our benchmark
  if (rank == 0)
    printf("%-7s%-9s%-12s%-12s%-14s%-16s%-16s\n", "Ranks", "Senders", "Bytes",\
	   "Sync (us)",	"Sync (MB/s)", "Sync GET (us)", "Sync GET (MB/s)");

  struct timespec time_s, time_e;
  
  for (ns = 0; ns < 1; ns++) {
    
    if (rank > ns) {
      wait_done();
    }

    for (a=0; a<=PHOTON_BUF_SIZE; a+=a) {
      i = (int)a;

      if (rank == 0) {
        printf("%-7d", nproc);
        printf("%-9u", ns + 1);
        printf("%-12u", i);
        fflush(stdout);
      }
      
      iters = (i > LARGE_LIMIT) ? LARGE_ITERS : ITERS;
      iters = (iters > ITERS) ? ITERS : iters;
      skip = (i > LARGE_LIMIT) ? LARGE_SKIP : SKIP;

      if (iters <= skip)
	skip = 0;

      j = pick_rand_rank(myrank, nproc);

      // PUT
      if (rank <= ns) {
        for (k=0; k<iters; k++) {
	  if (k == skip)
	    clock_gettime(CLOCK_MONOTONIC, &time_s);
	  lbuf.addr = (uintptr_t)send;
	  lbuf.size = i;
	  lbuf.priv = (struct photon_buffer_priv_t){0,0};
          photon_put_with_completion(j, i, &lbuf, &rbuf[j], lid, rid, 0);
	  send_comp++;
	  //if (k<skip)
	  wait_local(NULL);
        }
	clock_gettime(CLOCK_MONOTONIC, &time_e);
      }
      
      //wait_local(NULL);
      //clock_gettime(CLOCK_MONOTONIC, &time_e);

      //MPI_Barrier(MPI_COMM_WORLD);
      
      if (rank == 0) {
        double time_ns = (double)(((time_e.tv_sec - time_s.tv_sec) * 1e9) + (time_e.tv_nsec - time_s.tv_nsec));
        double time_us = time_ns/1e3;
        double latency = time_us/(iters-skip);
        double rate = ((double)(iters-skip)*i)/1024/1024/(time_us/1e6);
        printf("%-12.2f", latency);
	printf("%-14.2f", rate);
        fflush(stdout);
      }

      // GET
      if (rank <= ns) {
	for (k=0; k<iters; k++) {
	  if (k == skip)
	    clock_gettime(CLOCK_MONOTONIC, &time_s);
	  lbuf.addr = (uintptr_t)send;
	  lbuf.size = i;
	  lbuf.priv = (struct photon_buffer_priv_t){0,0};
	  photon_get_with_completion(j, i, &lbuf, &rbuf[j], lid, rid, 0);
	  send_comp++;
	  //if (k<skip)
	  wait_local(NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &time_e);
      }
      
      //wait_local(NULL);
      //clock_gettime(CLOCK_MONOTONIC, &time_e);

      //MPI_Barrier(MPI_COMM_WORLD);
      
      if (rank == 0) {
        double time_ns = (double)(((time_e.tv_sec - time_s.tv_sec) * 1e9) + (time_e.tv_nsec - time_s.tv_nsec));
        double time_us = time_ns/1e3;
        double latency = time_us/(iters-skip);
        double rate = ((double)(iters-skip)*i)/1024/1024/(time_us/1e6);
        printf("%-16.2f", latency);
        printf("%-16.2f\n", rate);
        fflush(stdout);
      }

      if (!a) a = 0.5;
    }
    
    if (rank <= ns) {
      send_done(nproc, rank);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);

 exit:
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
