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

#ifndef LIBHPX_ACTION_H
#define LIBHPX_ACTION_H

#ifndef __cplusplus
#error
#endif


#include "hpx/hpx.h"
#include <cstdarg>
#include <cstdint>

extern "C" {

/// Generic action handler type.
typedef void (*handler_t)(void);

/// A set of handlers that defines how to call an action in a certain context.
typedef struct {
  /// Fully asynchronous action invocation.
  ///
  /// This interface invokes the action at the @p addr, and sets the (@p rsync,
  /// @p rop) closure as the continuation. The (@p lsync, @p lop) continuation
  /// will be invoked once the @p args are safe to modify or deallocate.
  ///
  /// @param          o The action object.
  /// @param       addr The target for the invocation.
  /// @param      lsync The target for the local completion event handler.
  /// @param        lop The local completion event handler.
  /// @param      rsync The target for the remote continuation.
  /// @param        rop The continuation event handler.
  /// @param          n The number of @p args.
  /// @param       args The list of arguments for the action.
  ///
  /// @return           HPX_SUCCESS or an error code
  int (*async)(const void *o, hpx_addr_t addr,
               hpx_addr_t lsync, hpx_action_t lop,
               hpx_addr_t rsync, hpx_action_t rop,
               int n, va_list *args);

  /// Locally synchronous action invocation.
  ///
  /// This interface invokes the action at the @p addr, and sets the (@p rsync,
  /// @p rop) closure as the continuation. It is safe to modify or free @p args
  /// once this function returns.
  ///
  /// @param          o The action object.
  /// @param       addr The target for the invocation.
  /// @param      rsync The target for the remote continuation.
  /// @param        rop The continuation event handler.
  /// @param          n The number of @p args.
  /// @param       args The list of arguments for the action.
  ///
  /// @return           HPX_SUCCESS or an error code
  int (*lsync)(const void *o, hpx_addr_t addr,
               hpx_addr_t rsync, hpx_action_t rop,
               int n, va_list *args);

  /// Synchronous remote procedure call.
  ///
  /// This interface invokes the action at the @p addr, and returns once the
  /// action has completed. Any data produced by the action will be written into
  /// the @p rout buffer, and it is safe to modify or free @p args once the call
  /// returns.
  ///
  /// @param          o The action object.
  /// @param       addr The target for the invocation.
  /// @param       rout The buffer for the return value.
  /// @param     rbytes The size of the return buffer.
  /// @param          n The number of @p args.
  /// @param       args The list of arguments for the action.
  ///
  /// @return           HPX_SUCCESS or an error code
  int (*rsync)(const void *o, hpx_addr_t addr,
               void *rout, size_t rbytes,
               int n, va_list *args);

  /// Fully asynchronous dependent action invocation.
  ///
  /// This interface invokes the action at the @p addr when the @p gate is set,
  /// and sets the (@p rsync, @p rop) closure as the continuation. The (@p
  /// lsync, @p lop) continuation will be invoked once the @p args are safe to
  /// modify or deallocate.
  ///
  /// @param          o The action object.
  /// @param       addr The target for the invocation.
  /// @param       gate The LCO that the invocation is dependent on.
  /// @param      lsync The target for the local completion event handler.
  /// @param        lop The local completion event handler.
  /// @param      rsync The target for the remote continuation.
  /// @param        rop The continuation event handler.
  /// @param          n The number of @p args.
  /// @param       args The list of arguments for the action.
  ///
  /// @return           HPX_SUCCESS or an error code
  int (*when_async)(const void *o, hpx_addr_t addr, hpx_addr_t gate,
                    hpx_addr_t lsync, hpx_action_t lop,
                    hpx_addr_t rsync, hpx_action_t rop,
                    int n, va_list *args);

  /// Locally synchronous dependent action invocation.
  ///
  /// This interface invokes the action at the @p addr when the @p gate is set,
  /// and sets the (@p rsync, @p rop) closure as the continuation. It is safe to
  /// modify or free @p args once this function returns.
  ///
  /// @param          o The action object.
  /// @param       addr The target for the invocation.
  /// @param       gate The LCO that the invocation is dependent on.
  /// @param      rsync The target for the remote continuation.
  /// @param        rop The continuation event handler.
  /// @param          n The number of @p args.
  /// @param       args The list of arguments for the action.
  ///
  /// @return           HPX_SUCCESS or an error code
  int (*when_lsync)(const void *o, hpx_addr_t addr, hpx_addr_t gate,
                    hpx_addr_t rsync, hpx_action_t rop,
                    int n, va_list *args);

  /// Synchronous dependent remote procedure call.
  ///
  /// This interface invokes the action at the @p addr when the @p gate is set,
  /// and returns once the action has completed. Any data produced by the action
  /// will be written into the @p rout buffer, and it is safe to modify or free
  /// @p args once the call returns.
  ///
  /// @param          o The action object.
  /// @param       addr The target for the remote procedure call.
  /// @param       gate The LCO that the call is dependent on.
  /// @param       rout The buffer for the return value.
  /// @param     rbytes The size of the return buffer.
  /// @param          n The number of @p args.
  /// @param       args The list of arguments for the action.
  ///
  /// @return           HPX_SUCCESS or an error code
  int (*when_rsync)(const void *o, hpx_addr_t addr, hpx_addr_t gate,
                    void *rout, size_t rbytes,
                    int n, va_list *args);

  /// Generate a continuation.
  ///
  /// This is mostly equivalent to the lsync() interface with a NULL (rsync,
  /// rop), except that it uses the credit from the current thread to run and
  /// doesn't require the rsync operations.
  ///
  /// @param          o The action object.
  /// @param          p The current parcel---necessary for the credit transfer.
  /// @param          n The number of @p args.
  /// @param       args The arguments for the continuation.
  ///
  /// @return           HPX_SUCCESS or an error condition if generating the
  ///                   continuation causes an error.
  int (*cont)(const void *o, hpx_parcel_t *p, int n, va_list *args);

} calling_convention_vtable_t;

typedef struct {
  /// Execute a parcel.
  ///
  /// This encapsulates the invocation of a parcel for a particular action. The
  /// method for invoking an action depends on the action type, e.g., an
  /// ffi-typed action will use libffi to call the action handler while a
  /// marshaled C action will simply cast the handler and call it with the
  /// parcel's data buffer directly.
  ///
  /// @param        obj The action object.
  /// @param          p The parcel to call.
  ///
  /// @return           The result of the user handler.
  int (*exec_parcel)(const void *obj, hpx_parcel_t *p);

  /// Pack arguments into the parcel buffer.
  ///
  /// Parcel data must be packed into the parcel buffer, however the actual
  /// mechanism used for this operation is action type specific, e.g., an ffi
  /// action will use libffi to pack the arguments, while a marshaled C action
  /// may simply memcpy the argument data.
  ///
  /// @param        obj The action object.
  /// @param          p The parcel to pack.
  void (*pack_parcel)(const void *obj, hpx_parcel_t *p, int n, va_list *args);

  /// Create a parcel for an action.
  ///
  /// This will allocate, initialize, and pack a parcel in an action-specific
  /// way. It is *not* safe to deallocate or free the arguments after this call,
  /// though it is safe to finalize the va_list that they are passed in.
  ///
  /// @param        obj The action object.
  /// @param       addr The target address of the parcel.
  /// @param      rsync The continuation target.
  /// @param        rop The continuation operation.
  /// @param          n The number of @p args.
  /// @param       args The list of args for the parcel.
  ///
  /// @return           The newly allocated parcel, or NULL if there was a
  ///                   problem during allocation.
  hpx_parcel_t *(*new_parcel)(const void *obj, hpx_addr_t addr,
                              hpx_addr_t rsync, hpx_action_t rop, int n,
                              va_list *args);

  /// Exit a thread.
  ///
  /// This is used to provide an action a chance to clean up (e.g., unpin the
  /// target) before exiting through a non-local control transfer.
  ///
  /// @param          o The action object.
  /// @param          p The current parcel.
  void (*exit)(const void *o, hpx_parcel_t *p);

} parcel_management_vtable_t;

/// An action table action type.
///
/// The action type uses basic multiple inheritance to implement the calling
/// convention and call interfaces. Action types are allocated in an array, so
/// their records contain the union of all of the fields that all of the
/// different actions might need.
typedef struct {
  const parcel_management_vtable_t *parcel_class; //!< The parcel management
                                                  //!< vtable pointer.
  const calling_convention_vtable_t  *call_class; //!< The calling convention
                                                  //!< vtable pointer.
  void (*finish)(void*);                          //!< Called to finish
                                                  //!< registration.
  void (*fini)(void*);                            //!< The destructor.

  handler_t      handler; //!< The action handler function pointer.
  hpx_action_t       *id; //!< The pointer to the action id.
  const char        *key; //!< The pointer to the unique key for the action.
  hpx_action_type_t type; //!< The action type.
  uint32_t          attr; //!< Type attributes.
  void              *env; //!< Type-specific data (e.g., compiled OpenCL).
} action_t;

/// The libhpx action table.
///
/// This is overkill in a way, as we allocate 2^16 action records but are almost
/// certainly only using hundreds of them. We're careful to allocate them in the
/// .bss segment so that they don't take up space in the binary, and we
/// currently make the decision that we can afford to blow a couple of MB of
/// address space.
///
/// @{
#define LIBHPX_ACTION_MAX (UINT32_C(1) << (sizeof(hpx_action_t) * 8))

extern action_t actions[LIBHPX_ACTION_MAX] HPX_ALIGNED(HPX_PAGE_SIZE);
/// @}

#ifdef ENABLE_DEBUG
void CHECK_ACTION(hpx_action_t id);
#else
#define CHECK_ACTION(id)
#endif

/// Register an HPX action of a given @p type. This is similar to the
/// hpx_register_action routine, except that it gives us the chance to "tag"
/// LIBHPX actions in their own way. This can be useful for instrumentation or
/// optimization later. This must be called before hpx_init().
///
/// @param  type The type of the action to be registered.
/// @param  attr The attribute of the action (PINNED, PACKED, ...).
/// @param   key A unique string key for the action.
/// @param    id The action id for this action to be returned after
///                registration.
/// @param     n The variadic number of parameters that the action accepts.
/// @param   ... The HPX types of the action parameters (HPX_INT, ...).
///
void libhpx_register_action(hpx_action_type_t type, uint32_t attr,
                           const char *key, hpx_action_t *id, unsigned n, ...);

/// Called when all of the actions have been registered.
void action_registration_finalize(void);

/// Called to free any internal data allocated by the actions.
void action_table_finalize(void);

/// Report the number of actions registerd in the table
int action_table_size(void);

static inline int action_exec_parcel(hpx_action_t id, hpx_parcel_t *p) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return action->parcel_class->exec_parcel(action, p);
}

