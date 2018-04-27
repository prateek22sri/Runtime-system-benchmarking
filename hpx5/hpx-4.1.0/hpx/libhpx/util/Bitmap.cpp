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

/// @file libhpx/gas/bitmap.c
/// @brief Implement a simple parallel bitmap.

#include "libhpx/util/Bitmap.h"
#include "libhpx/debug.h"
#include "libhpx/libhpx.h"
#include "libhpx/util/math.h"
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <new>

namespace {
using libhpx::util::Bitmap;
using libhpx::util::ceil_div;
}

Bitmap::Bitmap(unsigned nbits, unsigned minAlign, unsigned baseAlign)
    : lock_(),
      minAlign_(minAlign),
      baseAlign_(baseAlign),
      nBits_(nbits),
      nBlocks_(ceil_div(nbits, BLOCK_BITS)),
      min_(0),
      max_(nbits),
      blocks_(new uintptr_t[nBlocks_])
{
  std::fill_n(blocks_, nBlocks_, UINTPTR_MAX);
}

Bitmap::~Bitmap()
{
  delete[] blocks_;
}

inline unsigned
Bitmap::BlockIFS(uintptr_t b)
{
  assert(b);
  return ctzl(b);
}

inline unsigned
Bitmap::BlockILS(uintptr_t b)
{
  assert(b);
  return BLOCK_BITS - clzl(b) - 1;
}

inline uintptr_t
Bitmap::CreateMask(unsigned offset, unsigned length)
{
  assert(offset < BLOCK_BITS);
  // We construct the mask by shifting the BLOCK_MASK constant right the
  // number of bits to result in the correct number of 1s, and then
  // left-shifting that string back up to the offset. If the left-shift happens
  // to push some bits back off the right end, that's fine and what we expect.
  unsigned rshift = (BLOCK_BITS < length) ? 0 : BLOCK_BITS - length;
  return (UINTPTR_MAX >> rshift) << offset;
}

unsigned
Bitmap::match(unsigned bit, unsigned nbits, unsigned (*f)(uintptr_t)) const
{
  assert(nbits);

  unsigned  block = bit / BLOCK_BITS;            // block index
  unsigned offset = bit % BLOCK_BITS;            // initial offset in block
  uintptr_t  mask = CreateMask(offset, nbits);   // mask to match
  unsigned      n = 0;                           // bits processed

  while (n < nbits) {
    if (uintptr_t mismatch = mask ^ (mask & blocks_[block++])) {
      // return the number of bits we've matched so far, plus the number of bits
      // in the current word depending on if the user wants the first or last
      // mismatch. The trailing BlockIFS(mask) deals with mismatches in the
      // first block only, the mask starts at index 0 for all other blocks.
      return n + f(mismatch) - BlockIFS(mask);
    }

    // we matched each bit in the mask
    n += popcountl(mask);
    mask = CreateMask(0, nbits - n);
  }

  assert(n == nbits);
  return n;
}

unsigned
Bitmap::findFirstFree(unsigned bit, unsigned max) const
{
  assert(bit < max);
  log_gas("finding the first free bit during allocation\n");

  unsigned  block = bit / BLOCK_BITS;
  unsigned offset = bit % BLOCK_BITS;
  uintptr_t  mask = CreateMask(offset, BLOCK_BITS);

  while (bit < max) {
    if (uintptr_t match = mask & blocks_[block++]) {
      // For a match in the first block, we need to account for the fact that
      // the first set bit in mask might not be the 0th bit, so we subtract that
      // out.
      return bit + BlockIFS(match) - BlockIFS(mask);
    }

    // we matched each bit in the mask
    bit += popcountl(mask);
    mask = UINTPTR_MAX;
  }

  return max;
}

void
Bitmap::set(unsigned bit, unsigned nbits)
{
  assert(nbits);

  unsigned  block = bit / BLOCK_BITS;
  unsigned offset = bit % BLOCK_BITS;
  uintptr_t  mask = CreateMask(offset, nbits);

  while (nbits > 0) {
    //    val   0 0 1 1
    //   mask   0 1 1 0
    //          -------
    // result   0 0 0 1
    blocks_[block++] &= ~mask;

    // we set each bit in the mask
    nbits -= popcountl(mask);
    mask = CreateMask(0, nbits);
  }
}

void
Bitmap::clear(unsigned bit, unsigned nbits)
{
  assert(nbits);

  unsigned  block = bit / BLOCK_BITS;
  unsigned offset = bit % BLOCK_BITS;
  uintptr_t  mask = CreateMask(offset, nbits);

  while (nbits > 0) {
    //    val   0 0 1 1
    //   mask   0 1 1 0
    //          -------
    // result   0 1 1 1
    blocks_[block++] |= mask;

    // we cleared each bit in the mask
    nbits -= popcountl(mask);
    mask = CreateMask(0, nbits);
  }
}

