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

#ifndef PHOTON_PWC_H
#define PHOTON_PWC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libphoton.h"
#include "photon_request.h"
#include "util.h"

#define PWC_ALIGN      8
#define PWC_BUFFER     0x01
#define PWC_LEDGER     0x02

typedef enum pwc_cid_type_t {
  PWC_CTYPE_NULL = 0,
  PWC_CTYPE_USER,
  PWC_CTYPE_INTERNAL,
  PWC_CTYPE_COLL,
  PWC_CTYPE_MAX
} pwc_cid_type;

typedef enum pwc_command_t {
  PWC_COMMAND_NULL = 0,
  PWC_COMMAND_PWC_REQ,
  PWC_COMMAND_PWC_REP,
  PWC_COMMAND_BARRIER,
  PWC_COMMAND_ALLREDUCE,
  PWC_COMMAND_MAX
} pwc_command;
  
typedef struct photon_eb_hdr_t {
  volatile uint8_t header;
  photon_cid_hdr cent;
  uintptr_t addr;
  uint16_t length;
  volatile uint8_t footer;
} photon_eb_hdr;

#define EB_MSG_SIZE(s)       (sizeof(struct photon_eb_hdr_t) + s + sizeof(uint8_t))
#define PWC_MAX_PACKED       (ALIGN(EB_MSG_SIZE(_photon_spsize + _photon_idsize), PWC_ALIGN))

// interface to deal with pwc used along with post/put/get/test
int photon_pwc_init(photonConfig cfg);
int photon_pwc_add_req(photonRequest req);
photonRequest photon_pwc_pop_req(int proc);
int photon_pwc_register_ctype_handler(pwc_cid_type type, void *handler);

int _photon_put_with_completion(int proc, uint64_t size,
				photonBuffer lbuf,
				photonBuffer rbuf,
				photon_cid local, photon_cid remote,
				int flags,
				pwc_cid_type type, pwc_command cmd);

int _photon_get_with_completion(int proc, uint64_t size,
				photonBuffer lbuf,
				photonBuffer rbuf,
				photon_cid local, photon_cid remote,
				int flags,
				pwc_cid_type type, pwc_command cmd);

int _photon_probe_completion(int proc, int *flag, int *remaining,
			     photon_cid *comp, int *src, 
			     void (*cb)(photon_cid), int flags);

#ifdef __cplusplus
}
#endif

#endif
