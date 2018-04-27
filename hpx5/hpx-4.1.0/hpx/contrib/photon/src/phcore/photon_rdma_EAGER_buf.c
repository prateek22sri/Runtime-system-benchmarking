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
#include <stddef.h>

#include "photon_rdma_EAGER_buf.h"
#include "photon_exchange.h"
#include "photon_event.h"
#include "logging.h"

static int _get_remote_progress(int proc, photonEagerBuf buf);

photonEagerBuf photon_rdma_eager_buf_create_reuse(uint8_t *eager_buffer, int size, int prefix) {
  photonEagerBuf new;
  
  new = (struct photon_rdma_eager_buf_t *)(eager_buffer + PHOTON_EBUF_SSIZE(size) -
					   sizeof(struct photon_rdma_eager_buf_t));
  
  memset(new, 0, sizeof(struct photon_rdma_eager_buf_t));
  
  new->data = eager_buffer;
  memset(new->data, 0, sizeof(uint8_t) * size);

  new->size = size;
  new->prog = 0;
  new->curr = 0;
  new->tail = 0;
  new->acct.rcur = 0;
  new->acct.rloc = 0;
  new->acct.event_prefix = prefix;

  return new;
}

void photon_rdma_eager_buf_free(photonEagerBuf buf) {
  //free(buf);
}

int photon_rdma_eager_buf_get_offset(int proc, photonEagerBuf buf, int size, int lim) {
  uint64_t curr, new, left, tail, rcur, wrap;
  int offset;
  int rc = __photon_backend->tx_size_left(proc);
  if (rc < 2) {
    return -1;
  }

  do {
    rcur = sync_load(&buf->acct.rcur, SYNC_RELAXED);
    curr = sync_load(&buf->curr, SYNC_RELAXED);
    tail = sync_load(&buf->tail, SYNC_RELAXED);

    if ((curr - tail) >= buf->size) {
      //log_err("Exceeded number of outstanding eager buf entries - increase size or wait for completion");
      return -1;
    }
    
    offset = curr & (buf->size - 1);
    left = buf->size - offset;
    if (left < lim) {
      new = curr + left + size;
      offset = 0;
      // if we wrap, then make sure remote has progressed by same amount
      wrap = left + size;
    }
    else {
      new = curr + size;
      wrap = 0;
    }

    if (((curr - rcur) + size + wrap) >= buf->size) {
      // receiver not ready, request an updated rcur
      _get_remote_progress(proc, buf);
      dbg_trace("No new offset until receiver catches up...");
      return -2;
    }
  } while (!sync_cas(&buf->curr, &curr, new, SYNC_RELAXED, SYNC_RELAXED));

  if (left < lim)
    sync_fadd(&buf->tail, left, SYNC_RELAXED);
  
  if (((curr - rcur) + size + wrap) >= (int)(buf->size * 0.8)) {
    // pro-actively request remote progress
    _get_remote_progress(proc, buf);
  }
  
  return offset;
}

static int _get_remote_progress(int proc, photonEagerBuf buf) {
  int rc;
  uint32_t rloc;
  uint64_t cookie;
  uintptr_t rmt_addr;
  photonRequest req;

  rc = __photon_try_one_event(&req);
  if (rc == PHOTON_EVENT_ERROR) {
    dbg_err("Failure getting event");
    return PHOTON_ERROR;
  }

  rloc = 0;
  if (!rloc && sync_cas(&buf->acct.rloc, &rloc, 1, SYNC_ACQUIRE, SYNC_RELAXED)) {
    
    dbg_trace("Fetching remote eager (%d) curr at rcur: %llu", proc, buf->acct.rcur);

    rmt_addr = buf->remote.addr + PHOTON_EBUF_SSIZE(buf->size) -
      sizeof(struct photon_rdma_eager_buf_t) + offsetof(struct photon_rdma_eager_buf_t, prog); 
    
    cookie = ( (uint64_t)buf->acct.event_prefix<<32) | proc;
    
    rc = __photon_backend->rdma_get(proc, (uintptr_t)&buf->acct.rcur, rmt_addr, sizeof(buf->acct.rcur),
				    &(shared_storage->buf), &buf->remote, cookie, 0);
    if (rc != PHOTON_OK) {
      dbg_err("RDMA GET for remote ledger progress counter failed");
      return PHOTON_ERROR;
    }
  }
  
  return PHOTON_OK;
}
