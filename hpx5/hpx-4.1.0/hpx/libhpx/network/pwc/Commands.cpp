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
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/events.h"
#include "libhpx/gpa.h"

namespace {
using libhpx::network::pwc::Command;
using libhpx::network::pwc::PhotonTransport;
using libhpx::network::pwc::PWCNetwork;
}

inline hpx_parcel_t*
Command::lcoSet(unsigned src) const
{
  hpx_addr_t lco = offset_to_gpa(here->rank, arg_);
  hpx_lco_set(lco, 0, nullptr, HPX_NULL, HPX_NULL);
  return nullptr;
}

inline hpx_parcel_t*
Command::lcoSetAtSource(unsigned src) const
{
  PhotonTransport::Op op;
  op.rank = src;
  op.lop = Command::Nop();
  op.rop = Command(LCO_SET, arg_);
  dbg_check( op.cmd() );
  return nullptr;
}

inline hpx_parcel_t*
Command::deleteParcel(unsigned src) const
{
  auto p = reinterpret_cast<hpx_parcel_t*>(arg_);
  log_parcel("releasing sent parcel %p\n", static_cast<void*>(p));
  hpx_parcel_t *ssync = p->next;
  p->next = nullptr;
  parcel_delete(p);
  return ssync;
}

inline hpx_parcel_t*
Command::resumeParcel(unsigned src) const
{
  auto p = reinterpret_cast<hpx_parcel_t*>(arg_);
  log_parcel("resuming %s, (%p)\n", actions[p->action].key, static_cast<void*>(p));
  return p;
}

inline hpx_parcel_t*
Command::resumeParcelAtSource(unsigned src) const
{
  PhotonTransport::Op op;
  op.rank = src;
  op.lop = Command::Nop();
  op.rop = Command(RESUME_PARCEL, arg_);
  dbg_check( op.cmd() );
  return nullptr;
}

inline hpx_parcel_t*
Command::reloadReply(unsigned src) const
{
  PWCNetwork::Instance().progressSends(src);
  return nullptr;
}

inline hpx_parcel_t*
Command::recvParcel(unsigned src) const
{
#ifdef __LP64__
  auto p = reinterpret_cast<hpx_parcel_t*>(arg_);
#else
  dbg_assert((arg_ & 0xffffffff) == arg_);
  auto p = reinterpret_cast<hpx_parcel_t*>((uintptr_t)arg_);
#endif
  p->src = src;
  parcel_set_state(p, PARCEL_SERIALIZED | PARCEL_BLOCK_ALLOCATED);
  EVENT_PARCEL_RECV(p->id, p->action, p->size, p->src, p->target);
  return p;
}

inline hpx_parcel_t*
Command::reloadRequest(unsigned src) const {
  PWCNetwork::Instance().reload(src, arg_);
  return nullptr;
}

/// The local event handler for the get-with-completion operation.
///
/// This is used to schedule the transferred parcel once the get operation has
/// completed. The command encodes the local address of the parcel to schedule.
inline hpx_parcel_t*
Command::rendezvousLaunch(unsigned src) const
{
  hpx_parcel_t *p = reinterpret_cast<hpx_parcel_t*>(arg_);
  parcel_set_state(p, PARCEL_SERIALIZED);
  EVENT_PARCEL_RECV(p->id, p->action, p->size, p->src, p->target);
  DEBUG_IF (!p->target || !p->action) {
    dbg_error("Rendezvous recv operation failed for %p\n", p);
  }
  return p;
}

hpx_parcel_t*
Command::operator()(unsigned src) const
{
  switch (op_) {
   case NOP: abort();
   case RESUME_PARCEL:
    return resumeParcel(src);
   case RESUME_PARCEL_SOURCE:
    return resumeParcelAtSource(src);
   case DELETE_PARCEL:
    return deleteParcel(src);
   case LCO_SET:
    return lcoSet(src);
   case LCO_SET_SOURCE:
    return lcoSetAtSource(src);
   case RECV_PARCEL:
    return recvParcel(src);
   case RENDEZVOUS_LAUNCH:
    return rendezvousLaunch(src);
   case RELOAD_REQUEST:
    return reloadRequest(src);
   case RELOAD_REPLY:
    return reloadReply(src);
  }
  unreachable();
}
