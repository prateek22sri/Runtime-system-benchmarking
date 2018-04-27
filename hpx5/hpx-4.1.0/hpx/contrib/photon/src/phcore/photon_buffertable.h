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

#ifndef PHOTON_BUFFERTABLE_H
#define PHOTON_BUFFERTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "photon_buffer.h"
#include "logging.h"

// return nonzero means error
int buffertable_init(int max_buffers);
void buffertable_finalize();

int buffertable_find_containing(void* start, uint64_t size, photonBI* result);
int buffertable_find_exact(void* start, uint64_t size, photonBI* result);

int buffertable_insert(photonBI buffer);
int buffertable_remove(photonBI buffer);

#ifdef __cplusplus
}
#endif

#endif

