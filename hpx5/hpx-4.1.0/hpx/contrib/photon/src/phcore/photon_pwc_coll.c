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
#include <sys/types.h>
#include <sys/queue.h>

#include "photon_backend.h"
#include "photon_buffer.h"
#include "photon_exchange.h"
#include "photon_pwc.h"
#include "photon_pwc_coll.h"

static int pwc_coll_init(photonConfig cfg);
static int pwc_comm_create(void *c, void *active, int num_active, int total);
static photon_coll_comm pwc_comm_get();
static int pwc_coll_probe(int proc, int *flag, photon_cid *c);
static int pwc_cid_handler(int proc, pwc_command cmd, photon_cid cid,
			   void *data, int size);
static int pwc_barrier(void *comm);
static int pwc_ibarrier(void *comm, photonRequest req);

photon_coll_interface pwc_collectives = {
  .init = pwc_coll_init,
  .comm_create = pwc_comm_create,
  .comm_get = pwc_comm_get,
  .probe = pwc_coll_probe,
  .cid_handler = pwc_cid_handler,
  .barrier = pwc_barrier,
  .ibarrier = pwc_ibarrier,
  .allreduce = NULL,
  .iallreduce = NULL,
  .scan = NULL,
  .iscan = NULL
};

static uint8_t *barrier_ary;
static uint32_t *ibarrier_ary;
static BIT_ARRAY *ibarrier_ba;
static two_lock_queue_t *ibarrier_q;

static tatas_lock_t barr_lock;
static struct photon_buffer_t lbuf;
static photonBuffer bufs;

static int pwc_coll_init(photonConfig cfg) {
  int rc;
  photonBI local;

  // our barrier array has one extra element used as a "send" byte
  barrier_ary = (uint8_t *)calloc(_photon_nproc + 1, sizeof(uint8_t));
  if (!barrier_ary) {
    log_err("Could not allocate barrier space");
    goto error_exit;
  }

  // register our local barrier buffer
  local = photon_buffer_create(barrier_ary, _photon_nproc + 1);
  photon_buffer_register(local, __photon_backend->context, BUFFER_FLAG_NIL);
  memcpy(&lbuf, &local->buf, sizeof(struct photon_buffer_t));
  
  bufs = (photonBuffer)calloc(_photon_nproc, sizeof(struct photon_buffer_t));
  if (!bufs) {
    goto error_exit_bar;
  }
  
  rc = photon_exchange_allgather(&lbuf, (void*)bufs, sizeof(lbuf));
  if (rc != PHOTON_OK) {
    log_err("Could not gather remote buffer info");
    goto error_exit;
  }
  
  // set the last element to 1 and adjust the send address
  barrier_ary[_photon_nproc] = 1;
  lbuf.addr += _photon_nproc;

  // setup ibarrier accounting
  ibarrier_q = sync_two_lock_queue_new();
  sync_tatas_init(&barr_lock);
  if (_photon_myrank == 0) {
    ibarrier_ary = calloc(_photon_nproc, sizeof(uint32_t));
    ibarrier_ba = bit_array_create(_photon_nproc);
    bit_array_clear_all(ibarrier_ba);
  }

  photon_buffer_free(local);
  
  return PHOTON_OK;

 error_exit_bar:
  free(barrier_ary);
 error_exit:
  return PHOTON_ERROR;
}

static int pwc_coll_probe(int proc, int *flag, photon_cid *c) {
  return PHOTON_OK;
}

static int pwc_cid_handler(int proc, pwc_command cmd, photon_cid cid,
			   void *data, int size) {
  switch(cmd) {
  case PWC_COMMAND_BARRIER:
    {
      // don't do anything for local completions
      if (proc == _photon_myrank)
	return PHOTON_OK;
      
      int flag = 0;
      photonRequest req;
      
      if (_photon_myrank == 0) {
	photon_pwc_barrier_set_and_check(proc, &flag);
      }
      
      if (flag || _photon_myrank) {
	req = photon_pwc_barrier_dequeue();
	assert(req);
	photon_pwc_add_req(req);
      }
    }
    break;
  default:
    break;
  }
  
  return PHOTON_OK;
}


static int pwc_comm_create(void *c, void *active, int num_active, int total) {
  return PHOTON_OK;
}

static photon_coll_comm pwc_comm_get() {
  return NULL;
}