static inline void action_pack_parcel_va(hpx_action_t id, hpx_parcel_t *p,
                                         int n, va_list *args) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return action->parcel_class->pack_parcel(action, p, n, args);
}

static inline hpx_parcel_t *action_new_parcel_va(hpx_action_t id,
                                                 hpx_addr_t addr,
                                                 hpx_addr_t rsync,
                                                 hpx_action_t rop,
                                                 int n, va_list *args) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return action->parcel_class->new_parcel(action, addr, rsync, rop, n, args);
}

static inline hpx_parcel_t *action_new_parcel(hpx_action_t id, hpx_addr_t addr,
                                              hpx_addr_t rsync,
                                              hpx_action_t rop, int n, ...) {
  va_list args;
  va_start(args, n);
  hpx_parcel_t *p = action_new_parcel_va(id, addr, rsync, rop, n, &args);
  va_end(args);
  return p;
}

static inline int action_call_async_va(hpx_action_t id, hpx_addr_t addr,
                                       hpx_addr_t lsync, hpx_action_t lop,
                                       hpx_addr_t rsync, hpx_action_t rop,
                                       int n, va_list *args) {
  CHECK_ACTION(id);
  const action_t *act = &actions[id];
  return act->call_class->async(act, addr, lsync, lop, rsync, rop, n, args);
}

