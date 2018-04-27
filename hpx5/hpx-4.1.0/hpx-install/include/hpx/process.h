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

#ifndef HPX_PROCESS_H
#define HPX_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

/// @defgroup processes Processes
/// @brief Functions and definitions for using HPX processes
/// @{

/// @file
/// @brief HPX process interface

/// HPX process id type
typedef hpx_addr_t hpx_pid_t;

/// HPX Process creation.
///
/// Processes in HPX are part of a termination group and can be waited on
/// through the @p termination LCO. The returned process object uniquely
/// represents a process and permits operations to be executed on the process.
///
/// NB: a process spawn is always local to the calling locality.
///
/// @param termination an lco to wait on
hpx_addr_t hpx_process_new(hpx_addr_t termination) HPX_PUBLIC;


/// A process-specific call interface.
///
/// This calls an action @p action inside a process @p process putting
/// the resulting value in @p result at some point in the future.
int    _hpx_process_call(hpx_addr_t process, hpx_addr_t addr, hpx_action_t action,
                         hpx_addr_t result, int nargs, ...) HPX_PUBLIC;
#define hpx_process_call(process, addr, action, result, ...)                \
  _hpx_process_call(process, addr, action, result, __HPX_NARGS(__VA_ARGS__) \
                    , ##__VA_ARGS__)

/// Delete a specified process
///
/// @param    process the hpx address of the process to delete
/// @param       sync an lco to wait on for the delete operation
void hpx_process_delete(hpx_addr_t process, hpx_addr_t sync) HPX_PUBLIC;

/// Get the pid of a process
///
/// @param    process the hpx address of the process
hpx_pid_t hpx_process_getpid(hpx_addr_t process) HPX_PUBLIC;

/// HPX process broadcast.
///
/// This is a parallel call interface that performs an @p action with @p args at
/// all the localities associated with the process. The output values are not
/// returned, but the completion of the broadcast operation can be tracked
/// through the @p lco LCO.
///
/// @param          pid The process to which to broadcast.
/// @param       action The action to perform.
/// @param        lsync The address of an LCO to trigger when the broadcast
///                       operation is complete locally, and the buffer can be
///                       reused or released.
/// @param        rsync The address of an LCO to trigger when the broadcast
///                       operation is complete globally, i.e., when all of the
///                       broadcast handlers have run.
/// @param        nargs The number of @p args
///
/// @returns      HPX_SUCCESS if no errors were encountered.
int _hpx_process_broadcast(hpx_pid_t pid, hpx_action_t action, hpx_addr_t lsync,
                           hpx_addr_t rsync, int nargs, ...)
  HPX_PUBLIC;

#define hpx_process_broadcast(pid, action, lsync, rsync, ...)           \
  _hpx_process_broadcast(pid, action, lsync, rsync, __HPX_NARGS(__VA_ARGS__), \
                         ##__VA_ARGS__)

/// HPX process broadcast.
///
/// This is a parallel call interface that performs an @p action with @p args at
/// all localities associated with a process. The output values are not
/// returned.
///
/// This variant of broadcast is locally synchronous. It will not return to the
/// caller until it is safe to reuse or delete the @p args buffer.
///
/// @param          pid The process to which to broadcast.
/// @param       action The action to perform.
/// @param        rsync The address of an LCO to trigger when the broadcast
///                       operation is complete globally, i.e., when all of the
///                       broadcast handlers have run.
/// @param        nargs The number of @p args
///
/// @returns      HPX_SUCCESS if no errors were encountered.
int _hpx_process_broadcast_lsync(hpx_pid_t pid, hpx_action_t action,
                                 hpx_addr_t rsync, int nargs, ...)
  HPX_PUBLIC;

#define hpx_process_broadcast_lsync(action, rsync, ...)                 \
  _hpx_process_broadcast_lsync(pid, action, rsync, __HPX_NARGS(__VA_ARGS__), \
                               ##__VA_ARGS__)

/// HPX process broadcast.
///
/// This is a parallel call interface that performs an @p action with @p args at
/// all localities associated with the process. The output values are not
/// returned.
///
/// This variant of broadcast is synchronous. It will not return to the caller
/// until specified action has run at every locality (this implies local
/// completion as well).
///
/// @param          pid The process to which to broadcast.
/// @param       action The action to perform.
/// @param        nargs The number of @p args
///
/// @returns      HPX_SUCCESS if no errors were encountered.
int _hpx_process_broadcast_rsync(hpx_pid_t pid, hpx_action_t action, int nargs,
                                 ...)
  HPX_PUBLIC;

#define hpx_process_broadcast_rsync(pid, action, ...)                   \
  _hpx_broadcast_rsync(pid, action, __HPX_NARGS(__VA_ARGS__) , ##__VA_ARGS__)

/// Allocate a distributed allreduce collective in the current process.
///
/// This allreduce has basically the same behavior as a traditional allreduce
/// collective in a SPMD model, except that there is no expectation that the
/// inputs to the allreduce are perfectly balanced.
///
/// Clients of the allreduce must register with the allreduce before use. While
/// inconvenient, this allows efficient implementations of the synchronization
/// structure. See hpx_process_collective_allreduce_subscribe() for details.
///
/// This allreduce does not guarantee a deterministic reduce order, so floating
/// point reductions must account for machine precision issues.
///
/// @param        bytes The size, in bytes, of the reduced value.
/// @param        reset A reset operation for the reduction type.
/// @param           op The reduce operation.
///
/// @returns            The global address to use for the allreduce, or HPX_NULL
///                     if there was an allocation problem.
hpx_addr_t hpx_process_collective_allreduce_new(size_t bytes,
                                                hpx_action_t reset,
                                                hpx_action_t op)
  HPX_PUBLIC;

/// Delete a process allreduce.
///
/// This is not synchronized, so the caller must ensure that there are no
/// concurrent accesses to the allreduce.
///
/// @param    allreduce The collective's address.
void hpx_process_collective_allreduce_delete(hpx_addr_t allreduce)
  HPX_PUBLIC;

/// Subscribe to use an allreduce.
///
/// This notifies the process that a new participant exists at the calling
/// locality. This is unsynchronized and may cause undefined results if it is
/// executed during an active reduction epoch.
///
/// The subscription is defined in terms of a continuation action to be invoked
/// with the reduced value after each epoch. The continuation must be a
/// marshalled action type, and be compatible with the size defined during
/// allreduce allocation. The subscription returns an identifier token that
/// should be used during the join operation.
///
/// @param    allreduce The allreduce to subscribe to.
/// @param     c_action The continuation action.
/// @param     c_target The continuation target.
///
/// @returns            A token to be used during the join operation, or -1 on
///                     error.
int32_t hpx_process_collective_allreduce_subscribe(hpx_addr_t allreduce,
                                                   hpx_action_t c_action,
                                                   hpx_addr_t c_target)
  HPX_PUBLIC;

/// Finalize subscription from an allreduce.
///
/// This notifies all the participant groups about the current active 
/// group and its properties in preparation for collective call ahead
///
/// @param    allreduce The allreduce to finalize from.
///
int hpx_process_collective_allreduce_subscribe_finalize(hpx_addr_t allreduce)

  HPX_PUBLIC;

/// Unsubscribe from an allreduce.
///
/// This notifies the process that a participant at the current locality is
/// leaving the allreduce. This is unsynchronized and may cause undefined
/// results if it is executed during an active reduction epoch.
///
/// @param    allreduce The allreduce to unsubscribe from.
/// @param        token The identifier associated with the continuation.
void hpx_process_collective_allreduce_unsubscribe(hpx_addr_t allreduce,
                                                  int32_t token)
  HPX_PUBLIC;

/// Join an allreduce asynchronously.
///
/// This interface joins an allreduce. Clients must provide an identifier that
/// is valid for the allreduce at the current locality.
///
/// @param    allreduce The collective address.
/// @param           id The input id.
/// @param        bytes The size of the allreduce value in bytes.
/// @param           in A pointer to the allreduce input value.
///
/// @return             HPX_SUCCESS if the local part of this operation
///                     completes successfully, or an error code if it fails.
int hpx_process_collective_allreduce_join(hpx_addr_t allreduce,
                                          int32_t id, size_t bytes,
                                          const void *in)
  HPX_PUBLIC;

/// Join an allreduce synchronously.
///
/// This interface joins an allreduce, and will block the calling thread until
/// the reduced value is available in @p out. Performance may be improved if the
/// @p out buffer should be from registered memory (i.e., a stack location, a
/// pinned global address, or memory returned from malloc_registered()).
///
/// @param    allreduce The collective address.
/// @param           id The input id.
/// @param        bytes The size of the allreduce value in bytes.
/// @param           in A pointer to the allreduce input value.
/// @param          out A pointer to at least @p bytes for the reduced value,
///                     may alias @p in.
///
/// @return             HPX_SUCCESS if the local part of this operation
///                     completes successfully, or an error code if it fails.
// int hpx_process_collective_allreduce_join_sync(hpx_addr_t collective, int id,
//                                                size_t bytes, const void *in,
//                                                void *out)
//   HPX_PUBLIC;

/// @}

#ifdef __cplusplus
}
#endif

#endif
