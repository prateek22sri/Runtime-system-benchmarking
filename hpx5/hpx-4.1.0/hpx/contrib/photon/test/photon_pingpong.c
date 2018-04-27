#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <mpi.h>

#include "test_cfg.h"

#define PHOTON_TAG  13
#define PING         0
#define PONG         1

#define SUBTRACT_TV(E,S) (((E).tv_sec - (S).tv_sec) + ((E).tv_usec - (S).tv_usec)/1e6)
#define dbg_printf(...) if(0) {printf(__VA_ARGS__);}

enum test_type {PHOTON_TEST,
                MPI_TEST,
                PWC_TEST
               };

int pp_test = PHOTON_TEST;
int global_iters = 100;
int msize = 1024;
int rank, other_rank, size;

struct pingpong_args {
  uint32_t type;
  uint32_t ping_id;
  uint32_t pong_id;
  char msg[];
};

struct pingpong_args *send_args;
struct pingpong_args *recv_args;
struct photon_buffer_t lbuf;
struct photon_buffer_t rbuf;

int send_pingpong(int dst, int ping_id, int pong_id, int pp_type) {
  //struct timeval start, end;
  photon_rid send_req;

  send_args->type = pp_type;
  send_args->ping_id = ping_id;
  send_args->pong_id = pong_id;
  send_args->msg[0] = '\0';

  if (pp_test == PHOTON_TEST) {
    //gettimeofday(&start, NULL);
    photon_post_send_buffer_rdma(dst, (char*)send_args, msize, PHOTON_TAG, &send_req);
    //photon_wait_recv_buffer_rdma(dst, PHOTON_ANY_SIZE, PHOTON_TAG, &send_req);
    //gettimeofday(&end, NULL);
    //if (rank == 0)
    //  printf("%d: post_send time: %f\n", rank, SUBTRACT_TV(end, start));
    
    //gettimeofday(&start, NULL);
    //photon_post_os_put(send_req, dst, (void*)send_args, msize, PHOTON_TAG, 0);
    //gettimeofday(&end, NULL);
    //if (rank == 0)
    //  printf("%d: os_put time: %f\n", rank, SUBTRACT_TV(end, start));
    while (1) {
      int flag, type;
      struct photon_status_t stat;
      photon_test(send_req, &flag, &type, &stat);
      if(flag) {
        dbg_printf("%d: send_pingpong(%d->%d)[%d] of size %d completed successfully\n", rank, rank, dst, pp_type, msize);
        break;
      }
      else {
        //usleep(10);
      }
    }
    //gettimeofday(&start, NULL);
    //photon_send_FIN(send_req, dst, 0);
    //gettimeofday(&end, NULL);
    //if (rank == 0)
    //  printf("%d: wait time: %f\n", rank, SUBTRACT_TV(end, start));
  }
  else if (pp_test == MPI_TEST) {
    MPI_Request mpi_r;
    MPI_Status stat;
    int flag;
    MPI_Isend((void*)send_args, msize, MPI_BYTE, dst, rank, MPI_COMM_WORLD, &mpi_r);
    //MPI_Wait(&mpi_r, &stat);
    while (1) {
      MPI_Test(&mpi_r, &flag, &stat);
      if (flag) {
        break;
      }
      else {
        //usleep(10);
      }
    }
  }
  else if (pp_test == PWC_TEST) {
    photon_cid lid = {
      .u64 = PHOTON_TAG,
      .size = 0
    };
    
    photon_cid rid = {
      .u64 = 0xcafebabe,
      .size =0
    };
    
    lbuf.addr = (uintptr_t)send_args;
    lbuf.size = msize;
    lbuf.priv = (struct photon_buffer_priv_t){0,0};
    photon_put_with_completion(dst, msize, &lbuf, &rbuf, lid, rid, 0);
    photon_cid request;
    int flag, src;
    do {
      photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &request, &src, NULL, PHOTON_PROBE_EVQ);
    } while (!flag || request.u64 != PHOTON_TAG);
  }

  return 0;
}

