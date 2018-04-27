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

#ifndef HPX_LCO_H
#define HPX_LCO_H

#ifdef __cplusplus
extern "C" {
#endif

/// @defgroup lcos LCOs
/// @brief Functions and definitions for using LCOs
/// @{

/// @file include/hpx/lco.h
/// @brief The HPX LCO interface.

#include <hpx/addr.h>
#include <hpx/attributes.h>
#include <hpx/types.h>

/// These are the operations associated with the generic LCO class.
/// @{

/// Delete an LCO.
///
/// @param   lco the address of the LCO to delete
/// @param rsync an LCO to signal remote completion
void hpx_lco_delete(hpx_addr_t lco, hpx_addr_t rsync)
  HPX_PUBLIC;

/// Delete an LCO synchronously.
///
/// @param   lco the address of the LCO to delete
void hpx_lco_delete_sync(hpx_addr_t lco)
  HPX_PUBLIC;

/// Delete a local array of LCOs.
///
/// This interface does not permit the user to wait on individual delete
/// operations to complete, it assumes that the user either wants to know when
/// all of the operations completed or that the user doesn't care at all. The @p
/// rsync lco will only be set *once*, and any errors will be reported there.
///
/// @param     n The number of LCOs to delete.
/// @param  lcos An array of the addresses of the LCOs to delete.
/// @param rsync An LCO to signal remote completion of all of the deletes.
///
/// @returns HPX_SUCCESS, or an error if the operation failed (errors in the
///          individual delete operations are reported through rsync).
int hpx_lco_delete_all(int n, hpx_addr_t *lcos, hpx_addr_t rsync)
  HPX_PUBLIC;

/// An action-based interface to the LCO delete operation.
/// The delete action is a user-packed action that takes a NULL buffer.
HPX_PUBLIC extern HPX_ACTION_DECL(hpx_lco_delete_action);

/// Propagate an error to an LCO.
///
/// If the error code is HPX_SUCCESS then this is equivalent to
///
/// @code
///   hpx_lco_set(lco, 0, NULL, HPX_NULL, rsync);
/// @endcode
///
/// If @p lco is HPX_NULL then this is equivalent to a no-op.
///
/// @param   lco the LCO's global address
/// @param  code a user-defined error code
/// @param rsync an LCO to signal remote completion
void hpx_lco_error(hpx_addr_t lco, hpx_status_t code, hpx_addr_t rsync)
  HPX_PUBLIC;
void hpx_lco_error_sync(hpx_addr_t lco, hpx_status_t code)
  HPX_PUBLIC;

/// Reset an LCO.
///
/// This operation allows reusing a LCO by resetting its internal
/// state. The reset operation is idempotent---resetting an already
/// unset LCO has no effect. All pending gets/waits on a LCO must have
/// finished before it can be reset successfully.
///
/// N.B. This operation does not reset/zero the data buffer associated
/// with the LCO.
///
/// @param  future the global address of the future to reset.
/// @param    sync the address of an LCO to set when the future is reset;
///                may be HPX_NULL
void hpx_lco_reset(hpx_addr_t future, hpx_addr_t sync)
  HPX_PUBLIC;
void hpx_lco_reset_sync(hpx_addr_t future)
  HPX_PUBLIC;

/// An action-based interface to the LCO reset operation.
/// The reset action is a user-packed action that takes a NULL buffer.
HPX_PUBLIC extern HPX_ACTION_DECL(hpx_lco_reset_action);

/// Set an LCO, optionally with data.
///
/// If @p LCO is HPX_NULL then this is equivalent to a no-op.
///
/// @param   lco The LCO to set, can be HPX_NULL.
/// @param  size The size of the data.
/// @param value The address of the value to set.
/// @param lsync An LCO to signal local completion (HPX_NULL == don't wait)
///                local completion indicates that the @p value may be freed
///                or reused.
/// @param rsync an LCO to signal remote completion (HPX_NULL == don't wait)
void hpx_lco_set(hpx_addr_t lco, size_t size, const void *value, hpx_addr_t lsync,
                 hpx_addr_t rsync)
  HPX_PUBLIC;

/// Set an LCO, optionally with data.
///
/// This version of hpx_lco_set is locally synchronous, that is that it will not
/// return until it is safe for the caller to modify the data pointed to by @p
/// value.
///
/// @param          lco The LCO to set, can be HPX_NULL.
/// @param         size The size of the data.
/// @param        value The address of the value to set.
/// @param        rsync an LCO to wait for completion (HPX_NULL == don't wait)
void hpx_lco_set_lsync(hpx_addr_t lco, size_t size, const void *value,
                       hpx_addr_t rsync)
  HPX_PUBLIC;

/// Set an LCO, optionally with data.
///
/// This version of hpx_lco_set is synchronous, that is that it will not return
/// until the lco has been set.
///
/// @param          lco The LCO to set, can be HPX_NULL.
/// @param         size The size of the data.
/// @param        value The address of the value to set.
///
/// @returns            The value from lco set. This value is LCO-specific, but
///                     is meant to be 1 if the set caused the LCO to "trigger."
int hpx_lco_set_rsync(hpx_addr_t lco, size_t size, const void *value)
  HPX_PUBLIC;


/// Set an LCO, optionally with data.
///
/// If @p LCO is HPX_NULL then this is equivalent to a no-op.
///
/// @param   lco The LCO to set, can be HPX_NULL.
/// @param  size The size of the data.
/// @param value The address of the value to set.
/// @param lsync An LCO to signal local completion (HPX_NULL == don't wait)
///                local completion indicates that the @p value may be freed
///                or reused.
/// @param raddr A continuation address for the set return value.
/// @param   rop The continuation operation (should be marshalled, and type
///                of value should be int).
void hpx_lco_set_with_continuation(hpx_addr_t lco, size_t size, const void *value,
                                   hpx_addr_t lsync,
                                   hpx_addr_t raddr, hpx_action_t rop)
  HPX_PUBLIC;

/// An action-based interface to the LCO set operation.
/// The set action is a user-packed action that takes a buffer.
HPX_PUBLIC extern HPX_ACTION_DECL(hpx_lco_set_action);

/// Perform a wait operation.
///
/// The LCO blocks the caller until an LCO set operation triggers the LCO. Each
/// LCO type has its own semantics for the state under which this occurs.
///
/// @param lco the LCO we're processing
/// @returns   HPX_SUCCESS or the code passed to hpx_lco_error()
hpx_status_t hpx_lco_wait(hpx_addr_t lco)
  HPX_PUBLIC;

/// Performs a compound atomic wait-and-reset operation.
///
/// @param lco the LCO we're processing
/// @returns   HPX_SUCCESS or the code passed to hpx_lco_error()
hpx_status_t hpx_lco_wait_reset(hpx_addr_t lco)
  HPX_PUBLIC;

/// Perform a get operation.
///
/// An LCO blocks the caller until it is set, and then copies its value
/// data into the provided buffer.
///
/// If the return status is not HPX_SUCCESS then the LCO was triggered by
/// hpx_lco_error() rather than hpx_lco_set(), in such a case the memory pointed
/// to by @p out will not be inspected.
///
/// @param        lco the LCO we're processing
/// @param       size the size of the data
/// @param[out] value the output location (may be null)
/// @returns          HPX_SUCCESS or the code passed to hpx_lco_error()
hpx_status_t hpx_lco_get(hpx_addr_t lco, size_t size, void *value)
  HPX_PUBLIC;

/// Perform a get operation.
///
/// This version of get includes a compound atomic reset operation. An LCO
/// blocks the caller until it is set, and then copies its value data into the
/// provided buffer.
///
/// If the return status is not HPX_SUCCESS then the LCO was triggered by
/// hpx_lco_error() rather than hpx_lco_set(), in such a case the memory pointed
/// to by @p out will not be inspected.
///
/// @param        lco the LCO we're processing
/// @param       size the size of the data
/// @param[out] value the output location (may be null)
/// @returns          HPX_SUCCESS or the code passed to hpx_lco_error()
hpx_status_t hpx_lco_get_reset(hpx_addr_t lco, size_t size, void *value)
  HPX_PUBLIC;

/// Perform a "get" operation on an LCO but instead of copying the LCO
/// buffer out, get a reference to the LCO's buffer.
///
/// If the return status is not HPX_SUCCESS then the LCO was triggered
/// by hpx_lco_error() rather than hpx_lco_set().
///
/// @param      lco the LCO we're processing
/// @param     size the size of the LCO buffer
/// @param[out] ref pointer to hold the reference to an LCO's buffer
///
/// @returns        HPX_SUCCESS or the code passed to hpx_lco_error()
hpx_status_t hpx_lco_getref(hpx_addr_t lco, size_t size, void **ref)
  HPX_PUBLIC;

/// Release the reference to an LCO's buffer.
///
/// @param      lco the LCO we're processing
/// @param[out] ref the reference to an LCO's buffer
void hpx_lco_release(hpx_addr_t lco, void *ref)
  HPX_PUBLIC;

/// Wait for all of the LCOs to be set.
///
/// This admits some parallelism in the implementation, and is preferable to
/// using hpx_lco_wait() in a loop. The calling thread will block until all of
/// the LCOs have been set. Entries in @p lcos that are HPX_NULL will be
/// ignored.
///
/// @param             n the number of LCOs in @p lcos
/// @param          lcos an array of LCO addresses (must be uniformly
///                      non-HPX_NULL, and correspond to global addresses
///                      associated with real LCOs)
/// @param[out] statuses an array of statuses, pass NULL if statuses are not
///                      required
/// @returns             the number of entries in @p statuses that have
///                      non-HPX_SUCCESS values, will be set irrespective of if
///                      @p statuses is NULL
int hpx_lco_wait_all(int n, hpx_addr_t lcos[], hpx_status_t statuses[])
  HPX_PUBLIC;

/// Get values for all of the LCOs.
///
/// This admits some parallelism in the implementation, and is preferable to
/// using hpx_lco_get() in a loop. The calling thread will block until all of
/// the LCOs are available. Entries in @p lcos that are set to HPX_NULL are
/// ignored, their corresponding values in @p values will not be written to.
///
/// @param             n the number of LCOs
/// @param          lcos an array of @p n global LCO addresses
/// @param         sizes an @p n element array of sizes that must correspond to
///                      @p lcos and @p values
/// @param[out]   values an array of @p n local buffers with sizes corresponding
///                      to @p sizes
/// @param[out] statuses an array of statuses, pass NULL if statuses are not
///                      required
///
/// @returns The number of entries in @p statuses that have non-HPX_SUCCESS
///                        values, will be set irrespective of if @p statuses is
///                        NULL.
int hpx_lco_get_all(int n, hpx_addr_t lcos[], size_t sizes[], void *values[],
                    hpx_status_t statuses[])
  HPX_PUBLIC;

/// Get the size of an LCO.
///
/// This may require communication.
///
/// @param          lco The address of the LCO.
/// @param         size The number of bytes in the LCO's data type.
size_t hpx_lco_size(hpx_addr_t lco, size_t size)
  HPX_PUBLIC;

/// Semaphores are builtin LCOs that represent resource usage.
///
/// @param init initial value semaphore will be created with
///
/// @returns The global address of the new semaphore.
hpx_addr_t hpx_lco_sema_new(unsigned init)
  HPX_PUBLIC;

/// Standard semaphore V (signal) operation.
///
/// Increments the count in the semaphore, signaling the LCO if the increment
/// transitions from 0 to 1.
///
/// This is locally asynchronous, it will potentially return before the
/// operation completes. Clients that need a signal when the set operation has
/// completed should use the @p rsync LCO.
///
/// @param        sema The global address of a semaphore.
/// @param       rsync An LCO to set so the caller can make this synchronous.
void hpx_lco_sema_v(hpx_addr_t sema, hpx_addr_t rsync)
  HPX_PUBLIC;

/// Standard semaphore V (signal) operation.
///
/// Increments the count in the semaphore, signaling the LCO if the increment
/// transitions from 0 to 1.
///
/// @param        sema The global address of a semaphore.
void hpx_lco_sema_v_sync(hpx_addr_t sema)
  HPX_PUBLIC;

/// Standard semaphore P (wait) operation.
///
/// Attempts to decrement the count in the semaphore; blocks if the count is 0.
///
/// @param        sema the global address of a semaphore
///
/// @returns HPX_SUCCESS, or an error code if the semaphore is in an error
///          condition
hpx_status_t hpx_lco_sema_p(hpx_addr_t sema)
  HPX_PUBLIC;

/// An "and" LCO represents an AND gate.
/// @{

///
/// Create an AND gate.
///
/// @param inputs the number of inputs to the and (must be >= 0)
///
/// @returns The global address of the new and gate.
hpx_addr_t hpx_lco_and_new(int64_t inputs)
  HPX_PUBLIC;

/// Join an "and" LCO, triggering it (i.e. setting it) if appropriate.
///
/// If this set is the last one the "and" LCO is waiting on, the "and" LCO
/// will be set.
///
/// @param  lco the global address of the "and" LCO to set.
/// @param sync the address of an LCO to set when the "and" LCO is set;
///             may be HPX_NULL
void hpx_lco_and_set(hpx_addr_t lco, hpx_addr_t sync)
  HPX_PUBLIC;


/// Set an "and" LCO @p num times, triggering it if appropriate.
///
/// @param  lco the global address of the "and" LCO to set.
/// @param  num number of times to set the "and" LCO.
/// @param sync the address of an LCO to set when the "and" LCO is set;
///             may be HPX_NULL
void hpx_lco_and_set_num(hpx_addr_t lco, int num, hpx_addr_t sync)
  HPX_PUBLIC;
/// @}

/// Create a future.
///
/// Futures are builtin LCOs that represent values returned from asynchronous
/// computation.
/// Futures are always allocated in the global address space, because their
/// addresses are used as the targets of parcels.
///
/// @param size the size in bytes of the future's value (may be 0)
/// @returns    the global address of the newly allocated future
hpx_addr_t hpx_lco_future_new(int size)
  HPX_PUBLIC;
/// @}

/// Allocate a global array of futures.
///
/// @param          n The (total) number of futures to allocate
/// @param       size The size of each futures' value
/// @param block_size The number of futures per block
hpx_addr_t hpx_lco_future_array_new(int n, int size, int block_size)
  HPX_PUBLIC;

/// Get an address of a future in a future array
///
/// @param      base The base address of the array of futures.
/// @param         i The index of the future to return.
/// @param      size The size of the data stored with each future.
/// @param     bsize The number of futures in each block.
///
/// @returns The address of the ith future in the array.
hpx_addr_t hpx_lco_future_array_at(hpx_addr_t base, int i, int size, int bsize)
  HPX_PUBLIC;

/// Get an address of a lco in a LCO array
///
/// @param      base The base address of the array of lcos.
/// @param         i The index of the lco to return.
/// @param      size The size of the data type in the LCO.
///
/// @returns The address of the ith lco in the array.
hpx_addr_t hpx_lco_array_at(hpx_addr_t base, int i, size_t size)
  HPX_PUBLIC;

/// Allocate a new generation counter.
///
/// A generation counter allows an application programmer to efficiently wait
/// for a counter. The @p ninplace indicates a bound on the typical number of
/// generations that are explicitly active---it does not impact correctness,
/// merely performance.
///
/// As an example, if there are typically three generations active (i.e.,
/// threads may exist for up to three generations ahead of the current
/// generation), then @p ninplace should be set to three. If it is set to two,
/// then the runtime will perform some extra work testing threads that should
/// not be tested.
///
/// @param ninplace the typical number of active generations
///
/// @returns The global address of the new generation count.
hpx_addr_t hpx_lco_gencount_new(unsigned long ninplace)
  HPX_PUBLIC;

/// Increment the generation counter.
///
/// @param gencnt the counter to increment
/// @param  rsync The global address of an LCO signal remote completion.
void hpx_lco_gencount_inc(hpx_addr_t gencnt, hpx_addr_t rsync)
  HPX_PUBLIC;

/// Wait for the generation counter to reach a certain value.
///
/// It is OK to wait for any generation. If the generation has already passed,
/// this will probably not block. If the generation is far in the future (far in
/// this case means more than the depth value provided in the counters
/// allocator) then the thread may (transparently) wake up more often than it
/// needs to.
///
/// When this returns, it is guaranteed that the current count is <= @p gen, and
/// progress is guaranteed (that is, all threads waiting for @p gen will run in
/// some bounded amount of time when the counter reaches @p gen).
///
/// @param gencnt The counter to wait for.
/// @param    gen The generation to wait for.
///
/// @returns HPX_SUCCESS or an error code.
hpx_status_t hpx_lco_gencount_wait(hpx_addr_t gencnt, unsigned long gen)
  HPX_PUBLIC;

/// Allocate a new reduce LCO.
///
/// The reduction is allocated in reduce-mode, i.e., it expects @p participants
/// to call the hpx_lco_set() operation as the first phase of operation.
///
/// @param inputs       The static number of inputs to the reduction.
/// @param size         The size of the data being reduced.
/// @param id           An initialization function for the data, this is used to
///                     initialize the data in every epoch.
/// @param op           The commutative-associative operation we're performing.
hpx_addr_t hpx_lco_reduce_new(int inputs, size_t size, hpx_action_t id,
                              hpx_action_t op)
  HPX_PUBLIC;

/// Allocate a new allreduce LCO.
///
/// The reduction is allocated in reduce-mode, i.e., it expects @p participants
/// to call the hpx_lco_set() operation as the first phase of operation, and @p
/// readers to "get" the value of the allreduce.
///
/// The preferred mode of operation with the allreduce, however, is through the
/// hpx_lco_allreduce_join(), hpx_lco_allreduce_join_async(),
/// hpx_lco_allreduce_join_sync(). In these contexts, the join operation is
/// considered both a write and a read operation.
///
/// @param participants The static number of participants in the reduction.
/// @param      readers The static number of the readers of the result of the
///                     reduction.
/// @param         size The size of the data being reduced.
/// @param           id A function that is used to initialize the data
///                     in every epoch.
/// @param           op The commutative-associative operation we're performing.
///
/// @returns            The newly allocated LCO, or HPX_NULL on error.
hpx_addr_t hpx_lco_allreduce_new(size_t participants, size_t readers, size_t size,
                                 hpx_action_t id, hpx_action_t op)
  HPX_PUBLIC;

/// Join an allreduce LCO.
///
/// This version of the join operation allows an arbitrary continuation for the
/// reduced data. This counts as both a set and a get operation, even if the
/// continuation is null.
///
/// This interface is locally synchronous---it is safe to reuse or free @p value
/// when it returns.
///
/// @param    allreduce The allreduce LCO to join.
/// @param           id The id of this input.
/// @param        bytes The number of bytes to send.
/// @param        value The value to send.
/// @param         cont A continuation action for the reduced value.
/// @param           at A continuation target for the reduced value.
///
/// @return             The status of the LCO.
hpx_status_t hpx_lco_allreduce_join(hpx_addr_t allreduce, int id, size_t bytes,
                                    const void *value, hpx_action_t cont,
                                    hpx_addr_t at)
  HPX_PUBLIC;

/// Join an allreduce LCO asynchronously.
///
/// This version of the join operation returns the reduced value asynchronously
/// to the caller. This counts as both a set and a get operation.
///
/// This interface is locally synchronous---it is safe to reuse or free @p value
/// when it returns.
///
/// @param    allreduce The allreduce LCO to join.
/// @param           id The id of this input.
/// @param        bytes The number of bytes to send.
/// @param        value The value to send.
/// @param          out The location to output the reduced value, must be at
///                     least @p size bytes.
/// @param         done An LCO that will be set when the operation completes.
///
/// @return             The status of the LCO.
hpx_status_t hpx_lco_allreduce_join_async(hpx_addr_t allreduce, int id,
                                          size_t bytes, const void *value,
                                          void *out, hpx_addr_t done)
  HPX_PUBLIC;

/// Join an allreduce LCO synchronously.
///
/// This version of the join operation returns the reduced value synchronously
/// to the caller. This counts as both a set and a get operation.
///
/// @param    allreduce The allreduce LCO to join.
/// @param           id The id of this input.
/// @param        bytes The number of bytes to send.
/// @param        value The value to send.
/// @param          out The location to output the reduced value, must be at
///                     least @p size bytes.
///
/// @return             The status of the LCO.
hpx_status_t hpx_lco_allreduce_join_sync(hpx_addr_t allreduce, int id,
                                         size_t bytes, const void *value,
                                         void *out)
  HPX_PUBLIC;

/// Set a gather LCO.
///
/// The gather LCO hpx_lco_set operation does not work correctly, because
/// there is no input variable. Use this setid operation instead of set.
///
/// @param gather The gather we're setting.
/// @param id        The ID of our rank.
/// @param size      The size of the input @p value.
/// @param value     A pointer to @p size bytes to set with.
/// @param lsync     An LCO to test for local completion.
/// @param rsync     An LCO to test for remote completion.
hpx_status_t hpx_lco_gather_setid(hpx_addr_t gather, unsigned id,
                                  int size, const void *value,
                                  hpx_addr_t lsync, hpx_addr_t rsync)
  HPX_PUBLIC;

/// Allocate a gather LCO.
///
/// This allocates an gather LCO with enough space for @p inputs of @p size.
///
/// @param  inputs The number of writers in the gather.
/// @param outputs The number of readers in the gather.
/// @param    size The size of the value type that we're gathering.
hpx_addr_t hpx_lco_gather_new(size_t inputs, size_t outputs, size_t size)
  HPX_PUBLIC;

/// Set an alltoall LCO.
///
/// The alltoall LCO hpx_lco_set operation does not work correctly, because
/// there is no input variable. Use this setid operation instead of set.
///
/// @param alltoall    The alltoall we're setting.
/// @param id          The ID of our rank.
/// @param size        The size of the input @p value.
/// @param value       A pointer to @p size bytes to set with.
/// @param lsync       An LCO to test for local completion.
/// @param rsync       An LCO to test for remote completion.
hpx_status_t hpx_lco_alltoall_setid(hpx_addr_t alltoall, unsigned id,
                                    int size, const void *value,
                                    hpx_addr_t lsync, hpx_addr_t rsync)
  HPX_PUBLIC;

/// Get the ID for an alltoall LCO. This is global getid for the user to use.
///
/// @param   alltoall    Global address of the alltoall LCO
/// @param   id          The ID of our rank
/// @param   size        The size of the data being gathered
/// @param   value       Address of the value buffer
hpx_status_t hpx_lco_alltoall_getid(hpx_addr_t alltoall, unsigned id, int size,
                                    void *value)
  HPX_PUBLIC;

/// Allocate an alltoall LCO.
///
/// This allocates an alltoall LCO with enough space for @p inputs of @p size.
///
/// @param inputs The number of participants in the alltoall.
/// @param size   The size of the value type that we're gathering.
hpx_addr_t hpx_lco_alltoall_new(size_t inputs, size_t size)
  HPX_PUBLIC;

/// Allocate a user-defined LCO.
///
/// @param size         The size of the LCO buffer.
/// @param id           An initialization function for the data, this is used to
///                     initialize the data in every epoch.
/// @param op           The commutative-associative operation we're performing.
/// @param predicate    Predicate to guard the LCO.
/// @param init         Buffer to initialize the LCO with.
/// @param init_size    The size of the initializer.
hpx_addr_t hpx_lco_user_new(size_t size, hpx_action_t id, hpx_action_t op,
                            hpx_action_t predicate, void *init,
                            size_t init_size)
  HPX_PUBLIC;

/// Get the user-defined LCO's user data. This allows to access the buffer
/// portion of the user-defined LCO regardless the LCO has been set or not.
/// @param        lco The LCO we're processing.
void *hpx_lco_user_get_user_data(void *lco) HPX_PUBLIC;

/// Allocate a dataflow LCO.
///
hpx_addr_t hpx_lco_dataflow_new(void)
  HPX_PUBLIC;

/// Add a node to the dataflow LCO.
///
/// @param lco      The dataflow LCO
/// @param action   The action to run when the inputs are available
/// @param out      The output LCO
/// @param n        Number of input LCOs
/// @param ...      The input LCOs
///
/// @returns        HPX_SUCCESS, or an error code.
int _hpx_lco_dataflow_add(hpx_addr_t lco, hpx_action_t action,
                          hpx_addr_t out, int n, ...)
  HPX_PUBLIC;

#define hpx_lco_dataflow_add(lco, action, out, ...)                   \
  _hpx_lco_dataflow_add(lco, action, out, __HPX_NARGS(__VA_ARGS__) , \
                        ##__VA_ARGS__)
/// @}

/// LCO reduction operators.
/// @{
///
/// The commutative-associative (monoid) operation type.
///
/// Common operations would be min, max, +, *, etc. The runtime will pass the
/// number of bytes that the allreduce was allocated with.
typedef void (*hpx_monoid_id_t)(void *i, size_t bytes);
typedef void (*hpx_monoid_op_t)(void *lhs, const void *rhs, size_t bytes);

/// Helper macro to declare the monoid operators for an LCO @p
/// reduction of a given @p TYPE.
#define _HPX_MONOID_DECL(TYPE,REDUCTION,dtype)                      \
  void HPX_##TYPE##REDUCTION##ID(dtype *, size_t);                  \
  void HPX_##TYPE##REDUCTION##OP(dtype *, const dtype *, size_t);

/// Helper macro to declare a LCO @p reduction for a list of types.
#define _HPX_REDUCTION_DECL(R)               \
  _HPX_MONOID_DECL(INT_,    R, int)          \
  _HPX_MONOID_DECL(DOUBLE_, R, double)       \
  _HPX_MONOID_DECL(FLOAT_,  R, float)

/// In-built LCO reduction operations (+,-,*,max,min) that can be used
/// with "reduction" LCOs.
_HPX_REDUCTION_DECL(SUM_);
_HPX_REDUCTION_DECL(PROD_);
_HPX_REDUCTION_DECL(MAX_);
_HPX_REDUCTION_DECL(MIN_);
/// @}

/// Local array operations for LCOs. These allow creation of LCO arrays
/// local to the calling locality.
/// @{
///
/// Allocate an array of future LCO local to the calling locality.
/// @param          n The (total) number of lcos to allocate
/// @param       size The size of each future's value
///
/// @returns the global address of the allocated array lco.
hpx_addr_t hpx_lco_future_local_array_new(int n, int size)
  HPX_PUBLIC;

/// Allocate an array of and LCO local to the calling locality.
/// @param          n The (total) number of lcos to allocate
/// @param     inputs number of inputs to the and (must be >= 0)
///
/// @returns the global address of the allocated array lco.
hpx_addr_t hpx_lco_and_local_array_new(int n, int inputs)
  HPX_PUBLIC;

/// Allocate an array of reduce LCO local to the calling locality.
/// @param          n The (total) number of lcos to allocate
/// @param     inputs The static number of inputs to the reduction.
/// @param       size The size of the data being reduced.
/// @param         id An initialization function for the data, this is
///                   used to initialize the data in every epoch.
/// @param         op The commutative-associative operation we're
///                   performing.
///
/// @returns the global address of the allocated array lco.
hpx_addr_t hpx_lco_reduce_local_array_new(int n, int inputs, size_t size,
                                          hpx_action_t id,
                                          hpx_action_t op)
  HPX_PUBLIC;


/// Allocate an array of gather LCO local to the calling locality.
/// @param          n The (total) number of lcos to allocate
/// @param     inputs Number of inputs to the gather
/// @param       size The size of the value for gather LCO
///
/// @returns the global address of the allocated array lco.
hpx_addr_t hpx_lco_gather_local_array_new(int n, size_t inputs, size_t outputs,
                                          size_t size)
  HPX_PUBLIC;

/// Allocate an array of allreduce LCO local to the calling locality.
/// @param            n The (total) number of lcos to allocate
/// @param participants The static number of participants in the reduction.
/// @param      readers The static number of the readers of the result of the reduction.
/// @param         size The size of the data being reduced.
/// @param           id An initialization function for the data, this is
///                     used to initialize the data in every epoch.
/// @param           op The commutative-associative operation we're
///                     performing.
///
/// @returns the global address of the allocated array lco.
hpx_addr_t hpx_lco_allreduce_local_array_new(int n, size_t participants,
                                             size_t readers, size_t size,
                                             hpx_action_t id,
                                             hpx_action_t op)
  HPX_PUBLIC;

/// Allocate an array of alltoall LCO local to the calling locality.
/// @param          n The (total) number of lcos to allocate
/// @param     inputs Number of inputs to alltoall LCO
/// @param       size The size of the value that we're gathering
///
/// @returns the global address of the allocated array lco.
hpx_addr_t hpx_lco_alltoall_local_array_new(int n, size_t inputs, size_t size)
  HPX_PUBLIC;

/// Allocate an array of user LCO local to the calling locality.
/// @param          n The (total) number of lcos to allocate
/// @param       size The size of the LCO Buffer
/// @param         id An initialization function for the data, this is
///                   used to initialize the data in every epoch.
/// @param         op The commutative-associative operation we're
///                   performing.
/// @param  predicate Predicate to guard the LCO.
/// @param       init Buffer to initialize the LCO with.
/// @param  init_size The size of the initializer.
///
/// @returns the global address of the allocated array lco.
hpx_addr_t hpx_lco_user_local_array_new(int n, size_t size,
                                        hpx_action_t id, hpx_action_t op,
                                        hpx_action_t predicate, void *init,
                                        size_t init_size)
  HPX_PUBLIC;
/// @}

#ifdef __cplusplus
}
#endif

#endif
