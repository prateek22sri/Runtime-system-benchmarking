// ==================================================================-*- C++ -*-
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

#ifndef LIBHPX_PADDING_H
#define LIBHPX_PADDING_H

/// Given a number of bytes, how many bytes of padding do we need to get a size
/// that is a multiple of HPX_CACHELINE_SIZE? Macro because it's used in
/// structure definitions for padding.

#define _CAT1(S, T) S##T
#define _CAT(S, T) _CAT1(S, T)
#define _BYTES(N, S) (N - ((S) % N))
#define PAD_TO_N(N, S) const char _CAT(_padding,__LINE__)[_BYTES(N, S)]
#define PAD_TO_CACHELINE(S) PAD_TO_N(HPX_CACHELINE_SIZE, S)

/// Alignment bytes to a a align=2^k alignment.
#define ALIGN(bytes, align)                                 \
  (((align) - ((bytes) & ((align) - 1))) & ((align) - 1))

#ifdef __cplusplus
#include <cstddef>
namespace libhpx {
namespace util {
inline constexpr size_t PadToCacheline(size_t bytes) {
  return _BYTES(HPX_CACHELINE_SIZE, bytes);
}

inline constexpr size_t align(size_t bytes, size_t align) {
  return ALIGN(bytes, align);
}
}
}
#endif

#endif // LIBHPX_PADDING_H
