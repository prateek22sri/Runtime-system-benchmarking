// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

#if !defined(PRNG_HEADER_)
#define PRNG_HEADER_

#include "Random123/threefry.h"

#include "pxgl/pxgl.h"

#if !defined(MAXWEIGHT)
#define MAXWEIGHT 255
#endif
//extern threefry4x32_key_t key;
				 
threefry4x32_ctr_t ctr1 (int64_t);
threefry4x32_ctr_t ctr2 (int64_t, int64_t);

void init_prng (void);
//int64_t scramble (int64_t);
uint8_t random_weight (int64_t);
void random_edgevals (float *, int64_t);
//void sample_roots (int64_t *, int64_t, int64_t);
void sample_roots(sssp_uint_t *root, sssp_uint_t nroot, size_t KEY, sssp_uint_t numvertices,adj_list_t *graph);
int32_t prng_check (void);

#endif /* PRNG_HEADER_ */
