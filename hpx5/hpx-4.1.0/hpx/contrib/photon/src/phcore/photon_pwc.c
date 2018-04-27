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
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "photon_backend.h"
#include "photon_buffertable.h"
#include "photon_event.h"
#include "photon_pwc.h"
#include "photon_coll.h"
#include "util.h"

static int photon_pwc_test_gwc_pwc(int proc, int *ret_offset);
static int photon_pwc_safe_ledger(photonRequest req);
static int photon_pwc_test_ledger(int proc, int *ret_offset);
static int photon_pwc_try_ledger(photonRequest req, int curr);
static int photon_pwc_safe_packed(photonRequest req);
static int photon_pwc_test_packed(int proc, uint64_t size, int *ret_offset);
static int photon_pwc_try_packed(photonRequest req, int offset);
static int photon_pwc_try_gwc(photonRequest req);
static int photon_pwc_try_gwc_pwc(photonRequest req, int offset);
static int photon_pwc_handle_comp_req(photonRequest req, int *flag,
				      photon_cid *c, void (*cb)(photon_cid));
static int photon_pwc_pack_cid(photonRequest req, photonCIDLedgerEntry entry);

static two_lock_queue_t  *comp_q;
static uint16_t         **pbufs;
static volatile uint32_t  qcount = 0;
static void              *pwc_ctype_handler_table[PWC_CTYPE_MAX];

//static inline uint32_t photon_pwc_lcid_size(photonRequest req) {
//  photon_cid *lid = &req->local_info.id;
//  return (lid->size) ? lid->size : sizeof(lid->u64);
//}

static inline uint32_t photon_pwc_rcid_size(photonRequest req) {
  photon_cid *rid = &req->remote_info.id;
  return (rid->size) ? rid->size : sizeof(rid->u64);
}

static inline uint64_t photon_pwc_packed_asize(photonRequest req) {
  uint32_t rsize = photon_pwc_rcid_size(req);
  return ALIGN(EB_MSG_SIZE(req->size + rsize), PWC_ALIGN);
}

static inline int photon_pwc_pack_cid(photonRequest req,
				      photonCIDLedgerEntry e) {
  photon_cid *rid = &(req->remote_info.id);
  uint32_t csize = (rid->size) ? rid->size : sizeof(rid->u64);

  e->hdr.type = req->chdr.type;
  e->hdr.cmd = req->chdr.cmd;
  e->hdr.size = csize;

  if (rid->size) {
    memcpy(e->data, rid->data, csize);
  }
  else {
    memcpy(e->data, &(rid->u64), csize);
  }
  return PHOTON_OK;
}

static inline int photon_pwc_save_lcid(photonRequest req) {
  photon_cid *lid = &(req->local_info.id);
  if (lid->size) {
    void *p = malloc(lid->size);
    memcpy(p, lid->data, lid->size);
    lid->data = p;
  }
  return PHOTON_OK;
}

static inline int photon_pwc_check_gwc_align(photonBuffer lbuf, photonBuffer rbuf,
					     uint64_t size) {
  int *align;
  int asize = 1;
  int rc;

  // default to no alignment constraint
  align = &asize;

  rc =  __photon_backend->get_info(NULL, 0, (void**)&align, &asize, PHOTON_GET_ALIGN);
  if (rc != PHOTON_OK) {
    dbg_warn("Could not get alignment info from backend");
  }

  if (!TEST_ALIGN(lbuf->addr, *align) ||
      !TEST_ALIGN(rbuf->addr, *align) ||
      !TEST_ALIGN(size, *align)) {
    return PHOTON_ERROR;
  }
  
  return PHOTON_OK;
}

static int photon_pwc_process_command(int proc, photon_cid_recv_hdr *rhdr,
				      photon_cid_hdr *chdr, void *cdata, 
				      uintptr_t addr, uint16_t size, void *ptr) {

  int ctype = chdr->type;
  int cmd = chdr->cmd;

  switch (ctype) {
  case PWC_CTYPE_INTERNAL:
    {
      switch (cmd) {
      case PWC_COMMAND_PWC_REQ:
	{
	  int rc;
	  photon_cid cid = {
	    .u64 = (uint64_t)addr,
	    .size = 0
	  };
	  struct photon_buffer_t lbuf, rbuf;
	  // make sure we took this branch with a valid payload
	  assert(ptr);
	  // switch the sent lbuf/rbuf
	  memcpy(&rbuf, ptr, sizeof(rbuf));
	  memcpy(&lbuf, ptr+sizeof(rbuf), sizeof(lbuf));
	  rc = _photon_put_with_completion(proc, lbuf.size, &lbuf, &rbuf, cid, cid,
					   PHOTON_REQ_PWC_NO_LCE,
					   PWC_CTYPE_INTERNAL, PWC_COMMAND_PWC_REP);
	  if (rc != PHOTON_OK) {
	    log_err("Could not complete PWC_REQ command");
	    goto error_exit;
	  }
	}
	break;
      case PWC_COMMAND_PWC_REP:
	{
	  // cid encodes the GWC request that initiated the PWC
	  photonRequest req;
	  photon_rid rid = *(photon_rid*)cdata;
	  
	  // copy the payload if reply was packed
	  if (addr && size) {
	    memcpy((void*)addr, ptr, size);

	    // if the message was packed, we're done
	    req = photon_lookup_request(rid);
	    if (!req) {
	      log_err("Could not find request in PWC_REP");
	      goto error_exit;
	    }
	    photon_pwc_add_req(req);
	  }
	  else {
	    // save the rid until we know the 2-put completed
	    sync_store(&(rhdr->req), rid, SYNC_RELAXED);
	  }
	}
	break;
      default:
	log_err("Unknown INTERNAL command received: 0x%016lx", cmd);
	break;
      }
    }
    break;
  default:
    {
      // hand off the completion info to a registered handler for the ctype
      void (*handler)(int, pwc_command, photon_cid, void*, int); 
      handler = pwc_ctype_handler_table[ctype];
      if (handler) {
	photon_cid cid = {
	  .data = cdata,
	  .size = chdr->size
	};
	handler(proc, cmd, cid, ptr, size);
      }
      else {
	dbg_info("No handler defined for ctype %d", ctype);
      }
    }
    break;
  }
  
  return PHOTON_OK;

 error_exit:
  return PHOTON_ERROR;
}

