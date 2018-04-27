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

#ifndef LIBHPX_MEMORY_H
#define LIBHPX_MEMORY_H

/// @file include/libhpx/hpx.h
/// @brief Address spaces.
///
/// This header defines the interface to the various kinds of memory that we
/// allocate. In addition to standard local memory, we can allocate network
/// registered memory, global memory, and global cyclic memory.

#include <libhpx/events.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  AS_REGISTERED = 0,
  AS_GLOBAL,
  AS_CYCLIC,
  AS_COUNT
};

#ifndef HAVE_NETWORK

/// If we don't have a network configured, then we don't do anything interesting
/// with memory. Our interface just forwards inline to the stdlib
/// allocators. This works fine, even if the user has configured
/// --enable-jemalloc, since jemalloc is handling the stdlib allocation in that
/// context.

# include <stdlib.h>
#include <libhpx/debug.h>
static inline void as_join(int id) {
}

static inline void as_leave(void) {
}

static inline void as_thread_init(void) {
}

static inline void *as_malloc(int id, size_t bytes) {
  return malloc(bytes);
}

static inline void *as_calloc(int id, size_t nmemb, size_t bytes) {
  return calloc(nmemb, bytes);
}

static inline void *as_memalign(int id, size_t boundary, size_t size) {
  void *ptr = NULL;
  dbg_check(posix_memalign(&ptr, boundary, size));
  return ptr;
}

static inline void as_free(int id, void *ptr) {
  free(ptr);
}

#elif defined(HAVE_JEMALLOC)

/// When a network has been configured then we actually need to do something
/// about these allocations. In this context we are guaranteed that the jemalloc
/// header is available, so we can use its types here.
///
/// The JEMALLOC_NO_DEMANGLE flag exposes jemalloc's je_-prefixed symbols so
/// that we can use them here. These are necessary because we prefix the
/// jemalloc symbols with --disable-jemalloc, so we don't inadvertently use
/// jemalloc's malloc/free functions if the user hasn't asked us to. The je_
/// prefixes deal with this, and will always "point" to the symbols that
/// jemalloc is actually exposing.
#define JEMALLOC_NO_DEMANGLE
#include <jemalloc/jemalloc.h>
#undef JEMALLOC_NO_DEMANGLE

/// Each thread "joins" the custom address space world by figuring out what
/// flags to pass to jemalloc for each address space, and storing them in this
/// array.
/// @{
extern __thread int as_flags[AS_COUNT];
/// @}

/// Set the allocator for an address space.
///
/// The address space does *not* take ownership of the allocator, and does not
/// try to free it at termination.
///
/// The default allocator is the local allocator, if no custom allocator is set
/// for an address space then it will use the local address space to satisfy
/// allocations, e.g., without a custom allocator as_malloc(ID, n) forwards to
/// malloc(n).
///
/// @param           id The address space id to update.
/// @param        hooks An allocator implementation.
void as_set_allocator(int id, const chunk_hooks_t *hooks);

/// Call by each thread to join the memory system.
///
/// After calling as_join(), threads can use the as_* allocation interface. This
/// must be called *after* any custom allocator has been installed.
void as_join(int id);

/// Called by each thread to leave the memory system.
///
/// This will flush any caches that have been set. It does not to anything to
/// the backing arenas since we don't know where that memory is being
/// used. Jemalloc may purge those arenas to reclaim the backing memory, and
/// they will be cleaned up at shutdown.
///
/// The main consequence of not freeing the arenas is that global address
/// regions can not be returned to the global bitmap for use
/// elsewhere. Essentially the arena will hold onto its chunks until the end of
/// time.
void as_leave(void);

/// Get the number of bytes associated with a "chunk" of memory, in an
/// allocator-independent way.
size_t as_bytes_per_chunk(void);

void *as_malloc(int id, size_t bytes);
void *as_calloc(int id, size_t nmemb, size_t bytes);
void *as_memalign(int id, size_t boundary, size_t size);
void as_free(int id, void *ptr);

/// Generic null hooks for events that we don't generally handle.
/// @{
bool as_null_commit(void *chunk, size_t size, size_t offset, size_t length,
                    unsigned arena_ind);

bool as_null_decommit(void *chunk, size_t size, size_t offset, size_t length,
                      unsigned arena_ind);

bool as_null_purge(void *chunk, size_t size, size_t offset, size_t length,
                   unsigned arena_ind);

bool as_null_split(void *chunk, size_t size, size_t size_a, size_t size_b,
                   bool committed, unsigned arena_ind);

bool as_null_merge(void *chunk_a, size_t size_a, void *chunk_b, size_t size_b,
                   bool committed, unsigned arena_ind);
/// @}

#elif defined(HAVE_TBBMALLOC)

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

extern void* pools[AS_COUNT];

void as_join(int id);
void as_leave(void);
size_t as_bytes_per_chunk(void);

void *as_malloc(int id, size_t bytes);
void *as_calloc(int id, size_t nmemb, size_t bytes);
void *as_memalign(int id, size_t boundary, size_t size);
void as_free(int id, void *ptr);

