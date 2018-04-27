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

#include "Wrappers.h"
#include "libhpx/action.h"
#include "libhpx/libhpx.h"
#include "libhpx/parcel.h"
#include <lz4.h>
#include <utility>

// TODO:
// 1. in-place compression and decompression.
// 2. compression and coalescing.

namespace {
using libhpx::Network;
using libhpx::network::NetworkWrapper;
using libhpx::network::CompressionWrapper;

struct Args {
  size_t bytes;
  char data[];
};

int
DecompressHandler(const Args& args, size_t)
{
  // retrieve the original size from the payload.
  auto *p = parcel_alloc(args.bytes);
  auto *buffer = reinterpret_cast<char*>(p);
  LZ4_decompress_fast(args.data, buffer, args.bytes);
  p->thread = nullptr;
  p->next = nullptr;
  parcel_set_state(p, PARCEL_SERIALIZED);
  parcel_launch(p);
  return HPX_SUCCESS;
}

LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, Decompress, DecompressHandler,
              HPX_POINTER, HPX_SIZE_T);
} // namespace

CompressionWrapper::CompressionWrapper(Network* impl)
    : NetworkWrapper(impl)
{
}

int
CompressionWrapper::send(hpx_parcel_t *p, hpx_parcel_t *ssync)
{
  if (action_is_compressed(p->action)) {
    size_t isize = parcel_size(p);
    size_t bytes = sizeof(Args) + LZ4_compressBound(isize);
    hpx_parcel_t *q = parcel_new(p->target, Decompress, 0, 0, p->pid, 0, bytes);
    auto* args = static_cast<Args*>(hpx_parcel_get_data(q));
    args->bytes = isize;
    auto* buffer = reinterpret_cast<char*>(p);
    if (auto csize = LZ4_compress_fast(buffer, args->data, isize, bytes, 1)) {
      q->size = sizeof(Args) + csize;
      std::swap(p, q);
    }
    parcel_delete(q);
  }
  return impl_->send(p, ssync);
}