static int photon_pwc_handle_comp_req(photonRequest req, int *flag, photon_cid *c,
				      void (*cb)(photon_cid)) {
  int rc;
  
  // sometimes the requestor doesn't care about the completion
  if (! (req->flags & REQUEST_FLAG_NO_LCE)) {
    *flag = 1;
    *c = req->local_info.id;
    
    // invoke the callback (CID data is freed with request)
    if (cb) {
      cb(*c);
    }
  }
  
  // in addition to the callback, invoke any internal handlers
  // registered for the PWC ctype used in the request
  void (*handler)(int, pwc_command, photon_cid, void*, int);
  handler = pwc_ctype_handler_table[req->chdr.type];
  if (handler) {
    handler(_photon_myrank, req->chdr.cmd, req->local_info.id, NULL, 0);
  }
  else {
    dbg_trace("No handler defined for ctype %d", req->chdr.type);
  }
  
  if (req->flags & REQUEST_FLAG_ROP) {
    // sends a remote completion after the GWC
    // this GWC request now becomes a PWC
    // and we reap the put completion internally
    req->flags |= (REQUEST_FLAG_NO_LCE | REQUEST_FLAG_CMD);
    req->flags &= ~(REQUEST_FLAG_ROP);
    req->rattr.cookie = req->id;
    rc = photon_pwc_safe_ledger(req);
    if (rc != PHOTON_OK) {
      photonRequestTable rt;
      rt = photon_processes[req->proc].request_table;
      sync_two_lock_queue_enqueue(rt->pwc_q, req);
      sync_fadd(&rt->pcount, 1, SYNC_RELAXED);
      sync_fadd(&qcount, 1, SYNC_RELAXED);
      dbg_trace("Enqueing ROP PWC req: 0x%016lx", req->id);
    }
    goto no_free;
  }

  dbg_trace("Completed and removing PWC/GWC request: 0x%016lx (lid=0x%016lx)",
	    req->id, req->local_info.id);
  photon_free_request(req);
  
 no_free:
  return PHOTON_OK;
}

static int photon_pwc_process_queued_gwc(int proc, photonRequestTable rt) {
  photonRequest req;
  uint32_t val;
  int offset, rc;

  val = sync_load(&rt->gcount, SYNC_RELAXED);
  if (!val) {
    goto pack;
  }

  sync_tatas_acquire(&rt->gq_loc);
  {  
    // try to get a regular queued gwc first
    val = sync_load(&rt->gcount, SYNC_RELAXED);
    if (val) {
      rc = __photon_backend->tx_size_left(proc);
      if (rc < 1) {
	sync_tatas_release(&rt->gq_loc);
	goto error_resource;
      }
      req = sync_two_lock_queue_dequeue(rt->gwc_q);
      rc = photon_pwc_try_gwc(req);
      if (rc != PHOTON_OK) {
	sync_tatas_release(&rt->gq_loc);
	goto error_exit;
      }
      sync_fadd(&rt->gcount, -1, SYNC_RELAXED);
      sync_fadd(&qcount, -1, SYNC_RELAXED);
      sync_tatas_release(&rt->gq_loc);
      goto exit_ok;
    }
  }
  sync_tatas_release(&rt->gq_loc);

 pack:
  val = sync_load(&rt->gpcount, SYNC_RELAXED);
  if (!val) {
    goto error_exit;
  }

  sync_tatas_acquire(&rt->pack_loc);
  {
    // try to get a queued gwc-pwc request
    val = sync_load(&rt->gpcount, SYNC_RELAXED);
    if (val) {
      rc = photon_pwc_test_gwc_pwc(proc, &offset);
      if (rc == PHOTON_ERROR_RESOURCE) {
	sync_tatas_release(&rt->pack_loc);
	goto error_resource;
      }
      else if (rc == PHOTON_OK) {
	req = sync_two_lock_queue_dequeue(rt->gwc_pwc_q);
	rc = photon_pwc_try_gwc_pwc(req, offset);
	if (rc != PHOTON_OK) {
	  dbg_err("Could not send queued PWC request");
	  sync_tatas_release(&rt->pack_loc);
	  goto error_exit;
	}
	sync_fadd(&rt->gpcount, -1, SYNC_RELAXED);
	sync_fadd(&qcount, -1, SYNC_RELAXED);
	sync_tatas_release(&rt->pack_loc);
	goto exit_ok;
      }
    }
  }
  sync_tatas_release(&rt->pack_loc);
  
 error_exit:
  return PHOTON_ERROR;
  
 error_resource:
  return PHOTON_ERROR_RESOURCE;
  
 exit_ok:
  return PHOTON_OK;
}

static int photon_pwc_process_queued_pwc(int proc, photonRequestTable rt) {
  photonRequest req;
  uint32_t val;
  int offset, rc;

  val = sync_load(&rt->pcount, SYNC_RELAXED);
  if (!val) {
    return PHOTON_ERROR;
  }
  
  sync_tatas_acquire(&rt->pq_loc);
  {
    val = sync_load(&rt->pcount, SYNC_RELAXED);
    if (!val) {
      goto error_exit;
    }
    // only dequeue a request if there is one and we can send it
    rc = photon_pwc_test_ledger(proc, &offset);
    if (rc == PHOTON_OK) {
      req = sync_two_lock_queue_dequeue(rt->pwc_q);
      rc = photon_pwc_try_ledger(req, offset);
      if (rc != PHOTON_OK) {
 	dbg_err("Could not send queued PWC request");
	goto error_exit;
      }
      sync_fadd(&rt->pcount, -1, SYNC_RELAXED);
      sync_fadd(&qcount, -1, SYNC_RELAXED);
    }
    else {
      goto error_resource;
    }
  }
  sync_tatas_release(&rt->pq_loc);
  
  return PHOTON_OK;

 error_resource:
  sync_tatas_release(&rt->pq_loc);
  return PHOTON_ERROR_RESOURCE;
  
 error_exit:
  sync_tatas_release(&rt->pq_loc);
  return PHOTON_ERROR;
}

