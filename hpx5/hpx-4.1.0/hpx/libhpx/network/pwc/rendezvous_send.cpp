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

/// The rendezvous send functionality is implemented as a request-get-reply
/// operation. The initial request operation is sent as a small parcel that we
/// know will not use rendezvous. That request operation will run as an
/// interrupt, and allocate a buffer to recv the large parcel being sent. It
/// will then initiate a get-with-completion operation that copies the parcel to
/// the local buffer. Attached to that get-with-completion operation are two
/// events, the remote event will free the sent parcel, while the local event
/// will schedule the parcel once the get has completed.

#include "PWCNetwork.h"
#include "libhpx/debug.h"
#include "libhpx/events.h"

namespace {
using libhpx::network::pwc::Command;
using libhpx::network::pwc::PhotonTransport;
using libhpx::network::pwc::PWCNetwork;
using Op = libhpx::network::pwc::PhotonTransport::Op;
using Key = libhpx::network::pwc::PhotonTransport::Key;
}

namespace {
struct _rendezvous_get_args_t {
  unsigned         rank;
  const hpx_parcel_t *p;
  size_t              n;
  Key               key;
};
}

/// The rendezvous request handler.
///
/// This handler will allocate a parcel to "get" into, and then initiate the
/// get-with-completion operation. It does not need to persist across the get
/// operation because it can attach the delete_parcel and _rendezvous_launch
/// event handlers to the get operation.
///
/// We need to use a marshaled operation because we send the transport key by
/// value and we don't have an FFI type to capture that.
static int
_rendezvous_get_handler(_rendezvous_get_args_t *args, size_t size)
{
  hpx_parcel_t *p = parcel_alloc(args->n - sizeof(*p));
  dbg_assert(p);
  Op op;
  op.rank = args->rank;
  op.n = args->n;
  op.dest = p;
  op.dest_key = PhotonTransport::FindKeyRef(p, args->n);
  op.src = args->p;
  op.src_key = &args->key;
  op.lop = Command::RendezvousLaunch(p);
  op.rop = Command::DeleteParcel(args->p);
  int e = op.get();
  dbg_check(e, "could not issue get during rendezvous parcel\n");
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_INTERRUPT, HPX_MARSHALLED, _rendezvous_get,
                     _rendezvous_get_handler, HPX_POINTER, HPX_SIZE_T);

int
PWCNetwork::rendezvousSend(const hpx_parcel_t *p)
{
  size_t n = parcel_size(p);
  _rendezvous_get_args_t args = {
    .rank = here->rank,
    .p = p,
    .n = n,
    .key = PhotonTransport::FindKey(p, n)
  };
  return hpx_call(p->target, _rendezvous_get, HPX_NULL, &args, sizeof(args));
}
