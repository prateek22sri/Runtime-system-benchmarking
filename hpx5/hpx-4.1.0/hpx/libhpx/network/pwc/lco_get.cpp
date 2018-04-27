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

#include "PWCNetwork.h"
#include "Commands.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include "libhpx/Worker.h"

namespace {
using libhpx::self;
using libhpx::network::pwc::Command;
using libhpx::network::pwc::PhotonTransport;
using libhpx::network::pwc::PWCNetwork;
using Op = libhpx::network::pwc::PhotonTransport::Op;
using Key = libhpx::network::pwc::PhotonTransport::Key;
}

/// This acts as a parcel_suspend transfer to allow _pwc_lco_get_request_handler
/// to wait for its pwc to complete.
static void
_get_reply_continuation(struct hpx_parcel *p, void *env)
{
  Op *op = static_cast<Op*>(env);
  op->lop  = Command::ResumeParcel(p);
  dbg_check( op->put() );
}

namespace {
struct _pwc_lco_get_request_args_t {
  struct hpx_parcel *p;
  size_t n;
  void *out;
  int reset;
  Key key;
  unsigned rank;
};
}

/// This function (*not* an action) consolidates the functionality to issue a
/// synchronous get reply via put-with-completion using the scheduler_suspend
/// interface.
///
/// We could actually do the Op construction in the reply transfer continuation,
/// but we'd need an environment to pass down there anyway, so we use the Op for
/// that.
static int
_get_reply(_pwc_lco_get_request_args_t *args, const void *ref, Command remote)
{
  // Create the transport operation to perform the rdma put operation
  Op op;
  op.rank = args->rank;
  op.n = args->n;
  op.dest = args->out;
  op.dest_key = &args->key;
  op.src = ref;
  op.src_key = PhotonTransport::FindKeyRef(ref, args->n);
  op.lop = Command();                          // set in _get_reply_continuation
  op.rop = remote;
  dbg_assert_str(op.src_key, "LCO reference must point to registered memory\n");

  // Issue the pwc and wait for synchronous local completion so that the ref
  // buffer doesn't move during the underlying rdma, if there is any
  self->suspend(_get_reply_continuation, &op);
  return HPX_SUCCESS;
}

/// This function (*not* an action) performs a get request to a temporary stack
/// location.
static int
_get_reply_stack(_pwc_lco_get_request_args_t *args, hpx_addr_t lco)
{
  char ref[args->n];

  int e = HPX_SUCCESS;
  if (args->reset) {
    e = hpx_lco_get_reset(lco, args->n, ref);
  }
  else {
    e = hpx_lco_get(lco, args->n, ref);
  }

  if (e != HPX_SUCCESS) {
    dbg_error("Cannot yet return an error from a remote get operation\n");
  }

  return _get_reply(args, ref, Command::ResumeParcel(args->p));
}

/// This function (*not* an action) performs a get request to a temporary
/// malloced location.
static int
_get_reply_malloc(_pwc_lco_get_request_args_t *args, hpx_addr_t lco)
{
  void *ref = registered_malloc(args->n);
  dbg_assert(ref);

  int e = HPX_SUCCESS;
  if (args->reset) {
    e = hpx_lco_get_reset(lco, args->n, ref);
  }
  else {
    e = hpx_lco_get(lco, args->n, ref);
  }

  if (e != HPX_SUCCESS) {
    dbg_error("Cannot yet return an error from a remote get operation\n");
  }

  e = _get_reply(args, ref, Command::ResumeParcel(args->p));
  registered_free(ref);
  return e;
}

/// This function (*not* an action) performs a two-phase get request without any
/// temporary storage.
static int
_get_reply_getref(_pwc_lco_get_request_args_t *args, hpx_addr_t lco)
{
  // Get a reference to the LCO data
  void *ref;
  int e = hpx_lco_getref(lco, args->n, &ref);

  if (e != HPX_SUCCESS) {
    dbg_error("Cannot yet return an error from a remote get operation\n");
  }

  // Send back the LCO data. This doesn't resume the remote thread because there
  // is a race where a delete can trigger a use-after-free during our subsequent
  // release.
  e = _get_reply(args, ref, Command());
  dbg_check(e, "Failed rendezvous put during remote lco get request.\n");

  // Release the reference.
  hpx_lco_release(lco, ref);

  // Wake the remote getter up.
  PhotonTransport::Op op;
  op.rank = args->rank;
  op.lop = Command::Nop();
  op.rop = Command::ResumeParcel(args->p);
  return op.cmd();
}

