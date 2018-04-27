// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2016, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifdef CONFIG_H
# include "config.h"
#endif

/// @file libsync/chase_lev_ws_dequeue.c
///
/// A workstealing deque implementation based on the design presented in
/// "Dynamic Circular Work-Stealing Deque" by David Chase and Yossi Lev
/// @url http://dl.acm.org/citation.cfm?id=1073974.
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <hpx/builtins.h>
#include <libsync/deques.h>
#include <libsync/sync.h>

/// A Chase-Lev WS Deque buffer.
///
/// The buffer contains its capacity, and an inline block for the buffer
/// itself. Buffers can't be resized themselves, they can only be replaced with
/// larger or smaller buffers.
typedef struct chase_lev_ws_deque_buffer {
  struct chase_lev_ws_deque_buffer *parent;
  uint32_t capacity;
  uint32_t     bits;
  uint64_t     mask;
  void      *buffer[];
} _buffer_t;

/// Allocate a new buffer of the right capacity.
static HPX_NOINLINE _buffer_t *_buffer_new(_buffer_t *parent, uint32_t capacity)
{
  assert(capacity > 0);

  // best-fit power of two
  uint32_t bits = ceil_log2_32(capacity);
  capacity = 1 << bits;
  _buffer_t *b = malloc(sizeof(_buffer_t) + capacity * sizeof(void*));
  assert(b);
  memset(&b->buffer, 0, capacity * sizeof(void*));
  b->parent = parent;
  b->capacity = capacity;
  b->bits = bits;
  b->mask = capacity - 1;
  return b;
}

/// Delete a buffer, and all of its parents.
static HPX_NOINLINE void _buffer_delete(_buffer_t *b) {
  _buffer_t *parent = b->parent;
  free(b);
  if (parent) {
    _buffer_delete(parent);
  }
}

/// Insert into the buffer, modulo its capacity.
static void _buffer_put(_buffer_t *b, uint64_t i, void *val) {
  b->buffer[i & b->mask] = val;
}

/// Lookup in the buffer, modulo its capacity.
static void *_buffer_get(_buffer_t *b, uint64_t i) {
  return b->buffer[i & b->mask];
}

/// Grow a Chase-Lev WS Deque buffer.
///
/// @param    old - the current buffer
/// @param bottom - the deque bottom index
/// @param    top - the deque top index
/// @returns      - the new buffer
static HPX_NOINLINE
_buffer_t *_buffer_grow(_buffer_t *old, uint64_t bottom, uint64_t top) {
  _buffer_t *new = _buffer_new(old, 2 * old->capacity);
  for (; top < bottom; ++top) {
    _buffer_put(new, top, _buffer_get(old, top));
  }
  return new;
}

/// Utility functions to set and CAS deque fields.
///
/// We don't have _get versions because we use different memory consistency
/// models for these in different places. Sets and CASes are always releases
/// though, based on how they are used.
///
/// @{
static void _deque_set_bottom(chase_lev_ws_deque_t *deque, uint64_t val) {
  sync_store(&deque->bottom, val, SYNC_RELEASE);
}

static void _deque_set_buffer(chase_lev_ws_deque_t *deque, _buffer_t *buffer) {
  sync_store(&deque->buffer, buffer, SYNC_RELEASE);
}

static void _deque_set_top(chase_lev_ws_deque_t *deque, uint64_t top) {
  sync_store(&deque->top, top, SYNC_RELEASE);
}

static bool _deque_try_inc_top(chase_lev_ws_deque_t *deque, uint64_t top) {
  return sync_cas(&deque->top, &top, top + 1, SYNC_RELEASE, SYNC_RELAXED);
}
/// @}

chase_lev_ws_deque_t *sync_chase_lev_ws_deque_new(uint32_t size) {
  chase_lev_ws_deque_t *deque = malloc(sizeof(*deque));
  if (deque) {
    sync_chase_lev_ws_deque_init(deque, size);
  }
  return deque;
}

void sync_chase_lev_ws_deque_init(chase_lev_ws_deque_t *d, uint32_t capacity) {
  _buffer_t *buffer = _buffer_new(NULL, capacity);
  assert(buffer);

  _deque_set_bottom(d, 1);
  _deque_set_top(d, 1);
  _deque_set_buffer(d, buffer);

  d->top_bound = 1;
}

