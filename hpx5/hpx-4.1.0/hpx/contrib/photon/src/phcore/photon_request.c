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
#include <assert.h>

#include "photon_backend.h"
#include "photon_request.h"
#include "photon_pwc.h"
#include "util.h"

static int __photon_cleanup_request(photonRequest req);
static int __photon_request_grow_table(photonRequestTable rt);

int photon_request_init(photonConfig cfg) {
  int i;
  // Setup request tables
  for (i = 0; i < (_photon_nproc + _photon_nforw); i++) {
    photon_processes[i].request_table = malloc(sizeof(struct photon_req_table_t));
    if (!photon_processes[i].request_table) {
      log_err("Could not allocate request table for proc %d", i);
      goto error_exit;
    }
    photonRequestTable rt = photon_processes[i].request_table;
    rt->count           = 0;
    rt->level           = 0;
    rt->next            = 0;
    rt->size            = cfg->cap.default_rd;
    rt->free            = (uint32_t*)malloc(DEF_NR_LEVELS * sizeof(uint32_t));
    rt->free[rt->level] = cfg->cap.default_rd;
    rt->reqs = (photonRequest*)malloc(DEF_NR_LEVELS * sizeof(photonRequest));
    if (!rt->reqs) {
      log_err("Could not allocate request array for proc %d", i);
      goto error_exit;
    }
    rt->reqs[rt->level] = (photonRequest)calloc(cfg->cap.default_rd, sizeof(struct photon_req_t));
    if (!rt->reqs[rt->level]) {
      log_err("Could not allocate request descriptors for proc %d", i);
      goto error_exit;
    }
    rt->pwc_q = sync_two_lock_queue_new();
    if (!rt->pwc_q) {
      log_err("Could not allocate pwc request queue for proc %d", i);
      goto error_exit;
    }
    rt->gwc_q = sync_two_lock_queue_new();
    if (!rt->gwc_q) {
      log_err("Could not allocate gwc request queue for proc %d", i);
      goto error_exit;
    }
    rt->gwc_pwc_q = sync_two_lock_queue_new();
    if (!rt->gwc_pwc_q) {
      log_err("Could not allocate gwc-pwc request queue for proc %d", i);
      goto error_exit;
    }
    rt->comp_q = sync_two_lock_queue_new();
    if (!rt->comp_q) {
      log_err("Could not allocate pwc completion queue for proc %d", i);
      goto error_exit;
    }
    rt->pcount = 0;
    rt->gcount = 0;
    rt->gpcount = 0;
    sync_tatas_init(&rt->tloc);
    sync_tatas_init(&rt->pq_loc);
    sync_tatas_init(&rt->gq_loc);
    sync_tatas_init(&rt->pack_loc);
  }

  return PHOTON_OK;

 error_exit:
  return PHOTON_ERROR;
}


photonRequest photon_get_request(int proc) {
  photonRequestTable rt;
  photonRequest      req, reqs;
  uint32_t           rid;
  
  assert(IS_VALID_PROC(proc));
  rt = photon_processes[proc].request_table;

  sync_tatas_acquire(&rt->tloc);
  {
    if (!rt->free[rt->level]) {
      dbg_trace("Request descriptors exhausted for proc %d, max=%u", proc, rt->size);
      if (__photon_request_grow_table(rt) != PHOTON_OK) {
	sync_tatas_release(&rt->tloc);
	return NULL;
      }
    }
    
    // get our current request buffer level
    reqs = rt->reqs[rt->level];
    
    // find the next free slot
    rt->next++;
    rt->next = (rt->next & (rt->size - 1));
    while (reqs[rt->next].id) {
      rt->next++;
      rt->next = (rt->next & (rt->size - 1));
    }
    rt->count++;
    rt->free[rt->level]--;
    
    req = &reqs[rt->next];  
    rid = (uint32_t)rt->level<<24;
    rid |= (uint32_t)((rt->next) + 1);
    
    memset(req, 0, sizeof(struct photon_req_t));
    req->id     = PROC_REQUEST_ID(proc, rid);
    req->state  = REQUEST_NEW;
    req->op     = REQUEST_OP_DEFAULT;
    req->flags  = REQUEST_FLAG_NIL;
    //bit_array_clear_all(req->mmask);
    
    dbg_trace("Returning a new request (count=%lu, free=%u) with id: 0x%016lx",
	      rt->count, rt->free[rt->level], req->id);
  }
  sync_tatas_release(&rt->tloc);
  
  return req;
}

photonRequest photon_lookup_request(photon_rid rid) {
  photonRequest req;
  photonRequestTable rt;
  uint32_t proc, id;
  uint16_t level;

  proc = (uint32_t)(rid>>32);
  level = (uint16_t)(rid<<32>>56);
  id = (uint32_t)(rid<<40>>40) - 1;

  assert(IS_VALID_PROC(proc));
  rt = photon_processes[proc].request_table;
  assert(id >= 0 && id < rt->size);
    
  sync_tatas_acquire(&rt->tloc);
  {
    req = &rt->reqs[level][id];
    if (req->state == REQUEST_FREE) {
      dbg_warn("Looking up a request that is freed, op=%d, type=%d, id=0x%016lx",
	       req->op, req->type, rid);
    }
  }
  sync_tatas_release(&rt->tloc);
  
  return req;
}

