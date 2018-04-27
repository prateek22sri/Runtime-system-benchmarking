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
#include <errno.h>

#include <infiniband/verbs.h>

#include "verbs_buffer.h"
#include "verbs_connect.h"
#include "logging.h"

static int __verbs_buffer_register(photonBI dbuffer, void *ctx, int flags);
static int __verbs_buffer_unregister(photonBI dbuffer, void *ctx);

struct photon_buffer_interface_t verbs_buffer_interface = {
  .buffer_create = _photon_buffer_create,
  .buffer_free = _photon_buffer_free,
  .buffer_register = __verbs_buffer_register,
  .buffer_unregister = __verbs_buffer_unregister,
};

static int __verbs_buffer_register(photonBI dbuffer, void *ctx, int flags) {
  enum ibv_access_flags ibv_flags;
  struct ibv_mr *mr;

  dbg_trace("buffer address: %p", dbuffer);
  
  if (dbuffer->is_registered)
    return 0;
  
  ibv_flags = IBV_ACCESS_LOCAL_WRITE|IBV_ACCESS_REMOTE_READ|IBV_ACCESS_REMOTE_WRITE;
  mr = ibv_reg_mr(((verbs_cnct_ctx*)ctx)->ib_pd, (void *)dbuffer->buf.addr,
		  dbuffer->buf.size, ibv_flags);
  if (!mr) {
    log_err("Could not register MR at %p: %s", (void*)dbuffer->buf.addr, strerror(errno));
    goto error_exit;
  }

  dbuffer->buf.priv.key0 = mr->lkey;
  dbuffer->buf.priv.key1 = mr->rkey;
  dbuffer->priv_ptr = (void*)mr;
  dbuffer->priv_size = sizeof(*mr);
  dbuffer->is_registered = 1;

  return PHOTON_OK;

error_exit:
  dbuffer->is_registered = 0;
  return PHOTON_ERROR;
}

static int __verbs_buffer_unregister(photonBI dbuffer, void *ctx) {
  int retval;

  dbg_trace("buffer address: %p", dbuffer);

  retval = ibv_dereg_mr((struct ibv_mr *)dbuffer->priv_ptr);
  if(retval) {
    log_err("Could not deregister MR at %p: %s", (void*)dbuffer->buf.addr, strerror(errno));
    return retval;
  }

  dbuffer->is_registered = 0;

  return PHOTON_OK;
}
