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

#include "photon_ugni_buffer.h"
#include "photon_ugni_connect.h"
#include "logging.h"

static int __ugni_buffer_register(photonBI dbuffer, void *ctx, int flags);
static int __ugni_buffer_unregister(photonBI dbuffer, void *ctx);

struct photon_buffer_interface_t ugni_buffer_interface = {
  .buffer_create = _photon_buffer_create,
  .buffer_free = _photon_buffer_free,
  .buffer_register = __ugni_buffer_register,
  .buffer_unregister = __ugni_buffer_unregister,
};

static int __ugni_buffer_register(photonBI dbuffer, void *ctx, int flags) {
  int status, rflags;
  ugni_cnct_ctx *uctx = (ugni_cnct_ctx*)ctx;
  gni_mem_handle_t mdh, *smdh;
  gni_cq_handle_t rcq_handle = NULL;

  if (dbuffer->is_registered)
    return PHOTON_OK;
  
  if (uctx->use_rcq) {
    rcq_handle = uctx->remote_cq_handles[0];
  }

  //rflags = GNI_MEM_READWRITE | GNI_MEM_STRICT_PI_ORDERING | GNI_MEM_PI_FLUSH;
  rflags = GNI_MEM_READWRITE;

  sync_tatas_acquire(&uctx->lock);
  {  
    status = GNI_MemRegister(uctx->nic_handle, (uint64_t)dbuffer->buf.addr,
			     dbuffer->buf.size, rcq_handle, rflags, -1, &mdh);
    if (status != GNI_RC_SUCCESS) {
      dbg_err("GNI_MemRegister ERROR status: %s (%d)", gni_err_str[status], status);
      goto error_exit;
    }
  }
  sync_tatas_release(&uctx->lock);

  dbg_trace("GNI_MemRegister size: %lu address: %p", dbuffer->buf.size, dbuffer->buf.addr);
  
  smdh = malloc(sizeof(mdh));
  *smdh = mdh;

  dbuffer->buf.priv.key0 = mdh.qword1;
  dbuffer->buf.priv.key1 = mdh.qword2;
  dbuffer->priv_ptr = smdh;
  dbuffer->priv_size = sizeof(*smdh);
  dbuffer->is_registered = 1;

  return PHOTON_OK;

error_exit:
  dbuffer->is_registered = 0;
  return PHOTON_ERROR;
}

static int __ugni_buffer_unregister(photonBI dbuffer, void *ctx) {
  int status;
  ugni_cnct_ctx *uctx = (ugni_cnct_ctx*)ctx;

  sync_tatas_acquire(&uctx->lock);
  {  
    status = GNI_MemDeregister(uctx->nic_handle, (gni_mem_handle_t*)dbuffer->priv_ptr);
    if (status != GNI_RC_SUCCESS) {
      dbg_err("GNI_MemDeregister ERROR status: %s (%d)", gni_err_str[status], status);
      goto error_exit;
    }
  }
  sync_tatas_release(&uctx->lock);

  dbg_trace("GNI_MemDeregister (%p)", (void *)dbuffer->buf.addr);

  dbuffer->is_registered = 0;

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}