int photon_count_request(int proc) {
  photonRequestTable rt;
  int i, j, start, end, count = 0;
  if (proc == PHOTON_ANY_SOURCE) {
    start = 0;
    end = _photon_nproc;
  }
  else {
    start = proc;
    end = proc+1;
  }
  for (i=start; i<end; i++) {
    rt = photon_processes[i].request_table;
    sync_tatas_acquire(&rt->tloc);
    {
      for (j=(rt->level); j >= 0; j--) {
	count += ((rt->size >> (rt->level-j)) - rt->free[j]);
      }
    }
    sync_tatas_release(&rt->tloc);
  }
  return count;
}

int photon_free_request(photonRequest req) {
  photonRequestTable rt;
  uint32_t proc = (uint32_t)(req->id>>32);
  uint16_t level = (uint16_t)(req->id<<32>>56);
  dbg_trace("Clearing request 0x%016lx", req->id);
  __photon_cleanup_request(req);
  rt = photon_processes[proc].request_table;
  sync_tatas_acquire(&rt->tloc);
  {
    if (req->local_info.id.size) {
      free(req->local_info.id.data);
    }
    req->id    = NULL_REQUEST;
    req->state = REQUEST_FREE;
    rt->free[level]++;
  }
  sync_tatas_release(&rt->tloc);
  return PHOTON_OK;
}

photonRequest photon_setup_request_direct(photonBuffer lbuf, photonBuffer rbuf, uint64_t size,
					  int proc, int events) {
  photonRequest req;
  
  req = photon_get_request(proc);
  if (!req) {
    log_err("Couldn't allocate request");
    goto error_exit;
  }

  dbg_trace("Setting up a direct request: %d/0x%016lx/%p", proc, req->id, req);

  req->state = REQUEST_PENDING;
  req->type = EVQUEUE;
  req->proc = proc;
  req->size = size;
  req->tag = 0;
  req->rattr.events = events;
  req->rattr.cookie = req->id;

  if (lbuf) {
    memcpy(&req->local_info.buf, lbuf, sizeof(*lbuf));
    req->local_info.id.u64 = 0;

    dbg_trace("Local info ...");
    dbg_trace("  Addr: %p", (void *)lbuf->addr);
    dbg_trace("  Size: %lu", lbuf->size);
    dbg_trace("  Keys: 0x%016lx / 0x%016lx", lbuf->priv.key0, lbuf->priv.key1);
  }
  
  if (rbuf) {
    // fill in the internal buffer with the rbuf contents
    memcpy(&req->remote_info.buf, rbuf, sizeof(*rbuf));
    // there is no matching request from the remote side, so fill in local values */
    req->remote_info.id.u64 = 0;
    
    dbg_trace("Remote info ...");
    dbg_trace("  Addr: %p", (void *)rbuf->addr);
    dbg_trace("  Size: %lu", rbuf->size);
    dbg_trace("  Keys: 0x%016lx / 0x%016lx", rbuf->priv.key0, rbuf->priv.key1);
  }
  
  return req;
  
 error_exit:
  return NULL;
}

photonRequest photon_setup_request_ledger_info(photonRILedgerEntry ri_entry, int curr, int proc) {
  photonRequest req;

  req = photon_get_request(proc);
  if (!req) {
    log_err("Couldn't allocate request\n");
    goto error_exit;
  }

  dbg_trace("Setting up a new send buffer request: %d/0x%016lx/%p", proc, req->id, req);

  req->state        = REQUEST_NEW;
  req->type         = EVQUEUE;
  req->proc         = proc;
  req->flags        = ri_entry->flags;
  req->size         = ri_entry->size;
  req->rattr.events = 1;

  // save the remote buffer in the request
  req->remote_info.id.u64       = ri_entry->request;
  req->remote_info.buf.addr = ri_entry->addr;
  req->remote_info.buf.size = ri_entry->size;
  req->remote_info.buf.priv = ri_entry->priv;
  
  dbg_trace("Remote request: 0x%016lx", ri_entry->request);
  dbg_trace("Addr: %p", (void *)ri_entry->addr);
  dbg_trace("Size: %lu", ri_entry->size);
  dbg_trace("Tag: %d",	ri_entry->tag);
  dbg_trace("Keys: 0x%016lx / 0x%016lx", ri_entry->priv.key0, ri_entry->priv.key1);

  // reset the info ledger entry
  ri_entry->header = 0;
  ri_entry->footer = 0;
  
  return req;
  
 error_exit:
  return NULL;
}