static int photon_pwc_try_gwc_pwc(photonRequest req, int offset) {
  photonEagerBuf eb;
  photon_eb_hdr *hdr;
  uint64_t asize, rsize, imm_data = 0;
  uintptr_t eager_addr;
  uint8_t *tail;
  void *dptr;
  int rc;

  dbg_trace("Performing GWC-PUT for req: 0x%016lx", req->id);
  
  // make sure the original request size is encoded in the local/remote bufs
  req->local_info.buf.size = req->size;
  req->remote_info.buf.size = req->size;

  req->flags        |= REQUEST_FLAG_1PWC;
  req->rattr.events  = 1; // N/A, waits for PWC reply from remote
  req->rattr.cookie  = ( (uint64_t)REQUEST_COOK_GPWC<<32) | req->proc;

  rsize = 2 * sizeof(struct photon_buffer_t);
  asize = ALIGN(EB_MSG_SIZE(rsize), PWC_ALIGN);
  req->psize = asize;  // accounting

  eb = photon_processes[req->proc].remote_pwc_buf;
  eager_addr = (uintptr_t)eb->remote.addr + offset;
  hdr = (photon_eb_hdr *)&(eb->data[offset]);
  hdr->header         = UINT8_MAX;
  hdr->cent.type      = PWC_CTYPE_INTERNAL;
  hdr->cent.cmd       = PWC_COMMAND_PWC_REQ;
  hdr->cent.size      = 0; // no need to send CID data here
  hdr->addr           = req->id;
  hdr->length         = rsize;
  hdr->footer         = UINT8_MAX;

  // copy the local buffer
  dptr = (void*)((uintptr_t)hdr + sizeof(*hdr));
  memcpy(dptr, (void*)&req->local_info.buf, sizeof(req->local_info.buf));

  // copy the remote buffer
  dptr += sizeof(req->local_info.buf);
  memcpy(dptr, (void*)&req->remote_info.buf, sizeof(req->remote_info.buf));

  // set a tail flag, the last byte in aligned buffer
  tail = (uint8_t*)(uintptr_t)((uintptr_t)hdr + asize - 1);
  *tail = UINT8_MAX;
  
  imm_data  = ENCODE_RCQ_32(PHOTON_ETYPE_NTFY,
			    PHOTON_EFLAG_PACK,
			    PHOTON_EFLAG_ONE,
			    offset,
			    _photon_myrank);
  
  rc = __photon_backend->rdma_put(req->proc, (uintptr_t)hdr, (uintptr_t)eager_addr, asize,
				  &(shared_storage->buf), &eb->remote, req->rattr.cookie,
				  imm_data, RDMA_FLAG_WITH_IMM);
  if (rc != PHOTON_OK) {
    dbg_err("RDMA PUT (GWC-PUT) failed for 0x%016lx", req->rattr.cookie);
    goto error_exit;
  }  
  
  dbg_trace("Posted GWC-PUT Request: 0x%016lx", req->id);
  
  return PHOTON_OK;
  
 error_exit:
  return PHOTON_ERROR;
}

static int photon_pwc_try_gwc(photonRequest req) {
  int rc;
  rc = __photon_backend->rdma_get(req->proc, req->local_info.buf.addr,
				  req->remote_info.buf.addr, req->size,
				  &req->local_info.buf,
				  &req->remote_info.buf,
				  req->rattr.cookie, RDMA_FLAG_NIL);
  
  if (rc != PHOTON_OK) {
    dbg_err("RDMA GET (PWC data) failed for 0x%016lx", req->rattr.cookie);
    goto error_exit;
  }
  
  dbg_trace("Posted GWC Request: %d/0x%016lx/0x%016lx", req->proc,
	    req->local_info.id,
	    req->remote_info.id);  
  
  return PHOTON_OK;
  
 error_exit:
  return PHOTON_ERROR;
}

static int photon_pwc_test_gwc_pwc(int proc, int *ret_offset) {
  int rc, offset;
  int rsize, asize;
  
  // determine if there's space to initiate the GWC-PWC request
  rsize = 2 * sizeof(struct photon_buffer_t);
  asize = ALIGN(EB_MSG_SIZE(rsize), PWC_ALIGN);
  
  rc = photon_pwc_test_packed(proc, asize, &offset);
  if (rc == PHOTON_OK) {
    *ret_offset = offset;
  }
  
  return rc;
}

static int photon_pwc_safe_packed(photonRequest req) {
  photonRequestTable rt;
  int rc, offset;
  uint64_t asize;

  // only do packed if size makes sense
  if ((req->size <= 0) ||
      (req->size > _photon_upsize) ||
      (req->size > _photon_pbsize)) {
    return PHOTON_ERROR_RESOURCE;
  }

  // keep buffer offsets aligned
  asize = photon_pwc_packed_asize(req);

  rt = photon_processes[req->proc].request_table;
  sync_tatas_acquire(&rt->pack_loc);
  {
    rc = photon_pwc_test_packed(req->proc, asize, &offset);
    if (rc == PHOTON_OK) {
      req->psize = asize;  // accounting
      rc = photon_pwc_try_packed(req, offset);
    }
  }
  sync_tatas_release(&rt->pack_loc);

  return rc;
}

static int photon_pwc_test_packed(int proc, uint64_t size, int *ret_offset) {
  photonEagerBuf eb;
  int offset;

  eb = photon_processes[proc].remote_pwc_buf;
  offset = photon_rdma_eager_buf_get_offset(proc, eb, size, PWC_MAX_PACKED);
  if (offset < 0) {
    return PHOTON_ERROR_RESOURCE;
  }
  *ret_offset = offset;
  return PHOTON_OK;
}

