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

#include <assert.h>

#include "photon_event.h"
#include "photon_pwc.h"

int __photon_handle_cq_special(photon_rid cookie) {
  uint32_t prefix;
  prefix = (uint32_t)(cookie>>32);
  
  if (cookie == NULL_REQUEST)
    return PHOTON_EVENT_OK;

  switch (prefix) {
  case REQUEST_COOK_GPWC:
  case REQUEST_COOK_EAGER:
    break;
  case REQUEST_COOK_EBUF:
    {
      int proc = (int)(cookie<<32>>32);
      assert(IS_VALID_PROC(proc));
      sync_store(&photon_processes[proc].remote_eager_buf->acct.rloc, 0, SYNC_RELEASE);
    }
    break;
  case REQUEST_COOK_PBUF:
    {
      int proc = (int)(cookie<<32>>32);
      assert(IS_VALID_PROC(proc));
      sync_store(&photon_processes[proc].remote_pwc_buf->acct.rloc, 0, SYNC_RELEASE);
    }
    break;
  case REQUEST_COOK_FIN:
    {
      int proc = (int)(cookie<<32>>32);
      assert(IS_VALID_PROC(proc));
      sync_store(&photon_processes[proc].remote_fin_ledger->acct.rloc, 0, SYNC_RELEASE);
    }
    break;
  case REQUEST_COOK_RINFO:
    {
      int proc = (int)(cookie<<32>>32);
      assert(IS_VALID_PROC(proc));
      sync_store(&photon_processes[proc].remote_rcv_info_ledger->acct.rloc, 0, SYNC_RELEASE);
    }
    break;
  case REQUEST_COOK_SINFO:
    {
      int proc = (int)(cookie<<32>>32);
      assert(IS_VALID_PROC(proc));
      sync_store(&photon_processes[proc].remote_snd_info_ledger->acct.rloc, 0, SYNC_RELEASE);
    }
    break;
  case REQUEST_COOK_ELEDG:
    {
      int proc = (int)(cookie<<32>>32);
      assert(IS_VALID_PROC(proc));
      sync_store(&photon_processes[proc].remote_eager_ledger->acct.rloc, 0, SYNC_RELEASE);
    }
    break;
  case REQUEST_COOK_PLEDG:
    {
      int proc = (int)(cookie<<32>>32);
      assert(IS_VALID_PROC(proc));
      sync_store(&photon_processes[proc].remote_pwc_ledger->acct.rloc, 0, SYNC_RELEASE);
    }
    break;
  default:
    return PHOTON_EVENT_ERROR;
    break;
  }
  
  return PHOTON_EVENT_OK;
}

int __photon_get_event(int proc, photon_rid *id) {
  int rc, nevents;
  rc = __photon_backend->get_event(proc, 1, id, &nevents);
  if (rc == PHOTON_EVENT_ERROR) {
    dbg_err("Error getting event, rc=%d", rc);
  }

  if (rc == PHOTON_EVENT_OK) {
    dbg_trace("got event: 0x%016lx", *id);
  }
  
  return rc;
}

int __photon_get_nevents(int proc, int max, photon_rid **ids, int *n) {
  int rc;
  rc = __photon_backend->get_event(proc, max, *ids, n);
  if (rc == PHOTON_EVENT_ERROR) {
    dbg_err("Error getting events");
  }

  if (rc == PHOTON_EVENT_OK) {
    dbg_trace("got %d events", *n);
  }
  
  return rc;
}

int __photon_get_revent(int proc, photon_rid *id, uint64_t *imm) {
  int rc, nevents;
  rc = __photon_backend->get_revent(proc, 1, id, imm, &nevents);
  if (rc == PHOTON_EVENT_ERROR) {
    dbg_err("Error getting revent");
  }
  
  if (rc == PHOTON_EVENT_OK) {
    dbg_trace("got revent: 0x%016lx (imm=0x%016lx)", *id, *imm);
  }
  
  return rc;
}

int __photon_get_revents(int proc, int max, photon_rid **ids, uint64_t **imms, int *n) {
  int rc;
  rc = __photon_backend->get_revent(proc, max, *ids, *imms, n);
  if (rc == PHOTON_EVENT_ERROR) {
    dbg_err("Error getting revents");
  }
  
  if (rc == PHOTON_EVENT_OK) {
    dbg_trace("got %d revents", *n);
  }
  
  return rc;
}

