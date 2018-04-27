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

#include "registered.h"
#include "PhotonTransport.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include "libhpx/system.h"

namespace {
using libhpx::network::pwc::PhotonTransport;
}

extern "C" void*
dl_mmap_wrapper(size_t length)
{
  if (void *base = system_mmap_huge_pages(NULL, NULL, length, 1)) {
    PhotonTransport::Pin(base, length, NULL);
    log_mem("mapped %zu registered bytes at %p\n", length, base);
    return base;
  }

  dbg_error("failed to mmap %zu bytes anywhere in memory\n", length);
  return NULL;
}

extern "C" void
dl_munmap_wrapper(void *ptr, size_t length)
{
  if (length) {
    PhotonTransport::Unpin(ptr, length);
    system_munmap_huge_pages(NULL, ptr, length);
  }
}

void
libhpx::network::pwc::registered_allocator_init(void)
{
  mspaces[AS_REGISTERED] = create_mspace(0, 1);
}