static int photon_pwc_try_packed(photonRequest req, int offset) {
  // see if we should pack into an eager buffer and send in one put
  photonEagerBuf eb;
  photon_cid *rid;
  photon_eb_hdr *hdr;
  uint64_t asize, imm_data = 0;
  uintptr_t eager_addr;
  void *dptr;
  uint8_t *tail;
  uint32_t rsize;
  int rc;
  
  req->flags        |= REQUEST_FLAG_1PWC;
  req->rattr.events  = 1;

  rid   = &(req->remote_info.id);
  rsize = photon_pwc_rcid_size(req);
  asize = photon_pwc_packed_asize(req);

  eb = photon_processes[req->proc].remote_pwc_buf;
  eager_addr = (uintptr_t)eb->remote.addr + offset;
  hdr = (photon_eb_hdr *)&(eb->data[offset]);
  hdr->header         = UINT8_MAX;
  hdr->cent.type      = req->chdr.type;
  hdr->cent.cmd       = req->chdr.cmd;
  hdr->cent.size      = rsize;
  hdr->addr           = req->remote_info.buf.addr;
  hdr->length         = (uint16_t)req->size;
  hdr->footer         = UINT8_MAX;

  // copy in the cid
  dptr = (void*)((uintptr_t)hdr + sizeof(*hdr));
  if (rid->size) {
    memcpy(dptr, rid->data, rsize);
  }
  else {
    memcpy(dptr, &(rid->u64), rsize);
  }
    
  // copy the payload
  dptr += rsize;
  memcpy(dptr, (void*)req->local_info.buf.addr, req->size);

  // set a tail flag, the last byte in aligned buffer
  tail = (uint8_t*)(uintptr_t)((uintptr_t)hdr + asize - 1);
  *tail = UINT8_MAX;
  
  imm_data  = ENCODE_RCQ_32(PHOTON_ETYPE_NTFY,
			    PHOTON_EFLAG_PACK,
			    PHOTON_EFLAG_ONE,
			    offset,
			    _photon_myrank);

  rc = __photon_backend->rdma_put(req->proc, (uintptr_t)hdr, (uintptr_t)eager_addr, asize,
				  &(shared_storage->buf), &eb->remote, req->rattr.cookie,
				  imm_data, RDMA_FLAG_WITH_IMM);
  if (rc != PHOTON_OK) {
    dbg_err("RDMA PUT (PWC EAGER) failed for 0x%016lx", req->rattr.cookie);
    goto error_exit;
  }
  
  dbg_trace("Posted PWC Request: %d/0x%016lx/0x%016lx", req->proc,
	    req->local_info.id,
	    req->remote_info.id);
  
  return PHOTON_OK;
  
 error_exit:
  return PHOTON_ERROR;
}

static int photon_pwc_safe_ledger(photonRequest req) {
  int rc, offset;
  photonRequestTable rt = photon_processes[req->proc].request_table;
  sync_tatas_acquire(&rt->pq_loc);
  {
    rc = photon_pwc_test_ledger(req->proc, &offset);
    if (rc == PHOTON_OK) {
      rc = photon_pwc_try_ledger(req, offset);
    }
  }
  sync_tatas_release(&rt->pq_loc);
  return rc;
}

static int photon_pwc_test_ledger(int proc, int *ret_offset) {
  photonLedger l;
  int curr;
  l = photon_processes[proc].remote_pwc_ledger;
  curr = photon_rdma_ledger_get_next(proc, l);
  if (curr < 0) {
    return PHOTON_ERROR_RESOURCE;
  }
  *ret_offset = curr;
  return PHOTON_OK;
}

static int photon_pwc_try_ledger(photonRequest req, int curr) {
  photonBI db;
  uintptr_t rmt_addr;
  uint64_t imm_data = 0;
  int rc, iflag, rflags;
  
  req->flags |= REQUEST_FLAG_2PWC;
  req->rattr.events = 1;

  if ((req->size) > 0 && !(req->flags & REQUEST_FLAG_CMD)) {
    if (!req->local_info.buf.priv.key0 && !req->local_info.buf.priv.key1) {
      if (buffertable_find_containing( (void *)req->local_info.buf.addr,
				       req->size, &db) != 0) {
	log_err("Tried posting from a buffer that's not registered");
	goto error_exit;
      }
      req->local_info.buf.priv = db->buf.priv;
    }
    
    if (! (req->flags & REQUEST_FLAG_NO_RCE)) {
      imm_data  = ENCODE_RCQ_32(PHOTON_ETYPE_DATA,
				PHOTON_EFLAG_LEDG,
				PHOTON_EFLAG_ONE,
				curr,
				_photon_myrank);
      rflags = RDMA_FLAG_WITH_IMM;
      req->rattr.events = 2;
    }
    else {
      rflags = RDMA_FLAG_NIL;
    }

    rc = __photon_backend->rdma_put(req->proc,
				    req->local_info.buf.addr,
				    req->remote_info.buf.addr,
				    req->size,
				    &req->local_info.buf,
				    &req->remote_info.buf,
				    req->rattr.cookie,
				    imm_data,
				    rflags);
    if (rc != PHOTON_OK) {
      dbg_err("RDMA PUT (PWC data) failed for 0x%016lx", req->rattr.cookie);
      goto error_exit;
    }
    iflag = PHOTON_EFLAG_TWO;
  }
  else {
    iflag = PHOTON_EFLAG_ONE;
  }
  
  if (! (req->flags & REQUEST_FLAG_NO_RCE)) {
    photonLedger l = photon_processes[req->proc].remote_pwc_ledger;
    photonCIDLedgerEntry entry = (photonCIDLedgerEntry)((uintptr_t)l->entries + 
							(curr * l->entry_size));
    
    photon_pwc_pack_cid(req, entry);
    
    rmt_addr = l->remote.addr + (curr * PHOTON_CID_RECV_ENTRY_SIZE) + 
      sizeof(photon_cid_recv_hdr);
    
    imm_data  = ENCODE_RCQ_32(PHOTON_ETYPE_NTFY,
			      PHOTON_EFLAG_LEDG,
			      iflag,
			      curr,
			      _photon_myrank);

    rc = __photon_backend->rdma_put(req->proc, (uintptr_t)entry, rmt_addr,
				    sizeof(*entry)+entry->hdr.size,
				    &(shared_storage->buf),
				    &(l->remote), req->rattr.cookie,
				    imm_data, RDMA_FLAG_WITH_IMM);

    if (rc != PHOTON_OK) {
      dbg_err("RDMA PUT (PWC comp) failed for 0x%016lx", req->rattr.cookie);
      goto error_exit;
    }
  }
  
  dbg_trace("Posted PWC Request: %d/0x%016lx/0x%016lx/0x%016lx", req->proc,
	    req->rattr.cookie,
	    req->local_info.id,
	    req->remote_info.id);
  
  return PHOTON_OK;

 error_exit:
  return PHOTON_ERROR;
}

