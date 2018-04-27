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

#ifndef HPX_THREAD_H
#define HPX_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup actions
/// @{

/// @file
/// @brief HPX thread interface
///
/// HPX threads are spawned as a result of hpx_parcel_send() or
/// hpx_parcel_send_sync(), or as a result of an hpx_call() (or variant) which
/// which relies on an implicit parcel send. They may return values to their
/// LCO continuations using the hpx_thread_exit() call, which terminates the
/// thread's execution.

/// Forward declare the parcel type.
/// @{
struct hpx_parcel;
/// @}

/// Get the current thread's progenitor parcel.
///
/// Every thread in HPX is created by a parcel. This allows users to get access
/// to that underlying parcel if they need it.
///
/// @returns            The current progenitor parcel, or NULL if HPX is not
///                     currently running.
const struct hpx_parcel *hpx_thread_current_parcel(void)
  HPX_PUBLIC;

/// Get the target of the current thread.
/// The target of the thread is the destination that a parcel was sent to
/// to spawn the current thread.
///
/// @returns the global address of the thread's target
hpx_addr_t hpx_thread_current_target(void)
  HPX_PUBLIC;

/// Get the action that the current thread is executing.
///
/// @return             The action ID of the current thread.
hpx_action_t hpx_thread_current_action(void)
  HPX_PUBLIC;

/// Get the address of the continuation for the current thread
///
/// @return             The address of the current thread's continuation.
hpx_addr_t hpx_thread_current_cont_target(void)
  HPX_PUBLIC;

/// Get the continuation action for the current thread.
///
/// @return             The continuation action for the current thread.
hpx_action_t hpx_thread_current_cont_action(void)
  HPX_PUBLIC;

/// Get the process identifier of the current thread
///
/// @return             The PID for the current thread.
hpx_pid_t hpx_thread_current_pid(void)
  HPX_PUBLIC;

/// Pause execution and gives other threads the opportunity to be scheduled.
///
/// This call is *not* a runtime hint. The runtime *must* guarantee progress for
/// non-yielding threads given a bounded number of threads in infinite yield
/// loops.
void hpx_thread_yield(void)
  HPX_PUBLIC;

/// Generates a consecutive new ID for a thread.
///
/// The first time this is called in a lightweight thread, it assigns the thread
/// the next available ID. Each time it's called after that it returns that same
/// id.
///
/// @return             < 0 if there is an error, otherwise a unique, compact id
///                     for the calling lightweight thread.
int hpx_thread_get_tls_id(void)
  HPX_PUBLIC;

/// Check to see if the current thread has enough space for an alloca.
///
/// @param        bytes The number of bytes to allocate.
///
/// @return             The number of bytes remaining on the stack after the
///                     alloca.
intptr_t hpx_thread_can_alloca(size_t bytes)
  HPX_PUBLIC;

/// Initiate the current thread's continuation.
///
/// This logically terminates the lightweight HPX thread, however it will
/// continue to run until it returns from the main action. During this
/// post-continue phase threads can clean up local resources, unpin pinned
/// buffers, and perform some HPX operations, however applications must not
/// infer an order between the continued action and the cleanup phase in the
/// current thread.
///
/// Threads must not continue twice.
///
/// @param      n The number of arguments being passed in.
/// @param    ... The arguments to continue (same as hpx_call)
///
/// @return       A status code, suitable for return from the main action.
int _hpx_thread_continue(int n, ...)
  HPX_PUBLIC;