int __photon_handle_cq_event(photonRequest req, photon_rid cookie, photonRequest *rreq) {
  int rc ;
  
  rc = __photon_handle_cq_special(cookie);
  if (rc == PHOTON_EVENT_OK) {
    return PHOTON_EVENT_OK;
  }
  
  // if we pass in a request to test
  if (req) {
    // mark complete if request is type EVQUEUE and request id matches the event
    if ((cookie == req->id) && (req->type == EVQUEUE)) {
      req->state = REQUEST_COMPLETED;
      *rreq = req;
      dbg_trace("Set request completed with rid: 0x%016lx", cookie);
      return PHOTON_EVENT_REQCOMP;
    }
  }
  
  // if no request, or request didn't match, then do a lookup
  photonRequest treq;
  treq = photon_lookup_request(cookie);
  if (treq) {
    if (treq->type == EVQUEUE) {
      int nentries = sync_addf(&treq->rattr.events, -1, SYNC_RELAXED);
      if (nentries == 0) {
	treq->state = REQUEST_COMPLETED;
	dbg_trace("Set request completed, rid: 0x%016lx", cookie);
	*rreq = treq;
	return PHOTON_EVENT_REQCOMP;
      }
      else if (nentries != 1) {
	dbg_info("Unexpected nentries value: %d, op=%d, 0x%016lx",
		  nentries, treq->op, treq->flags);
      }
    }
    else if (treq->type == LEDGER) {
      // this was an RDMA event associated with a ledger
      // mark the request as having the local completion popped
      treq->flags |= REQUEST_FLAG_LDONE;
      dbg_trace("Set local completion done flag for ledger rid: 0x%016lx", cookie);
    }
    *rreq = treq;
    return PHOTON_EVENT_REQFOUND;
  }
  else {
    dbg_warn("Got an event that we did not expect: 0x%016lx", cookie);
    return PHOTON_EVENT_ERROR;
  }
  
  return PHOTON_EVENT_OK;
}


///////////////////////////////////////////////////////////////////////////////
// __photon_nbpop_event() is non blocking and returns:
//     PHOTON_EVENT_ERROR   if an error occured.
//     PHOTON_EVENT_REQCOMP if the request (req) specified in the argument has completed.
//     PHOTON_EVENT_OK      if there was an event but not for the specified request (req).
//     PHOTON_EVENT_NONE    if there was no event
int __photon_nbpop_event(photonRequest req) {
  int rc;

  dbg_trace("(%d/0x%016lx)", req->proc, req->id);

  // EAGER exception
  // this request was EAGER and the associated os_get() is done
  if (req->flags & REQUEST_FLAG_EDONE) {
    req->state = REQUEST_COMPLETED;
  }
  
  if (req->state == REQUEST_PENDING) {
    photon_rid cookie;
    photonRequest treq;
    rc = __photon_get_event(PHOTON_ANY_SOURCE, &cookie);
    if (rc == PHOTON_EVENT_OK) {
      rc = __photon_handle_cq_event(req, cookie, &treq);
      if (rc == PHOTON_EVENT_ERROR) {
	goto error_exit;
      }
      else if ((rc == PHOTON_EVENT_REQCOMP) && treq &&
	       (treq->op == REQUEST_OP_PWC)) {
	photon_pwc_add_req(treq);
	dbg_trace("Enqueuing PWC local completion");
	return PHOTON_EVENT_OK;
      }
    }
    else {
      return rc;
    }
  }
  
  dbg_trace("returning %d", (req->state == REQUEST_COMPLETED)?PHOTON_EVENT_REQCOMP:PHOTON_EVENT_OK);
  return (req->state == REQUEST_COMPLETED)?PHOTON_EVENT_REQCOMP:PHOTON_EVENT_OK;
  
error_exit:
  return PHOTON_EVENT_ERROR;
}

int __photon_nbpop_sr(photonRequest req) {

  return PHOTON_OK;
}