static inline int action_call_async(hpx_action_t id, hpx_addr_t addr,
                                    hpx_addr_t lsync, hpx_action_t lop,
                                    hpx_addr_t rsync, hpx_action_t rop,
                                    int n, ...) {
  va_list args;
  va_start(args, n);
  int e = action_call_async_va(id, addr, lsync, lop, rsync, rop, n, &args);
  va_end(args);
  return e;
}

static inline int action_call_lsync_va(hpx_action_t id, hpx_addr_t addr,
                                       hpx_addr_t rsync, hpx_action_t rop,
                                       int n, va_list *args) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return action->call_class->lsync(action, addr, rsync, rop, n, args);
}

static inline int action_call_lsync(hpx_action_t id, hpx_addr_t addr,
                                    hpx_addr_t rsync, hpx_action_t rop,
                                    int n, ...) {
  va_list args;
  va_start(args, n);
  int e = action_call_lsync_va(id, addr, rsync, rop, n, &args);
  va_end(args);
  return e;
}

static inline int action_call_rsync_va(hpx_action_t id, hpx_addr_t addr,
                                       void *rout, size_t rbytes, int n,
                                       va_list *args) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return action->call_class->rsync(action, addr, rout, rbytes, n, args);
}

static inline int action_call_rsync(hpx_action_t id, hpx_addr_t addr,
                                    void *rout, size_t rbytes, int n, ...) {
  va_list args;
  va_start(args, n);
  int e = action_call_rsync_va(id, addr, rout, rbytes, n, &args);
  va_end(args);
  return e;
}

