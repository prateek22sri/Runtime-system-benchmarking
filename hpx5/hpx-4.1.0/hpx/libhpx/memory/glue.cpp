// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/// @file  libhpx/memory/glue.c
///
/// @brief This file just bind hpx_malloc_registered() and hpx_free_registered()
///        to their counterparts in the memory.h header.
#include <hpx/hpx.h>
#include <libhpx/memory.h>

void *hpx_malloc_registered(size_t bytes) {
  return as_malloc(AS_REGISTERED, bytes);
}

void *hpx_calloc_registered(size_t elements, size_t bytes) {
  return as_calloc(AS_REGISTERED, elements, bytes);
}

void *hpx_memalign_registered(size_t align, size_t bytes) {
  return as_memalign(AS_REGISTERED, align, bytes);
}

void hpx_free_registered(void *p) {
  as_free(AS_REGISTERED, p);
}
