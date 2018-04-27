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
#include <inttypes.h>

#include "libphoton.h"
#include "photon_backend.h"
#include "photon_exchange.h"
#include "logging.h"

#ifdef HAVE_MPI
#include <mpi.h>
#endif

#ifdef HAVE_PMI
#include <pmi_cray.h>
#endif

int photon_exchange_allgather(void *ptr, void *ivec_ptr, int n) {
  int rc;

  switch(__photon_config->meta_exch) {
  case PHOTON_EXCH_MPI:
    {
#ifdef HAVE_MPI
      //MPI_Comm _photon_comm = __photon_config->comm;
      MPI_Comm _photon_comm = 0;
      if (!_photon_comm)
	_photon_comm = MPI_COMM_WORLD;
      rc = MPI_Allgather(ptr, n, MPI_BYTE, ivec_ptr, n, MPI_BYTE, _photon_comm);
      if (rc != MPI_SUCCESS)
	goto error_exit;
#endif
    }
    break;
  case PHOTON_EXCH_PMI:
#ifdef HAVE_PMI
    rc = PMI_Allgather(ptr, ivec_ptr, n);
    if (rc != PMI_SUCCESS)
      goto error_exit;
#endif
    break;
  case PHOTON_EXCH_XSP:
#ifdef HAVE_XSP
#endif
    break;
  case PHOTON_EXCH_EXTERNAL:
    rc = __photon_config->exch.allgather(NULL, ptr, ivec_ptr, n);
    if (rc != 0) {
      log_err("Error in external exchange");
      goto error_exit;
    }
    break;
  default:
    log_err("Unrecognized exchange type requested!");
    goto error_exit;
    break;
  }
  return PHOTON_OK;

 error_exit:
  return PHOTON_ERROR;
}

int photon_exchange_barrier() {
  int rc;
  
  switch(__photon_config->meta_exch) {
  case PHOTON_EXCH_MPI:
    {
#ifdef HAVE_MPI
      //MPI_Comm _photon_comm = __photon_config->comm;
      MPI_Comm _photon_comm = 0;
      if (!_photon_comm)
	_photon_comm = MPI_COMM_WORLD;
      rc = MPI_Barrier(_photon_comm);
      if (rc != MPI_SUCCESS)
	goto error_exit;
#endif
    }
    break;
  case PHOTON_EXCH_PMI:
#ifdef HAVE_PMI
    rc = PMI_Barrier();
    if (rc != PMI_SUCCESS)
      goto error_exit;
#endif
    break;
  case PHOTON_EXCH_XSP:
#ifdef HAVE_XSP
#endif
    break;
  case PHOTON_EXCH_EXTERNAL:
    rc = __photon_config->exch.barrier(NULL);
    if (rc != 0) {
      log_err("Error in external barrier");
      goto error_exit;
    }
    break;
  default:
    log_err("Unrecognized exchange type requested!");
    goto error_exit;
    break;
  }
  
  return PHOTON_OK;

 error_exit:
  return PHOTON_ERROR;
}