#ifdef __cplusplus
}
#endif

#elif defined(HAVE_DLMALLOC)

#include <malloc-2.8.6.h>

extern mspace mspaces[AS_COUNT];

void as_join(int id);
void as_leave(void);
size_t as_bytes_per_chunk(void);

void *as_malloc(int id, size_t bytes);
void *as_calloc(int id, size_t nmemb, size_t bytes);
void *as_memalign(int id, size_t boundary, size_t size);
void as_free(int id, void *ptr);

#else
# error No gas allocator configured
#endif



static inline void *registered_malloc(size_t bytes) {
  EVENT_MEMORY_ALLOC_BEGIN(AS_REGISTERED, bytes, 0);
  void *ptr = as_malloc(AS_REGISTERED, bytes);
  EVENT_MEMORY_ALLOC_END(AS_REGISTERED, (uintptr_t)ptr);
  return ptr;
}

static inline void *registered_calloc(size_t nmemb, size_t bytes) {
  EVENT_MEMORY_ALLOC_BEGIN(AS_REGISTERED, nmemb * bytes, 0);
  void *ptr = as_calloc(AS_REGISTERED, nmemb, bytes);
  EVENT_MEMORY_ALLOC_END(AS_REGISTERED, (uintptr_t)ptr);
  return ptr;
}

static inline void *registered_memalign(size_t boundary, size_t size) {
  EVENT_MEMORY_ALLOC_BEGIN(AS_REGISTERED, size, boundary);
  void *ptr = as_memalign(AS_REGISTERED, boundary, size);
  EVENT_MEMORY_ALLOC_END(AS_REGISTERED, (uintptr_t)ptr);
  return ptr;
}

static inline void registered_free(void *ptr)  {
  EVENT_MEMORY_FREE_BEGIN(AS_REGISTERED, (uintptr_t)ptr);
  as_free(AS_REGISTERED, ptr);
  EVENT_MEMORY_FREE_END(AS_REGISTERED);
}

static inline void *global_malloc(size_t bytes) {
  EVENT_MEMORY_ALLOC_BEGIN(AS_GLOBAL, bytes, 0);
  void *ptr = as_malloc(AS_GLOBAL, bytes);
  EVENT_MEMORY_ALLOC_END(AS_GLOBAL, (uintptr_t)ptr);
  return ptr;
}

static inline void *global_calloc(size_t nmemb, size_t bytes) {
  EVENT_MEMORY_ALLOC_BEGIN(AS_GLOBAL, nmemb * bytes, 0);
  void *ptr = as_calloc(AS_GLOBAL, nmemb, bytes);
  EVENT_MEMORY_ALLOC_END(AS_GLOBAL, (uintptr_t)ptr);
  return ptr;
}

static inline void *global_memalign(size_t boundary, size_t size) {
  EVENT_MEMORY_ALLOC_BEGIN(AS_GLOBAL, size, boundary);
  void *ptr = as_memalign(AS_GLOBAL, boundary, size);
  EVENT_MEMORY_ALLOC_END(AS_GLOBAL, (uintptr_t)ptr);
  return ptr;
}

static inline void global_free(void *ptr)  {
  EVENT_MEMORY_FREE_BEGIN(AS_GLOBAL, (uintptr_t)ptr);
  as_free(AS_GLOBAL, ptr);
  EVENT_MEMORY_FREE_END(AS_GLOBAL);
}

static inline void *cyclic_malloc(size_t bytes) {
  EVENT_MEMORY_ALLOC_BEGIN(AS_CYCLIC, bytes, 0);
  void *ptr = as_malloc(AS_CYCLIC, bytes);
  EVENT_MEMORY_ALLOC_END(AS_CYCLIC, (uintptr_t)ptr);
  return ptr;
}

static inline void *cyclic_calloc(size_t nmemb, size_t bytes) {
  EVENT_MEMORY_ALLOC_BEGIN(AS_CYCLIC, nmemb * bytes, 0);
  void *ptr = as_calloc(AS_CYCLIC, nmemb, bytes);
  EVENT_MEMORY_ALLOC_END(AS_CYCLIC, (uintptr_t)ptr);
  return ptr;
}

static inline void *cyclic_memalign(size_t boundary, size_t size) {
  EVENT_MEMORY_ALLOC_BEGIN(AS_CYCLIC, size, boundary);
  void *ptr = as_memalign(AS_CYCLIC, boundary, size);
  EVENT_MEMORY_ALLOC_END(AS_CYCLIC, (uintptr_t)ptr);
  return ptr;
}

static inline void cyclic_free(void *ptr)  {
  EVENT_MEMORY_FREE_BEGIN(AS_CYCLIC, (uintptr_t)ptr);
  as_free(AS_CYCLIC, ptr);
  EVENT_MEMORY_FREE_END(AS_CYCLIC);
}

#ifdef __cplusplus
}
#endif

#endif // LIBHPX_MEMORY_H
