#include <stdio.h>
#include <arpa/inet.h>
#include "photon.h"

#define NUM_NODES 8

#define B001      0
#define B002      1
#define B003      2
#define B004      3
#define B005      4
#define B006      5
#define B007      6
#define B008      7

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

  inet_pton(AF_INET, "10.1.20.10", &node[B001].s_addr);
  inet_pton(AF_INET, "10.1.20.20", &node[B002].s_addr);
  inet_pton(AF_INET, "10.1.20.30", &node[B003].s_addr);
  inet_pton(AF_INET, "10.1.20.40", &node[B004].s_addr);
  inet_pton(AF_INET, "10.1.20.50", &node[B005].s_addr);
  inet_pton(AF_INET, "10.1.20.60", &node[B006].s_addr);
  inet_pton(AF_INET, "10.1.20.70", &node[B007].s_addr);
  inet_pton(AF_INET, "10.1.20.80", &node[B008].s_addr);

  for (i=0; i<NUM_NODES; i++) {
    for (j=0; j<num_blocks; j++) {
      uint32_t bid = ((uint32_t)(i+1)<<16) | (uint32_t)j;
      if (j==0) {
        //printf("B00%d base block: 0x%08x\n", i+1, bid);
      }
      node[i].block[j].blkaddr.blk3 = bid;
    }
  }
    
  // node   OF_port   MAC (for rewrite)
  // B001   12        c9:ff:fe:4b:1c:9c
  // B002   13        c9:ff:fe:35:03:50
  // B003   14        c9:ff:fe:17:c5:d0
  // B004   15        c9:ff:fe:18:07:00
  // B005   16        c9:ff:fe:18:1d:50
  // B006   17        c9:ff:fe:18:10:40
  // B007   20        14:ff:fe:01:71:70
  // B008   19        14:ff:fe:01:6f:f0
  
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
