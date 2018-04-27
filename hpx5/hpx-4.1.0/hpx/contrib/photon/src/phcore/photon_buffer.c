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
#include "photon_buffer.h"

static photonBufferInterface bi = NULL;

void photon_buffer_init(photonBufferInterface buf_interface) {
  if (buf_interface) {
    bi = buf_interface;
  }
  else {
    log_warn("Did not set buffer interface!");
  }
}

photonBI photon_buffer_create(void *buf, uint64_t size) {
  if (!bi) {
    log_err("Buffer interface not set!");
    return NULL;
  }

  return bi->buffer_create(buf, size);
}

void photon_buffer_free(photonBI buf) {
  if (!bi) {
    log_err("Biffer interface not set!");
    return;
  }

  bi->buffer_free(buf);
}

int photon_buffer_register(photonBI buf, void *ctx, int flags) {
  if (!bi) {
    log_err("Buffer interface not set!");
    return PHOTON_ERROR;
  }

  return bi->buffer_register(buf, ctx, flags);
}

int photon_buffer_unregister(photonBI buf, void *ctx) {
  if (!bi) {
    log_err("Buffer interface not set!");
    return PHOTON_ERROR;
  }

  return bi->buffer_unregister(buf, ctx);
}

/* default buffer interface methods */
photonBI _photon_buffer_create(void *buf, uint64_t size) {
  photonBI new_buf;

  dbg_trace("%llu", size);

  new_buf = malloc(sizeof(struct photon_buffer_internal_t));
  if (!new_buf) {
    log_err("malloc failed");
    return NULL;
  }

  memset(new_buf, 0, sizeof(*new_buf));

  dbg_trace("allocated buffer struct: %p", new_buf);
  dbg_trace("contains buffer pointer: %p of size %" PRIu64, buf, size);
  
  new_buf->buf.addr = (uintptr_t)buf;
  new_buf->buf.size = size;
  new_buf->buf.priv = (struct photon_buffer_priv_t){0,0};
  new_buf->ref_count = 1;
  new_buf->priv_ptr = NULL;
  new_buf->priv_size = 0;

  return new_buf;
}

void _photon_buffer_free(photonBI buf) {
  /* unregister is up to the user for now
  if (buf->is_registered) {
    if (!bi) {
      log_err("Buffer interface not set!");
      return;
    }
    bi->buffer_unregister(buf);
  }
  */
  free(buf);
}

int _photon_buffer_register(photonBI buf, void *ctx, int flags) {
  if (!bi) {
    log_err("Buffer interface not set!");
    return PHOTON_ERROR;
  }

  return bi->buffer_register(buf, ctx, flags);
}

int _photon_buffer_unregister(photonBI buf, void *ctx) {
  if (!bi) {
    log_err("Buffer interface not set!");
    return PHOTON_ERROR;
  }

  return bi->buffer_unregister(buf, ctx);
}