int photon_exchange_ledgers(ProcessInfo *processes, int flags) {
  int i, ret;
  uint64_t pval;
  uint64_t *key_0, *key_1, *va;

  key_0 = (uint64_t *)malloc(_photon_nproc * sizeof(uint64_t));
  key_1 = (uint64_t *)malloc(_photon_nproc * sizeof(uint64_t));
  va = (uint64_t *)malloc(_photon_nproc * sizeof(uint64_t));

  memset(key_0, 0, _photon_nproc);
  memset(key_1, 0, _photon_nproc);
  memset(va, 0, _photon_nproc);

  pval = shared_storage->buf.priv.key0;
  ret = photon_exchange_allgather(&pval, (void*)key_0, sizeof(pval));
  if (ret != PHOTON_OK) {
    log_err("Could not gather shared storage key_0");
    goto error_exit;
  }

  pval = shared_storage->buf.priv.key1;
  ret = photon_exchange_allgather(&pval, (void*)key_1, sizeof(pval));
  if (ret != PHOTON_OK) {
    log_err("Could not gather shared storage key_1");
    goto error_exit;
  }

  pval = (uint64_t)shared_storage->buf.addr;
  ret = photon_exchange_allgather(&pval, (void*)va, sizeof(pval));
  if (ret != PHOTON_OK) {
    log_err("Could not gather shared storage base ptrs");
    goto error_exit;
  }

  if (flags & LEDGER_INFO) {
    for (i=0; i<_photon_nproc; i++) {
      processes[i].remote_rcv_info_ledger->remote.addr = PHOTON_LRI_PTR(va[i]) + PHOTON_INFO_SIZE * _photon_myrank;
      processes[i].remote_rcv_info_ledger->remote.priv.key0 = key_0[i];
      processes[i].remote_rcv_info_ledger->remote.priv.key1 = key_1[i];

      processes[i].remote_snd_info_ledger->remote.addr = PHOTON_LSI_PTR(va[i]) + PHOTON_INFO_SIZE * _photon_myrank;
      processes[i].remote_snd_info_ledger->remote.priv.key0 = key_0[i];
      processes[i].remote_snd_info_ledger->remote.priv.key1 = key_1[i];
    }
  }
  
  if (flags & LEDGER_FIN) {
    for (i=0; i<_photon_nproc; i++) {
      processes[i].remote_fin_ledger->remote.addr = PHOTON_LF_PTR(va[i]) + PHOTON_LEDG_SIZE * _photon_myrank;
      processes[i].remote_fin_ledger->remote.priv.key0 = key_0[i];
      processes[i].remote_fin_ledger->remote.priv.key1 = key_1[i];
    }
  }

  if (flags & LEDGER_PWC) {
    for (i=0; i<_photon_nproc; i++) {
      processes[i].remote_pwc_ledger->remote.addr = PHOTON_LP_PTR(va[i]) + PHOTON_RPEDG_SIZE * _photon_myrank;
      processes[i].remote_pwc_ledger->remote.priv.key0 = key_0[i];
      processes[i].remote_pwc_ledger->remote.priv.key1 = key_1[i];
    }
  }

  if (flags & LEDGER_EAGER) {
    for (i=0; i<_photon_nproc; i++) {
      processes[i].remote_eager_ledger->remote.addr = PHOTON_LE_PTR(va[i]) + PHOTON_LEDG_SIZE * _photon_myrank;
      processes[i].remote_eager_ledger->remote.priv.key0 = key_0[i];
      processes[i].remote_eager_ledger->remote.priv.key1 = key_1[i];
    }
  }
  
  if (flags & LEDGER_BUF) {
    for (i=0; i<_photon_nproc; i++) {
      processes[i].remote_eager_buf->remote.addr = PHOTON_LEB_PTR(va[i]) + PHOTON_EBUF_SIZE * _photon_myrank;
      processes[i].remote_eager_buf->remote.priv.key0 = key_0[i];
      processes[i].remote_eager_buf->remote.priv.key1 = key_1[i];
    }
  }

  if (flags & LEDGER_PBUF) {
    for (i=0; i<_photon_nproc; i++) {
      processes[i].remote_pwc_buf->remote.addr = PHOTON_LPB_PTR(va[i]) + PHOTON_PBUF_SIZE * _photon_myrank;
      processes[i].remote_pwc_buf->remote.priv.key0 = key_0[i];
      processes[i].remote_pwc_buf->remote.priv.key1 = key_1[i];
    }
  }

  free(key_0);
  free(key_1);
  free(va);

  return PHOTON_OK;

 error_exit:
  free(key_0);
  free(key_1);
  free(va);
  return PHOTON_ERROR;
}

