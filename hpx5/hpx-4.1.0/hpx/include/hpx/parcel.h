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

#ifndef HPX_PARCEL_H
#define HPX_PARCEL_H

#ifdef __cplusplus
extern "C" {
#endif

/// @defgroup parcels Parcels
/// @brief Functions and definitions for using parcels (as distinct from actions)
/// @{

#include <stdarg.h>
#include <hpx/action.h>
#include <hpx/addr.h>
#include <hpx/builtins.h>
#include <hpx/process.h>

/// @file
/// @brief HPX parcel interface.
///
/// Parcels are the HPX message type.

/// HPX parcel type
typedef struct hpx_parcel hpx_parcel_t;

/// Acquire a parcel.
///
/// The application programmer can choose to associate an existing buffer with
/// the parcel, or can request that the parcel allocate its own buffer. Parcels
/// are runtime resources and must be released, either explicitly using
/// hpx_parcel_release() or implicitly through hpx_parcel_send() or
/// hpx_parcel_send_sync().
///
/// If the @p data pointer is NULL, then the runtime will allocate a @p bytes
/// sized buffer with the parcel that can be accessed using the
/// hpx_parcel_{get,set}_data() interface. *The parcel buffer data is not
/// initialized during this allocation routine.*
///
/// The parcel owns this buffer, and it will be deallocated when the local send
/// completes. The benefit to this mode of operation is that the runtime has the
/// opportunity to allocate an in-line buffer for the data---which can improve
/// performance, particularly for small buffers---and often results in
/// low-latency local completion. The disadvantage of this approach is that the
/// application programmer must explicitly copy data into the parcel, which
/// incurs serial overhead that may be avoided in some cases. In this mode,
/// there is no semantic difference between hpx_parcel_send() and
/// hpx_parcel_send_sync().
///
/// If the @p data pointer is NULL, and the @p bytes is 0, then
/// hpx_parcel_get_data() or hpx_parcel_set_data() may fail.
///
/// If the @p data pointer is non-NULL, then the runtime assumes that @p bytes
/// is an accurate size of @p data, and will use it as the buffer to be
/// sent. The runtime does not claim ownership of @p data, but does make it
/// available through the standard hpx_parcel_get_data()/hpx_parcel_set_data()
/// interface. The @p data buffer MAY be concurrently associated with more than
/// one parcel, however the @p data buffer MUST NOT be written to while any of
/// the parcels that it is associated with has an outstanding hpx_parcel_send()
/// operation, or concurrently with an hpx_parcel_send_sync().
///
/// @param         data A possibly NULL buffer to associate with the parcel.
/// @param        bytes The size of the @p data buffer.
///
/// @returns            A pointer to the parcel structure, or NULL on error.
hpx_parcel_t *hpx_parcel_acquire(const void *data, size_t bytes)
  HPX_MALLOC HPX_PUBLIC;

/// Prevent a parcel from being released by the system during a send operation.
///
/// Retained parcels can be useful for regular applications that resend the same
/// parcels repeatedly, and *must* be manually released using
/// hpx_parcel_release().
///
/// @param            p The parcel to retain.
void hpx_parcel_retain(hpx_parcel_t *p)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Explicitly release a parcel.
///
/// The @p p argument must correspond to a parcel pointer returned from
/// hpx_parcel_acquire().
///
/// @param            p The parcel to release.
void hpx_parcel_release(hpx_parcel_t *p)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Send a parcel with asynchronous local completion semantics.
///
/// hpx_parcel_send() has asynchronous local semantics. After returning from
/// this function, the caller must test the @p done future if it cares about
/// local completion. The @p done future may be HPX_NULL if such a test is not
/// performed---this may result in better performance.
///
/// Sending a parcel transfers ownership of the parcel to the runtime. The
/// parcel pointed to by @p p may not be reused and must not be released with
/// hpx_parcel_release().
///
/// @param            p The parcel to send, must correspond to a parcel returned
///                       from hpx_parcel_acquire().
///
/// @param        lsync The global address of an LCO to set once the send has
///                       completed locally (i.e., the parcel's buffer can be
///                       written to or freed), or HPX_NULL if the caller does
///                       not care.
///
/// @returns            HPX_SUCCESS or an error code
hpx_status_t hpx_parcel_send(hpx_parcel_t *p, hpx_addr_t lsync)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Send a parcel with synchronous local completion semantics.
///
/// hpx_parcel_send_sync() performs a synchronous local send. After returning
/// from this function, the caller is guaranteed that the local send has
/// completed.
///
/// Sending a parcel transfers ownership of the parcel to the runtime. The
/// parcel pointed to by @p p may not be reused and must not be released with
/// hpx_parcel_release().
///
/// @param            p The parcel to send, must correspond to a parcel returned
///                       from hpx_parcel_acquire().
///
/// @returns            HPX_SUCCESS or an error code
hpx_status_t hpx_parcel_send_sync(hpx_parcel_t *p)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Send a parcel "through" an LCO.
///
/// This treats the LCO as a gate that will delay the parcel until it has been
/// triggered. The @p lsync LCO will be signaled when this send operation has
/// been completed locally.
///
/// @param            p The parcel to send, must correspond to a parcel returned
///                       from hpx_parcel_acquire().
///
/// @param         gate The LCO that will serve as the gate, must be
///                       non-HPX_NULL.
///
/// @param        lsync The global address of an LCO to set once the send has
///                       completed locally (i.e., the parcel's buffer can be
///                       written to or freed), or HPX_NULL if the caller does
///                       not care.
///
/// @returns            HPX_SUCCESS or an error code.
hpx_status_t hpx_parcel_send_through(hpx_parcel_t *p, hpx_addr_t gate,
                                     hpx_addr_t lsync)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Send a parcel "through" an LCO.
///
/// This treats the LCO as a gate that will delay the parcel until it has been
/// triggered. This will not successfully return before the buffered data can be
/// reused. The remote operation is still asynchronous, and the sender can chose
/// to wait until the parcel has arrived at the LCO.
///
/// @param            p The parcel to send, must correspond to a parcel returned
///                       from hpx_parcel_acquire().
///
/// @param          lco The LCO that will serve as the gate, must be
///                       non-HPX_NULL.
///
/// @returns            HPX_SUCCESS or an error code.
hpx_status_t hpx_parcel_send_through_sync(hpx_parcel_t *p, hpx_addr_t lco)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Get the action associated with a parcel.
///
/// @param            p The parcel to query.
///
/// @returns            The action associated with @p p.
hpx_action_t hpx_parcel_get_action(const hpx_parcel_t *p)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Get the target address of a parcel.
///
/// @param            p The parcel to query.
///
/// @returns            The global address of the target of @p p.
hpx_addr_t hpx_parcel_get_target(const hpx_parcel_t *p)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Get the continuation action of a parcel.
///
/// @param            p The parcel to query.
///
/// @returns            The continuation action of @p p.
hpx_action_t hpx_parcel_get_cont_action(const hpx_parcel_t *p)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Get the address of the continuation associated with a parcel.
///
/// @param            p The parcel to query.
///
/// @returns            The global address of the continuation of @p p.
hpx_addr_t hpx_parcel_get_cont_target(const hpx_parcel_t *p)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Get the data buffer for a parcel.
///
/// The data for a parcel can be written to directly, which in some cases may
/// allow one to avoid an extra copy.
///
/// @param            p The parcel to query.
///
/// @returns            A pointer to the data buffer for the parcel.
void *hpx_parcel_get_data(hpx_parcel_t *p)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Get the size of the data buffer for a parcel.
///
/// @param            p The parcel to query.
///
/// @returns            The size of the data buffer for the parcel.
size_t hpx_parcel_get_size(const hpx_parcel_t *p)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Get the process ID for a parcel.
///
/// @param            p The parcel to query.
///
/// @returns            The process identifier of the process that the parcel @p
///                     p belongs to.
hpx_pid_t hpx_parcel_get_pid(const hpx_parcel_t *p)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Set the action for a parcel.
///
/// @param            p The parcel we're updating.
/// @param       action The action to be invoked when the parcel arrives at its
///                     target.
void hpx_parcel_set_action(hpx_parcel_t *p, hpx_action_t action)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Set a target address for a parcel.
///
/// @param            p The parcel we're updating.
/// @param         addr The global address of the target to send the parcel to.
void hpx_parcel_set_target(hpx_parcel_t *p, hpx_addr_t addr)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Set the continuation action for a parcel.
///
/// @param            p The parcel we're updating.
/// @param       action The continuation action to set for the parcel.
void hpx_parcel_set_cont_action(hpx_parcel_t *p, hpx_action_t action)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Set the continuation address for a parcel.
///
/// @param            p The parcel we're updating.
/// @param         addr The global address of the continuation.
void hpx_parcel_set_cont_target(hpx_parcel_t *p, hpx_addr_t addr)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Set the data buffer for a parcel.
///
/// The data will be copied (shallowly) into the parcel. When possible, it is
/// preferable to use hpx_parcel_get_data() and write to the parcel directly,
/// but hpx_parcel_set_data() may be used when a copy is unavoidable anyway.
///
/// @param            p The parcel we're updating.
/// @param         data The data buffer to copy into the parcel.
/// @param         size The size of the @p data buffer.
void hpx_parcel_set_data(hpx_parcel_t *p, const void *data, size_t size)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// Set the data buffer for a parcel using the given arguments.
///
/// @param            p The parcel we're updating.
/// @param        nargs The number of arguments.
/// @param         ...  Variadic arguments to serialize into the
///                     parcel buffer.
void   _hpx_parcel_set_args(hpx_parcel_t *p, int nargs, ...)
  HPX_NON_NULL(1) HPX_PUBLIC;
#define hpx_parcel_set_args(p, ...)                                     \
  _hpx_parcel_set_args(p, __HPX_NARGS(__VA_ARGS__), __VA_ARGS__)

/// Set the PID for a parcel
///
/// @param            p The parcel.
/// @param          pid The process identifier (PID) to associate with this
///                     parcel.
void hpx_parcel_set_pid(hpx_parcel_t *p, const hpx_pid_t pid)
  HPX_NON_NULL(1) HPX_PUBLIC;

/// @}

#ifdef __cplusplus
}
#endif

#endif