///////////////////////////////////////////////////////////////////////////////
// returns
//     PHOTON_EVENT_ERROR   if an error occured.
//     PHOTON_EVENT_REQCOMP if the FIN associated with "req" was found and popped, or
//		            the "req" is not pending. This is not an error, if a previous
//		            call to __photon_nbpop_ledger() popped the FIN that
//                          corresponds to "req".
//     PHOTON_EVENT_NONE    if the request is pending and the FIN has not arrived yet
int __photon_nbpop_ledger(photonRequest req) {
  uint64_t curr;
  int c_ind, i=-1;

  dbg_trace("(0x%016lx)", req->id);

  if (req->state == REQUEST_PENDING) {
    
    // clear any completed tasks from the event queue
    while (__photon_nbpop_event(req) != PHOTON_EVENT_NONE)
      ;

    // Check if an entry of the FIN LEDGER was written with "id" equal to "req"
    for(i = 0; i < _photon_nproc; i++) {
      photonRDMALedgerEntry curr_entry, entries;
      photonLedger l = photon_processes[i].local_fin_ledger;
      entries = (photonRDMALedgerEntry)l->entries;
      curr = sync_load(&l->curr, SYNC_RELAXED);
      c_ind = curr & (l->num_entries - 1);
      curr_entry = &(entries[c_ind]);
      if ((curr_entry->request != (uint64_t) NULL_REQUEST) &&
	  sync_cas(&l->curr, &curr, curr+1, SYNC_RELAXED, SYNC_RELAXED)) {
        dbg_trace("Found curr: %d, req: 0x%016lx while looking for req: 0x%016lx",
		  c_ind, curr_entry->request, req->id);
	
        if (curr_entry->request == req->id) {
          req->state = REQUEST_COMPLETED;
        }
        // handle requests we are not looking for
        else {
          photonRequest treq;
	  if ((treq = photon_lookup_request(curr_entry->request)) != NULL) {
	    dbg_trace("Setting request completed, rid: 0x%016lx", curr_entry->request);
	    treq->state = REQUEST_COMPLETED;
	  }
	}
	// reset entry
        curr_entry->request = NULL_REQUEST;
	sync_fadd(&l->prog, 1, SYNC_RELAXED);
      }
    }
  }

  if ((req->state != REQUEST_COMPLETED) && (req->state != REQUEST_PENDING)) {
    dbg_warn("req->state != (PENDING | COMPLETE), returning 0");
    return PHOTON_EVENT_OK;
  }
  
  dbg_trace("at end, returning %d", (req->state == REQUEST_COMPLETED)?PHOTON_EVENT_REQCOMP:PHOTON_EVENT_NONE);
  return (req->state == REQUEST_COMPLETED)?PHOTON_EVENT_REQCOMP:PHOTON_EVENT_NONE;
}

int __photon_wait_ledger(photonRequest req) {
  uint64_t curr;
  int c_ind, i=-1;

  dbg_trace("(0x%016lx)",req->id);

#ifdef CALLTRACE
  for(i = 0; i < _photon_nproc; i++) {
    photonRDMALedgerEntry curr_entry;
    curr = photon_processes[i].local_fin_ledger->curr;
    curr_entry = &(photon_processes[i].local_fin_ledger->entries[curr]);
    dbg_trace("curr_entry(proc==%d)=%p", i ,curr_entry);
  }
#endif
  while (req->state == REQUEST_PENDING) {

    // clear any completed tasks from the event queue
    while (__photon_nbpop_event(req) != PHOTON_EVENT_NONE)
      ;
    
    // Check if an entry of the FIN LEDGER was written with "id" equal to "req"
    for(i = 0; i < _photon_nproc; i++) {
      photonRDMALedgerEntry curr_entry, entries;
      photonLedger l = photon_processes[i].local_fin_ledger;
      entries = (photonRDMALedgerEntry)l->entries;
      curr = sync_load(&l->curr, SYNC_RELAXED);
      c_ind = curr & (l->num_entries - 1);
      curr_entry = &(entries[c_ind]);
      if ((curr_entry->request != (uint64_t) 0) &&
	  sync_cas(&l->curr, &curr, curr+1, SYNC_RELAXED, SYNC_RELAXED)) {
        dbg_trace("Found: %d/0x%016lx/0x%016lx", c_ind, curr_entry->request, req->id);
	
        if (curr_entry->request == req->id) {
          req->state = REQUEST_COMPLETED;
        }
        else {
	  photonRequest treq;
          if ((treq = photon_lookup_request(curr_entry->request)) != NULL) {
	    treq->state = REQUEST_COMPLETED;
	  }
	}
        curr_entry->request = NULL_REQUEST;
	sync_fadd(&l->prog, 1, SYNC_RELAXED);
      }
    }
  }

  return (req->state == REQUEST_COMPLETED)?PHOTON_OK:PHOTON_ERROR;
}

int __photon_try_one_event(photonRequest *rreq) {
  int rc;
  photon_rid cookie;
  photonRequest treq;

  rc = __photon_get_event(PHOTON_ANY_SOURCE, &cookie);
  if (rc != PHOTON_OK) {
    return rc;
  }

  rc = __photon_handle_cq_event(NULL, cookie, &treq);
  if (rc == PHOTON_EVENT_ERROR) {
    goto error_exit;
  }
  
  if ((rc == PHOTON_EVENT_REQCOMP) && treq) {
    if (treq->op == REQUEST_OP_PWC) {
      photon_pwc_add_req(treq);
      dbg_trace("Enqueuing PWC local completion");
    }
    else {
      *rreq = treq;
      return PHOTON_EVENT_REQCOMP;
    }
  }

  return PHOTON_EVENT_OK;
 error_exit:
  return PHOTON_EVENT_ERROR;
}

int __photon_wait_event(photonRequest req) {
  int rc;
  photonRequest treq = NULL;
  
  do {
    rc = __photon_try_one_event(&treq);
  } while (rc != PHOTON_EVENT_REQCOMP);
  
  assert(treq && treq->id == req->id);

  return PHOTON_OK;
}
