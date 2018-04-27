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

#include "FunneledNetwork.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/parcel.h"
#include "libhpx/Worker.h"
#include <cstring>

namespace {
using libhpx::self;
using libhpx::network::isir::FunneledNetwork;
}

typedef struct {
  hpx_parcel_t *p;
  void *out;
  char data[];
} _isir_lco_get_reply_args_t;

static int
_isir_lco_get_reply_handler(_isir_lco_get_reply_args_t *args, size_t n) {
  size_t bytes = n - sizeof(*args);
  if (bytes) {
    memcpy(args->out, args->data, bytes);
  }
  self->spawn(args->p);
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_INTERRUPT, HPX_MARSHALLED, _isir_lco_get_reply,
                     _isir_lco_get_reply_handler, HPX_POINTER, HPX_SIZE_T);

static int
_isir_lco_get_request_handler(hpx_parcel_t *p, size_t n, void *out, int reset) {
  dbg_assert(n > 0);

  // eagerly create a continuation parcel so that we can serialize the data into
  // it directly without an extra copy
  size_t bytes = sizeof(_isir_lco_get_reply_args_t) + n;
  hpx_parcel_t *cont = hpx_thread_generate_continuation(NULL, bytes);

  // forward the parcel and output buffer back to the sender
  _isir_lco_get_reply_args_t *args = static_cast<_isir_lco_get_reply_args_t*>(hpx_parcel_get_data(cont));
  args->p = p;
  args->out = out;

  // perform the blocking get operation
  int e = HPX_SUCCESS;
  hpx_addr_t target = hpx_thread_current_target();
  if (reset) {
    e = hpx_lco_get_reset(target, n, args->data);
  }
  else {
    e = hpx_lco_get(target, n, args->data);
  }

  // send the continuation
  parcel_launch_error(cont, e);

  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_DEFAULT, 0, _isir_lco_get_request,
                     _isir_lco_get_request_handler, HPX_POINTER, HPX_SIZE_T,
                     HPX_POINTER, HPX_INT);

typedef struct {
  hpx_addr_t lco;
  size_t n;
  void *out;
  int reset;
} _lco_get_env_t;

static void _lco_get_continuation(hpx_parcel_t *p, void *env) {
  _lco_get_env_t *e = (_lco_get_env_t *)env;
  hpx_addr_t addr = e->lco;
  size_t n = e->n;
  void *out = e->out;
  int reset = e->reset;
  hpx_action_t act = _isir_lco_get_request;
  hpx_addr_t rsync = HPX_HERE;
  hpx_action_t rop = _isir_lco_get_reply;
  dbg_check(action_call_lsync(act, addr, rsync, rop, 4, &p, &n, &out, &reset));
}

int
FunneledNetwork::get(hpx_addr_t lco, size_t n, void *out, int reset) {
  _lco_get_env_t env = {
    .lco = lco,
    .n = n,
    .out = out,
    .reset = reset
  };

  self->suspend(_lco_get_continuation, &env);
  return HPX_SUCCESS;
}