/// Count the number of unused blocks in the bitmap.
int
Bitmap::countUnusedBlocks() const
{
  unsigned unused = 0;
  for (unsigned i = 0, e = nBlocks_; i < e; ++i) {
    unused += popcountl(blocks_[i]);
  }

  unsigned extra = BLOCK_BITS - (nBits_ % BLOCK_BITS);
  return (int)(unused - extra);
}

int
Bitmap::oom(unsigned nbits, unsigned align) const
{
  dbg_error("Application ran out of global address space.\n"
            "\t%u blocks requested with alignment %u\n"
            "\t%u blocks available\n", nbits, align, countUnusedBlocks());
  return LIBHPX_ENOMEM;
}

int
Bitmap::reserve(unsigned nbits, unsigned align, unsigned *i)
{
  log_gas("searching for %u blocks with alignment %u.\n", nbits, align);
  if (nbits == 0) {
    return LIBHPX_EINVAL;
  }

  std::lock_guard<std::mutex> _(lock_);
  // scan for a match, starting with the minimum available bit
  unsigned bit = min_;
  while (true) {
    // crazy way of finding an aligned bit
    unsigned long val = bit * (1ul << minAlign_) + (1ul << baseAlign_);
    unsigned max = ctzl(val);
    while (align > max) {
      bit += 1;
      val = bit * (1ul << minAlign_) + (1ul << baseAlign_);
      max = ctzl(val);
    }

    // make sure the match is inbounds
    if (bit + nbits > max_) {
      return oom(nbits, align);
    }

    unsigned matched = match(bit, nbits, BlockILS);
    if (matched == nbits) {
      break;
    }

    bit = findFirstFree(bit + matched, nBits_);
  }

  // make sure we didn't run out of memory
  assert(bit + nbits <= max_);

  if (min_ == bit) {
    unsigned min = bit + nbits;
    log_gas("updated minimum free bit from %u to %u\n", min_, min);
    min_ = min;
  }

  set(bit, nbits);
  log_gas("found at offset %u.\n", bit);
  *i = bit;
  return LIBHPX_OK;
}

int
Bitmap::rreserve(unsigned nbits, unsigned align, unsigned *i)
{
  log_gas("reverse search for %u blocks with alignment %u.\n", nbits, align);
  if (nbits == 0) return LIBHPX_EINVAL;

  std::lock_guard<std::mutex> _(lock_);
  unsigned bit;
  // scan for a match, starting with the max available bit
  bit = max_;
  unsigned matched = 0;

  while (matched != nbits) {
    assert(matched <= nbits);

    // shift down by the number of bits we matched in the last round
    unsigned shift = nbits - matched;
    if (bit < shift) {
      return oom(nbits, align);
    }

    bit = bit - shift;

    // compute the closest aligned bit to the shifted bit
    unsigned long val = bit * (1ul << minAlign_) + (1ul << baseAlign_);
    unsigned max = ctzl(val);
    while (align > max) {
      if (bit == 0) {
        return oom(nbits, align);
      }
      bit -= 1;
      val = bit * (1ul << minAlign_) + (1ul << baseAlign_);
      max = ctzl(val);
    }

    if (bit < min_) {
      return oom(nbits, align);
    }

    // see how far we can match
    matched = match(bit, nbits, BlockIFS);
  }

  // make sure we didn't run out of memory
  assert(bit + nbits <= max_);

  unsigned max = bit + nbits;
  if (max_ == max) {
    log_gas("updated maximum bit from %u to %u\n", max_, bit);
    max_ = bit;
  }

  set(bit, nbits);
  log_gas("found at offset %u.\n", bit);

  *i = bit;
  return LIBHPX_OK;
}

void
Bitmap::release(unsigned bit, unsigned nbits)
{
  log_gas("release %u blocks at %u.\n", nbits, bit);

  std::lock_guard<std::mutex> _(lock_);
  clear(bit, nbits);

  if (bit < min_) {
    log_gas("updated minimum free bit from %u to %u\n", min_, bit);
    min_ = bit;
  }

  unsigned max = bit + nbits;
  if (max > max_) {
    log_gas("updated maximum bit from %u to %u\n", max_, max);
    max_ = max;
  }
}

bool
Bitmap::isSet(unsigned bit, unsigned nbits) const
{
  dbg_assert_str(bit + nbits <= nBits_,
                 "query out of range, %d + %d > %d\n", bit, nbits, nBits_);

  std::lock_guard<std::mutex> _(lock_);
  unsigned  block = bit / BLOCK_BITS;
  unsigned offset = bit % BLOCK_BITS;
  uintptr_t  mask = CreateMask(offset, nbits);

  while (nbits) {
    if (mask ^ (mask & blocks_[block++])) return true;
    nbits -= popcountl(mask);
    mask   = CreateMask(0, nbits);
  }

  return false;
}
