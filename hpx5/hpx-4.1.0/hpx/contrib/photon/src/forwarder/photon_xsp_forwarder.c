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
#include <libxsp_client.h>
#include "libphoton.h"
#include "photon_backend.h"
#include "photon_forwarder.h"
#include "logging.h"

#include "photon_xsp_forwarder.h"

#define INT_ASSIGN_MOVE(ptr, i) do {                             \
		*((int *)ptr) = i;						\
		ptr += sizeof(int);						\
} while(0)

extern photonBuffer shared_storage;

/* interface */
static int _photon_xsp_init(photonConfig cfg, ProcessInfo *photon_processes);
static int _photon_xsp_io_init(ProcessInfo *photon_processes, char *file, int amode, MPI_Datatype view, int niter);
static int _photon_xsp_io_finalize(ProcessInfo *photon_processes);
static int _photon_xsp_exchange(ProcessInfo *photon_processes);
static int _photon_xsp_connect(ProcessInfo *photon_processes);
static void _print_photon_io_info(void *io);
/* internal */
static int __photon_xsp_setup_session(libxspSess **sess, char *xsp_hop);
static int __photon_xsp_connect_forwarder(ProcessInfo *pi, int ind);
static int __photon_xsp_exchange_ledgers(ProcessInfo *pi, int ind, int type);
/* external, for the forwarder process */
int photon_xsp_register_session(libxspSess *sess);
int photon_xsp_unregister_session(libxspSess *sess);
int photon_xsp_forwarder_connect_peer(libxspSess *sess, void *remote_ci, void **ret_ci, int *ret_len);
int photon_xsp_set_info(libxspSess *sess, void *info, int size, void **ret_info, int *ret_size, photon_info_t type);
int photon_xsp_do_io(libxspSess *sess);

struct photon_forwarder_interface_t xsp_forwarder = {
  .init = _photon_xsp_init,
  .io_init = _photon_xsp_io_init,
  .io_finalize = _photon_xsp_io_finalize,
  .io_print = _print_photon_io_info,
  .exchange = _photon_xsp_exchange,
  .connect = _photon_xsp_connect
};

static pthread_mutex_t sess_mtx;
static int sess_count;

/* xsp_forwarder util */
static int photon_decode_MPI_Datatype(MPI_Datatype type, PhotonMPIDatatype *ptype) {
  int i;
  MPI_Datatype *types = NULL;

  MPI_Type_get_envelope(type, &ptype->nints, &ptype->naddrs,
                        &ptype->ndatatypes, &ptype->combiner);

  if (ptype->nints) {
    ptype->integers = malloc(sizeof(int)*ptype->nints);
    if (!ptype->integers) {
      fprintf(stderr, "photon_decode_MPI_Datatype(): out of memory");
      return -1;
    }
  }

  if (ptype->naddrs) {
    ptype->addresses = malloc(sizeof(MPI_Aint)*ptype->naddrs);
    if (!ptype->addresses) {
      fprintf(stderr, "photon_decode_MPI_Datatype(): out of memory");
      goto error_exit_addresses;
    }
  }

  if (ptype->ndatatypes) {
    types = malloc(sizeof(MPI_Datatype)*ptype->ndatatypes);
    ptype->datatypes = malloc(sizeof(int)*ptype->ndatatypes);
    if (!types || !ptype->datatypes) {
      fprintf(stderr, "photon_decode_MPI_Datatype(): out of memory");
      goto error_exit_datatypes;
    }
  }

  MPI_Type_get_contents(type, ptype->nints, ptype->naddrs, ptype->ndatatypes,
                        ptype->integers, ptype->addresses, types);

  /* Transform MPI_Datatypes to our own mapping to send over the wire.
   * There might be a better way to do this.
   */
  for (i = 0; i < ptype->ndatatypes; i++) {
    if (types[i] == MPI_DOUBLE)
      ptype->datatypes[i] = PHOTON_MPI_DOUBLE;
    else
      ptype->datatypes[i] = -1;
  }

  return 0;

error_exit_datatypes:
  free(ptype->addresses);
error_exit_addresses:
  free(ptype->integers);
  return -1;
}

