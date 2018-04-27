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

#ifndef PHOTON_BUFFER_H
#define PHOTON_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "config.h"
#include "photon.h"

#define BUFFER_FLAG_NIL     0x0000
#define BUFFER_FLAG_NOTIFY  (1<<1)

struct photon_acct_t {
  volatile uint64_t rcur;
  uint32_t rloc;
  uint32_t event_prefix;
};

typedef struct photon_buffer_internal_t {
  struct photon_buffer_t buf;
  uint16_t is_registered;
  uint16_t ref_count;
  // keep a reference to the registered mem handle
  void *priv_ptr;
  int priv_size;
} photon_buffer_internal;

typedef struct photon_buffer_internal_t * photonBI;

struct photon_buffer_interface_t {
  photonBI (*buffer_create)(void *buf, uint64_t size);
  void (*buffer_free)(photonBI buf);
  int (*buffer_register)(photonBI buf, void *ctx, int flags);
  int (*buffer_unregister)(photonBI buf, void *ctx);
};

typedef struct photon_buffer_interface_t * photonBufferInterface;

// internal buffer API
void photon_buffer_init(photonBufferInterface buf_interface);
photonBI photon_buffer_create(void *buf, uint64_t size);
void photon_buffer_free(photonBI buf);
int photon_buffer_register(photonBI buf, void *ctx, int flags);
int photon_buffer_unregister(photonBI buf, void *ctx);

// default buffer interface methods
photonBI _photon_buffer_create(void *buf, uint64_t size);
void _photon_buffer_free(photonBI buf);
int _photon_buffer_register(photonBI buf, void *ctx, int flags);
int _photon_buffer_unregister(photonBI buf, void *ctx);

#ifdef __cplusplus
}
#endif

#endif