int photon_pwc_init(photonConfig cfg) {
  comp_q = sync_two_lock_queue_new();
  if (!comp_q) {
    log_err("Could not allocate PWC completion queue");
    goto error_exit;
  }

  pbufs = (uint16_t**)malloc(_photon_nproc*sizeof(uint16_t*));
  if (!pbufs) {
    log_err("Could not allocate PWC recv acct buffers");
    goto error_exit;
  }
  for (int i = 0; i < _photon_nproc; i++) {
    pbufs[i] = (uint16_t*)calloc(_photon_pbsize, sizeof(uint16_t));
  }
  
  // initialize ctype handlers to NULL
  for (int i = 0; i < PWC_CTYPE_MAX; i++) {
    pwc_ctype_handler_table[i] = NULL;
  }
  
  return PHOTON_OK;
  
 error_exit:
  return PHOTON_ERROR;
}

int photon_pwc_register_ctype_handler(pwc_cid_type type, void *handler) {
  pwc_ctype_handler_table[type] = handler;
  dbg_trace("Registered ctype %d handler %p", type, handler);
  return PHOTON_OK;
}

int photon_pwc_add_req(photonRequest req) {
  sync_two_lock_queue_enqueue(comp_q, req);
  dbg_trace("Enqueing completed request: 0x%016lx", req->id);
  return PHOTON_OK;
}

photonRequest photon_pwc_pop_req(int proc) {
  return sync_two_lock_queue_dequeue(comp_q);
}

int _photon_put_with_completion(int proc, uint64_t size,
				photonBuffer lbuf,
				photonBuffer rbuf,
				photon_cid local, photon_cid remote,
				int flags,
				pwc_cid_type type, pwc_command cmd) {
  photonRequest req;
  photonRequestTable rt;
  int rc;

  dbg_trace("(%d, size: %lu, lid: 0x%016lx, rid: 0x%016lx, flags: %d)", proc,
	    size, local.u64, remote.u64, flags);

  if (local.size && local.size > _photon_idsize) {
    log_err("Local CID size is larger than configured max: %d/%d",
	    local.size, _photon_idsize);
    goto error_exit;
  }

  if (remote.size && remote.size > _photon_idsize) {
    log_err("Remote CID size is larger than configured max: %d/%d",
	    remote.size, _photon_idsize);
    goto error_exit;
  }

  if (size && !lbuf) {
    log_err("Trying to put size %lu and NULL lbuf", size);
    goto error_exit;
  }

  if (size && !rbuf) {
    log_err("Trying to put size %lu and NULL rbuf", size);
    goto error_exit;
  }

  if (!size && (flags & PHOTON_REQ_PWC_NO_RCE)) {
    dbg_warn("Nothing to send and no remote completion requested!");
    return PHOTON_OK;
  }

  if (size && !rbuf->priv.key0 && !rbuf->priv.key1) {
    dbg_warn("No remote buffer keys specified!");
  }

  req = photon_setup_request_direct(lbuf, rbuf, size, proc, 0);
  if (!req) {
    dbg_err("Could not allocate request");
    goto error_exit;
  }
  
  req->op             = REQUEST_OP_PWC;
  req->chdr.type      = type;
  req->chdr.cmd       = cmd;
  req->local_info.id  = local;
  req->remote_info.id = remote;

  photon_pwc_save_lcid(req);
  
  // control the return of the local id
  if (flags & PHOTON_REQ_PWC_NO_LCE) {
    req->flags |= REQUEST_FLAG_NO_LCE;
  }

  // control the return of the remote id
  if (flags & PHOTON_REQ_PWC_NO_RCE) {
    req->flags |= REQUEST_FLAG_NO_RCE;
    return photon_pwc_try_ledger(req, 0);
  }

  rt = photon_processes[proc].request_table;

  // process any queued requests for this peer first
  rc = photon_pwc_process_queued_pwc(proc, rt);
  if ((rc == PHOTON_OK) ||
      (rc == PHOTON_ERROR_RESOURCE)) {
    goto queue_exit;
  }

  // otherwise try to send the current request
  rc = photon_pwc_safe_packed(req);
  if (rc == PHOTON_ERROR_RESOURCE) {
    rc = photon_pwc_safe_ledger(req);
  }
  
  if ((rc == PHOTON_ERROR) || (rc == PHOTON_OK)) {
    return rc;
  }

 queue_exit:
  sync_two_lock_queue_enqueue(rt->pwc_q, req);
  sync_fadd(&rt->pcount, 1, SYNC_RELAXED);
  sync_fadd(&qcount, 1, SYNC_RELAXED);
  dbg_trace("Enqueued PWC request: 0x%016lx", req->id);
  return PHOTON_OK;

 error_exit:
  return PHOTON_ERROR;
}  

