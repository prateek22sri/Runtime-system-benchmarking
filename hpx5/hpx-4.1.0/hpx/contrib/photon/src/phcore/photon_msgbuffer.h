// =============================================================================
//  Photon RDMA Library (libphoton)
//
//  Copyright (c) 2016, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifndef PHOTON_MSGBUFFER_H
#define PHOTON_MSGBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "photon_buffer.h"

typedef struct photon_msgbuffer_entry_t {
  void *base;
  void *hptr;
  void *mptr;
  bool empty;
} photon_mbe;

struct photon_msgbuffer_t {
  photon_mbe *entries;
  int p_count;
  int s_index;
  int status;

  photonBI db;
  uint64_t p_size;
  uint64_t m_size;
  int p_offset;
  int p_hsize;
  
  pthread_mutex_t buf_lock;
} photon_msgbuf;

typedef struct photon_msgbuffer_t * photonMsgBuf;

/* 
   @param size:    - total allocated buffer space
   @param p_size   - size of the partitions (includes offset and header)
   @param p_offset - bytes in front of each message
   @param p_hsize  - bytes of header

   .base          .hptr         .mptr
   |...p_offset...|...p_hsize...|......msg......| */
PHOTON_INTERNAL photonMsgBuf photon_msgbuffer_new(uint64_t size, uint64_t p_size, int p_offset, int p_hsize);
PHOTON_INTERNAL int photon_msgbuffer_free(photonMsgBuf mbuf);
PHOTON_INTERNAL photon_mbe *photon_msgbuffer_get_entry(photonMsgBuf mbuf, int *ind);
PHOTON_INTERNAL int photon_msgbuffer_free_entry(photonMsgBuf mbuf, int ind);

#ifdef __cplusplus
}
#endif

#endif