/// This action is sent to execute the request half of a two-sided LCO get
/// operation.
static int
_pwc_lco_get_request_handler(_pwc_lco_get_request_args_t *args, size_t n)
{
  dbg_assert(n > 0);

  hpx_addr_t lco = hpx_thread_current_target();

  // We would like to rdma directly from the LCO's buffer, when
  // possible. Unfortunately, this induces a race where the returned put
  // operation completes at the get location before the rdma is detected as
  // completing here. This allows the user to correctly delete the LCO while the
  // local thread still has a reference to the buffer which leads to
  // use-after-free. At this point we can only do the getref() version using two
  // put operations, one to put back to the waiting buffer, and one to resume
  // the waiting thread after we drop our local reference.
  if (args->n > LIBHPX_SMALL_THRESHOLD && !args->reset) {
    return _get_reply_getref(args, lco);
  }

  // If there is enough space to stack allocate a buffer to copy, use the stack
  // version, otherwise malloc a buffer to copy to.
  else if (hpx_thread_can_alloca(args->n) >= HPX_PAGE_SIZE) {
    return _get_reply_stack(args, lco);
  }

  // Otherwise we get to a registered buffer and then do the put. The theory
  // here is that a single small-ish (< LIBHPX_SMALL_THRESHOLD) malloc, memcpy,
  // and free, is more efficient than doing two pwc() operations.
  //
  // NB: We need to verify that this heuristic is actually true, and that the
  //     LIBHPX_SMALL_THRESHOLD is appropriate. Honestly, given enough work to
  //     do, the latency of two puts might not be a big deal.
  else {
    return _get_reply_malloc(args, lco);
  }
}
static LIBHPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _pwc_lco_get_request,
                     _pwc_lco_get_request_handler, HPX_POINTER, HPX_SIZE_T);
namespace {
struct _pwc_lco_get_continuation_env_t {
  _pwc_lco_get_request_args_t request;
  hpx_addr_t lco;
};
}

/// Issue the get request parcel from a transfer continuation.
static void
_pwc_lco_get_continuation(struct hpx_parcel *p, void *env)
{
  auto e = static_cast<_pwc_lco_get_continuation_env_t*>(env);
  e->request.p = p;
  hpx_action_t act = _pwc_lco_get_request;
  size_t n = sizeof(e->request);
  dbg_check(action_call_lsync(act, e->lco, 0, 0, 2, &e->request, n));
}

/// This is the top-level LCO get handler that is called for (possibly) remote
/// LCOs. It builds and sends a get request parcel, that will reply using pwc()
/// to write directly to @p out.
///
/// This operation is synchronous and will block until the operation has
/// completed.
int
PWCNetwork::get(hpx_addr_t lco, size_t n, void *out, int reset)
{
  _pwc_lco_get_continuation_env_t env;
  env.request.p = nullptr;                   // set in _pwc_lco_get_continuation
  env.request.n = n;
  env.request.out = out;
  env.request.reset = reset;
  env.request.rank = here->rank;
  env.lco = lco;

  // If the output buffer is already registered, then we just need to copy the
  // key into the args structure, otherwise we need to register the region.
  auto *key = PhotonTransport::FindKeyRef(out, n);
  if (key) {
    env.request.key = *key;
  }
  else {
    PhotonTransport::Pin(out, n, &env.request.key);
  }

  // Perform the get operation synchronously.
  self->suspend(_pwc_lco_get_continuation, &env);

  // If we registered the output buffer dynamically, then we need to de-register
  // it now.
  if (!key) {
    PhotonTransport::Unpin(out, n);
  }
  return HPX_SUCCESS;
}
