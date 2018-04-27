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

#ifndef PHOTON_H
#define PHOTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "photon_config.h"
#include "photon_attributes.h"
#include "photon_collectives.h"
#include "photon_atomics.h"

#define PHOTON_OK              0x0000
#define PHOTON_ERROR_NOINIT    0x0001
#define PHOTON_ERROR           0x0002
#define PHOTON_ERROR_RESOURCE  0x0004
#define PHOTON_SHUTDOWN        0x0008

#define PHOTON_EXCH_TCP        0x0000
#define PHOTON_EXCH_MPI        0x0001
#define PHOTON_EXCH_PMI        0x0002
#define PHOTON_EXCH_XSP        0x0004
#define PHOTON_EXCH_EXTERNAL   0x0008

#define PHOTON_SEND_LEDGER     0x0000
#define PHOTON_RECV_LEDGER     0x0001

#define PHOTON_REQ_NIL         0x0000
#define PHOTON_REQ_COMPLETED   0x0001  // explitily set a request completed for FIN
#define PHOTON_REQ_PWC_NO_LCE  0x0002  // don't return a local rid (pwc-specific)
#define PHOTON_REQ_PWC_NO_RCE  0x0004  // don't send a remote rid (pwc-specific)

#define PHOTON_AMO_FADD        0x0001
#define PHOTON_AMO_CSWAP       0x0002

#define PHOTON_PROBE_ANY       0xffff
#define PHOTON_PROBE_EVQ       0x0001
#define PHOTON_PROBE_LEDGER    0x0002

#define PHOTON_ANY_TAG         -1
#define PHOTON_ANY_SOURCE      -1
#define PHOTON_ANY_SIZE        -1

typedef union photon_addr_t {
  uint8_t       raw[16];
  unsigned long s_addr;
  struct {
    uint64_t    prefix;
    uint64_t    proc_id;
  } global;
  struct {
    uint32_t    blk0;
    uint32_t    blk1;
    uint32_t    blk2;
    uint32_t    blk3;
  } blkaddr;
} photon_addr;

typedef uint64_t photon_rid;

typedef struct {
  union {
    uint64_t    u64;
    void      *data;
  };
  uint32_t      size;
} photon_cid;

// status for photon requests
struct photon_status_t {
  union photon_addr_t src_addr;
  photon_rid request;
  uint64_t size;
  int tag;
  int count;
  int error;
};

// registered buffer keys 
// current abstraction is two 64b values, covering existing photon backends
struct photon_buffer_priv_t {
  uint64_t key0;
  uint64_t key1;
};

// use to track both local and remote buffers
struct photon_buffer_t {
  uintptr_t addr;
  uint64_t size;
  struct photon_buffer_priv_t priv;
};

typedef union photon_addr_t         * photonAddr;
typedef struct photon_status_t      * photonStatus;
typedef struct photon_buffer_priv_t * photonBufferPriv;
typedef struct photon_buffer_t      * photonBuffer;

#include "photon_api.h"

#ifdef __cplusplus
}
#endif

#endif