int _photon_get_with_completion(int proc, uint64_t size,
				photonBuffer lbuf,
				photonBuffer rbuf,
				photon_cid local, photon_cid remote,
				int flags,
				pwc_cid_type type, pwc_command cmd) {
  photonRequest req;
  photonRequestTable rt;
  photonBI db;
  struct photon_buffer_t nlbuf;
  int rc, aok;
  
  dbg_trace("(%d, size: %lu, lid: 0x%016lx, rid: 0x%016lx, flags: %d)", proc,
	    size, local.u64, remote.u64, flags);

  if (local.size && local.size > _photon_idsize) {
    log_err("Local CID size is larger than configured max: %d/%d",
	    local.size, _photon_idsize);
    goto error_exit;
  }

  if (remote.size && remote.size > _photon_idsize) {
    log_err("Remote CID size is larger than configured max: %d/%d",
	    remote.size, _photon_idsize);
    goto error_exit;
  }

  if (size && !rbuf) {
    log_err("Tring to get size %lu and NULL rbuf", size);
    goto error_exit;
  }

  if (!size && (flags & PHOTON_REQ_PWC_NO_RCE)) {
    dbg_warn("Nothing to get and no remote completion requested!");
    return PHOTON_OK;
  }

  if (size && !rbuf->priv.key0 && !rbuf->priv.key1) {
    dbg_warn("No remote buffer keys specified!");
  }

  // always make sure local buffer is registered for gets
  nlbuf.addr = lbuf->addr;
  nlbuf.size = lbuf->size;
  nlbuf.priv = lbuf->priv;
  if (size && !nlbuf.priv.key0 && !nlbuf.priv.key1) {
    if (buffertable_find_containing( (void *)nlbuf.addr,
				     size, &db) != 0) {
      log_err("Tried get into a buffer that's not registered");
      goto error_exit;
    }
    nlbuf.priv = db->buf.priv;
  }
  
  req = photon_setup_request_direct(&nlbuf, rbuf, size, proc, 1);
  if (req == NULL) {
    dbg_trace("Could not setup direct buffer request");
    goto error_exit;
  }
  
  req->op             = REQUEST_OP_PWC;
  req->chdr.type      = type;
  req->chdr.cmd       = cmd; 
  req->local_info.id  = local;
  req->remote_info.id = remote;

  photon_pwc_save_lcid(req);

  // control the return of the local id
  if (flags & PHOTON_REQ_PWC_NO_LCE) {
    req->flags |= REQUEST_FLAG_NO_LCE;
  }

  // control the sending of the remote id to proc
  if (! (flags & PHOTON_REQ_PWC_NO_RCE)) {
    req->flags |= REQUEST_FLAG_ROP;
  }

  // we are already done if there's nothing to get
  // completing the request will send the remote cid
  if (!req->size) {
    photon_pwc_add_req(req);
    return PHOTON_OK;
  }

  aok = photon_pwc_check_gwc_align(&req->local_info.buf,
				   &req->remote_info.buf,
				   req->size);
  
  // process any queued requests for this peer first
  rt = photon_processes[proc].request_table;
  rc = photon_pwc_process_queued_gwc(proc, rt);
  if ((rc == PHOTON_OK) || 
      (rc == PHOTON_ERROR_RESOURCE)) {
    if (aok == PHOTON_OK)
      goto queue_gwc_exit;
    else
      goto queue_gwc_pwc_exit;
  }
  
  if (aok == PHOTON_OK) {
    // do a normal gwc
    // make sure there are TX resources first
    rc = __photon_backend->tx_size_left(proc);
    if (rc < 1) {
      goto queue_gwc_exit;
    }    
    rc = photon_pwc_try_gwc(req);
  }
  else {
    // do a gwc-pwc to handle the unsupported alignment
    int offset = 0;
    sync_tatas_acquire(&rt->pack_loc);
    {
      rc = photon_pwc_test_gwc_pwc(proc, &offset);
      if (rc == PHOTON_ERROR_RESOURCE) {
	sync_tatas_release(&rt->pack_loc);
	goto queue_gwc_pwc_exit;
      }
      rc = photon_pwc_try_gwc_pwc(req, offset);
    }
    sync_tatas_release(&rt->pack_loc);
  }
  
  return rc;
    
 queue_gwc_exit:
  sync_two_lock_queue_enqueue(rt->gwc_q, req);
  sync_fadd(&rt->gcount, 1, SYNC_RELAXED);
  sync_fadd(&qcount, 1, SYNC_RELAXED);
  dbg_trace("Enqueued GWC request: 0x%016lx", req->id);
  return PHOTON_OK;
  
 queue_gwc_pwc_exit:
  sync_two_lock_queue_enqueue(rt->gwc_pwc_q, req);
  sync_fadd(&rt->gpcount, 1, SYNC_RELAXED);
  sync_fadd(&qcount, 1, SYNC_RELAXED);
  dbg_trace("Enqueued GWC-PWC request: 0x%016lx", req->id);
  return PHOTON_OK;
  
 error_exit:
  return PHOTON_ERROR;
}

static int photon_pwc_probe_local(int proc, int *flag, photon_cid *c,
				  void (*cb)(photon_cid)) {
  photonRequest req;
  photon_rid cookie = NULL_REQUEST;
  int rc;

  // handle any pwc requests that were popped in some other path  
  req = photon_pwc_pop_req(proc);
  if (req != NULL) {
    photon_pwc_handle_comp_req(req, flag, c, cb);
    goto exit;
  }
  
  rc = __photon_get_event(proc, &cookie);
  if (rc == PHOTON_EVENT_ERROR) {
    dbg_err("Error getting event, rc=%d", rc);
    goto error_exit;
  }
  else if (rc == PHOTON_EVENT_OK) {
    // we found an event to process
    rc = __photon_handle_cq_event(NULL, cookie, &req);
    if (rc == PHOTON_EVENT_ERROR) {
      goto error_exit;
    }
    else if ((rc == PHOTON_EVENT_REQCOMP) && req &&
	     (req->op == REQUEST_OP_PWC)) {
      photon_pwc_handle_comp_req(req, flag, c, cb);
      goto exit;
    }
    else {
      dbg_trace("PWC probe handled non-completion event: 0x%016lx", cookie);
    }
  }
  
  return PHOTON_EVENT_NONE;

 exit:
  return PHOTON_EVENT_REQFOUND;

 error_exit:
  return PHOTON_EVENT_ERROR;
}

static inline int photon_pwc_handle_pooo(photonEagerBuf eb, int proc,
					 int offset, int size) {
  uint64_t left;
  uint64_t prog = sync_load(&(eb->prog), SYNC_RELAXED);
  int p = prog & (eb->size - 1);
  if (p != offset) {
    pbufs[proc][offset] = size;
  }
  else {
    int padd = size;
    int val = size;
    int iter = p;
    do {
      iter = (iter+val) & (eb->size - 1);
      left = eb->size - iter;
      if (left < PWC_MAX_PACKED) {
	padd += left;
	iter = 0;
      }
      val = pbufs[proc][iter];
      if (val) {
	sync_store(&pbufs[proc][iter], 0, SYNC_RELAXED);
	padd += val;
      }
    } while (val);
    sync_fadd(&eb->prog, padd, SYNC_RELAXED);
  }
  
  return PHOTON_OK;
}

