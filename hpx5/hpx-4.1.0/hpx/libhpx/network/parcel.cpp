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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/// @brief The parcel layer.
///
/// Parcels are the foundation of HPX. The parcel structure serves as both the
/// actual, "on-the-wire," network data structure, as well as the
/// "thread-control-block" descriptor for the threading subsystem.
#include <libhpx/action.h>
#include <libhpx/attach.h>
#include <libhpx/debug.h>
#include <libhpx/events.h>
#include <libhpx/GAS.h>
#include <libhpx/instrumentation.h>
#include <libhpx/libhpx.h>
#include <libhpx/locality.h>
#include <libhpx/memory.h>
#include "libhpx/Network.h"
#include <libhpx/padding.h>
#include <libhpx/parcel.h>
#include <libhpx/Topology.h>
#include "libhpx/Worker.h"
#include <hpx/hpx.h>
#include <ffi.h>
#include <cassert>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#ifdef HAVE_APEX
#include "apex.h"
#endif

namespace {
using libhpx::self;
using libhpx::scheduler::Thread;
}

// this will only be used during instrumentation
__thread uint64_t parcel_count = 1;

static int _delete_launch_through_parcel_handler(hpx_parcel_t *p) {
  hpx_addr_t lsync = hpx_thread_current_target();
  hpx_lco_wait(lsync);
  parcel_delete(p);
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_DEFAULT, 0, _delete_launch_through_parcel,
                     _delete_launch_through_parcel_handler, HPX_POINTER);

/// Serialize and bless a parcel before sending or copying it.
void parcel_prepare(hpx_parcel_t *p) {
  parcel_state_t state = parcel_get_state(p);
  if (!parcel_serialized(state) && p->size) {
    void *buffer = hpx_parcel_get_data(p);
    memcpy(&p->buffer, buffer, p->size);
    state |= PARCEL_SERIALIZED;
    parcel_set_state(p, state);
  }

  if (p->pid && !p->credit) {
    hpx_parcel_t *parent = self->getCurrentParcel();
    dbg_assert(parent->pid == p->pid);
    p->credit = ++parent->credit;
  }
}

void parcel_set_state(hpx_parcel_t *p, parcel_state_t state) {
  __atomic_store_n(&p->state, state, __ATOMIC_RELEASE);
}

parcel_state_t parcel_get_state(const hpx_parcel_t *p) {
  return __atomic_load_n(&p->state, __ATOMIC_ACQUIRE);
}

parcel_state_t parcel_exchange_state(hpx_parcel_t *p, parcel_state_t state) {
  return __atomic_exchange_n(&p->state, state, __ATOMIC_ACQ_REL);
}

void parcel_pin(hpx_parcel_t *p) {
  parcel_state_t state = parcel_get_state(p);
  dbg_assert_str(parcel_serialized(state), "cannot pin out-of-place parcels\n");
  dbg_assert_str(!parcel_nested(state), "cannot pin nested parcels\n");
  dbg_assert_str(!parcel_retained(state), "cannot pin retained parcels\n");
  parcel_set_state(p, state | PARCEL_PINNED);
}

void parcel_nest(hpx_parcel_t *p) {
  parcel_state_t state = parcel_get_state(p);
  dbg_assert_str(parcel_serialized(state), "cannot nest out-of-place parcels\n");
  dbg_assert_str(!parcel_pinned(state), "cannot nest pinned parcels\n");
  dbg_assert_str(!parcel_retained(state), "cannot nest retained parcels\n");
  parcel_set_state(p, state | PARCEL_NESTED);
}

void parcel_retain(hpx_parcel_t *p) {
  parcel_state_t state = parcel_get_state(p);
  dbg_assert_str(parcel_serialized(state), "cannot retain out-of-place parcels\n");
  dbg_assert_str(!parcel_pinned(state), "cannot retain pinned parcels\n");
  dbg_assert_str(!parcel_nested(state), "cannot retain nested parcels\n");
  parcel_set_state(p, state | PARCEL_RETAINED);
}

void parcel_release(hpx_parcel_t *p) {
  parcel_state_t state = parcel_get_state(p);
  dbg_assert_str(parcel_retained(state), "can only release retained parcels\n");
  parcel_set_state(p, state & ~PARCEL_RETAINED);
}

void parcel_launch(hpx_parcel_t *p) {
  dbg_assert(p->action);

  parcel_prepare(p);

  log_parcel("PID:%" PRIu64 " CREDIT:%" PRIu64 " %s(%p,%u)@(%" PRIu64
             ") => %s@(%" PRIu64 ")\n",
             p->pid,
             p->credit,
             actions[p->action].key,
             hpx_parcel_get_data(p),
             p->size,
             p->target,
             actions[p->c_action].key,
             p->c_target);

  EVENT_PARCEL_SEND(p->id, p->action, p->size,
                    hpx_thread_current_target(),
                    p->target);

  // do a local send through loopback, bypassing the network, otherwise dump the
  // parcel out to the network
  uint32_t target = here->gas->ownerOf(p->target);
  if (target == here->rank) {
    // instrument local "receives"
    EVENT_PARCEL_RECV(p->id, p->action, p->size, p->src, p->target);
    self->spawn(p);
  }
  else {
    int e = here->net->send(p, NULL);
#ifdef HAVE_APEX
    apex_send(p->id, p->size, target);
#endif
    dbg_check(e, "failed to perform a network send\n");
  }
}