// native pwc barrier
static int pwc_barrier(void *comm) {
  int i, rc;
  uint8_t val;
  struct photon_buffer_t rbuf;
  photon_cid cid = {.u64=0};

  if (_photon_myrank == 0) {
    for (i=1; i<_photon_nproc; i++) {
      do {
	val = sync_load(&barrier_ary[i], SYNC_RELAXED);
      } while (!val);
      sync_store(&barrier_ary[i], 0, SYNC_RELAXED);
    }

    // reply to each rank (position 0 at each rank)
    for (i=1; i<_photon_nproc; i++) {
      rbuf.addr = bufs[i].addr;
      rbuf.size = sizeof(val);
      rbuf.priv = bufs[i].priv;
      rc = _photon_put_with_completion(i, sizeof(val), &lbuf, &rbuf,
				       cid, cid,
				       PHOTON_REQ_PWC_NO_LCE|PHOTON_REQ_PWC_NO_RCE,
				       PWC_CTYPE_COLL, PWC_COMMAND_NULL);
      if (rc != PHOTON_OK) {
	log_err("Could not send barrier message to rank %d", i);
	goto error_exit;
      }
    }
  }
  else {
    // send to rank 0 at my barrier buffer entry
    // (base address plus rank offset)
    rbuf.addr = bufs[0].addr + _photon_myrank * sizeof(val);
    rbuf.size = sizeof(val);
    rbuf.priv = bufs[0].priv;
    rc = _photon_put_with_completion(0, sizeof(val), &lbuf, &rbuf,
				     cid, cid,
				     PHOTON_REQ_PWC_NO_LCE|PHOTON_REQ_PWC_NO_RCE,
				     PWC_CTYPE_COLL, PWC_COMMAND_NULL);
    if (rc != PHOTON_OK) {
      log_err("Could not send barrier message to rank 0");
      goto error_exit;
    }
    
    do {
      val = sync_load(&barrier_ary[0], SYNC_RELAXED);
    } while (!val);
    sync_store(&barrier_ary[0], 0, SYNC_RELAXED);
  }
  return PHOTON_OK;

 error_exit:
  return PHOTON_ERROR;
}

// native pwc ibarrier
static int pwc_ibarrier(void *comm, photonRequest req) {
  int rc;
  photon_cid cid = {.u64=0};

  // ibarriers are enqueued in the order in which they are called
  sync_two_lock_queue_enqueue(ibarrier_q, req);

  if (_photon_myrank) {
    rc = _photon_put_with_completion(0, 0, NULL, NULL,
				     cid, cid,
				     PHOTON_REQ_PWC_NO_LCE,
				     PWC_CTYPE_COLL, PWC_COMMAND_BARRIER);
    if (rc != PHOTON_OK) {
      log_err("Could not send barrier message to rank 0");
      goto error_exit;
    }
  }
  else {
    int flag = 0;
    photon_pwc_barrier_set_and_check(_photon_myrank, &flag);
    if (flag) {
      // rank 0 might be the last to join
      photonRequest req = sync_two_lock_queue_dequeue(ibarrier_q);
      assert(req);
      photon_pwc_add_req(req);
    }
  }

  return PHOTON_OK;

 error_exit:
  sync_two_lock_queue_dequeue(ibarrier_q);
  return PHOTON_ERROR;
}

int photon_pwc_barrier_set_and_check(int proc, int *flag) {
  int i, rc;
  bit_index_t tst;
  photon_cid cid = {.u64=0};

  *flag = 0;

  sync_tatas_acquire(&barr_lock);
  {
    bit_array_set(ibarrier_ba, proc);
    ibarrier_ary[proc]++;
    
    tst = bit_array_num_bits_set(ibarrier_ba);
    if (tst == _photon_nproc) {
      for (i=0; i<_photon_nproc; i++) {
	ibarrier_ary[i]--;
	if (ibarrier_ary[i] == 0) {
	  bit_array_clear(ibarrier_ba, i);
	}
      }
      *flag = 1;
    }
  }
  sync_tatas_release(&barr_lock);
  
  if (*flag) {
    for (i=1; i<_photon_nproc; i++) {
      rc = _photon_put_with_completion(i, 0, NULL, NULL,
				       cid, cid,
				       PHOTON_REQ_PWC_NO_LCE,
				       PWC_CTYPE_COLL, PWC_COMMAND_BARRIER);
      if (rc != PHOTON_OK) {
	log_err("Could not send barrier reply to rank %d", i);
	goto error_exit;
      }
    }
  }
  
  return PHOTON_OK;
  
 error_exit:
  return PHOTON_ERROR;
}

photonRequest photon_pwc_barrier_dequeue() {
  return sync_two_lock_queue_dequeue(ibarrier_q);
}
