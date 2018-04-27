// ==================================================================-*- C++ -*-
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

#ifndef LIBHPX_PARCEL_H
#define LIBHPX_PARCEL_H

#ifdef HAVE_APEX
# include <apex.h>
# include <apex_policies.h>
#endif

#include <hpx/hpx.h>
#include <libhpx/instrumentation.h>

// @todo Update the parcel implementation to C++.

namespace libhpx {
namespace scheduler {
class Thread;
} // namespace scheduler
} // namespace libhpx

typedef uint16_t parcel_state_t;

static const parcel_state_t      PARCEL_SERIALIZED = UINT16_C(0x1 << 1);
static const parcel_state_t        PARCEL_RETAINED = UINT16_C(0x1 << 2);
static const parcel_state_t          PARCEL_NESTED = UINT16_C(0x1 << 3);
static const parcel_state_t PARCEL_BLOCK_ALLOCATED = UINT16_C(0x1 << 4);
static const parcel_state_t          PARCEL_PINNED = UINT16_C(0x1 << 5);

void parcel_pin(hpx_parcel_t *p);
void parcel_nest(hpx_parcel_t *p);
void parcel_retain(hpx_parcel_t *p);
void parcel_release(hpx_parcel_t *p);

static inline uint16_t parcel_serialized(parcel_state_t state) {
  return state & PARCEL_SERIALIZED;
}

static inline uint16_t parcel_retained(parcel_state_t state) {
  return state & PARCEL_RETAINED;
}

static inline uint16_t parcel_nested(parcel_state_t state) {
  return state & PARCEL_NESTED;
}

static inline uint16_t parcel_block_allocated(parcel_state_t state) {
  return state & PARCEL_BLOCK_ALLOCATED;
}

static inline uint16_t parcel_pinned(parcel_state_t state) {
  return state & PARCEL_PINNED;
}

/// The hpx_parcel structure is what the user-level interacts with.
///
struct hpx_parcel {
  using Thread = libhpx::scheduler::Thread;

  Thread*          thread;         //!< A pointer to a thread.
  struct hpx_parcel *next;         //!< A pointer to the next parcel.
  uint32_t            src;         //!< The src rank for the parcel.
  uint32_t           size;         //!< The data size in bytes.
  parcel_state_t    state;         //!< The parcel's state bits.
  uint16_t         offset;         //!< Reserved for future use.
  hpx_action_t     action;         //!< The target action identifier.
  hpx_action_t   c_action;         //!< The continuation action identifier.
  hpx_addr_t       target;         //!< The target address for parcel_send().
  hpx_addr_t     c_target;         //!< The target address for the continuation.
  hpx_pid_t           pid;         //!< The process ID.
  uint64_t         credit;         //!< Credit held by the parcel.
#ifdef ENABLE_INSTRUMENTATION
  uint64_t             id;         //!< A unique identifier for parcel tracing.
  uint64_t        padding;         //!< Ensure consistent buffer alignment with
                                                //!< instrumentation.
#endif
  char           buffer[];        //!< Either an in-place payload, or a pointer.
};

// Verify an assumption about how big the parcel structure is.
#ifndef __LP64__
  #ifdef ENABLE_INSTRUMENTATION
      _HPX_ASSERT(sizeof(hpx_parcel_t) == 72, parcel_size);
  #else
      _HPX_ASSERT(sizeof(hpx_parcel_t) == 56, parcel_size);
  #endif
#else
  #ifdef ENABLE_INSTRUMENTATION
      _HPX_ASSERT(sizeof(hpx_parcel_t) == 80, parcel_size);
  #else
      _HPX_ASSERT(sizeof(hpx_parcel_t) == HPX_CACHELINE_SIZE, parcel_size);
  #endif
#endif

void parcel_init(hpx_addr_t target, hpx_action_t action, hpx_addr_t c_target,
                 hpx_action_t c_action, hpx_pid_t pid, const void *data,
                 size_t len, hpx_parcel_t *p);

hpx_parcel_t *parcel_alloc(size_t payload);

hpx_parcel_t *parcel_new(hpx_addr_t target, hpx_action_t action, hpx_addr_t c_target,
                         hpx_action_t c_action, hpx_pid_t pid, const void *data,
                         size_t len)
  HPX_MALLOC;

hpx_parcel_t *parcel_clone(const hpx_parcel_t *p)
  HPX_MALLOC;

void parcel_delete(hpx_parcel_t *p);

/// Swap the stack for a parcel.
///
/// For debugging purposes, this operation is done using an atomic exchange when
/// ENABLE_DEBUG is set.
libhpx::scheduler::Thread* parcel_set_thread(hpx_parcel_t *p,
                                             libhpx::scheduler::Thread *thread);

/// The core send operation.
///
/// This sends the parcel synchronously. This will eagerly serialize the parcel,
/// and will assign it credit from the currently executing process if it has a
/// pid set.
void parcel_launch(hpx_parcel_t *p);

/// This will launch all of the parcel in the stack of parcels.
void parcel_launch_all(hpx_parcel_t *stack);

void parcel_launch_error(hpx_parcel_t *p, int error);

void parcel_launch_through(hpx_parcel_t *p, hpx_addr_t gate);

void parcel_set_state(hpx_parcel_t *p, parcel_state_t state);

parcel_state_t parcel_get_state(const hpx_parcel_t *p);

parcel_state_t parcel_exchange_state(hpx_parcel_t *p, parcel_state_t state);

/// Treat a parcel as a stack of parcels, and pop the top.
///
/// @param[in,out] stack The address of the top parcel in the stack, modified
///                      as a side effect of the call.
///
/// @returns            NULL, or the parcel that was on top of the stack.
static inline hpx_parcel_t *parcel_stack_pop(hpx_parcel_t **stack) {
  hpx_parcel_t *top = *stack;
  if (top) {
    *stack = top->next;
    top->next = NULL;
  }
  return top;
}

/// Treat a parcel as a stack of parcels, and push the parcel.
///
/// @param[in,out] stack The address of the top parcel in the stack, modified
///                      as a side effect of the call.
/// @param[in]         p The new top of the stack.
static inline void parcel_stack_push(hpx_parcel_t **stack, hpx_parcel_t *p) {
  p->next = *stack;
  *stack = p;
}

static inline uint32_t parcel_size(const hpx_parcel_t *p) {
  return sizeof(*p) + p->size;
}

static inline uint32_t parcel_payload_size(const hpx_parcel_t *p) {
  return p->size;
}

void parcel_prepare(hpx_parcel_t *p)
  HPX_NON_NULL(1);

#endif // LIBHPX_PARCEL_H