void
parcel_launch_all(hpx_parcel_t* stack)
{
  while (auto p = parcel_stack_pop(&stack)) {
    parcel_launch(p);
  }
}

void parcel_launch_error(hpx_parcel_t *p, int error) {
  if (error != HPX_SUCCESS) {
    dbg_error("Launching en error is not yet implemented");
  }
  parcel_launch(p);
}

void parcel_launch_through(hpx_parcel_t *p, hpx_addr_t gate) {
  dbg_assert(gate);
  parcel_prepare(p);
  hpx_pid_t pid = hpx_thread_current_pid();
  p = parcel_new(gate, lco_attach, 0, 0, pid, p, parcel_size(p));
  parcel_launch(p);
}

void parcel_init(hpx_addr_t target, hpx_action_t action, hpx_addr_t c_target,
                 hpx_action_t c_action, hpx_pid_t pid, const void *data,
                 size_t len, hpx_parcel_t *p)
{
  p->thread   = nullptr;
  p->next     = nullptr;
  p->src      = here->rank;
  p->size     = len;
  p->offset   = 0;
  p->action   = action;
  p->c_action = c_action;
  p->target   = target;
  p->c_target = c_target;
  p->pid      = pid;
  p->credit   = 0;

#ifdef ENABLE_INSTRUMENTATION
  //if (inst_trace_class(HPX_TRACE_PARCEL) && self) {
  if (self) {
    parcel_count++;
    int rank   = HPX_LOCALITY_ID;
    int thread = HPX_THREAD_ID;
    p->id = Topology::topo_offset_to_value(rank, thread, parcel_count);
  } else {
    p->id = 0;
  }
#endif

  // If there's a user-defined buffer, then remember it---we'll serialize it
  // later, during the send operation.
  if (data && len) {
    // use memcpy to preserve strict aliasing
    memcpy(&p->buffer, &data, sizeof(data));
    parcel_set_state(p, 0);
  }
  else {
    parcel_set_state(p, PARCEL_SERIALIZED);
  }
}

hpx_parcel_t *parcel_alloc(size_t payload) {
  size_t size = sizeof(hpx_parcel_t);
  if (payload != 0) {
    size += max_size_t(sizeof(void*), payload);
    size += _BYTES(8, size);
  }

  auto p = static_cast<hpx_parcel_t *>(as_memalign(AS_REGISTERED, HPX_CACHELINE_SIZE, size));
  dbg_assert_str(p, "parcel: failed to allocate %zu registered bytes.\n", size);
#ifdef ENABLE_INSTRUMENTATION
  *(uint64_t*)&p->padding = UINT64_C(0);        // initialize read-only padding
#endif
  return p;
}

hpx_parcel_t *parcel_new(hpx_addr_t target, hpx_action_t action,
                         hpx_addr_t c_target, hpx_action_t c_action,
                         hpx_pid_t pid, const void *data, size_t len) {
  hpx_parcel_t* p = parcel_alloc(len);
  parcel_init(target, action, c_target, c_action, pid, data, len, p);
  if (libhpx::Worker* w = libhpx::self) {
    EVENT_PARCEL_CREATE(p->id, p->action, p->size, w->getCurrentParcel()->id);
  }
  else {
    EVENT_PARCEL_CREATE(p->id, p->action, p->size, 0);
  }
  return p;
}

hpx_parcel_t *parcel_clone(const hpx_parcel_t *p) {
  dbg_assert(parcel_serialized(parcel_get_state(p)) || p->size == 0);
  size_t n = parcel_size(p);
  auto clone = static_cast<hpx_parcel_t *>(as_memalign(AS_REGISTERED, HPX_CACHELINE_SIZE, n));
  memcpy(clone, p, n);
  clone->thread = nullptr;
  clone->next = nullptr;
  parcel_set_state(clone, PARCEL_SERIALIZED);
  return clone;
}

void parcel_delete(hpx_parcel_t *p) {
  if (!p) {
    return;
  }

  parcel_state_t state = parcel_get_state(p);

  if (unlikely(parcel_retained(state))) {
    return;
  }

  if (unlikely(parcel_nested(state))) {
    size_t n = parcel_size(p);
    auto parent = reinterpret_cast<hpx_parcel_t*>((char*)p - sizeof(*p));
    size_t m = parent->size;
    if (n != m) {
      dbg_error("expected payload of %zu for parent, saw %zu\n", n, m);
    }
    parcel_delete(parent);
    return;
  }

  if (unlikely(parcel_pinned(state))) {
    state &= ~PARCEL_PINNED;
    state = parcel_exchange_state(p, state);
  }

  if (unlikely(parcel_pinned(state))) {
    return;
  }

  if (parcel_block_allocated(state)) {
    dbg_assert(parcel_serialized(state));
    here->net->deallocate(p);
    return;
  }

  EVENT_PARCEL_DELETE(p->id, p->action);
  as_free(AS_REGISTERED, p);
}

Thread* parcel_set_thread(hpx_parcel_t *p, Thread *next) {
  // This can detect races in the scheduler when two threads try and process the
  // same parcel.
#ifdef ENABLE_DEBUG
    return __atomic_exchange_n(&p->thread, next, __ATOMIC_ACQ_REL);
#else
    std::swap(p->thread, next);
    return next;
#endif
}
