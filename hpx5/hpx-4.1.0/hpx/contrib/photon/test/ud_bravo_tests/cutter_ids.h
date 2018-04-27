#include <stdio.h>
#include <arpa/inet.h>
#include "photon.h"

#define NUM_NODES 16

#define B001      0
#define B002      1
#define B003      2
#define B004      3
#define B005      4
#define B006      5
#define B007      6
#define B008      7
#define B009      8
#define B010      9
#define B011      10
#define B012      11
#define B013      12
#define B014      13
#define B015      14
#define B016      15

typedef struct bravo_node_t {
  int           index;
  uint32_t      s_addr;
  photon_addr  *block;
  int           nblocks;
} bravo_node;

static bravo_node *node;

bravo_node *init_bravo_ids(int num_blocks) {
  int i, j;

  node = malloc(NUM_NODES * sizeof(bravo_node));

  for (i=0; i<NUM_NODES; i++) {
    node[i].block = calloc(num_blocks, sizeof(photon_addr));
    node[i].index = i;
    node[i].nblocks = num_blocks;
  }

  inet_pton(AF_INET, "192.168.3.2", &node[B001].s_addr);
  inet_pton(AF_INET, "192.168.3.3", &node[B002].s_addr);
  inet_pton(AF_INET, "192.168.3.4", &node[B003].s_addr);
  inet_pton(AF_INET, "192.168.3.5", &node[B004].s_addr);
  inet_pton(AF_INET, "192.168.3.6", &node[B005].s_addr);
  inet_pton(AF_INET, "192.168.3.7", &node[B006].s_addr);
  inet_pton(AF_INET, "192.168.3.8", &node[B007].s_addr);
  inet_pton(AF_INET, "192.168.3.9", &node[B008].s_addr);
  inet_pton(AF_INET, "192.168.3.10", &node[B009].s_addr);
  inet_pton(AF_INET, "192.168.3.11", &node[B010].s_addr);
  inet_pton(AF_INET, "192.168.3.12", &node[B011].s_addr);
  inet_pton(AF_INET, "192.168.3.13", &node[B012].s_addr);
  inet_pton(AF_INET, "192.168.3.14", &node[B013].s_addr);
  inet_pton(AF_INET, "192.168.3.15", &node[B014].s_addr);
  inet_pton(AF_INET, "192.168.3.16", &node[B015].s_addr);
  inet_pton(AF_INET, "192.168.3.17", &node[B016].s_addr);

  for (i=0; i<NUM_NODES; i++) {
    for (j=0; j<num_blocks; j++) {
      uint32_t bid = ((uint32_t)(i+1)<<16) | (uint32_t)j;
      // no need for the 0xe for IPv6 multicast
      //bid |= (uint32_t)(0xe<<28);
      if (j==0) {
        //printf("B00%d base block: 0x%08x\n", i+1, bid);
      }
      node[i].block[j].blkaddr.blk3 = bid;
    }
  }

  // node   OF_port   
  // B001   2
  // B002   3
  // B003   4
  // B004   5
  // B005   6
  // B006   7
  // B007   8
  // B008   9
  // B008   10
  // B008   11
  // B008   12
  // B008   13
  // B008   14       
  // B008   15
  // B008   16
  
  return node;
}

bravo_node *find_bravo_node(photonAddr naddr) {
  int i;
  for (i=0; i< NUM_NODES; i++) {
    if (naddr->s_addr == node[i].s_addr)
      return &node[i];
  }
  return NULL;
}

bravo_node *get_bravo_node(int id) {
  return &node[id];
}