int photon_setup_ri_ledger(ProcessInfo *photon_processes, char *buf, int num_entries) {
  int i;
  int ledger_size, offset;
  photonRILedgerEntry bptr;

  dbg_trace("Setting up RI ledgers");

  ledger_size = PHOTON_INFO_SSIZE(num_entries);

  // Allocate the receive info ledgers
  for(i = 0; i < PHOTON_TPROC; i++) {
    bptr = (photonRILedgerEntry)(buf + ledger_size * i);

    dbg_trace("allocating rcv info ledger for %d: %p", i, bptr);
    dbg_trace("Offset: %d", ledger_size * i);

    // allocate the ledger
    photon_processes[i].local_rcv_info_ledger = photon_ri_ledger_create_reuse(bptr, num_entries, REQUEST_COOK_RINFO);
    if (!photon_processes[i].local_rcv_info_ledger) {
      log_err("couldn't create local rcv info ledger for process %d", i);
      return PHOTON_ERROR;
    }

    bptr = (photonRILedgerEntry)(buf + ledger_size * PHOTON_TPROC + ledger_size * i);

    dbg_trace("allocating remote ri ledger for %d: %p", i, bptr);
    dbg_trace("Offset: %d", ledger_size * PHOTON_TPROC + ledger_size * i);

    photon_processes[i].remote_rcv_info_ledger = photon_ri_ledger_create_reuse(bptr, num_entries, REQUEST_COOK_RINFO);
    if (!photon_processes[i].remote_rcv_info_ledger) {
      log_err("couldn't create remote rcv info ledger for process %d", i);
      return PHOTON_ERROR;
    }
  }

  // Allocate the send info ledgers
  offset = 2 * ledger_size * PHOTON_TPROC;
  for(i = 0; i < PHOTON_TPROC; i++) {
    bptr = (photonRILedgerEntry)(buf + offset + ledger_size * i);

    dbg_trace("allocating snd info ledger for %d: %p", i, bptr);
    dbg_trace("Offset: %d", offset + ledger_size * i);

    // allocate the ledger
    photon_processes[i].local_snd_info_ledger = photon_ri_ledger_create_reuse(bptr, num_entries, REQUEST_COOK_SINFO);
    if (!photon_processes[i].local_snd_info_ledger) {
      log_err("couldn't create local snd info ledger for process %d", i);
      return PHOTON_ERROR;
    }

    bptr = (photonRILedgerEntry)(buf + offset + ledger_size * PHOTON_TPROC + ledger_size * i);

    dbg_trace("allocating remote ri ledger for %d: %p", i, bptr);
    dbg_trace("Offset: %d", offset + ledger_size * PHOTON_TPROC + ledger_size * i);

    photon_processes[i].remote_snd_info_ledger = photon_ri_ledger_create_reuse(bptr, num_entries, REQUEST_COOK_SINFO);
    if (!photon_processes[i].remote_snd_info_ledger) {
      log_err("couldn't create remote snd info ledger for process %d", i);
      return PHOTON_ERROR;
    }
  }

  return PHOTON_OK;
}

int photon_setup_fin_ledger(ProcessInfo *photon_processes, char *buf, int num_entries) {
  int i, esize;
  int ledger_size;
  photonRDMALedgerEntry bptr;

  dbg_trace("Setting up FIN ledgers");

  esize = sizeof(struct photon_rdma_ledger_entry_t);
  ledger_size = PHOTON_LEDG_SSIZE(num_entries, esize);

  for(i = 0; i < PHOTON_TPROC; i++) {
    // allocate the ledger
    dbg_trace("allocating local FIN ledger for %d", i);

    bptr = (photonRDMALedgerEntry)(buf + ledger_size * i);
    photon_processes[i].local_fin_ledger = photon_rdma_ledger_create_reuse(bptr, num_entries, esize, REQUEST_COOK_FIN);
    if (!photon_processes[i].local_fin_ledger) {
      log_err("couldn't create local FIN ledger for process %d", i);
      return PHOTON_ERROR;
    }

    dbg_trace("allocating remote FIN ledger for %d", i);

    bptr = (photonRDMALedgerEntry)(buf + ledger_size * PHOTON_TPROC + ledger_size * i);
    photon_processes[i].remote_fin_ledger = photon_rdma_ledger_create_reuse(bptr, num_entries, esize, REQUEST_COOK_FIN);
    if (!photon_processes[i].remote_fin_ledger) {
      log_err("couldn't create remote FIN ledger for process %d", i);
      return PHOTON_ERROR;
    }
  }

  return PHOTON_OK;
}

int photon_setup_pwc_ledger(ProcessInfo *photon_processes, char *buf, int num_entries) {
  int i, esize, ersize;
  int ledger_size, recv_ledger_size;
  photonCIDLedgerEntry bptr;
  photonCIDRecvLedgerEntry rbptr;

  dbg_trace("Setting up PWC ledgers");

  esize  = PHOTON_CID_ENTRY_SIZE;
  ersize = PHOTON_CID_RECV_ENTRY_SIZE;
  ledger_size      = PHOTON_LEDG_SSIZE(num_entries, esize);
  recv_ledger_size = PHOTON_LEDG_SSIZE(num_entries, ersize);

  for(i = 0; i < PHOTON_TPROC; i++) {
    // allocate the ledger
    dbg_trace("allocating local (recv) PWC ledger for %d", i);

    rbptr = (photonCIDRecvLedgerEntry)(buf + recv_ledger_size * i);
    photon_processes[i].local_pwc_ledger = photon_rdma_ledger_create_reuse(rbptr, num_entries, ersize, REQUEST_COOK_PLEDG);
    if (!photon_processes[i].local_pwc_ledger) {
      log_err("couldn't create local PWC ledger for process %d", i);
      return PHOTON_ERROR;
    }

    dbg_trace("allocating remote PWC ledger for %d", i);

    bptr = (photonCIDLedgerEntry)(buf + recv_ledger_size * PHOTON_TPROC + ledger_size * i);
    photon_processes[i].remote_pwc_ledger = photon_rdma_ledger_create_reuse(bptr, num_entries, esize, REQUEST_COOK_PLEDG);
    if (!photon_processes[i].remote_pwc_ledger) {
      log_err("couldn't create remote PWC ledger for process %d", i);
      return PHOTON_ERROR;
    }
  }

  return PHOTON_OK;
}

