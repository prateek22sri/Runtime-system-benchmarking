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

#ifndef PHOTON_VERBS
#define PHOTON_VERBS

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>
#include <netinet/in.h>
#include <infiniband/verbs.h>
#include <rdma/rdma_cma.h>

#include "libphoton.h"
#include "photon_backend.h"

#include "verbs_buffer.h"

#define MAX_CQ_POLL   8
#define MAX_QP        1

#define PHOTON_VERBS_PUT_ALIGN  1
#define PHOTON_VERBS_GET_ALIGN  1

#define PHOTON_VERBS_DEF_ORDER  (PHOTON_ORDER_STRICT)
#define PHOTON_VERBS_COMP_ORDER (PHOTON_ORDER_STRICT)

#define VERBS_DEF_NUM_SRQ       0
#define VERBS_DEF_MULT          4

extern struct photon_backend_t photon_verbs_backend;

#ifdef __cplusplus
}
#endif

#endif