static inline int photon_pwc_handle_looo(photonLedger l, uint64_t imm,
					 photon_cid *comp, int do_comp, int *src,
					 void (*cb)(photon_cid), int *flag) {
  
  int type = DECODE_RCQ_32_TYPE(imm);
  int proc = DECODE_RCQ_32_PROC(imm);
  int rflg = DECODE_RCQ_32_FLAG(imm);
  int loc  = (type==PHOTON_ETYPE_DATA) ? DECODE_RCQ_32_CURR(imm) : *src;
  
  photonCIDRecvLedgerEntry le = 
    (photonCIDRecvLedgerEntry)((uintptr_t)l->entries +
			       (loc * PHOTON_CID_RECV_ENTRY_SIZE));
  photon_cid_recv_hdr *rhdr = &le->rhdr;
  photon_cid_hdr *hdr = &le->hdr;
  
  // now deal with the ordering of 2-put
  switch (type) {
  case PHOTON_ETYPE_DATA:
    {
      int count = sync_addf(&(rhdr->cnt), 1, SYNC_RELAXED);
      if (count == 2) {
	*flag = 1;
	*src = proc;
	comp->size = hdr->size;
	comp->data = (void*)le->data;
	sync_store(&(rhdr->cnt), 0, SYNC_RELAXED);
	goto exit_advance;
      }
      break;
    }
  case PHOTON_ETYPE_NTFY:
    {
      // we're done if this is a single ledger put
      if (rflg == PHOTON_EFLAG_ONE) {
	*flag = 1;
	goto exit_advance;
      }
      
      int count = sync_addf(&(rhdr->cnt), 1, SYNC_RELAXED);
      if (count == 2) {
	*flag = 1;
	sync_store(&(rhdr->cnt), 0, SYNC_RELAXED);
	goto exit_advance;
      }
    }
    break;
  default:
    break;
  }
  
  return PHOTON_EVENT_NONE;

 exit_advance:
  if (rhdr->req) {
    // pop the internal request we saved earlier
    photonRequest req = photon_lookup_request(rhdr->req);
    if (!req) {
      log_err("Could not lookup saved request ID");
    }
    else {
      photon_pwc_add_req(req);
      sync_store(&(rhdr->req), NULL_REQUEST, SYNC_RELAXED);
    }
    // this also means no CID via the API
    *flag = 0;
  }

  // set flag only if we have a successful completion
  // AND we actually want to return the CID (i.e. it is CTYPE_USER)
  *flag = *flag && do_comp;

  // invoke the CID callback if we're returning completion data
  if (*flag && cb) {
    cb(*comp);
  }
  
  // return 64 bit CIDs by value in out param, indicated by size 0
  if (*flag && (comp->size == sizeof(comp->u64))) {
    comp->u64 = *(uint64_t*)comp->data;
    comp->size = 0;
  }

  // progress book keeping
  uint64_t prog = sync_load(&(l->prog), SYNC_RELAXED);
  int p = prog & (_LEDGER_SIZE - 1);
  if (p != loc) {
    // mark slot as received
    sync_store(&(rhdr->cnt), -1, SYNC_RELAXED);
  }
  else {
    int padd = 1;
    int val, iter = p;
    do {
      // calculate additional progress already completed due to ooo events
      iter = (iter+1) & (_LEDGER_SIZE - 1);
      le = (photonCIDRecvLedgerEntry)((uintptr_t)l->entries +
				      (iter * PHOTON_CID_RECV_ENTRY_SIZE));
      val = sync_load(&(le->rhdr.cnt), SYNC_RELAXED);
      if (val < 0) {
	sync_store(&(le->rhdr.cnt), 0, SYNC_RELAXED);
	padd++;
      }
    } while (val < 0);
    // advance ledger progress
    sync_fadd(&(l->prog), padd, SYNC_RELAXED);
  }
  
  dbg_trace("Popped ledger event with id: 0x%016lx (%lu)",
	    comp->u64, comp->u64);

  return PHOTON_OK;
}

static inline int photon_pwc_advance_ledger(photonLedger l, photon_cid *comp, int do_comp,
					    void (*cb)(photon_cid), int *flag) {
  *flag = 1;
  // set flag only if we have a successful completion
  // AND we actually want to return the CID (i.e. it is CTYPE_USER)
  *flag = *flag && do_comp;
  
  // invoke the CID callback if we're returning completion data
  if (*flag && cb) {
    cb(*comp);
  }
  
  // return 64 bit CIDs by value in out param, indicated by size 0
  if (*flag && (comp->size == sizeof(comp->u64))) {
    comp->u64 = *(uint64_t*)comp->data;
    comp->size = 0;
  }
  
  // advance ledger progress by one
  sync_fadd(&(l->prog), 1, SYNC_RELAXED);
  
  dbg_trace("Popped ledger event with id: 0x%016lx (%lu)",
	    comp->u64, comp->u64);
  
  return PHOTON_OK;
}

