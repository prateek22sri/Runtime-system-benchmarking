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

#ifndef HPX_PAR_H
#define HPX_PAR_H

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup actions
/// @{

/// @file
/// @brief HPX parallel loop interface

/// The type of functions that can be passed to hpx_par_for().
///
/// These functions are invoked by HPX in parallel and if they access
/// shared data, care must be taken to synchronize the shared data
/// structures. The first argument @p i represents the current
/// iteration being executed. The second argument @p arg represents
/// the arguments passed through the hpx_par_for call.
typedef int (*hpx_for_action_t)(int i, void *arg);

/// Perform a "for" loop in parallel.
///
/// This encapsulates a simple local parallel for loop:
///
/// @code
/// for (int i = min, e = max; i < e; ++i) {
///   hpx_call(HPX_HERE, action, sync, &i, &args);
/// }
/// @endcode
///
/// The work is divided in equal chunks among the number of "worker"
/// threads available. Work is actively pushed to each worker thread
/// but is not affinitized and can be stolen by other worker threads.
///
/// @param        f The "for" loop body function.
/// @param      min The minimum index in the loop.
/// @param      max The maximum index in the loop.
/// @param     args The arguments to the for function @p f.
/// @param     sync An LCO that indicates the completion of all iterations.
///
//// @returns An error code, or HPX_SUCCESS.
int hpx_par_for(hpx_for_action_t f, int min, int max, void *args,
                hpx_addr_t sync) HPX_PUBLIC;

int hpx_par_for_sync(hpx_for_action_t f, int min, int max,
                     void *args) HPX_PUBLIC;

/// Perform a parallel call.
///
/// This encapsulates a simple parallel for loop with the following semantics.
///
/// @code
/// for (int i = min, e = max; i < e; ++i) {
///   char args[arg_size];
///   arg_init(args, i, env);
///   hpx_call(HPX_HERE, action, sync, args, arg_size);
/// }
/// @endcode
///
/// The loop may actually be spawned as a tree, in which case @p
/// branching_factor controls how many chunks each range is partitioned into,
/// and @p cutoff controls the smalled chunk that is split.
///
/// @param           action The action to perform.
/// @param              min The minimum index in the loop.
/// @param              max The maximum index in the loop.
/// @param branching_factor The tree branching factor for tree-spawn.
/// @param           cutoff The largest chunk we won't split.
/// @param         arg_size The size of the arguments to action.
/// @param         arg_init A callback to initialize the arguments
/// @param         env_size The size of the environment of arg_init.
/// @param              env An environment to pass to arg_init.
/// @param             sync An LCO to set as the continuation for each iteration.
///
/// @returns An error code, or HPX_SUCCESS.
int hpx_par_call(hpx_action_t action,
                const int min, const int max,
                const int branching_factor, const int cutoff,
                const size_t arg_size,
                void (*arg_init)(void*, const int, const void*),
                const size_t env_size, const void *env,
                hpx_addr_t sync) HPX_PUBLIC;


int hpx_par_call_sync(hpx_action_t action,
                     const int min, const int max,
                     const int branching_factor, const int cutoff,
                     const size_t arg_size,
                     void (*arg_init)(void*, const int, const void*),
                     const size_t env_size,
                     const void *env) HPX_PUBLIC;

int hpx_count_range_call(hpx_action_t action,
             const hpx_addr_t addr,
             const size_t count,
             const size_t increment,
             const uint32_t bsize,
             const size_t arg_size,
             void *const arg) HPX_PUBLIC;

/// @}

#ifdef __cplusplus
}
#endif

#endif // HPX_PAR_H