void sync_chase_lev_ws_deque_fini(chase_lev_ws_deque_t *d) {
  _buffer_t *buffer = sync_load(&d->buffer, SYNC_RELAXED);
  if (buffer) {
    _buffer_delete(buffer);
  }
}

void sync_chase_lev_ws_deque_delete(chase_lev_ws_deque_t *d) {
  if (d) {
    sync_chase_lev_ws_deque_fini(d);
    free(d);
  }
}

uint64_t sync_chase_lev_ws_deque_size(chase_lev_ws_deque_t *d) {
  uint64_t bottom = sync_load(&d->bottom, SYNC_RELAXED);
  uint64_t top = sync_load(&d->top, SYNC_ACQUIRE);
  return (bottom - top);
}

uint64_t sync_chase_lev_ws_deque_push(chase_lev_ws_deque_t *d, void *val) {
  // read bottom and buffer, using Chase-Lev 2.3 for top upper bound
  uint64_t bottom = sync_load(&d->bottom, SYNC_RELAXED);
  _buffer_t *buffer = sync_load(&d->buffer, SYNC_RELAXED);

  // if the deque seems to be full then update its top bound, if the deque is
  // *really* full then expand its capacity---no underflow potential here
  // because pop() and steal() leave the queue canonical
  uint64_t size = bottom - d->top_bound;
  if (size >= buffer->capacity) {
    d->top_bound = sync_load(&d->top, SYNC_ACQUIRE);
    size = bottom - d->top_bound;
    if (size >= buffer->capacity) {
      buffer = _buffer_grow(buffer, bottom, d->top_bound);
      _deque_set_buffer(d, buffer);
    }
  }

  // update the bottom
  _buffer_put(buffer, bottom, val);
  _deque_set_bottom(d, bottom + 1);
  return (size + 1);
}

void *sync_chase_lev_ws_deque_pop(chase_lev_ws_deque_t *d) {
  // read and update bottom
  uint64_t bottom = sync_addf(&d->bottom, -1, SYNC_RELEASE);
  uint64_t top = sync_load(&d->top, SYNC_ACQUIRE);

  // update bound, since we just read it anyway
  d->top_bound = top;

  // if the queue was empty, reset bottom (canonicalize empty)
  if (bottom < top) {
    _deque_set_bottom(d, top);
    return NULL;
  }

  // read the value from the buffer
  _buffer_t *buffer = sync_load(&d->buffer, SYNC_RELAXED);
  void *val = _buffer_get(buffer, bottom);
  if (bottom > top) {
    return val;
  }

  // we're popping the last element, need to race with concurrent
  // steal()s on top. Either way, canonicalize the list after the CAS.
  if (!_deque_try_inc_top(d, top)) {
    val = NULL;
  }

  _deque_set_bottom(d, top + 1);
  return val;
}

void *sync_chase_lev_ws_deque_steal(chase_lev_ws_deque_t *d) {
  // read top and bottom
  uint64_t top = sync_load(&d->top, SYNC_ACQUIRE);
  uint64_t bottom = sync_load(&d->bottom, SYNC_ACQUIRE);

  // if the deque seems to be empty, fail the steal
  if (bottom <= top) {
    return NULL;
  }

  // Read the buffer and the value. Have to read the value before the CAS,
  // otherwise we could miss some push-pops and get the wrong value due to the
  // underlying cyclic array (see Chase-Lev 2.2).
  //
  // NB: it doesn't matter if the buffer grows a number of times between these
  //     two operations, because _buffer_get(top) will always return the same
  //     value---this is a result of the magic and beauty of this
  //     algorithm. If we want to shrink the buffer then we'll have to pay
  //     more attention.
  _buffer_t *buffer = sync_load(&d->buffer, SYNC_ACQUIRE);
  void *val = _buffer_get(buffer, top);

  // if we update the bottom, return the stolen value, otherwise retry
  if (_deque_try_inc_top(d, top)) {
    return val;
  }

  return NULL;
}
