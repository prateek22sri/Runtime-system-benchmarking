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

#ifndef HPX_GAS_H
#define HPX_GAS_H

#ifdef __cplusplus
extern "C" {
#endif

/// @defgroup agas Global Address Space
/// @brief Functions and definitions for using the global address space
/// @{

/// @file  include/hpx/gas.h
/// @brief Functions for allocating and using memory in the HPX global address
///        space.
#include <hpx/addr.h>

/// The largest block size supported by the implementation.
extern HPX_PUBLIC uint64_t HPX_GAS_BLOCK_BYTES_MAX;

/// User-defined GAS distribution function.
typedef hpx_addr_t (*hpx_gas_dist_t)(uint32_t i, size_t n, size_t bsize);

// GAS distribution types.
#define HPX_GAS_DIST_LOCAL   (hpx_gas_dist_t)0x1  //!< Local distribution type
#define HPX_GAS_DIST_CYCLIC  (hpx_gas_dist_t)0x2  //!< Cyclic distribution type
#define HPX_GAS_DIST_BLOCKED (hpx_gas_dist_t)0x4  //!< Blocked distribution type

// GAS Attributes.
#define HPX_GAS_ATTR_NONE  0x0  //!< Empty attribute.
#define HPX_GAS_ATTR_RO    0x1  //!< This block is read-only.
#define HPX_GAS_ATTR_LB    0x2  //!< Consider for automatic load balancing.
#define HPX_GAS_ATTR_LCO   0x4  //!< This block is an LCO.

/// Allocate distributed global memory given a distribution.
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
/// @param         dist The gas distribution type.
/// @param         attr The attributes of this global allocation space
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_alloc(size_t n, size_t bsize, uint32_t boundary,
                         hpx_gas_dist_t dist, uint32_t attr)
  HPX_PUBLIC;

/// Allocate distributed zeroed global memory given a distribution.
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
/// @param         dist The gas distribution type.
/// @param         attr The attributes of this global allocation space
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_calloc(size_t n, size_t bsize, uint32_t boundary,
                          hpx_gas_dist_t dist, uint32_t attr)
  HPX_PUBLIC;

/// Allocate cyclically distributed global memory.
///
/// This is not a collective operation; the returned address is returned only to
/// the calling thread, and must either be written into already-allocated global
/// memory, or sent via a parcel, for anyone else to address the allocation.
///
/// The total amount of usable memory allocated is @p n * @p bsize.
///
/// The alignment of each block (and thus the base alignment of the entire
/// array), will be 2^{align=ceil_log2_32(bsize)}, i.e., the minimum power of 2 to
/// bsize such that align >= bsize.
///
/// In UPC-land, the returned global address would have the following
/// distribution:
///
///    shared [bytes] char foo[n * bytes];
///
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_alloc_cyclic(size_t n, size_t bsize, uint32_t boundary)
  HPX_PUBLIC;

/// Allocate cyclically distributed global zeroed memory.
///
/// This call is similar to hpx_gas_alloc_cyclic except that the
/// global memory returned is initialized to 0.
///
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_calloc_cyclic(size_t n, size_t bsize, uint32_t boundary)
  HPX_PUBLIC;

/// Allocate distributed global memory laid out in a
/// super-block-cyclic manner where the size of each super-block is
/// equal to @p n/HPX_LOCALITIES.
///
/// This is not a collective operation; the returned address is returned only to
/// the calling thread, and must either be written into already-allocated global
/// memory, or sent via a parcel, for anyone else to address the allocation.
///
/// The total amount of usable memory allocated is @p n * @p bsize.
///
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_alloc_blocked(size_t n, size_t bsize, uint32_t boundary)
  HPX_PUBLIC;

/// Allocate partitioned, super-block-cyclically distributed global
/// zeroed memory.
///
/// This call is similar to hpx_gas_alloc_blocked except that the
/// global memory returned is initialized to 0.
///
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_calloc_blocked(size_t n, size_t bsize, uint32_t boundary)
  HPX_PUBLIC;

/// Allocate a block of global memory.
///
/// This is a non-collective call to allocate memory in the global
/// address space that can be moved. The allocated memory, by default,
/// has affinity to the allocating node, however in low memory conditions the
/// allocated memory may not be local to the caller. As it allocated in the GAS,
/// it is accessible from any locality, and may be relocated by the
/// runtime.
///
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_alloc_local(size_t n, size_t bsize, uint32_t boundary)
  HPX_PUBLIC;

hpx_addr_t hpx_gas_alloc_local_at_sync(size_t n, size_t bsize, uint32_t boundary,
                                       hpx_addr_t loc)
  HPX_PUBLIC;

void hpx_gas_alloc_local_at_async(size_t n, size_t bsize, uint32_t boundary,
                                  hpx_addr_t loc, hpx_addr_t lco)
  HPX_PUBLIC;

extern HPX_PUBLIC HPX_ACTION_DECL(hpx_gas_alloc_local_at_action);

/// Allocate a 0-initialized block of global memory.
///
/// This is a non-collective call to allocate memory in the global address space
/// that can be moved. The allocated memory, by default, has affinity to the
/// allocating node, however in low memory conditions the allocated memory may
/// not be local to the caller. As it allocated in the GAS, it is accessible
/// from any locality, and may be relocated by the runtime.
///
/// *Note however that we do not track the alignment of allocations.* Users
/// should make sure to preserve alignment during move.
///
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_calloc_local(size_t n, size_t bsize, uint32_t boundary)
  HPX_PUBLIC;

hpx_addr_t hpx_gas_calloc_local_at_sync(size_t n, size_t bsize,
                                        uint32_t boundary, hpx_addr_t loc)
  HPX_PUBLIC;

void hpx_gas_calloc_local_at_async(size_t n, size_t bsize,
                                   uint32_t boundary, hpx_addr_t loc,
                                   hpx_addr_t out)
  HPX_PUBLIC;

extern HPX_PUBLIC HPX_ACTION_DECL(hpx_gas_calloc_local_at_action);

/// Global memory allocation routines with GAS attributes.
///
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
/// @param         attr The attributes of this global allocation space
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_alloc_cyclic_attr(size_t n, size_t bsize, uint32_t boundary,
                                     uint32_t attr)
  HPX_PUBLIC;
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
/// @param         attr The attributes of this global allocation space
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_calloc_cyclic_attr(size_t n, size_t bsize, uint32_t boundary,
                                      uint32_t attr)
  HPX_PUBLIC;
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
/// @param         attr The attributes of this global allocation space
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_alloc_blocked_attr(size_t n, size_t bsize, uint32_t boundary,
                                      uint32_t attr)
  HPX_PUBLIC;
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
/// @param         attr The attributes of this global allocation space
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_calloc_blocked_attr(size_t n, size_t bsize, uint32_t boundary,
                                       uint32_t attr)
  HPX_PUBLIC;
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
/// @param         attr The attributes of this global allocation space
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_alloc_local_attr(size_t n, size_t bsize, uint32_t boundary,
                                    uint32_t attr)
  HPX_PUBLIC;
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
/// @param         attr The attributes of this global allocation space
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_calloc_local_attr(size_t n, size_t bsize, uint32_t boundary,
                                     uint32_t attr)
  HPX_PUBLIC;
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
/// @param         dist The gas distribution type.
/// @param         attr The attributes of this global allocation space
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_alloc_user_attr(size_t n, size_t bsize, uint32_t boundary,
                                   hpx_gas_dist_t dist, uint32_t attr)
  HPX_PUBLIC;
/// @param            n The number of blocks to allocate.
/// @param        bsize The number of bytes per block.
/// @param     boundary The alignment (2^k).
/// @param         dist The gas distribution type.
/// @param         attr The attributes of this global allocation space
///
/// @returns            The global address of the allocated memory.
hpx_addr_t hpx_gas_calloc_user_attr(size_t n, size_t bsize, uint32_t boundary,
                                    hpx_gas_dist_t dist, uint32_t attr)
  HPX_PUBLIC;

/// Set an attribute for a global address.
///
/// @param  addr         The global address.
/// @param  attr         The GAS attribute to set.
void hpx_gas_set_attr(hpx_addr_t addr, uint32_t attr)
  HPX_PUBLIC;

/// Free a global allocation.
///
/// This global free is asynchronous. The @p sync LCO address can be used to
/// test for completion of the free.
///
/// @param         addr The global address of the memory to free.
/// @param        rsync An LCO we can use to detect that the free has occurred.
void hpx_gas_free(hpx_addr_t addr, hpx_addr_t rsync) HPX_PUBLIC;
void hpx_gas_free_sync(hpx_addr_t addr) HPX_PUBLIC;
extern HPX_PUBLIC HPX_ACTION_DECL(hpx_gas_free_action);

/// Change the locality-affinity of a global distributed memory address.
///
/// This operation is only valid in the AGAS GAS mode. For PGAS, it is effectively
/// a no-op.
///
/// @param          src The source address to move.
/// @param          dst The address pointing to the target locality to move the
///                       source address @p src to.
/// @param[out]     lco LCO object to check to wait for the completion of move.
void hpx_gas_move(hpx_addr_t src, hpx_addr_t dst, hpx_addr_t lco)
  HPX_PUBLIC;

/// Performs address translation.
///
/// This will try to perform a global-to-local translation on the global @p
/// addr, and set @p local to the local address if it is successful. If @p
/// local is NULL, then this only performs address translation.
///
/// If the address is not local, will return false. Or, if @p local is not
/// NULL and the pin fails, this will return false, otherwise it will return
/// true.
///
/// Successful pinning operations must be matched with an unpin operation, if
/// the underlying data is ever to be moved.
///
/// @param         addr The global address.
/// @param[out]   local The pinned local address.
///
/// @returns       true If @p addr is local and @p local is NULL
///                true If @p addr is local and @p local is not NULL and pin is
///                       successful.
///               false If @p is not local.
///               false If @p is local and @p local is not NULL and pin fails.
bool hpx_gas_try_pin(hpx_addr_t addr, void **local)
  HPX_PUBLIC;

/// Unpin a previously pinned block.
///
/// @param         addr The address of global memory to unpin.
void hpx_gas_unpin(hpx_addr_t addr) HPX_PUBLIC;

/// Register a local buffer for use in the memget/memput functions.
///
/// Any memory can be used for memget/memput, however registered memory access
/// is potentially more efficient. This interface allows application users to
/// register local memory, and is provided for legacy compatibility. New
/// applications should prefer memory allocated with the registered memory
/// interface.
///
/// @note This interface is synchronous---the buffer may be used immediately
///       following the call.
/// @note This interface is only available after hpx_init(), but can be called
///       outside of an HPX epoch.
/// @note Lightweight stack and pinned global addresses are implicitly
///       registered and should not be explicitly passed to this routine.
/// @note The registered buffer must not overlap with other registrations.
///
/// @param       buffer The start of the buffer to register.
/// @param        bytes The length of the buffer to register.
///
/// @returns            HPX_SUCCESS or an error code.
int hpx_register_memory(const void *buffer, size_t bytes)
  HPX_PUBLIC;

/// Deregister a previously registered local buffer.
///
/// The @p buffer must have been previously registered with
/// hpx_register_memory(), and @p bytes must also match the registration call.
///
/// @param       buffer The start of the buffer that was registered.
/// @param        bytes The length of the buffer that was registered.
///
/// @returns            HPX_SUCCESS or an error code.
int hpx_deregister_memory(const void *buffer, size_t bytes)
  HPX_PUBLIC;

/// Allocate local memory for use in the memget/memput functions.
///
/// Any memory can be used in these functions, however only thread stacks and
/// buffers allocated with the registered interface are considered to be fast
/// sources or targets for these operations.
///
/// @param        bytes The number of bytes to allocate.
///
/// @returns      The buffer, or NULL if there was an error.
void *hpx_malloc_registered(size_t bytes)
  HPX_PUBLIC;

/// Allocate local zeroed memory for use in the memget/memput functions.
///
/// Any memory can be used in these functions, however only thread stacks and
/// buffers allocated with the registered interface are considered to be fast
/// sources or targets for these operations.
///
/// @param     elements The number of elements to allocate.
/// @param        bytes The number of bytes to allocate.
void *hpx_calloc_registered(size_t elements, size_t bytes)
  HPX_PUBLIC;

/// Allocate local aligned memory for use in the memget/memput functions.
///
/// Any memory can be used in these functions, however only thread stacks and
/// buffers allocated with the registered interface are considered to be fast
/// sources or targets for these operations.
///
/// @param        align The alignment requested (must be 2^k)
/// @param        bytes The number of bytes to allocate.
void *hpx_memalign_registered(size_t align, size_t bytes)
  HPX_PUBLIC;

/// Free local memory that was allocated with hpx_malloc_registered().
///
/// @param            p The buffer.
void hpx_free_registered(void *p)
  HPX_PUBLIC;

/// This copies data from a global address to a local buffer, asynchronously.
///
/// The global address range [from, from + size) must be within a single block
/// allocated using one of the family of GAS allocation routines. This
/// requirement may not be checked. Copying data across a block boundary, or
/// from unallocated memory, will result in undefined behavior.
///
/// This operation is not atomic. memgets with concurrent memputs to overlapping
/// addresses ranges will result in a data race with undefined behavior. Users
/// should synchronize with some out-of-band mechanism.
///
/// @param           to The local address to copy to, must be a stack location
///                       or an address allocated with hpx_malloc_registered().
/// @param         from The global address to copy from.
/// @param         size The size, in bytes, of the buffer to copy
/// @param        lsync The address of a zero-sized future that can be used to
///                       wait for completion of the memget.
///
/// @returns HPX_SUCCESS
int hpx_gas_memget(void *to, hpx_addr_t from, size_t size, hpx_addr_t lsync)
  HPX_PUBLIC;

/// Synchronous interface to memget.
int hpx_gas_memget_sync(void *to, hpx_addr_t from, size_t size) HPX_PUBLIC;

/// This copies data from a local buffer to a global address, asynchronously.
///
/// The global address range [to, to + size) must be within a single block
/// allocated using one of the family of GAS allocation routines. This
/// requirement is not checked. Copying data across a block boundary, or to
/// unallocated memory, will result in undefined behavior.
///
/// This operation is not atomic. Concurrent memputs to overlapping addresses
/// ranges will result in a data race with undefined behavior. Users should
/// synchronize with some out-of-band mechanism.
///
/// @note A set to @p rsync implies @p lsync has also been set.
///
/// @param           to The global address to copy to.
/// @param         from The local address to copy from, must be a stack location
///                       or an address allocated with hpx_malloc_registered().
/// @param         size The size, in bytes, of the buffer to copy
/// @param        lsync The address of a zero-sized future that can be used to
///                       wait for local completion of the memput. Once this is
///                       signaled the @p from buffer may be reused or freed.
/// @param        rsync The address of a zero-sized future that can be used to
///                       wait for remote completion of the memput. Once this is
///                       signaled the put has become globally visible.
///
/// @returns  HPX_SUCCESS
int hpx_gas_memput(hpx_addr_t to, const void *from, size_t size,
                   hpx_addr_t lsync, hpx_addr_t rsync)
  HPX_PUBLIC;

/// This copies data from a local buffer to a global address with locally
/// synchronous semantics.
///
/// This shares the same functionality as hpx_gas_memput(), but will not return
/// until the local @p from buffer can be reused. This exposes the potential for
/// a more efficient mechanism for synchronous operation, and should be
/// preferred where locally-synchronous semantics are desired.
///
/// @param           to The global address to copy to.
/// @param         from The local address to copy from, must be a stack location
///                       or an address allocated with hpx_malloc_registered().
/// @param         size The size, in bytes, of the buffer to copy
/// @param        rsync The address of a zero-sized future that can be used to
///                       wait for remote completion of the memput. Once this is
///                       signaled the put has become globally visible.
///
/// @returns  HPX_SUCCESS
int hpx_gas_memput_lsync(hpx_addr_t to, const void *from, size_t size,
                         hpx_addr_t rsync)
  HPX_PUBLIC;

/// This copies data synchronously from a local buffer to a global address.
///
/// This shares the same functionality as hpx_gas_memput(), but will not return
/// until the write has completed remotely. This exposes the potential for
/// a more efficient mechanism for synchronous operation, and should be
/// preferred where fully synchronous semantics are necessary.
///
/// @param           to The global address to copy to.
/// @param         from The local address to copy from, must be a stack location
///                       or an address allocated with hpx_malloc_registered().
/// @param         size The size, in bytes, of the buffer to copy
///
/// @returns  HPX_SUCCESS
int hpx_gas_memput_rsync(hpx_addr_t to, const void *from, size_t size)
  HPX_PUBLIC;

/// This copies data from a global address to a global address, asynchronously.
///
/// The global address range [from, from + size) and [to, to + size) must be
/// within single blocks, respectively, allocated using one of the family of GAS
/// allocation routines. This requirement may not be checked. Copying data
/// across a block boundary, or from unallocated memory, will result in
/// undefined behavior.
///
/// This operation is not atomic. Concurrent memcpys to overlapping addresses
/// ranges will result in a data race with undefined behavior. Users should
/// synchronize with some out-of-band mechanism. Concurrent memcpys from
/// overlapping regions will be race free, as long as no concurrent memputs or
/// memcpys occur to that region.
///
/// @param           to The global address to copy to.
/// @param         from The global address to copy from.
/// @param         size The size, in bytes, of the buffer to copy
/// @param         sync The address of a zero-sized future that can be used to
///                       wait for completion of the memcpy.
///
/// @returns  HPX_SUCCESS
int hpx_gas_memcpy(hpx_addr_t to, hpx_addr_t from, size_t size, hpx_addr_t sync)
  HPX_PUBLIC;

/// This copies data synchronously from a global address to another
/// global address.
///
/// This shares the same functionality as hpx_gas_memcpy(), but will not return
/// until the copy has completed remotely. This exposes the potential for
/// a more efficient mechanism for synchronous operation, and should be
/// preferred where fully synchronous semantics are necessary.
///
/// @param           to The global address to copy to.
/// @param         from The global address to copy from.
/// @param         size The size, in bytes, of the buffer to copy
///
/// @returns  HPX_SUCCESS
int hpx_gas_memcpy_sync(hpx_addr_t to, hpx_addr_t from, size_t size)
  HPX_PUBLIC;

/// GAS collectives (hpx_gas_bcast_with_continuation).
///
/// This is a parallel call (bcast) that performs an @p action with @p
/// args on a global array with base @p base and @p n elements. The
/// output "continued" by the action is returned to the continuation
/// @p cont at the address @p caddr.
///
/// @param       action The action to run.
/// @param         base The base of the array.
/// @param            n The number of elements in the array.
/// @param       offset The offset within each element to target.
/// @param        bsize The block size for the array.
/// @param         cont The continuation.
/// @param        caddr The continuation address.
/// @param        nargs The number of arguments for the action.
/// @param          ... The addresses of each argument.
int
_hpx_gas_bcast_with_continuation(hpx_action_t action, hpx_addr_t base, int n,
                                 size_t offset, size_t bsize, hpx_action_t cont,
                                 hpx_addr_t caddr, int nargs, ...)
  HPX_PUBLIC;

#define hpx_gas_bcast_with_continuation(ACTION, BASE, N, OFFSET, BSIZE, CONT,  \
                                        CADDR, ...)                            \
  _hpx_gas_bcast_with_continuation(ACTION, BASE, N, OFFSET, BSIZE, CONT, CADDR,\
                                   __HPX_NARGS(__VA_ARGS__),##__VA_ARGS__)

/// GAS collectives (hpx_gas_bcast).
///
/// This is an asynchronous parallel call (bcast) that performs an @p
/// action with @p args on a global array with base @p base and @p n
/// elements. The output "continued" by the action, if any, is not
/// returned.
///
/// @param       ACTION The action to run.
/// @param         BASE The base of the array.
/// @param            N The number of elements in the array.
/// @param       OFFSET The offset within each element to target.
/// @param        BSIZE The block size for the array.
/// @param          ... The addresses of each argument.
///
/// @returns      HPX_SUCCESS if no errors were encountered.
#define hpx_gas_bcast(ACTION, BASE, N, OFFSET, BSIZE, ...)                   \
  _hpx_gas_bcast_with_continuation(ACTION, BASE, N, OFFSET, BSIZE,           \
                                   HPX_ACTION_NULL, HPX_NULL,                \
                                   __HPX_NARGS(__VA_ARGS__),##__VA_ARGS__)

/// GAS collectives (hpx_gas_bcast_sync).
///
/// This is a synchronous parallel call (bcast) that performs an @p
/// action with @p args on a global array with base @p base and @p n
/// elements. The output "continued" by the action, if any, is not
/// returned.
///
/// @param       action The action to run.
/// @param         base The base of the array.
/// @param            n The number of elements in the array.
/// @param       offset The offset within each element to target.
/// @param        bsize The block size for the array.
/// @param        nargs The number of arguments for the action.
/// @param          ... The addresses of each argument.
///
/// @returns      HPX_SUCCESS if no errors were encountered.
int
_hpx_gas_bcast_sync(hpx_action_t action, hpx_addr_t base, int n,
                    size_t offset, size_t bsize, int nargs, ...)
  HPX_PUBLIC;

#define hpx_gas_bcast_sync(ACTION, BASE, N, OFFSET, BSIZE, ...) \
  _hpx_gas_bcast_sync(ACTION, BASE, N, OFFSET, BSIZE,           \
                      __HPX_NARGS(__VA_ARGS__) , ##__VA_ARGS__)


/// Rebalance GAS blocks in the system. (EXPERIMENTAL).
///
/// This operation tries to rebalance blocks allocated in the global
/// address space based on the tracing information it collects at
/// runtime. All accesses to blocks marked with the HPX_GAS_ATTR_LB
/// are recorded, and when the "rebalance" operation is invoked,
/// blocks are moved automatically to come up with a better
/// distribution.
void hpx_gas_rebalance(hpx_addr_t async, hpx_addr_t psync, hpx_addr_t msync)
  HPX_PUBLIC;

/// Set the (soft) affinity of a global address to a worker ID.
///
/// Under normal conditions, global addresses have affinity to localities, but
/// parcels targeting the global addresses have no preference as to which
/// execution resource within the locality ends up executing the resulting
/// thread.
///
/// This interface allows an application to set a soft affinity preference for
/// execution for a particular global address, with the expectation that
/// implementations that support soft affinity will attempt to execute parcels
/// targeting the global address on the preferred worker. This can assist in
/// maintaining cache locality for independent-but-related threads, and with
/// scheduling.
///
/// The affinity is not guaranteed and no expectation should be made about
/// concurrency or locality between disjoint execution on the same targets.
///
/// The @p worker id is a dense integer between 0 and hpx_get_num_threads() that
/// designates the resource. Setting the affinity of a global address that
/// already has affinity is supported, though this may not impact the execution
/// of concurrent parcels or threads. Setting the affinity of a global address
/// that is not currently bound to the locality results in undefined
/// behavior. Moving a global address that is currently bound to a worker
/// results in undefined behavior. Implementations are encouraged to provide
/// debug builds that check for these conditions dynamically.
///
/// Future versions of this interface may provide the ability to set the
/// affinity for a range of addresses, to allow parcel sends to "internal"
/// addresses rather than simple unique targets.
///
/// @param         addr The address to bind.
/// @param       worker The computation resource id to bind affinity to.
void hpx_gas_set_affinity(hpx_addr_t addr, int worker)
  HPX_PUBLIC;

/// Clear the (soft) affinity for a global address.
///
/// This clears the affinity binding for a global address. It is not an error to
/// clear the affinity for a global address that is not already bound, however
/// clearing the affinity for a global address that is not mapped to the current
/// locality results in undefined behavior.
///
/// Clearing affinity may have no impact on concurrent parcels or
/// threads. Clearing affinity is necessary before moving a global address
/// explicitly or through implicit load balancing.
///
/// @param         addr The address to clear.
void hpx_gas_clear_affinity(hpx_addr_t addr)
  HPX_PUBLIC;

/// @}

#ifdef __cplusplus
}
#endif

#endif
