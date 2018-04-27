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

#ifndef LIBHPX_REBALANCING_H
#define LIBHPX_REBALANCING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hpx/hpx.h>

#if defined(HAVE_AGAS) && defined(HAVE_REBALANCING)
    
// AGAS-based Rebalancer API
int rebalancer_init(void);
void rebalancer_finalize(void);

// Record a GAS block access in the BST
void rebalancer_add_entry(int src, int dst, hpx_addr_t block, size_t size);
int rebalancer_start(hpx_addr_t async, hpx_addr_t psync, hpx_addr_t msync);

#else

#define rebalancer_init()
#define rebalancer_finalize()
#define rebalancer_add_entry(...)
#define rebalancer_start(...)

#endif

#ifdef __cplusplus
}
#endif

#endif // LIBHPX_REBALANCING_H
