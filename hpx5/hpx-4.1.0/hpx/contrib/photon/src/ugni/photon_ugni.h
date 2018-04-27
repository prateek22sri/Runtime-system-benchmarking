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

#ifndef PHOTON_UGNI_H
#define PHOTON_UGNI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>

#include "gni_pub.h"

#include "libphoton.h"
#include "photon_backend.h"
#include "photon_rdma_INFO_ledger.h"
#include "photon_rdma_ledger.h"
#include "photon_ugni_buffer.h"

#define MAX_CQ_POLL            8

#define UGNI_DEF_BTE_THRESH    (1<<12)
#define UGNI_DEF_NUM_PDS       (1<<9)

#define PHOTON_UGNI_PUT_ALIGN  1
#define PHOTON_UGNI_GET_ALIGN  4
#define PHOTON_UGNI_DEF_ORDER  (PHOTON_ORDER_NONE)
#define PHOTON_UGNI_COMP_ORDER (PHOTON_ORDER_NONE | \
				PHOTON_ORDER_STRICT)

extern struct photon_backend_t photon_ugni_backend;

#ifdef __cplusplus
}
#endif

#endif
