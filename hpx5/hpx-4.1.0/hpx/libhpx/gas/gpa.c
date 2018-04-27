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

#include <inttypes.h>
#include <hpx/builtins.h>
#include <libhpx/debug.h>
#include <libhpx/locality.h>
#include <libhpx/gpa.h>

/// Compute the phase (offset within block) of a global address.
static uint32_t _phase_of(hpx_addr_t gpa, uint32_t bsize) {
  uint32_t bsize_log2 = ceil_log2_32(bsize);
  uint64_t mask = (1ul << bsize_log2) - 1;
  uint64_t phase = mask & gpa;
  dbg_assert(phase < UINT32_MAX);
  return (uint32_t)phase;
}

/// Compute the block ID for a global address.
static uint64_t _block_of(hpx_addr_t addr, uint32_t bsize) {
  gpa_t       gpa = { .addr = addr };
  uint32_t rshift = ceil_log2_32(bsize);
  uint64_t  block = gpa.bits.offset >> rshift;
  return block;
}

static hpx_addr_t _triple_to_gpa(int64_t rank, int64_t bid, int64_t phase,
                                 uint32_t bsize) {
  // make sure that the values are actually in the expected range.
  dbg_assert(0 <= rank && rank < UINT16_MAX);
  dbg_assert(0 <= bid);
  dbg_assert(0 <= phase && phase < UINT32_MAX);

  // make sure that the phase is not inside the padded region
  DEBUG_IF (bsize && phase) {
    if (phase >= bsize) {
      dbg_error("phase %"PRId64" must be less than %"PRIu32"\n", phase, bsize);
    }
  }

  DEBUG_IF (!bsize && phase) {
    dbg_error("cannot initialize a non-cyclic gpa with a phase of %"PRId64"\n",
              phase);
  }

  // forward to pgas_offset_to_gpa(), by computing the offset by combining bid
  // and phase
  uint64_t shift = (bsize) ? ceil_log2_32(bsize) : 0;
  uint64_t offset = (bid << shift) + phase;
  return offset_to_gpa(rank, offset);
}

/// Forward declaration to be used in checking of address difference.
static hpx_addr_t _add_cyclic(hpx_addr_t, int64_t, uint32_t, bool);

/// Implementation of address difference to be called from the public interface.
/// The debug parameter is used to stop recursion in debugging checks.
static int64_t _sub_cyclic(hpx_addr_t lhs, hpx_addr_t rhs, uint32_t bsize,
                           bool check) {
  // for a block cyclic computation, we look at the three components
  // separately, and combine them to get the overall offset
  uint64_t plhs = _phase_of(lhs, bsize);
  uint64_t prhs = _phase_of(rhs, bsize);
  uint64_t llhs = gpa_to_rank(lhs);
  uint64_t lrhs = gpa_to_rank(rhs);
  uint64_t blhs = _block_of(lhs, bsize);
  uint64_t brhs = _block_of(rhs, bsize);

  int64_t dphase = plhs - prhs;
  int64_t dlocality = llhs - lrhs;
  int64_t dblock = blhs - brhs;

  // each difference in the phase is just one byte,
  // each difference in the locality is bsize bytes, and
  // each difference in the block is entire cycle of bsize bytes
  int64_t d = dblock * here->ranks * bsize +
              dlocality * bsize +
              dphase;

  if (check) {
    // make sure we're not crazy
    dbg_assert_str(_add_cyclic(rhs, d, bsize, false) == lhs,
                   "Address difference %"PRIu64"-%"PRIu64
                   " computed incorrectly as %"PRId64"\n",
                   lhs, rhs, d);
  }

  return d;
}


/// Implementation of address addition to be called from the public interface.
/// The debug parameter is used to stop recursion in debugging checks.
///
/// This code has been carefully tuned to minimize the number of `idiv`
/// operations and to eliminate branching in the common case.
static hpx_addr_t _add_cyclic(hpx_addr_t gpa, int64_t n, uint32_t bsize,
                              bool check) {
  // Add the extra bsize because this has no impact on % and correctly deals
  // with the blocks when @p n is negative. The compiler is smart enough in this
  // case to use one division for both the % and / operation.
  int64_t  _phase = _phase_of(gpa, bsize) + n + bsize;
  int64_t   phase = _phase % bsize;
  int64_t  blocks = _phase / bsize - 1;

  // Add the extra here->ranks for the same reason as above.
  int64_t   _rank = gpa_to_rank(gpa) + blocks + here->ranks;
  int64_t    rank = _rank % here->ranks;
  int64_t  cycles = _rank / here->ranks - 1;
  int64_t blockid = _block_of(gpa, bsize) + cycles;

  // Fix phase and rank in case they were negative. The blockid is already fixed
  // by the `-1` in initialization.
  phase += (phase < 0) ? bsize : 0;
  rank += (rank < 0) ? here->ranks : 0;

  if (!check) {
    return _triple_to_gpa(rank, blockid, phase, bsize);
  }

  // Sanity check during debugging, the difference between the two addresses
  // should be n.
  hpx_addr_t addr = _triple_to_gpa(rank, blockid, phase, bsize);
  int64_t    diff = _sub_cyclic(addr, gpa, bsize, false);
  dbg_assert_str(diff == n, "Address %"PRIu64"+%"PRId64" computed as %"PRIu64"."
                 " Expected %"PRId64".\n", gpa, n, addr, diff);
  return addr;
  (void)diff;
}

int64_t gpa_sub_cyclic(hpx_addr_t lhs, hpx_addr_t rhs, uint32_t bsize) {
  return _sub_cyclic(lhs, rhs, bsize, DEBUG);
}

hpx_addr_t gpa_add_cyclic(hpx_addr_t gpa, int64_t bytes, uint32_t bsize) {
  return (bsize) ? _add_cyclic(gpa, bytes, bsize, DEBUG) : gpa + bytes;
}