static inline int action_when_async_va(hpx_action_t id, hpx_addr_t addr,
                                       hpx_addr_t gate, hpx_addr_t lsync,
                                       hpx_action_t lop, hpx_addr_t rsync,
                                       hpx_action_t rop, int n, va_list *args) {
  CHECK_ACTION(id);
  const action_t *act = &actions[id];
  return act->call_class->when_async(act, addr, gate, lsync, lop, rsync, rop, n,
                                     args);
}


static inline int action_when_lsync_va(hpx_action_t id, hpx_addr_t addr,
                                       hpx_addr_t gate, hpx_addr_t rsync,
                                       hpx_action_t rop, int n, va_list *args) {
  CHECK_ACTION(id);
  const action_t *act = &actions[id];
  return act->call_class->when_lsync(act, addr, gate, rsync, rop, n, args);
}

static inline int action_when_rsync_va(hpx_action_t id, hpx_addr_t addr,
                                       hpx_addr_t gate, void *rout,
                                       size_t rbytes, int n, va_list *args) {
  CHECK_ACTION(id);
  const action_t *act = &actions[id];
  return act->call_class->when_rsync(act, addr, gate, rout, rbytes, n, args);
}

static inline int action_continue_va(hpx_action_t id, hpx_parcel_t *p, int n,
                                     va_list *args) {
  CHECK_ACTION(id);
  const action_t *act = &actions[id];
  return act->call_class->cont(act, p, n, args);
}

static inline void action_exit(hpx_action_t id, hpx_parcel_t *p) {
  CHECK_ACTION(id);
  const action_t *act = &actions[id];
  act->parcel_class->exit(act, p);
}

static const char* const HPX_ACTION_ATTR_TO_STRING[] = {
  "NONE",
  "MARSHALLED",
  "PINNED",
  "INTERNAL",
  "VECTORED",
  "COALESCED",
  "COMPRESSED"
};

static inline bool action_is_pinned(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->attr & HPX_PINNED);
}

static inline bool action_is_marshalled(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->attr & HPX_MARSHALLED);
}

static inline bool action_is_vectored(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->attr & HPX_VECTORED);
}

static inline bool action_is_internal(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->attr & HPX_INTERNAL);
}

static inline bool action_is_coalesced(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->attr & HPX_COALESCED);
}

static inline bool action_is_compressed(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->attr & HPX_COMPRESSED);
}

static const char* const HPX_ACTION_TYPE_TO_STRING[] = {
  "DEFAULT",
  "TASK",
  "INTERRUPT",
  "FUNCTION",
  "OPENCL"
};

static inline bool action_is_default(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->type == HPX_DEFAULT);
}

static inline bool action_is_task(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->type == HPX_TASK);
}

static inline bool action_is_interrupt(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->type == HPX_INTERRUPT);
}

static inline bool action_is_function(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->type == HPX_FUNCTION);
}

static inline bool action_is_opencl(hpx_action_t id) {
  CHECK_ACTION(id);
  const action_t *action = &actions[id];
  return (action->type == HPX_OPENCL);
}

/// Wraps the libhpx_register_action() function to make it slightly
/// more convenient to use.
///
/// @param        type The type of the action (THREAD, TASK, INTERRUPT, ...).
/// @param        attr The attribute of the action (PINNED, PACKED, ...).
/// @param          id The action id (the hpx_action_t address).
#define LIBHPX_REGISTER_ACTION(type, attr, id,  ...)                    \
  libhpx_register_action(type, attr, __FILE__ ":" _HPX_XSTR(id),        \
                         &id, __HPX_NARGS(__VA_ARGS__), ##__VA_ARGS__)

/// Create an action id for a function, so that it can be called asynchronously.
///
/// This macro handles all steps of creating a usable action. It declares the
/// identifier and registers an action in a static constructor.  The static
/// automates action registration, eliminating the need for explicit action
/// registration.
///
/// Note that the macro can be preceded by the \c static keyword if the action
/// should only be visible in the current file.  This macro is intended to be
/// used only for actions that are part of libhpx.
///
/// @param         type The action type.
/// @param         attr The action attributes.
/// @param           id The action id.
/// @param      handler The handler.
#define LIBHPX_ACTION(type, attr, id, handler, ...)                  \
  HPX_ACTION_DECL(id) = HPX_ACTION_INVALID;                          \
  static HPX_CONSTRUCTOR void _register##_##id(void) {               \
    LIBHPX_REGISTER_ACTION(type, attr, id, handler , ##__VA_ARGS__); \
  }                                                                  \
  static HPX_CONSTRUCTOR void _register##_##id(void)

}

#endif // LIBHPX_ACTION_H
