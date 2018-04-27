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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "photon.h"
#include "logging.h"
#include "photon_msgbuffer.h"

photonMsgBuf photon_msgbuffer_new(uint64_t size, uint64_t p_size, int p_offset, int p_hsize) {

  photonMsgBuf mbuf;
  char *bptr;
  int ret, i;
    
  mbuf = malloc(sizeof(struct photon_msgbuffer_t));
  if (!mbuf)
    goto error_exit;
  
  memset(mbuf, 0, sizeof(*mbuf));

  // TODO: probably want to align each entry (p_size) on a cacheline  
  ret = posix_memalign((void*)&bptr, getpagesize(), size);
  if (ret) {
    dbg_err("could not allocate buffer space");
    goto error_exit_buf;
  }

  mbuf->db = photon_buffer_create(bptr, size);
  if (!mbuf->db) {
    dbg_err("could not create photon buffer");
    goto error_exit_buf;
  }

  mbuf->p_size = p_size;
  mbuf->m_size = p_size - p_offset - p_hsize;
  mbuf->p_offset = p_offset;
  mbuf->p_hsize = p_hsize;
  mbuf->p_count = (int)(size / mbuf->p_size);

  // create metadata to track buffer offsets
  mbuf->entries = (photon_mbe*)malloc(mbuf->p_count * sizeof(photon_mbe));
  if (!mbuf->entries) {
    dbg_err("could not allocate buf entries");
    goto error_exit_db;
  }

  for (i = 0; i < mbuf->p_count; i++) {
    mbuf->entries[i].base = (void*)((mbuf->db)->buf.addr + (i * p_size));
    mbuf->entries[i].hptr = (void*)((mbuf->db)->buf.addr + (i * p_size) + p_offset);
    mbuf->entries[i].mptr = (void*)((mbuf->db)->buf.addr + (i * p_size) + p_offset + p_hsize);
    mbuf->entries[i].empty = true;
  }

  mbuf->s_index = 0;
  mbuf->status = 0;

  pthread_mutex_init(&mbuf->buf_lock, NULL);

  return mbuf;

 error_exit_db:
  free(bptr);
 error_exit_buf:
  free(mbuf);
 error_exit:
  return NULL;
}

int photon_msgbuffer_free(photonMsgBuf mbuf) {
  if (mbuf) {
    if (mbuf->entries)
      free(mbuf->entries);
    if (mbuf->db)
      photon_buffer_free(mbuf->db);
    free(mbuf);
    return PHOTON_OK;
  }
  return PHOTON_ERROR;
}

photon_mbe *photon_msgbuffer_get_entry(photonMsgBuf mbuf, int *ind) {
  pthread_mutex_lock(&mbuf->buf_lock);
  {
    while (mbuf->entries[mbuf->s_index].empty == false) {
      mbuf->s_index++;
      if (mbuf->s_index == mbuf->p_count)
        mbuf->s_index = 0;
    }
    mbuf->entries[mbuf->s_index].empty = false;
  }
  pthread_mutex_unlock(&mbuf->buf_lock);

  *ind = mbuf->s_index;
  return &mbuf->entries[mbuf->s_index];
}

int photon_msgbuffer_free_entry(photonMsgBuf mbuf, int ind) {
  if ((ind < 0) || (ind >= mbuf->p_count)) {
    dbg_err("index %d is out of bounds", ind);
    return PHOTON_ERROR;
  }

  mbuf->entries[ind].empty = true;

  return PHOTON_OK;
}
