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

#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <sys/mman.h>
#include <libhpx/debug.h>
#include <libhpx/system.h>

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
  char *buffer = (char*)mmap(addr, n + align, prot, flags, fd, off);
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
    system_munmap(NULL, buffer, prefix);
  }
  if (suffix) {
    system_munmap(NULL, buffer + prefix + n, suffix);
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

  system_munmap(NULL, buffer, n);
  return _mmap_aligned(addr, n, prot, flags, fd, off, align);
}

void *system_mmap(void *UNUSED, void *addr, size_t n, size_t align) {
  static const  int prot = PROT_READ | PROT_WRITE;
  static const int flags = MAP_ANON | MAP_PRIVATE;
  return _mmap_lucky(addr, n, prot, flags, -1, 0, align);
}

void *system_mmap_huge_pages(void *UNUSED, void *addr, size_t n, size_t align) {
  return system_mmap(UNUSED, addr, n, align);
}

void system_munmap(void *UNUSED, void *addr, size_t size) {
  int e = munmap(addr, size);
  if (e < 0) {
    dbg_error("munmap failed: %s.  addr is %" PRIuPTR", and size is %zu\n",
          strerror(errno), (uintptr_t)addr, size);
  }
}

void system_munmap_huge_pages(void *UNUSED, void *addr, size_t size) {
  system_munmap(UNUSED, addr, size);
}