int photon_setup_eager_ledger(ProcessInfo *photon_processes, char *buf, int num_entries) {
  int i, esize;
  int ledger_size;
  photonRDMALedgerEntry bptr;

  dbg_trace("Setting up EAGER ledgers");

  esize = sizeof(struct photon_rdma_ledger_entry_t);
  ledger_size = PHOTON_LEDG_SSIZE(num_entries, esize);

  for(i = 0; i < PHOTON_TPROC; i++) {
    // allocate the ledger
    dbg_trace("allocating local EAGER ledger for %d", i);

    bptr = (photonRDMALedgerEntry)(buf + ledger_size * i);
    photon_processes[i].local_eager_ledger = photon_rdma_ledger_create_reuse(bptr, num_entries, esize, REQUEST_COOK_ELEDG);
    if (!photon_processes[i].local_eager_ledger) {
      log_err("couldn't create local EAGER ledger for process %d", i);
      return PHOTON_ERROR;
    }

    dbg_trace("allocating remote EAGER ledger for %d", i);

    bptr = (photonRDMALedgerEntry)(buf + ledger_size * PHOTON_TPROC + ledger_size * i);
    photon_processes[i].remote_eager_ledger = photon_rdma_ledger_create_reuse(bptr, num_entries, esize, REQUEST_COOK_ELEDG);
    if (!photon_processes[i].remote_eager_ledger) {
      log_err("couldn't create remote EAGER ledger for process %d", i);
      return PHOTON_ERROR;
    }
  }

  return PHOTON_OK;
}

int photon_setup_eager_buf(ProcessInfo *photon_processes, char *buf, int size) {
  int i;
  int buf_size;
  uint8_t *bptr;

  dbg_trace("Setting up EAGER buffers");

  //buf_size = sizeof(struct photon_rdma_eager_buf_entry_t) * num_entries;
  buf_size = PHOTON_EBUF_SSIZE(size);

  for(i = 0; i < PHOTON_TPROC; i++) {
    
    dbg_trace("allocating local eager buffer for %d", i);
    
    bptr = (uint8_t *) (buf + buf_size * i);
    photon_processes[i].local_eager_buf = photon_rdma_eager_buf_create_reuse(bptr, size, REQUEST_COOK_EBUF);
    if (!photon_processes[i].local_eager_buf) {
      log_err("couldn't create local eager buffer for process %d", i);
      return PHOTON_ERROR;
    }

    dbg_trace("allocating remote eager buffer for %d", i);
    
    bptr = (uint8_t *) (buf + buf_size * PHOTON_TPROC + buf_size * i);
    photon_processes[i].remote_eager_buf = photon_rdma_eager_buf_create_reuse(bptr, size, REQUEST_COOK_EBUF);
    if (!photon_processes[i].remote_eager_buf) {
      log_err("couldn't create remote eager buffer for process %d", i);
      return PHOTON_ERROR;
    }
  } 
  
  return PHOTON_OK;
}

int photon_setup_pwc_buf(ProcessInfo *photon_processes, char *buf, int size) {
  int i;
  int buf_size;
  uint8_t *bptr;

  dbg_trace("Setting up PWC buffers");

  buf_size = PHOTON_EBUF_SSIZE(size);

  for(i = 0; i < PHOTON_TPROC; i++) {
    
    dbg_trace("allocating local pwc eager buffer for %d", i);
    
    bptr = (uint8_t *) (buf + buf_size * i);
    photon_processes[i].local_pwc_buf = photon_rdma_eager_buf_create_reuse(bptr, size, REQUEST_COOK_PBUF);
    if (!photon_processes[i].local_eager_buf) {
      log_err("couldn't create local pwc eager buffer for process %d", i);
      return PHOTON_ERROR;
    }

    dbg_trace("allocating remote pwc eager buffer for %d", i);
    
    bptr = (uint8_t *) (buf + buf_size * PHOTON_TPROC + buf_size * i);
    photon_processes[i].remote_pwc_buf = photon_rdma_eager_buf_create_reuse(bptr, size, REQUEST_COOK_PBUF);
    if (!photon_processes[i].remote_eager_buf) {
      log_err("couldn't create remote pwc eager buffer for process %d", i);
      return PHOTON_ERROR;
    }
  } 
  
  return PHOTON_OK;
}