static int photon_pwc_probe_ledger(int proc, int *flag, photon_cid *comp,
				   int *src, void (*cb)(photon_cid)) {
  photonLedger ledger;
  photonCIDRecvLedgerEntry entry;
  photonEagerBuf eb;
  photon_eb_hdr *hdr;
  photon_rid cookie = NULL_REQUEST;
  uint64_t imm;
  int i, rc, start, end, lead, etype, pos = -1;
  int scan_packed = 1, scan_ledger = 1;
  int rcq = 0;
  
  if (proc == PHOTON_ANY_SOURCE) {
    rc = __photon_get_revent(proc, &cookie, &imm);
    if (rc == PHOTON_EVENT_OK) {
      
      etype = DECODE_RCQ_32_TYPE(imm);
      start = DECODE_RCQ_32_PROC(imm);
      end = start+1;
      
      // Check for completion of first part of a 2PWC request
      if (etype == PHOTON_ETYPE_DATA) {
	ledger = photon_processes[start].local_pwc_ledger;
	return photon_pwc_handle_looo(ledger, imm, comp, 1, src, cb, flag);
      }

      lead  = DECODE_RCQ_32_LEAD(imm);
      scan_packed = (lead == PHOTON_EFLAG_PACK) ? 1 : 0;
      scan_ledger = (lead == PHOTON_EFLAG_LEDG) ? 1 : 0;

      // some backends have no ordering guarantees and will give us
      // the next entry location
      if (__photon_backend->attr->comp_order & PHOTON_ORDER_NONE) {
	pos = DECODE_RCQ_32_CURR(imm);
      }

      rcq = 1;
    }
    // If we don't have remote completion support, must scan entire ledger
    else if (rc == PHOTON_EVENT_NOTIMPL) {
      start = 0;
      end = _photon_nproc;
      imm = UINT64_MAX;
    }
    // if no event or error, simply return with code
    else {
      return rc;
    }
  }
  else {
    log_err("Probing specific ranks is currently unsupported");
    return PHOTON_ERROR;
  }
  
  uint64_t offset, curr, new, left;
  for (i=start; i<end; i++) {
    // first we check the packed buffer space if necessary
    if (scan_packed) {
      eb = photon_processes[i].local_pwc_buf;
      curr = sync_load(&eb->curr, SYNC_RELAXED);
      if (pos >= 0) {
	offset = pos;
      }
      else {
	offset = curr & (eb->size - 1);
	left = eb->size - offset;
	if (left < PWC_MAX_PACKED) {
	  new = left + curr;
	  if (pos < 0)
	    offset = 0;
	}
	else {
	  new = curr;
	}
      }
      hdr = (photon_eb_hdr *)&(eb->data[offset]);
      if ((hdr->header == UINT8_MAX) && (hdr->footer == UINT8_MAX)) {
	photon_cid_hdr *cent = &hdr->cent;
	uintptr_t addr = hdr->addr;
	uint16_t size = hdr->length;
	uint64_t asize = ALIGN(EB_MSG_SIZE(size + cent->size), PWC_ALIGN);
	void *cidptr = (void*)(uintptr_t)hdr + sizeof(*hdr);
	void *payload = (void*)(uintptr_t)hdr + sizeof(*hdr) + cent->size;
	if (sync_cas(&eb->curr, &curr, new+asize, SYNC_RELAXED, SYNC_RELAXED)) {
	  // now check for tail flag (or we could return to check later)
	  volatile uint8_t *tail = (uint8_t*)(uintptr_t)((uintptr_t)hdr + asize - 1);
	  while (*tail != UINT8_MAX)
	    ;
	  // check for PWC commands to process
	  if (cent->type != PWC_CTYPE_USER) {
	    photon_pwc_process_command(i, NULL, cent, cidptr, addr, size, payload);
	  }
	  else {
	    memcpy((void*)addr, payload, size);
	    comp->size = cent->size;
	    comp->data = cidptr;
	    *src = i;
	    *flag = 1;
	    
	    if (cb) {
	      cb(*comp);
	    }
	    
	    // return 64 bit CIDs by value in out param, indicated by size 0
	    if (comp->size == sizeof(comp->u64)) {
	      comp->u64 = *(uint64_t*)comp->data;
	      comp->size = 0;
	    }
	    
	    dbg_trace("Copied message of size %u into 0x%016lx for request 0x%016lx",
		      size, addr, comp->u64);
	  }
	  
	  memset((void*)hdr, 0, asize);
	  
	  if (pos >= 0) {
	    photon_pwc_handle_pooo(eb, i, offset, asize);
	  }
	  else {
	    sync_store(&eb->prog, new+asize, SYNC_RELAXED);
	  }
	  
	  goto exit;
	}
      }
    }

    if (scan_ledger) {
      // then check pwc ledger slots
      ledger = photon_processes[i].local_pwc_ledger;
      curr = sync_load(&ledger->curr, SYNC_RELAXED);
      if (pos >= 0) {
	offset = pos;
      }
      else {
	offset = curr & (ledger->num_entries - 1);
      }
      entry = (photonCIDRecvLedgerEntry)((uintptr_t)ledger->entries +
					 (offset * PHOTON_CID_RECV_ENTRY_SIZE));
      if (entry->hdr.type != PWC_CTYPE_NULL &&
	  sync_cas(&ledger->curr, &curr, curr+1, SYNC_RELAXED, SYNC_RELAXED)) {
        uint16_t type = entry->hdr.type;
	comp->size = entry->hdr.size;
	comp->data = (void*)entry->data;
	*src = i;
	
	// check for internal PWC commands to process
	int user = (type == PWC_CTYPE_USER) ? 1 : 0;
	if (!user) {
	  photon_pwc_process_command(i, &entry->rhdr, &entry->hdr,
				     entry->data, 0, 0, NULL);
	}

	// reset entry
	entry->hdr.type = PWC_CTYPE_NULL;

	// advance progress
	if (rcq) {
	  photon_pwc_handle_looo(ledger, imm, comp, user, (int*)&offset, cb, flag);
	}
	else {
	  photon_pwc_advance_ledger(ledger, comp, user, cb, flag);
	}
	
	goto exit;
      }
    }
  }
  
  if (rcq) {
    log_err("Missing RCQ lookup: %d, %d", scan_packed, scan_ledger);
  }
  
  return PHOTON_EVENT_NONE;
  
 exit:
  return PHOTON_EVENT_REQFOUND;
}  

int _photon_probe_completion(int proc, int *flag, int *remaining,
			     photon_cid *comp, int *src,
			     void (*cb)(photon_cid), int flags) {
  int i, rc = PHOTON_EVENT_NONE;

  *flag = 0;
  *src = proc;

  // check local CQs
  if (flags & PHOTON_PROBE_EVQ) {
    rc = photon_pwc_probe_local(proc, flag, comp, cb);
    if (rc == PHOTON_EVENT_REQFOUND) {
      goto exit;
    }
    else if (rc == PHOTON_EVENT_ERROR) {
      goto error_exit;
    }
  }
  
  // check recv ledger
  if (flags & PHOTON_PROBE_LEDGER) {
    rc = photon_pwc_probe_ledger(proc, flag, comp, src, cb);
    if (rc == PHOTON_EVENT_REQFOUND) {
      goto exit;
    }
    else if (rc == PHOTON_EVENT_ERROR) {
      goto error_exit;
    }
  }

  // process any queued requests, only when EVQ requested
  if ((rc == PHOTON_EVENT_NONE) && (flags & PHOTON_PROBE_EVQ)) {
    uint32_t cnt = sync_load(&qcount, SYNC_RELAXED);
    if (cnt) {
      for (i=0; i<_photon_nproc; i++) {
	photon_pwc_process_queued_gwc(i, photon_processes[i].request_table);
	photon_pwc_process_queued_pwc(i, photon_processes[i].request_table);
      }
    }
    
    // also check if any collectives have completed
    rc = photon_coll_probe(proc, flag, comp);
    if (rc == PHOTON_EVENT_REQFOUND) {
      goto exit;
    }
  }

 exit:
  if (remaining) {
    *remaining = photon_count_request(proc);
    dbg_trace("%d requests remaining", *remaining);
  }

  return PHOTON_OK;
  
 error_exit:
  return PHOTON_ERROR;
}

