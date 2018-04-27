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

#ifndef HPX_RPC_H
#define HPX_RPC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hpx/builtins.h>
#include <hpx/process.h>
#include <hpx/thread.h>

/// @file include/hpx/rpc.h

/// @addtogroup actions
/// @{
/// @brief HPX action call interface

/// Fully synchronous call interface.
///
/// Performs @p action on given arguments at @p addr, and sets @p out with the
/// resulting value. The output value @p out can be NULL (or the
/// corresponding @p olen could be zero), in which case no return
/// value is generated.
///
/// @param         addr The address that defines where the action is executed.
/// @param       action The action to perform.
/// @param          out Address of the output buffer.
/// @param         olen The length of the @p output buffer.
/// @param            n The number of arguments for @p action.
/// @param          ... The arguments for the call.
///
/// @returns            HPX_SUCCESS, or an error code if the action generated an
///                     error that could not be handled remotely.
int _hpx_call_sync(hpx_addr_t addr, hpx_action_t action, void *out, size_t olen,
                   int n, ...)
  HPX_PUBLIC;

/// Convenience wrapper for the fully synchronous call interface.
#define hpx_call_sync(addr, action, out, olen, ...)                   \
  _hpx_call_sync(addr, action, out, olen, __HPX_NARGS(__VA_ARGS__) ,  \
                 ##__VA_ARGS__)

/// Locally synchronous call interface.
///
/// This is a locally-synchronous, globally-asynchronous variant of
/// the remote-procedure call interface. If @p result is not HPX_NULL,
/// hpx_call puts the the resulting value in @p result at some point
/// in the future.
///
/// @param         addr The address that defines where the action is executed.
/// @param       action The action to perform.
/// @param       result An address of an LCO to trigger with the result.
/// @param            n The number of arguments for @p action.
/// @param          ... The arguments for the call.
///
/// @returns            HPX_SUCCESS, or an error code if there was a problem
///                     locally during the hpx_call invocation.
int _hpx_call(hpx_addr_t addr, hpx_action_t action, hpx_addr_t result, int n,
              ...)
  HPX_PUBLIC;

/// Convenience wrapper for the locally synchronous call interface.
#define hpx_call(addr, action, result, ...)                             \
  _hpx_call(addr, action, result, __HPX_NARGS(__VA_ARGS__) , ##__VA_ARGS__)

#define _HPX_ADDRESSOF(x) &x

/// An experimental version of hpx_call that takes parameter symbols directly.
#define hpx_xcall(addr, action, result, ...) \
  hpx_call(addr, action, result, __HPX_FOREACH(_HPX_ADDRESSOF, __VA_ARGS__))

/// Locally synchronous call interface when LCO is set.
///
/// This is a locally-synchronous, globally-asynchronous variant of
/// the remote-procedure call interface which implements the hpx_parcel_send_
/// through() function. The gate must be non-HPX_NULL.
///
/// @param         gate The LCO that will serve as the gate (not HPX_NULL).
/// @param         addr The address that defines where the action is executed.
/// @param       action The action to perform.
/// @param       result An address of an LCO to trigger with the result.
/// @param            n The number of arguments for @p action.
/// @param          ... The arguments for the call.
///
/// @returns            HPX_SUCCESS, or an error code if there was a problem
///                     locally during the hpx_call invocation.
int _hpx_call_when(hpx_addr_t gate, hpx_addr_t addr, hpx_action_t action,
                   hpx_addr_t result, int n, ...)
  HPX_PUBLIC;

/// A convenience wrapper for the locally synchronous call_when interface.
#define hpx_call_when(gate, addr, action, result, ...)                  \
  _hpx_call_when(gate, addr, action, result, __HPX_NARGS(__VA_ARGS__) , \
                 ##__VA_ARGS__)

/// Locally synchronous call_when with continuation interface.
///
/// The gate must be non-HPX_NULL.
///
/// @param         gate The LCO that will serve as the gate (not HPX_NULL).
/// @param         addr The address that defines where the action is executed.
/// @param       action The action to perform.
/// @param     c_target The address where the continuation action is executed.
/// @param     c_action The continuation action to perform.
/// @param            n The number of arguments for @p action.
/// @param          ... The arguments for the call.
///
/// @returns            HPX_SUCCESS, or an error code if there was a problem
///                     locally during the hpx_call invocation.
int _hpx_call_when_with_continuation(hpx_addr_t gate, hpx_addr_t addr,
                                     hpx_action_t action, hpx_addr_t c_target,
                                     hpx_action_t c_action, int n, ...)
  HPX_PUBLIC;

/// A convenience wrapper for locally synchronous call_when's with continuations
#define hpx_call_when_with_continuation(gate, addr, action, c_target,   \
                                        c_action, ...)                  \
  _hpx_call_when_with_continuation(gate, addr, action, c_target,        \
                                   c_action, __HPX_NARGS(__VA_ARGS__) , \
                                   ##__VA_ARGS__)

/// Fully synchronous call interface which implements hpx_parcel_send_through()
/// when an LCO is set.
///
/// Performs @p action on given arguments at @p addr, and sets @p out with the 
/// resulting value. The output value @p out can be NULL (or the corresponding 
/// @p olen could be zero), in which case no return value is generated.
///
/// The gate must be non-HPX_NULL.
///
/// @param         gate The LCO that will serve as the gate (non HPX_NULL).
/// @param         addr The address that defines where the action is executed.
/// @param       action The action to perform.
/// @param          out Address of the output buffer.
/// @param         olen The length of the @p output buffer.
/// @param            n The number of arguments for @p action.
/// @param          ... The arguments for the call.
///
/// @returns            HPX_SUCCESS, or an error code if the action generated an
///                     error that could not be handled remotely.
int _hpx_call_when_sync(hpx_addr_t gate, hpx_addr_t addr, hpx_action_t action,
                        void *out, size_t olen, int n, ...)
  HPX_PUBLIC;

/// A convenience wrapper for the fully synchronous call interface which 
/// implements hpx_parcel_send_through() when an LCO is set.
#define hpx_call_when_sync(gate, addr, action, out, olen, ...)  \
  _hpx_call_when_sync(gate, addr, action, out, olen,            \
                      __HPX_NARGS(__VA_ARGS__) , ##__VA_ARGS__)

/// Locally synchronous call with continuation interface.
///
/// This is similar to hpx_call with additional parameters to specify the
/// continuation action @p c_action to be executed at a continuation address @p
/// c_target.
///
/// @param         addr The address that defines where the action is executed.
/// @param       action The action to perform.
/// @param     c_target The address where the continuation action is executed.
/// @param     c_action The continuation action to perform.
/// @param            n The number of arguments for @p action.
/// @param          ... The arguments for the call.
///
/// @returns            HPX_SUCCESS, or an error code if there was a problem
///                     locally during the hpx_call invocation.
int _hpx_call_with_continuation(hpx_addr_t addr, hpx_action_t action,
                                hpx_addr_t c_target, hpx_action_t c_action,
                                int n, ...)
  HPX_PUBLIC;

/// A convenience wrapper for locally synchronous calls 
/// with continuation interface.
#define hpx_call_with_continuation(addr, action, c_target, c_action, ...) \
  _hpx_call_with_continuation(addr, action, c_target, c_action,           \
                              __HPX_NARGS(__VA_ARGS__) , ##__VA_ARGS__)

/// Fully asynchronous call interface.
///
/// This is a completely asynchronous variant of the remote-procedure
/// call interface. If @p result is not HPX_NULL, hpx_call puts the
/// the resulting value in @p result at some point in the future. This
/// function returns even before the argument buffer has been copied
/// and is free to reuse. If @p lsync is not HPX_NULL, it is set
/// when local operations have completed.
///
/// @param         addr The address that defines where the action is executed.
/// @param       action The action to perform.
/// @param        lsync The global address of an LCO to signal local completion
///                     (i.e., R/W access to, or free of @p args is safe),
///                     HPX_NULL if we don't care.
/// @param       result The global address of an LCO to signal with the result.
/// @param            n The number of arguments for @p action.
/// @param          ... The arguments for the call.
///
/// @returns            HPX_SUCCESS, or an error code if there was a problem
///                     locally during the hpx_call_async invocation.
int _hpx_call_async(hpx_addr_t addr, hpx_action_t action, hpx_addr_t lsync,
                    hpx_addr_t result, int n, ...)
  HPX_PUBLIC;

/// A convenience wrapper for fully asynchronous calls.
#define hpx_call_async(addr, action, lsync, result, ...)                  \
  _hpx_call_async(addr, action, lsync, result, __HPX_NARGS(__VA_ARGS__) , \
                  ##__VA_ARGS__)

/// Call with current continuation.
///
/// This calls an action passing the current thread's continuation as the
/// continuation for the called action.
///
/// The gate must be non-HPX_NULL.
///
/// @param         gate An LCO for a dependent call (must be non HPX_NULL).
/// @param         addr The address where the action is executed.
/// @param       action The action to perform.
/// @param            n The number of arguments for @p action.
/// @param          ... The arguments for the call.
///
/// @returns            HPX_SUCCESS, or an error code if there was a problem
///                     during the hpx_call_cc invocation.
int _hpx_call_when_cc(hpx_addr_t gate, hpx_addr_t addr, hpx_action_t action,
                      int n, ...)
  HPX_PUBLIC;

/// A convenience wrapper for call_when with current continuation.
#define hpx_call_when_cc(gate, addr, action, ...)                       \
  _hpx_call_when_cc(gate, addr, action, __HPX_NARGS(__VA_ARGS__), ##__VA_ARGS__)

/// Call with current continuation.
///
/// This calls an action passing the currrent thread's continuation as the
/// continuation for the called action.
///
/// @param         addr The address where the action is executed.
/// @param       action The action to perform.
/// @param            n The number of arguments for @p action.
/// @param          ... The arguments for the call.
///
/// @returns            HPX_SUCCESS, or an error code if there was a problem
///                     during the hpx_call_cc invocation.
int _hpx_call_cc(hpx_addr_t addr, hpx_action_t action, int n, ...)
  HPX_PUBLIC;

/// A convenience wrapper for call with current continuation.
#define hpx_call_cc(addr, action, ...)                                  \
  _hpx_call_cc(addr, action, __HPX_NARGS(__VA_ARGS__), ##__VA_ARGS__)

/// A convenience wrapper for an HPX process broadcast.
#define hpx_bcast(action, lsync, rsync, ...)                            \
  _hpx_process_broadcast(hpx_thread_current_pid(), action, lsync, rsync, \
                         __HPX_NARGS(__VA_ARGS__) , ##__VA_ARGS__)

/// A convenience wrapper for a locally synchronous HPX process broadcast.
#define hpx_bcast_lsync(action, rsync, ...)                             \
  _hpx_process_broadcast_lsync(hpx_thread_current_pid(), action, rsync, \
                               __HPX_NARGS(__VA_ARGS__) , ##__VA_ARGS__)

/// A convenience wrapper for a fully synchronous HPX process broadcast.
#define hpx_bcast_rsync(action, ...)                                    \
  _hpx_process_broadcast_rsync(hpx_thread_current_pid(), action,        \
                               __HPX_NARGS(__VA_ARGS__) , ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
