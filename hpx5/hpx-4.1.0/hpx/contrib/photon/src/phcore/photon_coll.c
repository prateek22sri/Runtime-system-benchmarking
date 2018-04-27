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

#include "photon_backend.h"
#include "photon_pwc.h"
#include "photon_coll.h"
#include "logging.h"

#include "photon_pwc_coll.h"

#ifdef ENABLE_NBC
#include "photon_pwc_nbc.h"
#endif

static photon_coll_interface *coll_iface;
static void photon_coll_handle_cid(int proc, pwc_command cmd, photon_cid cid,
                                   void *data, int size);

static void *photon_coll_map[PHOTON_COLL_IFACE_MAX] = {
    NULL,             // no default
    &pwc_collectives, // pwc interface
#ifdef ENABLE_NBC
    &nbc_collectives // nbc interface
#else
    NULL
#endif
};

int photon_coll_init(photonConfig cfg) {
  int rc;

  assert(cfg->coll > PHOTON_COLL_IFACE_DEFAULT &&
         cfg->coll < PHOTON_COLL_IFACE_MAX);

  // default to the builtin PWC collectives
  coll_iface = photon_coll_map[cfg->coll];

  if (!coll_iface) {
    log_err("Requested collective interface is not enabled: %s",
            PHOTON_COLL_IFACE_TO_STRING[cfg->coll]);
    return PHOTON_ERROR;
  }

  one_debug("Initializing the \"%s\" collective interface",
            PHOTON_COLL_IFACE_TO_STRING[cfg->coll]);

  // do any initialization needed by given collective implementation
  if (coll_iface->init) {
    rc = coll_iface->init(cfg);
    if (rc != PHOTON_OK) {
      return PHOTON_ERROR;
    }
  }

  // register our handler to deal with collective completions
  photon_pwc_register_ctype_handler(PWC_CTYPE_COLL, &photon_coll_handle_cid);

  return PHOTON_OK;
}

int photon_coll_probe(int proc, int *flag, photon_cid *comp) {

  // do any probing needed by the configured collective interface
  *flag = 0;
  (*comp).u64 = 0;

  if (coll_iface->probe) {
    return coll_iface->probe(proc, flag, comp);
  }

  return PHOTON_OK;
}

int _photon_collective_comm_create(void *active, int num_active, int total,
                                   photon_coll_comm *c) {
  int rc;
  rc = coll_iface->comm_create(c, active, num_active, total);
  if (rc != PHOTON_OK) {
    log_err("Could not create communicator");
    return PHOTON_ERROR;
  }
  return PHOTON_OK;
}

int _photon_collective_init(photon_coll_comm c, photon_coll ctype,
                            photon_cid local, photon_rid *request, int flags) {
  photonRequest req;
  photon_coll_ctx *nctx;

  assert(ctype && ctype < PHOTON_COLL_MAXVAL);

  req = photon_get_request(_photon_myrank);
  if (!req) {
    log_err("Could not allocate new request");
    goto error_exit;
  }

  nctx = malloc(sizeof(*nctx));
  if (!nctx) {
    log_err("Could not allocate new collective context");
    goto error_exit_free;
  }

  if (c != NULL) {
    nctx->comm = c;
  } else {
    nctx->comm = coll_iface->comm_get();
  }

  req->op = REQUEST_OP_COLL;
  req->type = ctype;
  req->local_info.id = local;
  req->remote_info.id.u64 = 0x0;
  req->ctx = nctx;

  *request = req->id;

  return PHOTON_OK;

error_exit_free:
  photon_free_request(req);
error_exit:
  return PHOTON_ERROR;
}

int _photon_collective_init_new_comm(void *active, int num_active, int total,
                                     photon_coll ctype, photon_cid local,
                                     photon_rid *request, int flags) {
  int rc;
  void *comm = NULL;
  photonRequest req;
  photon_coll_ctx *nctx;

  assert(ctype && ctype < PHOTON_COLL_MAXVAL);

  req = photon_get_request(_photon_myrank);
  if (!req) {
    log_err("Could not allocate new request");
    goto error_exit;
  }

  nctx = malloc(sizeof(*nctx));
  if (!nctx) {
    log_err("Could not allocate new collective context");
    goto error_exit_free;
  }

  rc = _photon_collective_comm_create(active, num_active, total, &comm);
  if (rc != PHOTON_OK) {
    log_err("Could not create communicator");
    goto error_exit_ctx;
  }

  nctx->comm = comm;

  req->op = REQUEST_OP_COLL;
  req->type = ctype;
  req->local_info.id = local;
  req->remote_info.id.u64 = 0x0;
  req->ctx = nctx;

  *request = req->id;

  return PHOTON_OK;

error_exit_ctx:
  free(nctx);
error_exit_free:
  photon_free_request(req);
error_exit:
  return PHOTON_ERROR;
}

int _photon_collective_join(photon_rid request, void *in, void *out, int count,
                            photon_datatype datatype, void *op) {

  photonRequest req;
  photon_coll_ctx *ctx;

  req = photon_lookup_request(request);
  if (!req) {
    log_err("No request found with rid: 0x%016lx\n", request);
    goto error_exit;
  }

  assert(req->op == REQUEST_OP_COLL);

  ctx = (photon_coll_ctx *)req->ctx;

  switch (req->type) {
    case PHOTON_COLL_BARRIER: {
      assert(coll_iface->barrier);
      coll_iface->barrier(ctx->comm);
      // once sync barrier returns, we tell PWC that the request is complete
      photon_pwc_add_req(req);
      break;
    }
    case PHOTON_COLL_IBARRIER: {
      // here we return immediately and the request completes later
      // either via coll_probe above or through native PWC ibarrier
      assert(coll_iface->ibarrier);
      return coll_iface->ibarrier(ctx->comm, req);
      break;
    }
    case PHOTON_COLL_ALLREDUCE: {
      assert(coll_iface->allreduce);
      coll_iface->allreduce(in, out, count, &datatype, op, ctx->comm);
      photon_pwc_add_req(req);
      break;
    }
    case PHOTON_COLL_IALLREDUCE: {
      // return immediately
      assert(coll_iface->iallreduce);
      return coll_iface->iallreduce(in, out, count, &datatype, op, ctx->comm,
                                    req);
      break;
    }
    case PHOTON_COLL_SCAN:
    case PHOTON_COLL_ISCAN: break;
    default:
      log_err("unsupported collective type: %d", req->type);
      goto error_exit;
      break;
  }

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

static void photon_coll_handle_cid(int proc, pwc_command cmd, photon_cid cid,
                                   void *data, int size) {
  if (coll_iface->cid_handler) {
    coll_iface->cid_handler(proc, cmd, cid, data, size);
  }
}