inline void photon_destroy_mpi_datatype (PhotonMPIDatatype *pd) {
  if (pd->nints)			free(pd->integers);
  if (pd->naddrs)			free(pd->addresses);
  if (pd->ndatatypes) free(pd->datatypes);
}

static void _print_photon_io_info(void *io_info) {
  PhotonIOInfo *io = (PhotonIOInfo*)io_info;
  fprintf(stderr, "PhotonIOInfo:\n"
          "fileURI\t\t = %s\n"
          "amode\t\t = %d\n"
          "niter\t\t = %d\n"
          "v.combiner\t = %d\n"
          "v.nints\t\t = %d\n"
          "v.ints[0]\t = %d\n"
          "v.naddrs\t = %d\n"
          "v.ndts\t\t = %d\n"
          "v.dts[0]\t = %d\n",
          io->fileURI, io->amode, io->niter, io->view.combiner,
          io->view.nints, io->view.integers[0], io->view.naddrs,
          io->view.ndatatypes, io->view.datatypes[0]
         );
}

/* See photon_xsp.h for message format */
void *photon_create_xsp_io_init_msg(PhotonIOInfo *io, int *size) {
  char *msg;
  char *msg_ptr;
  int totalsize = 0;

  totalsize += sizeof(int) + strlen(io->fileURI) + 1;
  totalsize += sizeof(int)*3;
  totalsize += sizeof(int) + io->view.nints*sizeof(int);
  totalsize += sizeof(int) + io->view.naddrs*sizeof(MPI_Aint);
  totalsize += sizeof(int) + io->view.ndatatypes*sizeof(int);

  msg_ptr = msg = malloc(totalsize);
  if (!msg) {
    dbg_err("photon_create_xsp_io_init_msg(): out of memory");
    return NULL;
  }

  INT_ASSIGN_MOVE(msg_ptr, strlen(io->fileURI) + 1);
  strcpy((char*)msg_ptr, io->fileURI);
  msg_ptr += strlen(io->fileURI) + 1;

  INT_ASSIGN_MOVE(msg_ptr, io->amode);
  INT_ASSIGN_MOVE(msg_ptr, io->niter);
  INT_ASSIGN_MOVE(msg_ptr, io->view.combiner);

  INT_ASSIGN_MOVE(msg_ptr, io->view.nints);
  memcpy(msg_ptr, io->view.integers, io->view.nints*sizeof(int));
  msg_ptr += io->view.nints*sizeof(int);

  INT_ASSIGN_MOVE(msg_ptr, io->view.naddrs);
  memcpy(msg_ptr, io->view.addresses, io->view.naddrs*sizeof(MPI_Aint));
  msg_ptr += io->view.naddrs*sizeof(MPI_Aint);

  INT_ASSIGN_MOVE(msg_ptr, io->view.ndatatypes);
  memcpy(msg_ptr, io->view.datatypes, io->view.ndatatypes*sizeof(int));

  *size = totalsize;
  return msg;
}
/* end xsp_forwarder util */

