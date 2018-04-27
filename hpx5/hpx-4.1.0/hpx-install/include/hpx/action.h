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

#ifndef HPX_ACTION_H
#define HPX_ACTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hpx/builtins.h>

/// @defgroup actions Actions and threads
/// @brief Functions and definitions for registering, calling, and controlling
///        actions and threads
/// @{

/// @file
/// @brief Types and functions for registering HPX actions.

/// The handle type for HPX actions.  This handle is obtained via
/// hpx_register_action(). It is safe to use this handle only after a
/// call to hpx_run(). It is used as a parameter type for any HPX
/// function that needs an action (e.g.  hpx_run(), hpx_call(),
/// hpx_parcel_set_action()).
typedef uint16_t hpx_action_t;

/// The type of functions that can be registered with hpx_register_action().
typedef int (*hpx_action_handler_t)(void*, size_t);

/// The type of functions that can be registered with pinned actions.
typedef int (*hpx_pinned_action_handler_t)(void *, void*, size_t);

/// The type of functions that can be registered with vectored actions.
typedef int (*hpx_vectored_action_handler_t)(int, void*, size_t *);

/// The type of functions that can be registered with pinned vectored actions.
typedef int (*hpx_pinned_vectored_action_handler_t)(void *, int, void*, size_t *);

/// The equivalent of NULL for HPX actions.
#define HPX_ACTION_NULL ((hpx_action_t)0u)

/// An invalid action
#define HPX_ACTION_INVALID ((hpx_action_t)UINT16_MAX)

/// Action types.
typedef enum {
  /// Standard action that is scheduled and has its own stack.
  HPX_DEFAULT = 0,
  /// Tasks are threads that do not block.
  HPX_TASK,
  /// Interrupts are simple actions that have function call semantics.
  HPX_INTERRUPT,
  /// Functions are simple functions that have uniform ids across localities,
  /// but can not be called with the set of hpx_call operations or as the action
  /// or continuation in a parcel.
  HPX_FUNCTION,
  /// Action that runs OpenCL kernels
  HPX_OPENCL,
} hpx_action_type_t;

/// @name Action attributes.
/// These attributes control aspects of actions.  Attributes can be combined
/// using bitwise or.
//@{
// Null attribute.
#define HPX_ATTR_NONE  0x0
// Action takes a pointer to marshalled arguments and their size.
#define HPX_MARSHALLED 0x1
// Action automatically pins memory.
#define HPX_PINNED     0x2
// Action is a libhpx action
#define HPX_INTERNAL   0x4
// Action is a vectored action
#define HPX_VECTORED   0x8
// Action is a coalesced action
#define HPX_COALESCED 0x10
// Action is a compressed action
#define HPX_COMPRESSED 0x20
//@}

/// Register an HPX action of a given @p type.
///
/// This must be called prior to hpx_init().
///
/// @param  type The type of the action to be registered.
/// @param  attr The attribute of the action (PINNED, PACKED, ...).
/// @param   key A unique string key for the action.
/// @param    id The action id for this action to be returned after
/// @param     n The variadic number of parameters that the action accepts.
/// @param   ... The HPX types of the action parameters (HPX_INT, ...).
///
/// @returns     HPX_SUCCESS or an error code
int hpx_register_action(hpx_action_type_t type, uint32_t attr, const char *key,
                        hpx_action_t *id, unsigned n, ...)
  HPX_PUBLIC;

/// Wraps the hpx_register_action() function to make it slightly
/// more convenient to use.
///
/// @param        type The type of the action (THREAD, TASK, INTERRUPT, ...).
/// @param        attr The attribute of the action (PINNED, PACKED, ...).
/// @param          id The action id (the hpx_action_t address).
#define HPX_REGISTER_ACTION(type, attr, id, ...)                    \
  hpx_register_action(type, attr, __FILE__ ":" _HPX_XSTR(id),       \
                      &id, __HPX_NARGS(__VA_ARGS__), ##__VA_ARGS__)

/// Declare an action.
///
/// This doesn't actually do anything interesting, but if we ever needed to
/// mangle the symbol then we would do it here. This can be prefixed with a
/// storage modifier (i.e., extern, static).
///
/// @param       symbol The symbol for the action.
#define HPX_ACTION_DECL(symbol) hpx_action_t symbol

/// Create an action id for a function, so that it can be called asynchronously.
///
/// This macro handles all steps of creating a usable action. It declares the
/// identifier and registers an action in a static constructor.  The static
/// automates action registration, eliminating the need for explicit action
/// registration.
///
/// Note that the macro can be preceded by the \c static keyword if the action
/// should only be visible in the current file.
///
/// @param         type The action type.
/// @param         attr The action attributes.
/// @param      handler The handler.
/// @param           id The action id.
#define HPX_ACTION(type, attr, id, handler, ...)                  \
  HPX_ACTION_DECL(id) = HPX_ACTION_INVALID;                       \
  static HPX_CONSTRUCTOR void _register##_##id(void) {            \
    HPX_REGISTER_ACTION(type, attr, id, handler , ##__VA_ARGS__); \
  }                                                               \
  static HPX_CONSTRUCTOR void _register##_##id(void)

/// Get the handler associated with a given action id.
hpx_action_handler_t hpx_action_get_handler(hpx_action_t id)
  HPX_PUBLIC;

/// @}

#ifdef __cplusplus
}
#endif

#endif
