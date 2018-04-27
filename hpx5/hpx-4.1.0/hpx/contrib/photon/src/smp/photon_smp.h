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

#ifndef PHOTON_SMP
#define PHOTON_SMP

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>

#include "libphoton.h"
#include "photon_backend.h"
#include "photon_rdma_ledger.h"

#define PHOTON_SMP_PUT_ALIGN  1
#define PHOTON_SMP_GET_ALIGN  1

#define PHOTON_SMP_COMP_ORDER (PHOTON_ORDER_NONE)

#ifdef __cplusplus
}
#endif

#endif
