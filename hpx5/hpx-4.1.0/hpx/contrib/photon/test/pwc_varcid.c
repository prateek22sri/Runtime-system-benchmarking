#include "common.h"
#include "test_cfg.h"

#define PHOTON_TAG       UINT32_MAX
#define PHOTON_BUF_SIZE (1024*1024*4) // 4M
#define MAX_CID          1024

#define LARGE_LIMIT      8192
#define LARGE_ITERS      100
#define SKIP             1000

static int  ITERS      = 10000;
static int  CSIZE      = 8;

static int csizes[] = {
  1,
  2,
  4,
  8,
  16,
  32,
  64,
  128
};

static int iters;
static int skip;
static int send_comp = 0;
static int recv_comp = 0;
static int myrank;

static char lCID[MAX_CID+1];
static char rCID[MAX_CID+1];

void handle_lcid(photon_cid cid) {
  if (!cid.size) {
    send_comp--;
  }
  else {
    char *b = cid.data;
    int ind = cid.size-1;
    if (b[ind] == lCID[ind])
      send_comp--;
  }
}

void handle_rcid(photon_cid cid) {
  char *b = cid.data;
  int ind = cid.size-1;
  if ((*(uint64_t*)b != 0xdeadbeef))
    assert(b[ind] == rCID[ind]);
}

// could be a thread
void *wait_local(void *arg) {
  photon_cid id;
  int flag, rc, src;

  while (send_comp) {
    rc = photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &id, &src, handle_lcid, PHOTON_PROBE_ANY);
    if (rc != PHOTON_OK)
      continue;  // no events
    if (flag) {}
  }
  return NULL;
}

int send_done(int n, int r) {
  int i;
  photon_cid lid = {
    .u64 = 0xcafebabe,
    .size = 0
  };
  photon_cid rid = {
    .u64 = 0xdeadbeef,
    .size = 0
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
  photon_cid id;
  int flag, src;
  do {
    photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &id, &src, handle_rcid, PHOTON_PROBE_ANY);
  } while (id.u64 != 0xdeadbeef);
  
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
  cfg.cap.max_cid_size = MAX_CID;

  myrank = rank;

  if (argc > 1) {
    CSIZE = atoi(argv[1]);
    if (!myrank)
      printf("CSIZE: %d\n", CSIZE);
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

  for (i=0; i<MAX_CID; i++) {
    lCID[i] = i % 26 + 97;
    rCID[i]= i % 26 + 97;
  }
  lCID[MAX_CID] = '\0';
  rCID[MAX_CID] = '\0';

  if (rank == 0) {
    //printf("lCID: %s\n", lCID);
    //printf("rCID: %s\n", rCID);
  }

  lid.data = lCID;
  lid.size = CSIZE;
  rid.data = rCID;
  rid.size = CSIZE;

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
    printf("%-7s%-9s%-12s%-11s%-12s\n", "Ranks", "Senders", "Bytes", "Sync (us)", " Sync GET");

  struct timespec time_s, time_e;
  
  for (ns = 0; ns < 1; ns++) {
    
    if (rank > ns) {
      wait_done();
      break;
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
      skip = (i > LARGE_LIMIT) ? 0 : SKIP;

      if (iters <= skip)
	skip = 0;
      
      // send to random rank, including self
      j = rand() % nproc;
      // send to random rank, excluding self
      while (j == rank)
        j = rand() % nproc;
      // send to your next rank only
      //j = next;

      // PUT
      if (rank <= ns) {
        clock_gettime(CLOCK_MONOTONIC, &time_s);
        for (k=0; k<iters; k++) {
	  if (k == skip)
	    clock_gettime(CLOCK_MONOTONIC, &time_s);
	  lbuf.addr = (uintptr_t)send;
	  lbuf.size = i;
	  lbuf.priv = (struct photon_buffer_priv_t){0,0};
          photon_put_with_completion(j, i, &lbuf, &rbuf[j], lid, rid, 0);
	  send_comp++;
          wait_local(NULL);
        }
        clock_gettime(CLOCK_MONOTONIC, &time_e);
      }

      //MPI_Barrier(MPI_COMM_WORLD);
      
      if (rank == 0) {
        double time_ns = (double)(((time_e.tv_sec - time_s.tv_sec) * 1e9) + (time_e.tv_nsec - time_s.tv_nsec));
        double time_us = time_ns/1e3;
        double latency = time_us/(iters-skip);
        printf("%1.2f\t", latency);
        fflush(stdout);
      }

      // GET
      if (rank <= ns) {
        if (i) {
	  for (k=0; k<iters; k++) {
	    if (k == skip)
	      clock_gettime(CLOCK_MONOTONIC, &time_s);
	    lbuf.addr = (uintptr_t)send;
	    lbuf.size = i;
	    lbuf.priv = (struct photon_buffer_priv_t){0,0};
	    photon_get_with_completion(j, i, &lbuf, &rbuf[j], lid, rid, 0);
	    send_comp++;
	    wait_local(NULL);
	  }
	  clock_gettime(CLOCK_MONOTONIC, &time_e);
	}
      }
      
      //MPI_Barrier(MPI_COMM_WORLD);
      
      if (rank == 0 && i) {
        double time_ns = (double)(((time_e.tv_sec - time_s.tv_sec) * 1e9) + (time_e.tv_nsec - time_s.tv_nsec));
        double time_us = time_ns/1e3;
        double latency = time_us/(iters-skip);
        printf("%1.2f\n", latency);
        fflush(stdout);
      }
      else if (rank == 0) {
        printf("N/A\n");
        fflush(stdout);
      }
      
      if (!a) a = 0.5;
    }
    
    if (rank <= ns) {
      send_done(nproc, rank);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);

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
