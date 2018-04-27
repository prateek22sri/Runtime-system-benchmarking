// ================================================================= -*- C++ -*-
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

#ifndef HPX_CXX_RUNTIME_H
#define HPX_CXX_RUNTIME_H

#include <hpx/runtime.h>

namespace hpx {
/// @defgroup system System
/// @brief Functions that control the overall runtime
/// @{

/// @file
/// @brief HPX system interface.
/// init() initializes the scheduler, network, and locality
///
/// run() is called from the native thread after init() and action
/// registration is complete, in order
///
/// abort() is called from an HPX lightweight thread to terminate scheduler
/// execution asynchronously
///
/// exit() is called from an HPX lightweight thread to terminate
/// scheduler execution

/// Initializes the HPX runtime.
///
/// This must be called before other HPX functions.  init() initializes the
/// scheduler, network, and locality and should be called before any other HPX
/// functions.
///
/// @param argc   count of command-line arguments
/// @param argv   array of command-line arguments
/// @returns      HPX_SUCCESS on success
inline int init(int *argc, char ***argv) {
  return hpx_init(argc, argv);
}

/// Finalize/cleanup from the HPX runtime.
///
/// This function will remove almost all data structures and allocations, and
/// will finalize the underlying network implementation. Note that run()
/// must never be called after finalize().
inline void finalize() {
  hpx_finalize();
}

/// Start the HPX runtime, and run a given action.
///
/// This creates an HPX "main" process, and calls the given action @p
/// entry in the context of this process. The @p entry action is
/// invoked only on the root locality. On termination, it deletes the
/// main process and returns the status returned by exit()
///
/// run finalizes action registration, starts up any scheduler and
/// native threads that need to run, and transfers all control into
/// the HPX scheduler, beginning execution of the top action in the
/// scheduler queue.
///
/// The scheduler queue could be empty, in which case the entire
/// scheduler instance is running, waiting for a successful
/// inter-locality steal operation (if that is implemented) or a
/// network parcel.
///
/// @param entry an action to execute, or HPX_ACTION NULL to wait for an
///              incoming parcel or a inter-locality steal (if implemented)
/// @param  args arguments to pass to @p entry
/// @returns     the status code passed to exit() upon
///              termination.
template <typename Act, typename... Args>
inline int run(Act &action, Args &&... args) {
  return action.run(std::forward<Args>(args)...);
}

/// Exit the HPX runtime.
///
/// This causes the run() in the main native thread to return the @p code.
/// The returned thread is executing the original native thread, and all
/// supplementary scheduler threads and network will have been shutdown, and any
/// library resources will have been cleaned up.
///
/// This call is cooperative and synchronous, so it may not return if there are
/// misbehaving HPX lightweight threads.
///
/// It is safe to call run() again after exit().
template <typename T>
inline void HPX_NORETURN exit(const T& out) {
  hpx_exit(sizeof(out), &out);
}

inline void HPX_NORETURN exit() {
  hpx_exit(0, NULL);
}


/// Abort the HPX runtime.
///
/// This causes the main native thread to return the @p code from run(). The
/// returned thread is executing the original native thread, and all
/// supplementary scheduler threads an network will ahve been shutdown.
///
/// This is not cooperative, and will not clean up any resources. Furthermore,
/// the state of the system after the return is not well defined. The
/// application's main native thread should only rely on the async-safe
/// interface provided in signal(7).
inline void HPX_NORETURN abort(void) {
  hpx_abort();
}

/// Print the help string associated with the runtime configuration
/// options supported by the HPX runtime.
///
inline void print_help(void) {
  hpx_print_help();
}

/// Print the version string associated with the HPX interface implemented by
/// the runtime.
inline void print_version(void) {
  hpx_print_version();
}

/// @}
} // namespace hpx

#endif // HPX_CXX_RUNTIME_H
