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

#include "libhpx/debug.h"
#include "libhpx/system.h"
#include <errno.h>
#ifdef HAVE_HUGETLBFS
# include <cstddef>
extern "C" {
# include <hugetlbfs.h>
}
#endif
#include <cinttypes>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <atomic>

/// We use the hugetlbfs interface. In order to avoid locking around the huge
/// page fd, we simply open the file once at startup.
#ifdef HAVE_HUGETLBFS
static int _hugepage_fd = -1;
static long _hugepage_size = 0;
static long _hugepage_mask = 0;

static void HPX_CONSTRUCTOR _system_init(void) {
  _hugepage_fd = hugetlbfs_unlinked_fd();
  if (_hugepage_fd < 0) {
    log_error("could not get huge tlb file descriptor.\n");
  }
  _hugepage_size = gethugepagesize();
  _hugepage_mask = _hugepage_size - 1;
}

static void HPX_DESTRUCTOR _system_fini(void) {
  if (_hugepage_fd >= 0 && close(_hugepage_fd)) {
    dbg_error("failed to close the hugetlbfs file descriptor\n");
  }
}
#endif

#ifdef ENABLE_DEBUG
static std::atomic<uintptr_t> _total(0);
#endif

static uintptr_t HPX_USED _update_total(intptr_t n) {
#ifndef ENABLE_DEBUG
  return 0;
#else
  return _total.fetch_add(n, std::memory_order_acq_rel) + n;
#endif
}

/// A simple mmap wrapper that guarantees alignment.
///
/// This calls munmap at least once, and will attempt to allocate more data than
/// is always necessary, so it should only be used on a fallback path. This
/// implementation simply over allocates space so that we are guaranteed that
/// there is a properly aligned region inside of the mapping. It then munmaps
/// the parts of the allocation that aren't part of this region.
///
/// @param         addr A "suggested" address. This is most likely ignored.
/// @param            n The number of bytes to map, must be 2^n.
/// @param         prot The protection flags.
/// @param        flags Additional flags for the mapping.
/// @param           fd A file descriptor to map from.
/// @param          off The file offset to map at.
/// @param        align The alignment, must be 2^n.
///
/// @returns The properly-aligned mapped region, or NULL if there was an error.
static void *_mmap_aligned(void *addr, size_t n, int prot, int flags, int fd,
                           int off, size_t align) {
  char *buffer = static_cast<char*>(mmap(addr, n + align, prot, flags, fd, off));
  if (buffer == MAP_FAILED) {
    dbg_error("could not map %zu bytes with %zu alignment\n", n, align);
  }

  // find the range in the allocation that matches what we want
  uintptr_t   bits = (uintptr_t)buffer;
  uintptr_t   mask = (align - 1);
  uintptr_t suffix = bits & mask;
  uintptr_t prefix = align - suffix;

  // return the overallocated pages back to the OS, system_munmap here is fine
  // because we know our sizes are okay even for huge allocations
  if (prefix) {
    dbg_check( munmap(buffer, prefix) );
  }
  if (suffix) {
    dbg_check( munmap(buffer + prefix + n, suffix) );
  }

  // and return the correctly aligned range
  return buffer + prefix;
}

/// This mmap wrapper tries once to mmap, and then forwards to _mmap_aligned().
///
/// @param         addr A "suggested" address. This is most likely ignored.
/// @param            n The number of bytes to map, must be 2^n.
/// @param         prot The protection flags.
/// @param        flags Additional flags for the mapping.
/// @param           fd A file descriptor to map from.
/// @param          off The file offset to map at.
/// @param        align The alignment, must be 2^n.
///
/// @returns The properly-aligned mapped region, or NULL if there was an error.
static void *_mmap_lucky(void *addr, size_t n, int prot, int flags, int fd,
                         int off, size_t align) {
  void *buffer = mmap(addr, n, prot, flags, fd, off);
  if (buffer == MAP_FAILED) {
    dbg_error("could not mmap %zu bytes from file %d\n", n, fd);
  }

  uintptr_t   bits = (uintptr_t)buffer;
  uintptr_t   mask = align - 1;
  uintptr_t modulo = bits & mask;
  if (!modulo) {
    return buffer;
  }

  dbg_check(munmap(buffer, n));
  return _mmap_aligned(addr, n, prot, flags, fd, off, align);
}

void *system_mmap(void *UNUSED, void *addr, size_t n, size_t align) {
  static const  int prot = PROT_READ | PROT_WRITE;
  static const int flags = MAP_ANONYMOUS | MAP_PRIVATE;
  void *p = _mmap_lucky(addr, n, prot, flags, -1, 0, align);
  log_mem("mmap %zu bytes at %p for a total of %zu\n", n, p, _update_total(n));
  return p;
}

void *system_mmap_huge_pages(void *UNUSED, void *addr, size_t n, size_t align) {
#ifndef HAVE_HUGETLBFS
  return system_mmap(UNUSED, addr, n, align);
#else
  static const int  prot = PROT_READ | PROT_WRITE;
  static const int flags = MAP_PRIVATE;
  if (align & _hugepage_mask) {
    log_mem("increasing alignment from %zu to %ld in huge page allocation\n",
            align, _hugepage_size);
    align = _hugepage_size;
  }
  if (n & _hugepage_mask) {
    long r = n & _hugepage_mask;
    long padding = _hugepage_size - r;
    log_mem("adding %ld bytes to huge page allocation request\n", padding);
    n += padding;
  }
  void *p = NULL;
  if (_hugepage_fd >= 0) {
    p = _mmap_lucky(addr, n, prot, flags, _hugepage_fd, 0, align);
  }
  else {
    p = system_mmap(UNUSED, addr, n, align);
  }
  log_mem("mmap %zu bytes at %p from huge pages for a total of %zu\n", n, p,
          _update_total(n));
  return p;
#endif
}

void system_munmap(void *UNUSED, void *addr, size_t size) {
  int e = munmap(addr, size);
  if (e < 0) {
    dbg_error("munmap failed: %s.  addr is %p, and size is %zu\n",
          strerror(errno), addr, size);
  }
  log_mem("munmapped %zu bytes at %p for a total of %zu\n", size, addr,
          _update_total(-size));
}

void system_munmap_huge_pages(void *UNUSED, void *addr, size_t size) {
#ifdef HAVE_HUGETLBFS
  if (size & _hugepage_mask) {
    long r = size & _hugepage_mask;
    long padding = _hugepage_size - r;
    size += padding;
  }
#endif
  system_munmap(UNUSED, addr, size);
}
