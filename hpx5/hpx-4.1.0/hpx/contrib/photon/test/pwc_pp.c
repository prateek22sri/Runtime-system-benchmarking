#include "common.h"
#include "test_cfg.h"

#define SUBTRACT_TS_NS(E,S) (((E.tv_sec - S.tv_sec) * 1e9) + (E.tv_nsec - S.tv_nsec))
#define dbg_printf(...) if(0) {printf(__VA_ARGS__);}

#define PHOTON_TAG       13
#define MAX_CID          1024
#define PING             0
#define PONG             1

extern int DONE;

static int PHOTON_BUF_SIZE = (1024*1024*4);
static int global_iters    = 1000;
static int count           = 0;
static int ssize           = 8;

static int rank, other_rank;
static char *send, *recv;
static struct photon_buffer_t lbuf;
static struct photon_buffer_t rbuf;

static char ppid[MAX_CID];
static int csize           = 8;

int send_pingpong(int dst, void *id, ssize_t size) {
  photon_cid lid = {
    .u64 = PHOTON_TAG,
    .size = 0
  };
  
  photon_cid rid = {
    .data = id,
    .size = size
  };

  lbuf.addr = (uintptr_t)send;
  lbuf.size = ssize;
  lbuf.priv = (struct photon_buffer_priv_t){0,0};
  photon_put_with_completion(dst, ssize, &lbuf, &rbuf, lid, rid, 0);
  photon_cid cid;
  int flag, src;
  do {
    photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &cid, &src, NULL, PHOTON_PROBE_EVQ);
  } while (!flag || cid.u64 != PHOTON_TAG);
  
  return 0;
}

void recv_handler(photon_cid c) {
  uint64_t *id = c.data;

  if (++count == global_iters)
    DONE = 1;

  if (!DONE || rank)
    send_pingpong(other_rank, ppid, csize);
}

void *receiver(void *args) {
  photon_cid request;
  int flag, src;
  do {
    photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &request, &src, recv_handler, PHOTON_PROBE_LEDGER);
  } while (!DONE);

  return NULL;
}

int main(int argc, char **argv) {
  photon_rid sendReq, recvReq;
  struct timespec time_s, time_e;
  int nproc, i;

  if (argc > 2) {
    global_iters = atoi(argv[1]);
    csize = atoi(argv[2]);
  }
  
  assert(csize <= MAX_CID);

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);
  other_rank = (rank+1) % nproc;

  cfg.nproc = nproc;
  cfg.address = rank;
  cfg.cap.max_cid_size = MAX_CID;

  photon_init(&cfg);

  send = (char*)malloc(PHOTON_BUF_SIZE);
  recv = (char*)malloc(PHOTON_BUF_SIZE);

  photon_register_buffer(send, PHOTON_BUF_SIZE);
  photon_register_buffer(recv, PHOTON_BUF_SIZE);  
  
  photon_post_recv_buffer_rdma(other_rank, recv, PHOTON_BUF_SIZE, PHOTON_TAG, &recvReq);
  photon_wait_recv_buffer_rdma(other_rank, PHOTON_ANY_SIZE, PHOTON_TAG, &sendReq);
  photon_get_buffer_remote(sendReq, &rbuf);
  photon_send_FIN(sendReq, other_rank, PHOTON_REQ_COMPLETED);
  photon_wait(recvReq);
  
  if (rank == 0) {
    printf("%-7s%-12s%-14s%-14s\n", "CSIZE", "Bytes", "RTT (us)", "Rate (MB/s)");
    fflush(stdout);
    // warmup loop
    send_pingpong(other_rank, ppid, csize);
  }
  receiver(NULL);

  float a;
  for (a=0; a<=PHOTON_BUF_SIZE; a+=a) {

    i = (int) a;
    ssize = i;
    count = 0;
    DONE = 0;

    if (rank == 0) {
      printf("%-7d", csize);
      printf("%-12d", ssize);
      fflush(stdout);
    }

    clock_gettime(CLOCK_MONOTONIC, &time_s);
    // sender (0) gets us started
    if (rank == 0) {
      send_pingpong(other_rank, ppid, csize);
    }
    receiver(NULL);
    clock_gettime(CLOCK_MONOTONIC, &time_e);
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 0) {
      float usec = SUBTRACT_TS_NS(time_e, time_s)/1e3;
      long long bytes = (long long) ssize * global_iters * 2;

      printf("%-14.2f", usec / global_iters);
      printf("%-14.2f\n", (double)bytes/1024/1024 / (usec/1e6));
      fflush(stdout);

      /*
	printf("%d: total time: %f\n", rank, SUBTRACT_TS_NS(time_e, time_s)/1e9);            
	printf("%lld bytes in %.2f seconds = %.2f Mbit/sec\n",
	bytes, usec / 1000000., bytes * 8. / usec);
	printf("%d iters in %.2f seconds = %.2f usec/iter\n",
	global_iters, usec / 1000000., usec / global_iters);
      */
    }
    
    if (!a) a = 0.5;
  }

  photon_finalize();
  MPI_Finalize();

  return 0;
}
