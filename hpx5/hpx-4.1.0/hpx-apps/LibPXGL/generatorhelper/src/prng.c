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

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>
#include <stdatomic.h>
#include <assert.h>

#include "generatorhelper/include/Random123/threefry.h"
#define R123_USE_U01_DOUBLE 1
#include "generatorhelper/include/Random123/u01fixedpt.h"

#include "generatorhelper/include/globals.h"
#include "generatorhelper/include/prng.h"

#include "hpx/hpx.h"
#include "pxgl/pxgl.h"
#include "libpxgl/sssp_common.h"


static hpx_action_t _sssp_visit_source = 0;
static hpx_action_t _sssp_check_for_nonzero_neighbor = 0;

static threefry4x32_key_t key = {{0xdeadbeef, 0xdecea5ed,
                  0x0badcafe, 0x5ca1ab1e}};


static uint64_t scramble0, scramble1;
static inline uint64_t bitreverse(uint64_t);
//static inline threefry4x32_ctr_t ctr1 (int64_t);
//static inline threefry4x32_ctr_t ctr2 (int64_t, int64_t);
//inline threefry4x32_ctr_t ctr2 (int64_t, int64_t);
static inline float fprng (int64_t, int64_t);
static inline double dprng (int64_t, int64_t);

void
init_prng (void)
{
  threefry4x32_ctr_t ctr, out;

  /* Grab any changed seeds for experimental runs. */
  errno = 0;
#define GET_SEED(k)             \
  if (getenv ("SEED" #k)) {         \
    long t;                 \
    t = strtoul (getenv ("SEED" #k), NULL, 10); \
    if (t == ULONG_MAX) {           \
      if (errno) {              \
    perror ("Error setting seed" #k ":");   \
    abort ();               \
      }                     \
    }                       \
    key.v[k] = t;               \
  }
  GET_SEED(0);
  GET_SEED(1);
  GET_SEED(2);
  GET_SEED(3);

  /* Initialize scrambling. */
  ctr.v[0] = ctr.v[1] = ctr.v[2] = ctr.v[3] = -1;
  out = threefry4x32 (ctr, key);
  scramble0 = ((uint64_t)out.v[0]) << 32 | (uint64_t)out.v[1];
  scramble1 = ((uint64_t)out.v[2]) << 32 | (uint64_t)out.v[3];
}

/* Apply a permutation to scramble vertex numbers; a randomly generated
 * permutation is not used because applying it at scale is too expensive. */
/*int64_t
scramble (int64_t v0)
{
  uint64_t v = (uint64_t)v0;
  v += scramble0 + scramble1;
  v *= (scramble0 | UINT64_C(0x4519840211493211));
  v = (bitreverse(v) >> (64 - SCALE));
  assert ((v >> SCALE) == 0);
  v *= (scramble1 | UINT64_C(0x3050852102C843A5));
  v = (bitreverse(v) >> (64 - SCALE));
  assert ((v >> SCALE) == 0);
  return (int64_t)v;
}
*/
uint8_t
random_weight (int64_t idx)
{
  uint8_t out;
  float outf;
  outf = ceilf (MAXWEIGHT * dprng (idx, 0));
  out = (uint8_t) outf;
  if (!out) fprintf (stderr, "wtf %d %g %d\n", MAXWEIGHT, (double)outf, (int)out);
  assert (out > 0);
  return out;
}

void
random_edgevals (float * v, int64_t idx)
{
  /* v is SCALE x 2 */
  for (int scl = 0; scl < SCALE; scl += 2) {
    threefry4x32_ctr_t outc;
    outc = threefry4x32 (ctr2 (idx, 1+scl/2), key);
    v[scl] = u01fixedpt_open_open_32_24 (outc.v[0]);
    v[SCALE + scl] = u01fixedpt_open_open_32_24 (outc.v[1]);
    if (scl < SCALE-1) {
      v[scl+1] = u01fixedpt_open_open_32_24 (outc.v[2]);
      v[SCALE + scl+1] = u01fixedpt_open_open_32_24 (outc.v[3]);
    }
  }
}


static int _sssp_check_for_nonzero_neighbor_action() {
  const hpx_addr_t target = hpx_thread_current_target();
  adj_list_vertex_t *vertex;
  if (!hpx_gas_try_pin(target, (void**)&vertex))
    return HPX_RESEND;
  const size_t num_edges = vertex->num_edges;
  hpx_gas_unpin(target);
  return HPX_THREAD_CONTINUE(num_edges);
}

static int _sssp_visit_source_action() {
  const hpx_addr_t target = hpx_thread_current_target();
  hpx_addr_t vertex;
  hpx_addr_t *v;
  if (!hpx_gas_try_pin(target, (void**)&v))
    return HPX_RESEND;
  vertex = *v;
  hpx_gas_unpin(target);
  size_t num_edges;
  hpx_call_sync(vertex, _sssp_check_for_nonzero_neighbor, &num_edges,
                sizeof(num_edges), NULL, 0);
  return HPX_THREAD_CONTINUE(num_edges);
}

void sample_roots(sssp_uint_t *root, sssp_uint_t nroot, size_t KEY,sssp_uint_t numvertices,adj_list_t *graph)
{
  /* Method A in Jeffrey Scott Vitter, "An Efficient Algorithm for
  Sequential Random Sampling," ACM Transactions on Mathematical
  Software, 13(1), March 1987, 58-67. */

  /* double n = numvertices;//NV; */
  /* int64_t top = numvertices  - nroot; //NV */
  /* int64_t cur = -1; */
  /* int64_t S; */
  /* double r; */
  sssp_uint_t m, t;

  for (m = 0; m < nroot; ++m) root[m] = SSSP_UINT_MAX;
  m = 0; t = 0;
  while (m < nroot && t < numvertices) {
    size_t num_edges = 0;
    const hpx_addr_t index = hpx_addr_add(*graph, t * sizeof(hpx_addr_t), _index_array_block_size);
    hpx_call_sync(index, _sssp_visit_source, &num_edges, sizeof(num_edges),
                  NULL, 0);
    if (!num_edges) ++t;
    else root[m++] = t++;
  }

  /* for (m = 0; m < nroot; ++m) { */
  /*   double quot; */
  /*   r = dprng (KEY, m); */
  /*   S = 0; */
  /*   quot = top / n; */
  /*   while (quot > r) { */
  /*     S += 1; */
  /*     top -= 1; */
  /*     n -= 1; */
  /*     quot *= top / n; */
  /*   } */
  /*   cur += S; */
  /*   const hpx_addr_t index = hpx_addr_add(*graph, cur * sizeof(hpx_addr_t), _index_array_block_size); */
  /*   size_t num_edges = 0; */
  /*   hpx_call_sync(index, _sssp_visit_source, &num_edges,sizeof(num_edges), NULL, 0); */
  /*   if(num_edges==0){ */
  /*     m--; */
  /*     continue; */
  /*   } */
  /*   root[m] = cur; */
  /*   n -= 1; */
  /* } */
  /* while(true){ */
  /*   r = dprng (KEY, nroot-1); */
  /*   S = floor (n * r); */
  /*   cur += S+1; */
  /*   const hpx_addr_t index = hpx_addr_add(*graph, cur * sizeof(hpx_addr_t), _index_array_block_size); */
  /*   SSSP_UINT_T num_edges; */
  /*   hpx_call_sync(index, _sssp_visit_source, &num_edges,sizeof(num_edges), NULL, 0); */
  /*   if(num_edges==0){ */
  /*     continue; */
  /*   } */
  /*   root[nroot-1] = cur; */
  /*   break; */
  /* } */

#if !defined (NDEBUG)
  for (sssp_uint_t m = 0; m < nroot; ++m) {
    // printf("root[%" SSSP_UINT_PRI "] is %" SSSP_UINT_PRI ".\n", m, root[m]);
    assert (root[m] >= 0 && root[m] < numvertices);
    for (sssp_uint_t m2 = m+1; m2 < nroot; ++m2)
      assert (root[m2] != root[m]);
  }
#endif

  /* for (sssp_uint_t m = 0; m < nroot-1; ++m){ */
  /*   printf("Root: %" PRIu64 " \n",root[m]); */
  /* } */
}

int32_t
prng_check (void)
{
    threefry4x32_ctr_t out;
    out = threefry4x32 (ctr2 (SCALE, EF), key);
    return out.v[0];
}

/* Helper routines. */

threefry4x32_ctr_t
ctr1 (int64_t k)
{
  threefry4x32_ctr_t ctr;
  ctr.v[0] = ((uint64_t)k) >> 32;
  ctr.v[1] = ((uint64_t)k) & 0xFFFFFFFFul;
  ctr.v[2] = ctr.v[3] = 0;
  return ctr;
}

threefry4x32_ctr_t
ctr2 (int64_t k1, int64_t k2)
{
  threefry4x32_ctr_t ctr;
  ctr.v[0] = ((uint64_t)k1) >> 32;
  ctr.v[1] = ((uint64_t)k1) & 0xFFFFFFFFul;
  ctr.v[2] = ((uint64_t)k2) >> 32;
  ctr.v[3] = ((uint64_t)k2) & 0xFFFFFFFFul;
  return ctr;
}

float
fprng (int64_t v1, int64_t v2)
{
  threefry4x32_ctr_t outc;
  float out;
  outc = threefry4x32 (ctr2 (v1, v2), key);
  out = u01fixedpt_open_open_32_24 (outc.v[0]);
  assert (out > 0);
  return out;
}

double
dprng (int64_t v1, int64_t v2)
{
  union {
    threefry4x32_ctr_t outc;
    int64_t v[2];
  } u;
  u.outc = threefry4x32 (ctr2 (v1, v2), key);
  return u01fixedpt_closed_open_64_53 (u.v[0]);
}

/* Reverse bits in a number; this should be optimized for performance
  (including using bit- or byte-reverse intrinsics if your platform
  has them). */
uint64_t
bitreverse(uint64_t x) {
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)
#define USE_GCC_BYTESWAP /* __builtin_bswap* are in 4.3 but not 4.2 */
#endif

#ifdef FAST_64BIT_ARITHMETIC

  /* 64-bit code */
#ifdef USE_GCC_BYTESWAP
  x = __builtin_bswap64(x);
#else
  x = (x >> 32) | (x << 32);
  x = ((x >> 16) & UINT64_C(0x0000FFFF0000FFFF)) |
    ((x & UINT64_C(0x0000FFFF0000FFFF)) << 16);
  x = ((x >>  8) & UINT64_C(0x00FF00FF00FF00FF)) |
    ((x & UINT64_C(0x00FF00FF00FF00FF)) <<  8);
#endif
  x = ((x >>  4) & UINT64_C(0x0F0F0F0F0F0F0F0F)) |
    ((x & UINT64_C(0x0F0F0F0F0F0F0F0F)) <<  4);
  x = ((x >>  2) & UINT64_C(0x3333333333333333)) |
    ((x & UINT64_C(0x3333333333333333)) <<  2);
  x = ((x >>  1) & UINT64_C(0x5555555555555555)) |
    ((x & UINT64_C(0x5555555555555555)) <<  1);
  return x;

#else

  /* 32-bit code */
  uint32_t h = (uint32_t)(x >> 32);
  uint32_t l = (uint32_t)(x & UINT32_MAX);
#ifdef USE_GCC_BYTESWAP
  h = __builtin_bswap32(h);
  l = __builtin_bswap32(l);
                        \
  #else
  h = (h >> 16) | (h << 16);
  l = (l >> 16) | (l << 16);
  h = ((h >> 8) & UINT32_C(0x00FF00FF)) |
    ((h & UINT32_C(0x00FF00FF)) << 8);
  l = ((l >> 8) & UINT32_C(0x00FF00FF)) |
    ((l & UINT32_C(0x00FF00FF)) << 8);
#endif
  h = ((h >> 4) & UINT32_C(0x0F0F0F0F)) |
    ((h & UINT32_C(0x0F0F0F0F)) << 4);
  l = ((l >> 4) & UINT32_C(0x0F0F0F0F)) |
    ((l & UINT32_C(0x0F0F0F0F)) << 4);
  h = ((h >> 2) & UINT32_C(0x33333333)) |
    ((h & UINT32_C(0x33333333)) << 2);
  l = ((l >> 2) & UINT32_C(0x33333333)) |
    ((l & UINT32_C(0x33333333)) << 2);
  h = ((h >> 1) & UINT32_C(0x55555555)) |
    ((h & UINT32_C(0x55555555)) << 1);
  l = ((l >> 1) & UINT32_C(0x55555555)) |
    ((l & UINT32_C(0x55555555)) << 1);
  return ((uint64_t)l << 32) | h; /* Swap halves */
#endif
}


static HPX_CONSTRUCTOR void _sample_graph_register_actions() {
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              _sssp_visit_source, _sssp_visit_source_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              _sssp_check_for_nonzero_neighbor, _sssp_check_for_nonzero_neighbor_action,
              HPX_POINTER, HPX_SIZE_T);

}
