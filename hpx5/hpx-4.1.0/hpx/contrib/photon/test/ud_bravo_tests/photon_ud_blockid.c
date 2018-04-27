#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <mpi.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <openssl/md5.h>

#include "photon.h"
#include "cutter_ids.h"
#include "test_cfg.h"

#define PHOTON_SEND_SIZE 1024
#define PHOTON_TAG       13

int main(int argc, char *argv[]) {
  photon_rid sendReq;
  char *send,*recv;
  int rank,size;

  MD5_CTX ctx;
  unsigned char hash[16];
  photon_addr naddr;
  char buf[INET_ADDRSTRLEN];
  
  srand(time(NULL));

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  cfg.nproc = size;
  cfg.address = rank;
  cfg.ibv.use_ud = 1;
  cfg.ibv.ud_gid_prefix = "ff0e::ffff:0000:0000";  // mcast

  photon_init(&cfg);

  int rc = posix_memalign((void **) &send, 64, PHOTON_SEND_SIZE*sizeof(char));
  rc = posix_memalign((void **) &recv, 64, PHOTON_SEND_SIZE*sizeof(char));

  int i;
  for (i=0; i<PHOTON_SEND_SIZE; i++) {
    send[i] = rand() % 26 + 97;
  }

  send[0] = rank + 97;

  //printf("%d: send (%d): %s\n", rank, strlen(send), send);

  MD5_Init(&ctx);
  MD5_Update(&ctx, send, strlen(send));
  MD5_Final(hash, &ctx);

  printf("%d: sending message with md5 sig: ", rank);
  for (i=0; i<16; i++) {
    printf("%x", hash[i]);
  }
  printf("\n");

  photon_get_dev_addr(AF_INET, &naddr);

  inet_ntop(AF_INET, &naddr.s_addr, buf, sizeof(buf));
  printf("%d: dev addr: %s\n", rank, buf);

  bravo_node *dnode = NULL, *mynode = NULL;
  init_bravo_ids(1);
  mynode = find_bravo_node(&naddr);

  if (!mynode) {
    printf("could not find my node\n");
    return 1;
  }

  switch (mynode->index) {
  case B001:
    dnode = get_bravo_node(B002);
    break;
  case B002:
    dnode = get_bravo_node(B001);
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

  printf("%d: sending to block (%d): 0x%08x\n", rank, dnode->index+1, dnode->block[0].blkaddr.blk3);

  // send to the destination node
  photon_send(&dnode->block[0], send, PHOTON_SEND_SIZE, 0, &sendReq);

  while(1) {
    int flag, type;
    struct photon_status_t stat;
    int tst = photon_test(sendReq, &flag, &type, &stat);
    if( tst < 0 ) {
      fprintf(stderr,"%d: An error occured in photon_test(recv)\n", rank);
      exit(-1);
    }
    else if( tst > 0 ) {
      fprintf(stderr,"%d: That shouldn't have happened in this code\n", rank);
      exit(0);
    }
    else {
      if( flag ) {
        fprintf(stderr,"%d: put(%d, %d) of size %d completed successfully\n", rank, (int)stat.src_addr.global.proc_id, stat.tag, PHOTON_SEND_SIZE);
        break;
      }
      else {
        //fprintf(stderr,"%d: Busy waiting for recv\n", rank);
        usleep(10*1000); // 1/100th of a second
      }
    }
  }

  // once our send completes, let's see if someone sent us something
  struct photon_status_t stat;
  while(1) {
    photon_addr addr = {.s_addr = 0};
    int flag;
    int tst = photon_probe(&addr, &flag, &stat);
    //int tst = photon_test(recvReq, &flag, &type, &stat);
    if( tst < 0 ) {
      fprintf(stderr,"%d: An error occured in photon_test(recv)\n", rank);
      exit(-1);
    }
    else if( tst > 0 ) {
      //fprintf(stderr,"%d: That shouldn't have happened in this code\n", rank);
      //exit(0);
    }
    else {
      if( flag ) {
        // probe says we got some message, let's get it
        photon_recv(stat.request, recv, stat.size, 0);
        fprintf(stderr,"%d: recv(%d, %d) of size %lu completed successfully\n", rank, (int)stat.src_addr.global.proc_id, stat.tag, stat.size);
        break;
      }
      else {
        //fprintf(stderr,"%d: Busy waiting for recv\n", rank);
        usleep(10*1000); // 1/100th of a second
      }
    }
  }

  //printf("%d: recv (%d): %s\n", rank, strlen(recv), recv);

  MD5_Init(&ctx);
  MD5_Update(&ctx, recv, strlen(recv));
  MD5_Final(hash, &ctx);

  printf("%d: received message with md5 sig: ", rank);
  for (i=0; i<16; i++) {
    printf("%x", hash[i]);
  }
  printf("\n");

  MPI_Barrier(MPI_COMM_WORLD);

  photon_unregister_addr(&mynode->block[0], AF_INET6);
  free(send);
  free(recv);

  photon_finalize();
  MPI_Finalize();
  return 0;
}