photonRequest photon_setup_request_ledger_eager(photonRDMALedgerEntry entry, int curr, int proc) {
  photonRequest req;

  req = photon_get_request(proc);
  if (!req) {
    log_err("Couldn't allocate request\n");
    goto error_exit;
  }

  dbg_trace("Setting up a new eager buffer request: %d/0x%016lx/%p", proc, req->id, req);

  req->state        = REQUEST_NEW;
  req->type         = EVQUEUE;
  req->proc         = proc;
  req->flags        = REQUEST_FLAG_EAGER;
  req->size         = (entry->request>>32);
  req->rattr.events = 1;

  req->remote_info.buf.size = req->size;
  req->remote_info.id.u64   = (( (uint64_t)_photon_myrank)<<32) | (entry->request<<32>>32);
  
  // reset the info ledger entry
  entry->request = 0;

  return req;
  
 error_exit:
  return NULL;
}

/* generates a new request for the recv wr
   calling this means we got an event for a corresponding post_recv()
   we know the recv mbuf entry index
   we inspected the UD hdr and determined the current sequence number
   this setup method also returns the request pointer... */
photonRequest photon_setup_request_recv(photonAddr addr, int msn, int msize, int bindex, int nbufs) {
  photonRequest req;

  req = photon_get_request(addr->global.proc_id);
  if (!req) {
    log_err("Couldn't allocate request\n");
    goto error_exit;
  }

  req->tag          = PHOTON_ANY_TAG;
  req->state        = REQUEST_PENDING;
  req->type         = SENDRECV;
  req->proc         = addr->global.proc_id;
  req->size         = msize;
  req->rattr.events = nbufs;
  //req->bentries[msn] = bindex;
  //memcpy(&req->addr, addr, sizeof(*addr));
  
  //bit_array_set(req->mmask, msn);

  return req;
  
 error_exit:
  return NULL;
}

photonRequest photon_setup_request_send(photonAddr addr, int *bufs, int nbufs) {
  photonRequest req;

  req = photon_get_request(addr->global.proc_id);
  if (!req) {
    log_err("Couldn't allocate request\n");
    goto error_exit;
  }

  req->tag          = PHOTON_ANY_TAG;
  req->state        = REQUEST_PENDING;
  req->type         = SENDRECV;
  req->size         = 0;
  req->rattr.events = nbufs;
  //memcpy(&req->addr, addr, sizeof(*addr));
  //memcpy(req->bentries, bufs, sizeof(int)*DEF_MAX_BUF_ENTRIES);
  
  return req;
  
 error_exit:
  return NULL;
}

static int __photon_cleanup_request(photonRequest req) {
  switch (req->op) {
  case REQUEST_OP_SENDBUF:
    if (req->flags & REQUEST_FLAG_EAGER) {
      MARK_DONE(photon_processes[req->proc].remote_eager_buf, req->size);
      MARK_DONE(photon_processes[req->proc].remote_eager_ledger, 1);
    }
    else {
      MARK_DONE(photon_processes[req->proc].remote_snd_info_ledger, 1);
    }
    break;
  case REQUEST_OP_SENDREQ:
    MARK_DONE(photon_processes[req->proc].remote_snd_info_ledger, 1);
    break;
  case REQUEST_OP_SENDFIN:
    break;
  case REQUEST_OP_RECVBUF:
    MARK_DONE(photon_processes[req->proc].remote_rcv_info_ledger, 1);
    break;
  case REQUEST_OP_PWC:
    if (req->flags & REQUEST_FLAG_1PWC) {
      assert(req->psize);
      MARK_DONE(photon_processes[req->proc].remote_pwc_buf, req->psize);
    }
    if ((req->flags & REQUEST_FLAG_2PWC) &&
	!(req->flags & REQUEST_FLAG_NO_RCE)) {
      MARK_DONE(photon_processes[req->proc].remote_pwc_ledger, 1);
    }
    break;
  case REQUEST_OP_COLL:
    break;
  case REQUEST_OP_DEFAULT:
    break;
  default:
    log_err("Tried to cleanup a request op we don't recognize: %d", req->op);
    break;
  }

  return PHOTON_OK;
}

static int __photon_request_grow_table(photonRequestTable rt) {
  uint64_t nsize = rt->size << 1;

  if ((rt->level + 1) >= DEF_NR_LEVELS) {
    dbg_err("Exceeded max request table buffer allocations: %u", DEF_NR_LEVELS);
    return PHOTON_ERROR;
  }

  if (__photon_config->cap.max_rd && (nsize > __photon_config->cap.max_rd)) {
    dbg_err("Exceeded max allowable request descriptors: %d",
	    __photon_config->cap.max_rd);
    return PHOTON_ERROR;
  }

  rt->level++;

  rt->reqs[rt->level] = (photonRequest)calloc(nsize, sizeof(struct photon_req_t));
  if (!rt->reqs[rt->level]) {
    dbg_err("Could not increase request table size to %lu", nsize);
    return PHOTON_ERROR;
  }
  
  rt->size            = nsize;
  rt->free[rt->level] = nsize;
  rt->next            = 0;
  
  dbg_trace("Resized request table: %lu (next: %lu)", nsize, rt->next);

  return PHOTON_OK;
}