/* interface */
static int _photon_xsp_init(photonConfig cfg, ProcessInfo *photon_processes) {
  /* TODO: track fowarders internally and expose */
  int i, fnum = 0;

  /* if we are actually a forwarder, then just set some state and return */
  if (_forwarder) {
    pthread_mutex_init(&sess_mtx, NULL);
    sess_count = 0;
    return PHOTON_OK;
  }

  /* otherwise we are a peer and we want to connect if possible */
  if (!cfg->forwarder.forwarder_eids) {
    dbg_err("Error: no photon forwarder(s) specified.");
    goto error_exit;

  }

  /* establish session with all forwards */
  for (i = _photon_nproc; i < (_photon_nproc + _photon_nforw); i++) {
    dbg_info("Connecting to forwarder at %s", cfg->forwarder.forwarder_eids[fnum]);

    if (__photon_xsp_setup_session(&(photon_processes[i].sess), cfg->forwarder.forwarder_eids[fnum]) != 0) {
      dbg_err("Error: could not setup XSP session with %s", cfg->forwarder.forwarder_eids[fnum]);
      goto error_exit;
    }
  }

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

static int _photon_xsp_io_finalize(ProcessInfo *photon_processes) {
  return PHOTON_OK;
}

// stub function for higher-level I/O operation
static int _photon_xsp_io_init(ProcessInfo *photon_processes, char *file, int amode, MPI_Datatype view, int niter) {
  PhotonIOInfo io;
  void *msg;
  int msg_size;

  io.fileURI = file;
  io.amode = amode;
  io.niter = niter;

  if (photon_decode_MPI_Datatype(view, &io.view) != 0)
    return -1;

  msg = photon_create_xsp_io_init_msg(&io, &msg_size);
  if (msg == NULL) {
    photon_destroy_mpi_datatype(&io.view);
    return -1;
  }

  _print_photon_io_info(&io);

  if (xsp_send_msg(photon_processes[_photon_fproc].sess, msg, (uint64_t)msg_size, XSP_PHOTON_IO) <= 0) {
    dbg_err("Couldn't send IO info");
    photon_destroy_mpi_datatype(&io.view);
    free(msg);
    return -1;
  }

  /* TODO: Maybe we should receive an ACK? */
  photon_destroy_mpi_datatype(&io.view);
  free(msg);

  return 0;
}

static int _photon_xsp_exchange(ProcessInfo *photon_processes) {
  int i, ret = PHOTON_OK;

  for (i = _photon_nproc; i < (_photon_nproc + _photon_nforw); i++) {
    ret = __photon_xsp_exchange_ledgers(&photon_processes[i], i, PHOTON_RI);
    ret = __photon_xsp_exchange_ledgers(&photon_processes[i], i, PHOTON_SI);
    ret = __photon_xsp_exchange_ledgers(&photon_processes[i], i, PHOTON_FI);
  }

  return ret;
}

static int _photon_xsp_connect(ProcessInfo *photon_processes) {
  int i;

  for (i = _photon_nproc; i < (_photon_nproc + _photon_nforw); i++) {
    if (__photon_xsp_connect_forwarder(&photon_processes[i], i) != 0) {
      dbg_err("couldn't exchange connect information with forwarder");
      goto error_exit;
    }
  }

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}
/* end interface */

/* internal */
static int __photon_xsp_setup_session(libxspSess **sess, char *xsp_hop) {
  if (libxsp_init() < 0) {
    perror("libxsp_init(): failed");
    return -1;
  }

  *sess = xsp_session();
  if (!sess) {
    perror("xsp_session() failed");
    return -1;
  }

  xsp_sess_appendchild(*sess, xsp_hop, XSP_HOP_NATIVE);

  if (xsp_connect(*sess)) {
    perror("xsp_connect(): connect failed");
    return -1;
  }

  dbg_info("XSP session established with %s", xsp_hop);

  return 0;
}

static int __photon_xsp_connect_forwarder(ProcessInfo *pi, int ind) {
  int ret;
  int rmsg_type;
  int ci_size;
  uint64_t rmsg_size;

  void *local_ci;
  void *remote_ci;

  ret = __photon_backend->get_info(pi, ind, &local_ci, &ci_size, PHOTON_CI);
  if (ret != PHOTON_OK) {
    dbg_err("Could not get local connect info for forwarder %d", ind);
    goto error_exit;
  }

  if (xsp_send_msg(pi->sess, local_ci, (uint64_t)ci_size, XSP_PHOTON_CI) <= 0) {
    dbg_err("Couldn't send connect info");
    goto error_exit;
  }

  if (xsp_recv_msg(pi->sess, &remote_ci, &rmsg_size, &rmsg_type) <= 0) {
    dbg_err("Couldn't receive remote connect info for forwarder %d", ind);
    goto error_recv;
  }

  if (rmsg_type != XSP_PHOTON_CI) {
    dbg_err("Received message other than XSP_PHOTON_CI");
    goto error_recv;
  }

  if (rmsg_size != ci_size) {
    dbg_err("Bad received message size: %d", (int)rmsg_size);
    goto error_recv;
  }

  ret = __photon_backend->connect(local_ci, remote_ci, ind, NULL, NULL, PHOTON_CONN_ACTIVE);
  if (ret != PHOTON_OK) {
    dbg_err("Cannot connect peer with forwarder %d", ind);
    goto error_recv;
  }

  free(local_ci);
  free(remote_ci);

  return PHOTON_OK;

error_recv:
  free(remote_ci);
error_exit:
  return PHOTON_ERROR;
}

static int __photon_xsp_exchange_ledgers(ProcessInfo *pi, int ind, int type) {
  int ret;
  void *info;
  void *ret_info;
  int info_len;
  uint64_t ret_len;
  int ret_type;
  int xsp_msg_type;

  dbg_info("Transmitting ledger info (%d) to forwarder %d", type, ind);

  switch (type) {
  case PHOTON_RI:
    xsp_msg_type = XSP_PHOTON_RI;
    break;
  case PHOTON_SI:
    xsp_msg_type = XSP_PHOTON_SI;
    break;
  case PHOTON_FI:
    xsp_msg_type = XSP_PHOTON_FI;
    break;
  default:
    goto error_exit;
    break;
  }

  ret = __photon_backend->get_info(pi, ind, &info, &info_len, type);
  if (ret != PHOTON_OK) {
    dbg_err("Could not get info (%d)", type);
    goto error_exit;
  }

  if (xsp_send_msg(pi->sess, info, (uint64_t)info_len, xsp_msg_type) <= 0) {
    dbg_err("Could not send ledger info (%d)", type);
    goto error_exit;
  }

  free(info);

  if (xsp_recv_msg(pi->sess, &ret_info, &ret_len, &ret_type) <= 0) {
    dbg_err("Could not receive ledger info (%d)", type);
    goto error_exit;
  }

  ret = __photon_backend->set_info(pi, ind, ret_info, ret_len, type);
  if (ret != PHOTON_OK) {
    dbg_err("Could not set info (%d)", type);
    goto error_rcv;
  }

  free(ret_info);

  return PHOTON_OK;

error_rcv:
  free(ret_info);
error_exit:
  return PHOTON_ERROR;
}
/* end internal */

/* begin external */
int photon_xsp_register_session(libxspSess *sess) {
  int ind;
  ProcessInfo *pi;

  pthread_mutex_lock(&sess_mtx);

  if (photon_xsp_unused_proc(&pi, &ind) != PHOTON_OK) {
    dbg_err("Could not find unused process info");
    return PHOTON_ERROR;
  }

  dbg_info("Registering session to proc: %d", ind);

  pi->sess = sess;
  sess_count++;

  pthread_mutex_unlock(&sess_mtx);

  return PHOTON_OK;
}

int photon_xsp_unregister_session(libxspSess *sess) {
  int ind;
  ProcessInfo *pi;

  pthread_mutex_lock(&sess_mtx);
  if (photon_xsp_lookup_proc(sess, &pi, &ind) != PHOTON_OK) {
    dbg_err("Couldn't find proc associated with session");
    pthread_mutex_unlock(&sess_mtx);
    return PHOTON_ERROR;
  }

  pi->sess = NULL;
  sess_count--;

  pthread_mutex_unlock(&sess_mtx);

  return PHOTON_OK;
}

int photon_xsp_forwarder_connect_peer(libxspSess *sess, void *remote_ci, void **ret_ci, int *ret_len) {
  int ind, size, ret;
  ProcessInfo *pi;
  void *local_ci;

  if (photon_xsp_lookup_proc(sess, &pi, &ind) != PHOTON_OK) {
    dbg_err("Couldn't find proc associated with session");
    goto error_exit;
  }

  /* get our local connect info */
  ret = __photon_backend->get_info(pi, ind, &local_ci, &size, PHOTON_CI);
  if (ret != PHOTON_OK) {
    dbg_err("Could not get connect info from backend");
    goto error_exit;
  }

  /* ask the backend to connect this peer identified by the received remote connect info */
  ret = __photon_backend->connect(local_ci, remote_ci, ind, ret_ci, ret_len, PHOTON_CONN_PASSIVE);
  if (ret != PHOTON_OK) {
    dbg_err("Forwarder could not connect peer using configured backend");
    goto error_exit;
  }

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

int photon_xsp_set_info(libxspSess *sess, void *info, int size, void **ret_info, int *ret_size, photon_info_t type) {
  int ind, ret;
  ProcessInfo *pi;

  if (photon_xsp_lookup_proc(sess, &pi, &ind) < 0) {
    dbg_err("Couldn't find proc associated with session");
    return PHOTON_ERROR;
  }

  ret = __photon_backend->set_info(pi, ind, info, size, type);
  if (ret != PHOTON_OK) {
    dbg_err("Backend could not set info of type: %d", type);
    goto error_exit;
  }

  ret = __photon_backend->get_info(pi, ind, ret_info, ret_size, type);
  if (ret != PHOTON_OK) {
    dbg_err("Backend could not get info of type: %d", type);
    goto error_exit;
  }

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

/* move this to backends...
int photon_xsp_set_ri(libxspSess *sess, PhotonLedgerInfo *ri, PhotonLedgerInfo **ret_ri) {
	int ind;
	ProcessInfo *pi;

	if (photon_xsp_lookup_proc(sess, &pi, &ind) < 0) {
		dbg_err("Couldn't find proc associated with session");
		return PHOTON_ERROR;
	}

	if (!ri)
		return PHOTON_ERROR;

	pi->remote_rcv_info_ledger->remote.rkey = ri->rkey;
	pi->remote_rcv_info_ledger->remote.addr = (uintptr_t)ri->va;

	dbg_info("Setting rcv_info ledger info of %d: %p", ind, ri->va);

	*ret_ri = (PhotonLedgerInfo*)malloc(sizeof(PhotonLedgerInfo));
	if( !*ret_ri ) {
		return PHOTON_ERROR;
	}

	(*ret_ri)->rkey = shared_storage->mr->rkey;
	(*ret_ri)->va = (void*)(pi->local_rcv_info_ledger->entries);

	return PHOTON_OK;
}

int photon_xsp_set_si(libxspSess *sess, PhotonLedgerInfo *si, PhotonLedgerInfo **ret_si) {
	int ind;
	ProcessInfo *pi;

	if (photon_xsp_lookup_proc(sess, &pi, &ind) < 0) {
		dbg_err("Couldn't find proc associated with session");
		return PHOTON_ERROR;
	}

	if (!si)
		return PHOTON_ERROR;

	pi->remote_snd_info_ledger->remote.rkey = si->rkey;
	pi->remote_snd_info_ledger->remote.addr = (uintptr_t)si->va;

	dbg_info("Setting snd_info ledger info of %d: %p", ind, si->va);

	*ret_si = (PhotonLedgerInfo*)malloc(sizeof(PhotonLedgerInfo));
	if( !*ret_si ) {
		return PHOTON_ERROR;
	}

	(*ret_si)->rkey = shared_storage->mr->rkey;
	(*ret_si)->va = (void*)(pi->local_snd_info_ledger->entries);

	return 0;
}

int photon_xsp_set_fi(libxspSess *sess, PhotonLedgerInfo *fi, PhotonLedgerInfo **ret_fi) {
	int ind;
	ProcessInfo *pi;

	if (photon_xsp_lookup_proc(sess, &pi, &ind) < 0) {
		dbg_err("Couldn't find proc associated with session");
		return PHOTON_ERROR;
	}

	if (!fi)
		return PHOTON_ERROR;

	pi->remote_FIN_ledger->remote.rkey = fi->rkey;
	pi->remote_FIN_ledger->remote.addr = (uintptr_t)fi->va;

	*ret_fi = (PhotonLedgerInfo*)malloc(sizeof(PhotonLedgerInfo));
	if( !*ret_fi ) {
		return PHOTON_ERROR;
	}

	(*ret_fi)->rkey = shared_storage->mr->rkey;
	(*ret_fi)->va = (void*)(pi->local_FIN_ledger->entries);

	return PHOTON_OK;
}

int photon_xsp_set_io(libxspSess *sess, PhotonIOInfo *io) {
	int ind;
	ProcessInfo *pi;

	if (photon_xsp_lookup_proc(sess, &pi, &ind) < 0) {
		dbg_err("Couldn't find proc associated with session");
		return PHOTON_ERROR;
	}

	pi->io_info = io;

	return PHOTON_OK;
}

*/
int photon_xsp_do_io(libxspSess *sess) {
  int i;
  int ind;
  int ndimensions;
  int bufsize;
  char *filename;
  FILE *file;
  void *buf[2];
  MPI_Aint dtextent;
  photon_rid request;
  ProcessInfo *pi;

  if (photon_xsp_lookup_proc(sess, &pi, &ind) < 0) {
    dbg_err("Couldn't find proc associated with session");
    return PHOTON_ERROR;
  }

  if (!pi->io_info) {
    dbg_err("Trying to do I/O without I/O Info set");
    return PHOTON_ERROR;
  }

  /* TODO: So this is how I think this will go: */
  if (pi->io_info->view.combiner != MPI_COMBINER_SUBARRAY) {
    dbg_err("Unsupported combiner");
    return PHOTON_ERROR;
  }

  /* We can't do this because it requires MPI_Init. We need to figure out
   * the best way to support MPI_Datatypes. Also, OpenMPI doesn't use simple
   * int constants like MPICH2, so I'm not sure how we would transfer these.
   *
   * MPI_Type_get_true_extent(p->io_info->view.datatypes[0], &dtlb, &dtextent);
   */
  if (pi->io_info->view.datatypes[0] == PHOTON_MPI_DOUBLE) {
    dtextent = 8;
  }
  else {
    dbg_err("Unsupported datatype");
    return PHOTON_ERROR;
  }

  bufsize = dtextent;
  ndimensions = pi->io_info->view.integers[0];
  for(i = ndimensions+1; i <= 2*ndimensions; i++) {
    bufsize *= pi->io_info->view.integers[i];
  }

  buf[0] = malloc(bufsize);
  buf[1] = malloc(bufsize);
  if (buf[0] == NULL || buf[1] == NULL) {
    dbg_err("Out of memory");
    return PHOTON_ERROR;
  }

  if(photon_register_buffer(buf[0], bufsize) != 0) {
    dbg_err("Couldn't register first receive buffer");
    return PHOTON_ERROR;
  }

  if(photon_register_buffer(buf[1], bufsize) != 0) {
    dbg_err("Couldn't register second receive buffer");
    return PHOTON_ERROR;
  }

  /* For now we just write locally (fileURI is a local path on phorwarder) */
  filename = malloc(strlen(pi->io_info->fileURI) + 10);
  sprintf(filename, "%s_%d", pi->io_info->fileURI, ind);
  file = fopen(filename, "w");
  if (file == NULL) {
    dbg_err("Couldn't open local file %s", filename);
    return -1;
  }

  photon_post_recv_buffer_rdma(ind, buf[0], bufsize, 0, &request);
  /* XXX: is the index = rank? FIXME: not right now! */
  for (i = 1; i < pi->io_info->niter; i++) {
    photon_wait(request);
    /* Post the second buffer so we can overlap the I/O */
    photon_post_recv_buffer_rdma(ind, buf[i%2], bufsize, i, &request);

    if(fwrite(buf[(i-1)%2], 1, bufsize, file) != bufsize) {
      dbg_err("Couldn't write to local file %s: %m", filename);
      return -1;
    }
    /* For now we just write locally, AFAIK this is blocking */
    /*
     * Process the buffer:
     * This is one iteration of (I/O) data in a contiguous buffer.
     * The actual data layout is specified by p->io_info.view.
     * Basically what we do is have p->io_info.view described on the
     * eXnode as the metadata for what this particular node is writing.
     * Each node will write to its own file and we keep track of the
     * offsets using p->io_info.view and the iteration number.
     * Right now I'm assuming only new files and write only.
     *
     * TODO: add 'file offset' to PhotonIOInfo so we can keep track.
     *	 This should also allow the client to start writing at any place
     *	 in the file.
     *
     * So this buffer would actually need to be moved somewhere else
     * where we manage the transfer to the I/O server (or write it locally).
     */
  }
  /* wait for last write */
  photon_wait(request);
  if(fwrite(buf[(i-1)%2], 1, bufsize, file) != bufsize) {
    dbg_err("Couldn't write to local file %s: %m", filename);
    return PHOTON_ERROR;
  }
  fclose(file);
  free(filename);
  free(buf[0]);
  free(buf[1]);

  return PHOTON_OK;
}
/* end external forwarder */