void *receiver(void *args) {
  //struct timeval start, end;

  while (1) {
    photon_rid recv_req;

    /* check if we have something to receive first, mimicking HPX parcel handler
       instead of blocking in wait_send_buffer... */
    /*
    if (pp_test == PHOTON_TEST) {
      struct photon_status_t stat;
      int flag, ret;
      do {
        ret = photon_probe_ledger(other_rank, &flag, PHOTON_SEND_LEDGER, &stat);
      } while (flag != 1);
    }
    else if (pp_test == MPI_TEST) {
      MPI_Status stat;
      int flag, ret;
      do {
        ret = MPI_Iprobe(other_rank, other_rank, MPI_COMM_WORLD, &flag, &stat);
      } while (flag != 1);
    }
    */

    if (pp_test == PHOTON_TEST) {
      //gettimeofday(&start, NULL);
      photon_wait_send_buffer_rdma(other_rank, PHOTON_ANY_SIZE, PHOTON_TAG, &recv_req);
      photon_post_os_get(recv_req, other_rank, (void*)recv_args, msize, PHOTON_TAG, 0);
      //photon_post_recv_buffer_rdma(other_rank, (void*)recv_args, msize, PHOTON_TAG, &recv_req);
      while (1) {
        int flag, type;
        struct photon_status_t stat;
	photon_test(recv_req, &flag, &type, &stat);
        if(flag) {
          dbg_printf("%d: recv_ping(%d<-%d) of size %d completed successfully\n", rank, rank, (int)stat.src_addr.global.proc_id, msize);
          break;
        }
        else {
          //usleep(10);
        }
      }
      //gettimeofday(&end, NULL);
      //if (rank == 0)
      //printf("%d: recv time: %f\n", rank, SUBTRACT_TV(end, start));
      photon_send_FIN(recv_req, other_rank, 0);
    }
    else if (pp_test == MPI_TEST) {
      MPI_Request mpi_r;
      MPI_Status stat;
      int flag;
      MPI_Irecv((void*)recv_args, msize, MPI_BYTE, other_rank, other_rank, MPI_COMM_WORLD, &mpi_r);
      //MPI_Wait(&mpi_r, &stat);
      while (1) {
        MPI_Test(&mpi_r, &flag, &stat);
        if (flag) {
          break;
        }
        else {
          //usleep(10);
        }
      }
    }
    else if (pp_test == PWC_TEST) {
      photon_cid request;
      int flag, src;
      do {
        photon_probe_completion(PHOTON_ANY_SOURCE, &flag, NULL, &request, &src, NULL, PHOTON_PROBE_LEDGER);
      } while (!flag || request.u64 != 0xcafebabe);
    }

    switch(recv_args->type) {
    case PING:
      {
        int ping_id = recv_args->ping_id;
        dbg_printf("%d: got ping %d from %d, sending pong back...\n", rank, ping_id, other_rank);
        send_pingpong(other_rank, -1, ping_id, PONG);
        if (ping_id == (global_iters-1)) {
          return NULL;
        }
      }
      break;
    case PONG:
      {
        int pong_id = recv_args->pong_id;
        dbg_printf("%d: got pong %d from %d, sending ping back...\n", rank, pong_id, other_rank);
        if (pong_id == (global_iters-1)) {
          return NULL;
        }
        send_pingpong(other_rank, recv_args->pong_id+1, -1, PING);
      }
      break;
    default:
      fprintf(stderr, "%d: Unknown pingpong type\n", rank);
      exit(-1);
      break;
    }
  }

  return NULL;
}

int main(int argc, char **argv) {
  char *test = "PHOTON";
  pthread_t th;
  struct timeval start, end;

  if (argc > 3) {
    test = strdup(argv[1]);
    global_iters = atoi(argv[2]);
    msize = atoi(argv[3]);
  }
  if (argc <= 3 || global_iters < 1) {
    printf("Usage: %s <type> <iters> <size>\n", argv[0]);
    exit(-1);
  }

  if (!strcasecmp(test, "PHOTON"))
    pp_test = PHOTON_TEST;
  else if (!strcasecmp(test, "MPI"))
    pp_test = MPI_TEST;
  else if (!strcasecmp(test, "PWC"))
    pp_test = PWC_TEST;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  other_rank = (rank+1) % size;

  cfg.nproc = size;
  cfg.address = rank;

  photon_init(&cfg);

  send_args = (struct pingpong_args*)malloc(msize);
  recv_args = (struct pingpong_args*)malloc(msize);

  if (pp_test == PHOTON_TEST || pp_test == PWC_TEST) {
    photon_register_buffer((char*)send_args, msize);
    photon_register_buffer((char*)recv_args, msize);
  }

  if (pp_test == PWC_TEST) {
    photon_rid sendReq, recvReq;
    photon_post_recv_buffer_rdma(other_rank, recv_args, msize, PHOTON_TAG, &recvReq);
    photon_wait_recv_buffer_rdma(other_rank, PHOTON_ANY_SIZE, PHOTON_TAG, &sendReq);
    photon_get_buffer_remote(sendReq, &rbuf);
    photon_send_FIN(sendReq, other_rank, PHOTON_REQ_COMPLETED);
    photon_wait(recvReq);
  }

  gettimeofday(&start, NULL);

  if (rank == 0) {
    send_pingpong(other_rank, 0, -1, PING);
  }

  pthread_create(&th, NULL, receiver, NULL);
  pthread_join(th, NULL);

  MPI_Barrier(MPI_COMM_WORLD);

  gettimeofday(&end, NULL);

  if (rank == 0) {
    printf("%d: total time: %f\n", rank, SUBTRACT_TV(end, start));
    
    float usec = (end.tv_sec - start.tv_sec) * 1000000 +
      (end.tv_usec - start.tv_usec);
    long long bytes = (long long) msize * global_iters * 2;
    
    printf("%lld bytes in %.2f seconds = %.2f Mbit/sec\n",
           bytes, usec / 1000000., bytes * 8. / usec);
    printf("%d iters in %.2f seconds = %.2f usec/iter\n",
           global_iters, usec / 1000000., usec / global_iters);
  }

  if (pp_test == PHOTON_TEST)
    photon_finalize();

  MPI_Finalize();

  return 0;
}
