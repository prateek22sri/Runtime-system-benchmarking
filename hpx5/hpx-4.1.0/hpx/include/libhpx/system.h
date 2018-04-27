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

#ifndef LIBHPX_SYSTEM_H
#define LIBHPX_SYSTEM_H

#include <pthread.h>
#include <libhpx/config.h>
#include <hpx/attributes.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Get the current program name without using argv.
///
/// @returns            A string with the program name.
const char *system_get_program_name(void);

/// Set the affinity of worker thread with @p id according to the
/// globally specified thread affinity @p policy.
int system_set_worker_affinity(int id, libhpx_thread_affinity_t policy);

/// Get the number of available cores we can run on.
int system_get_available_cores(void);

/// Get the pthread's stack extent.
///
/// @param       thread The thread id to query.
/// @param[out]    base The bottom (lowest address) of the stack.
/// @param[out]    size The size of the stack.
void system_get_stack(pthread_t thread, void **base, size_t *size);

/// An abstract interface to mmap-like operations.
///
/// As opposed to mmap, this guarantees alignment. It will try and place the
/// corresponding allocation at @p addr, but it won't try too hard.
///
/// @param          obj User data to match the object oriented mmap interface.
/// @param         addr A hint about where to try and place the allocation.
/// @param        bytes The size in bytes of the allocation (must be 2^n).
/// @param        align The alignment in bytes of the allocation (must be 2^n).
///
/// @returns The allocated region.
void *system_mmap(void *obj, void *addr, size_t bytes, size_t align);

typedef void *(*system_mmap_t)(void *, void *, size_t, size_t);

/// An abstract interface to mmap-like operations for huge pages.
///
/// As opposed to mmap, this guarantees alignment. It will try and place the
/// corresponding allocation at @p addr, but it won't try too hard.
///
/// @param          obj User data to match the object oriented mmap interface.
/// @param         addr A hint about where to try and place the allocation.
/// @param        bytes The size in bytes of the allocation (must be 2^n).
/// @param        align The alignment in bytes of the allocation (must be 2^n).
///
/// @returns The allocated region.
void *system_mmap_huge_pages(void *obj, void *addr, size_t bytes, size_t align);

/// Unmap memory.
void system_munmap(void *obj, void *addr, size_t size);

/// Unmap memory from a huge page allocation.
void system_munmap_huge_pages(void *obj, void *addr, size_t size);

typedef void (*system_munmap_t)(void *, void *, size_t);

/// Sleep for microseconds.
void system_usleep(size_t useconds);

/// Print a stack trace.
void system_print_trace(void *fd);

/// Cross-platform pthread barrier interface

#if defined(__linux)
#define system_barrierattr_t    pthread_barrierattr_t
#define system_barrier_t        pthread_barrier_t

#define system_barrier_init     pthread_barrier_init
#define system_barrier_destroy  pthread_barrier_destroy
#define system_barrier_wait     pthread_barrier_wait

#elif defined(__APPLE__)
typedef int system_barrierattr_t;
typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int count;
  int trip_count;
} system_barrier_t;

/// System specific thread barrier init function
int system_barrier_init(system_barrier_t *barrier,
                        const system_barrierattr_t *attr, unsigned int count);

/// System specific thread barrier destroy function
int system_barrier_destroy(system_barrier_t *barrier);

/// System specific thread barrier wait function
int system_barrier_wait(system_barrier_t *barrier);

#endif

#ifdef __cplusplus
}
#endif

#endif // LIBHPX_SYSTEM_H
