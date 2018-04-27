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

#ifndef LIBHPX_LIBHPX_H
#define LIBHPX_LIBHPX_H

#include <errno.h>
#include <stdbool.h>
#include <hpx/hpx.h>
#include <libhpx/config.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  LIBHPX_ENOMEM = -(ENOMEM),
  LIBHPX_EINVAL = -(EINVAL),
  LIBHPX_ERROR = -2,
  LIBHPX_EUNIMPLEMENTED = -1,
  LIBHPX_OK = 0,
  LIBHPX_RETRY
};

/// Forward declare the libhpx configuration type.
/// @{
struct config;
typedef struct config libhpx_config_t;
/// @}

/// Get the current configuration.
const libhpx_config_t *libhpx_get_config(void)
  HPX_PUBLIC;

/// Print the version of libhpx to stdout.
void libhpx_print_version(void)
  HPX_PUBLIC;

#define LIBHPX_MAJOR 0
#define LIBHPX_MINOR 1
#define LIBHPX_PATCH 2

/// Get the current version.
///
/// The returned version can be inspected using the LIBHPX_VERSION indeces.
///
///   int version[3];
///   libhpx_get_version(version);
///
///   printf("major version %d\n, version[LIBHPX_MAJOR]);
///   printf("minor version %d\n, version[LIBHPX_MINOR]);
///   printf("patch version %d\n, version[LIBHPX_PATCH]);
///
/// @param[out]  version The output version.
void libhpx_get_version(int version[3])
  HPX_PUBLIC;

/// Start the instramentation infrastructure
void libhpx_inst_phase_begin()
  HPX_PUBLIC;

/// Stop the instramentation infrastructure
void libhpx_inst_phase_end()
  HPX_PUBLIC;

/// Check if the tracer is currently active
bool libhpx_inst_tracer_active()
  HPX_PUBLIC;

/// Get libhpx-specific parcel data.
uint32_t libhpx_parcel_get_source(hpx_parcel_t*)
  HPX_PUBLIC;

/// Traditional condition synchronization with HPX lightweight threads.
typedef short libhpx_mutex_t;

#define LIBHPX_MUTEX_INITIALIZER {0}

HPX_PUBLIC int libhpx_mutex_init(libhpx_mutex_t* mutex);
HPX_PUBLIC int libhpx_mutex_destroy(libhpx_mutex_t* mutex);
HPX_PUBLIC int libhpx_mutex_lock(libhpx_mutex_t* mutex);
HPX_PUBLIC int libhpx_mutex_unlock(libhpx_mutex_t* mutex);

typedef void* libhpx_cond_t;

#define LIBHPX_COND_INITIALIZER {NULL}

HPX_PUBLIC int libhpx_cond_init(libhpx_cond_t* cond);
HPX_PUBLIC int libhpx_cond_destroy(libhpx_cond_t* cond);
HPX_PUBLIC int libhpx_cond_wait(libhpx_cond_t* cond, libhpx_mutex_t* mutex);
HPX_PUBLIC int libhpx_cond_when(libhpx_cond_t* cond, const hpx_parcel_t* p);
HPX_PUBLIC int libhpx_cond_signal(libhpx_cond_t* cond);
HPX_PUBLIC int libhpx_cond_broadcast(libhpx_cond_t* cond);
HPX_PUBLIC int libhpx_cond_is_empty(const libhpx_cond_t* cond);

typedef void (*CallbackType)(void);
/// Register a callback function for the start of a worker thread
int libhpx_register_begin_callback(CallbackType callback);
/// Register a callback function before a worker switches lightweight thread
int libhpx_register_before_transfer_callback(CallbackType callback);
/// Register a callback function after a worker switches lightweight thread
int libhpx_register_after_transfer_callback(CallbackType callback);

#ifdef __cplusplus
}
#endif
#endif  // LIBHPX_LIBHPX_H