#define hpx_thread_continue(...)                                        \
  _hpx_thread_continue(__HPX_NARGS(__VA_ARGS__) , ##__VA_ARGS__)

/// Finish the current thread's execution, sending @p v to the thread's
/// continuation address
/// @param v the value to be sent to the thread's continuation
#define HPX_THREAD_CONTINUE(v) hpx_thread_continue(&v, sizeof(v))

/// Generate the thread's continuation parcel explicitly.
///
/// This will allocate and pack a thread's continuation parcel explicitly. The
/// user is responsible for launching the parcel before the thread
/// terminates. Failure to do so is a programming error that cannot be tracked
/// by HPX.
///
/// This does not logically terminate the current thread, and may be less
/// efficient than explicitly continuing a value. The main use case for this
/// interface is if the amount of data to be sent to the continuation is large
/// and needs to be processed. This interface can eliminate an allocation and/or
/// memcpy in that context.
///
/// @note This is an advanced interface and should be used with care.
///
/// @param            n The number of parameters.
/// @param          ... The parameters to pack the parcel with.
///
/// @returns            A parcel suitable for use as the continuation, or NULL
///                     if there was an error.
struct hpx_parcel *_hpx_thread_generate_continuation(int n, ...)
  HPX_PUBLIC;

#define hpx_thread_generate_continuation(...) \
  _hpx_thread_generate_continuation(__HPX_NARGS(__VA_ARGS__) , ##__VA_ARGS__)

/// Capture the current thread's continuation as a parcel.
///
/// This allows users to implement call-with-current-continuation semantics
/// using the parcel transport mechanism. It creates a new parcel in the same
/// way as the hpx_parcel_acquire() interface and gives ownership of the current
/// thread's continuation to that parcel.
///
/// The user should set the target and action for the returned parcel, and set
/// any argument data explicitly (if it is not passed through the *p buffer
/// argument). The user must send the returned parcel, failure to do so may lead
/// to deadlock and cannot be tracked by the implementation.
///
/// * Use of the _hpx_thread_continue() interface after capturing a continuation
///   leads to undefined behavior.
/// * Use of the _hpx_thread_generate_continuation() after this interface leads
///   to undefined behavior.
/// * Subsequent uses of this interface are equivalent to hpx_parcel_acquire().
///
/// @param       buffer A buffer for the new parcel (see hpx_parcel_acquire()).
/// @param        bytes The number of bytes for the new parcel (see
///                     hpx_parcel_acquire()).
///
/// @returns            A parcel suitable for use in the parcel API, that has
///                     ownership of the current continuation.
struct hpx_parcel *hpx_thread_capture_continuation(const void* buffer,
                                                   size_t bytes)
  HPX_PUBLIC;

/// Finish the current thread's execution.
///
/// The behavior of this call depends on the @p status parameter, and is
/// semantically equivalent to returning @p status from the action. Due to the
/// non-local control nature of this routine it may be much more expensive than
/// simply returning the status code from the main action handler, and should be
/// avoided when possible.
///
/// @note This routine invokes a non-local control transfer, however the runtime
///       must correctly unwind the stack for languages that require such
///       behavior (e.g., C++).
///
/// Possible status codes:
///
///         HPX_SUCCESS: Normal termination, send a parcel with 0-sized data to
///                      the thread's continuation address.
///
///           HPX_ERROR: Abnormal termination.
///
///          HPX_RESEND: Terminate execution, and resend the thread's parcel
///                      (NOT the continuation parcel). This can be used for
///                      application-level forwarding when hpx_addr_try_pin()
///                      fails.
///
///   HPX_LCO_EXCEPTION: Continue an exception to the continuation address.
///
/// @param status a status to be returned to the function that created this
///        thread
void hpx_thread_exit(int status)
  HPX_PUBLIC HPX_NORETURN;

#define HPX_SIGNONE     (INT32_C(0))
#define HPX_SIGSEGV     (INT32_C(1) << 0)
#define HPX_SIGABRT     (INT32_C(1) << 1)
#define HPX_SIGFPE      (INT32_C(1) << 2)
#define HPX_SIGILL      (INT32_C(1) << 3)
#define HPX_SIGBUS      (INT32_C(1) << 4)
#define HPX_SIGIOT      (INT32_C(1) << 5)
#define HPX_SIGSYS      (INT32_C(1) << 6)
#define HPX_SIGTRAP     (INT32_C(1) << 7)

#define HPX_SIG_BLOCK   (INT32_C(0))
#define HPX_SIG_UNBLOCK (INT32_C(1))
#define HPX_SIG_SET     (INT32_C(2))

/// Update the signal mask for the current lightweight thread.
///
/// This updates the current signal mask to be @p sig, and returns the previous
/// mask. Note that currently the only signals that lightweight threads can
/// manipulate are the "program error signals" defined in
/// http://www.gnu.org/software/libc/manual/html_node/Program-Error-Signals.html
/// though in the future HPX may allow access to a larger set of signals.
///
/// Changing the signal mask will increase the overheads of lightweight thread
/// blocking, context switching, and termination for the current thread, but
/// will not impact other concurrent threads.
///
/// The @p how parameter tells HPX how to modify the signal mask.
///
///   HPX_SIG_BLOCK:   Mask the passed set.
///   HPX_SIG_UNBLOCK: Unmask the passed set.
///   HPX_SIG_SET:    Replace the existing mask with the @p mask.
///
/// @param           how One of HPX_SIG_{BLOCK, UNBLOCK, SET}.
/// @param          mask A bitset set of the signals to modify.
///
/// @returns             The previous signal mask.
int hpx_thread_sigmask(int how, int mask)
  HPX_PUBLIC;

/// @}

#ifdef __cplusplus
}
#endif

#endif
