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

#ifndef HPX_TOPOLOGY_H
#define HPX_TOPOLOGY_H

#ifdef __cplusplus
extern "C" {
#endif

/// @defgroup other Other
/// @brief Miscellaneous support functions and definitions
/// @{

/// @file include/hpx/topology.h
/// @brief HPX topology interface

/// Get the "rank" the current code is running on.
///
/// @returns            The rank at which the current code is executing, or -1
///                     if the runtime is not yet active.
int hpx_get_my_rank(void)
  HPX_PUBLIC;

/// Get the number of ranks currently available.
///
/// @returns            The number of ranks in the system, or -1 if the runtime
///                     is not yet active.
int hpx_get_num_ranks(void)
  HPX_PUBLIC;

/// Get the number of heavy-weight threads at the current locality.
///
/// These threads are the system threads used internally by HPX and are not the
/// same as HPX threads.
///
/// @returns            The number of system threads at the current locality, or
///                     -1 if the runtime is not fully active yet.
int hpx_get_num_threads(void)
  HPX_PUBLIC;

/// Check if HPX is inside of a run epoch.
///
/// Useful to check if particular code is inside a hpx_run.
///
/// @returns            1 if hpx is active, 0 otherwise.
int hpx_is_active(void)
  HPX_PUBLIC;

/// Get the thread id of the current system thread.
///
/// If you need an id for a light-weight HPX thread use
/// hpx_thread_get_tls_id().
///
/// @returns            An dense id representing the current system thread, or
///                     -1 if the HPX system is not yet fully initialized.
int hpx_get_my_thread_id(void)
  HPX_PUBLIC;

/// @copydoc hpx_get_my_rank()
#define HPX_LOCALITY_ID hpx_get_my_rank()

/// @copydoc hpx_get_num_ranks()
#define HPX_LOCALITIES hpx_get_num_ranks()

/// @copydoc hpx_get_num_threads()
#define HPX_THREADS hpx_get_num_threads()

/// @copydoc hpx_get_my_thread_id()
#define HPX_THREAD_ID hpx_get_my_thread_id()

/// @}

#ifdef __cplusplus
}
#endif

#endif
