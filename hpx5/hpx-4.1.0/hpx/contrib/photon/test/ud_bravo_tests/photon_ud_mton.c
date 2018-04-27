#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "photon.h"
#include "bravo_ids.h"
#include "test_cfg.h"

#define PHOTON_SEND_SIZE 1024*64
#define PHOTON_TAG       13

#define NUM_REQ          9

int main(int argc, char *argv[]) {
  uint64_t sendReq[NUM_REQ];
  char *send[NUM_REQ], *recv[NUM_REQ];
  int rank,size,i;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  cfg.nproc = size;
  cfg.address = rank;
  cfg.ibv.use_ud = 1;
  cfg.ibv.ud_gid_prefix = "ff0e::ffff:0000:0000";  // mcast

  photon_init(&cfg);
  
  for (i=0; i<NUM_REQ; i++) {
    send[i] = (char*)malloc(PHOTON_SEND_SIZE*sizeof(char));
    recv[i] = (char*)malloc(PHOTON_SEND_SIZE*sizeof(char));
  }

  photon_addr naddr;
  char buf[INET_ADDRSTRLEN];
  photon_get_dev_addr(AF_INET, &naddr);

  inet_ntop(AF_INET, &naddr.s_addr, buf, sizeof(buf));
  printf("%d: dev addr: %s\n", rank, buf);

  bravo_node *dnode = NULL, *mynode = NULL;
  init_bravo_ids(10);
  mynode = find_bravo_node(&naddr);

  if (!mynode) {
    printf("could not find my node\n");
    return 1;
  }
  
  switch (mynode->index) {
  case B001:
    dnode = get_bravo_node(B004);
    break;
  case B002:
    dnode = get_bravo_node(B004);
    break;
  case B003:
    dnode = get_bravo_node(B004);
    break;
  case B004:
    // receiver in this test
    dnode = get_bravo_node(B004);
    break;
  case B005:
    dnode = get_bravo_node(B004);
    break;
  case B006:
    dnode = get_bravo_node(B004);
    break;
  case B007:
    dnode = get_bravo_node(B004);
    break;
  case B008:
    dnode = get_bravo_node(B004);
    break;
  default:
    break;
  }
  
  if (!dnode) {
    printf("could not find dest node\n");
    return 1;
  }

  for (i=0; i<mynode->nblocks; i++) {
    photon_register_addr(&mynode->block[i], AF_INET6);
  }

  // need each node to be ready to receive before we send...
  MPI_Barrier(MPI_COMM_WORLD);

  if (mynode->index != B004) {
    photon_send(&dnode->block[0], send[0], PHOTON_SEND_SIZE, 0, &sendReq[0]);
    while(1) {
      int flag, type;
      struct photon_status_t stat;
      photon_test(sendReq[0], &flag, &type, &stat);
      if (flag) {
        fprintf(stderr,"%d: send(%d, %d) of size %d completed successfully\n",
                rank, (int)stat.src_addr.global.proc_id, stat.tag, PHOTON_SEND_SIZE);
        break;
      }
    }
  }
  else {
    // receive from everyone else
    for (i=0;i<size-1; i++) {
      int flag;
      struct photon_status_t stat;
      //gettimeofday(&start, NULL);
      while (1) {
        photon_addr addr = {.s_addr = 0};
        photon_probe(&addr, &flag, &stat);
        if (flag) {
          break;
        }
      }
      photon_recv(stat.request, recv[i], stat.size, 0);
      fprintf(stderr,"%d: recv(%d, %d) of size %lu completed successfully\n",
              rank, (int)stat.src_addr.global.proc_id, stat.tag, stat.size);
    }
  }
  
  MPI_Barrier(MPI_COMM_WORLD);

  for (i=0; i<NUM_REQ; i++) {
    free(send[i]);
    free(recv[i]);
  }

  photon_finalize();
  MPI_Finalize();
  return 0;
}
