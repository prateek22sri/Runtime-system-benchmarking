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

#ifndef PHOTON_PWC_COLL_H
#define PHOTON_PWC_COLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libphoton.h"
#include "photon_coll.h"

extern photon_coll_interface pwc_collectives;

PHOTON_INTERNAL int photon_pwc_barrier_set_and_check(int proc, int *flag);
PHOTON_INTERNAL photonRequest photon_pwc_barrier_dequeue();

#ifdef __cplusplus
}
#endif

#endif
