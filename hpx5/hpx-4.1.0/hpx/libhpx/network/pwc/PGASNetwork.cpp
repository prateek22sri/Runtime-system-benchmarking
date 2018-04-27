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
#include "libhpx/gpa.h"
#include "libhpx/libhpx.h"
#include "libhpx/locality.h"
#include "libhpx/Network.h"
#include "libhpx/Worker.h"

namespace {
using libhpx::GAS;
using libhpx::network::pwc::Command;
using libhpx::network::pwc::PGASNetwork;
using libhpx::network::pwc::PWCNetwork;
using CacheAligned = libhpx::util::Aligned<HPX_CACHELINE_SIZE>;
}

PGASNetwork::PGASNetwork(const config_t *cfg, const boot::Network& boot,
                         GAS *gas)
    : PWCNetwork(cfg, boot, gas),
      CacheAligned()
{
}

void
PGASNetwork::memget(void *to, hpx_addr_t from, size_t size, hpx_addr_t lsync,
                    hpx_addr_t rsync)
{
  auto lcmd = Command::Nop();
  auto rcmd = Command::Nop();

  if (lsync) {
    if (gpa_to_rank(lsync) == rank_) {
      lcmd = Command::SetLCO(lsync);
    }
    else {
      hpx_parcel_t *p = action_new_parcel(hpx_lco_set_action, lsync, 0, 0, 0);
      dbg_assert(p);
      lcmd = Command::ResumeParcel(p);
    }
  }

  if (rsync) {
    if (gpa_to_rank(rsync) == rank_) {
      rcmd = Command::SetLCOAtSource(rsync);
    }
    else if (gpa_to_rank(rsync) == gpa_to_rank(from)) {
      rcmd = Command::SetLCO(rsync);
    }
    else {
      hpx_parcel_t *p = action_new_parcel(hpx_lco_set_action, rsync, 0, 0, 0);
      rcmd = Command::ResumeParcelAtSource(p);
    }
  }

  get(to, from, size, lcmd, rcmd);
}

/// The remote-synchronous memget operation.
///
/// This is a bit of a strange beast. This will not return until the remote
/// operation has completed, but that's a bit weird since that means that a
/// message has returned from the remote locality. This is probably not
/// different than just using the fully lsync version---except that there is
/// potentially some local rdma going on in that case.
///
/// We currently don't have an interface for get-with-remote-completion, so
/// we're not actually handling this correctly.
/// @{
namespace {
struct _pwc_memget_rsync_env_t {
  void         *to;
  hpx_addr_t  from;
  size_t         n;
  hpx_addr_t lsync;
};
}

static void
_pwc_memget_rsync_continuation(hpx_parcel_t *p, void *env)
{
  auto e = static_cast<_pwc_memget_rsync_env_t*>(env);
  auto lcmd = Command::Nop();
  auto rcmd = Command::ResumeParcelAtSource(p);

  if (e->lsync) {
    if (gpa_to_rank(e->lsync) == here->rank) {
      lcmd = Command::SetLCO(e->lsync);
    }
    else {
      hpx_parcel_t *c = action_new_parcel(hpx_lco_set_action, e->lsync, 0, 0, 0);
      lcmd = Command::ResumeParcel(c);
    }
  }

  PWCNetwork::Instance().get(e->to, e->from, e->n, lcmd, rcmd);
}

void
PGASNetwork::memget(void *to, hpx_addr_t from, size_t n, hpx_addr_t lsync)
{
  _pwc_memget_rsync_env_t env = {
    .to     = to,
    .from  = from,
    .n     = n,
    .lsync = lsync
  };
  self->suspend(_pwc_memget_rsync_continuation, &env);
}
/// @}

/// The synchronous memget operation.
///
/// This doesn't return until the memget operation has completed.
/// @{
namespace {
struct _pwc_memget_lsync_env_t {
  void        *to;
  hpx_addr_t from;
  size_t        n;
};
}

static void
_pwc_memget_lsync_continuation(hpx_parcel_t *p, void *env)
{
  auto e = static_cast<_pwc_memget_lsync_env_t*>(env);
  auto lcmd = Command::ResumeParcel(p);
  auto rcmd = Command();
  PWCNetwork::Instance().get(e->to, e->from, e->n, lcmd, rcmd);
}

void
PGASNetwork::memget(void *to, hpx_addr_t from, size_t size)
{
  _pwc_memget_lsync_env_t env = {
    .to = to,
    .from = from,
    .n = size
  };
  self->suspend(_pwc_memget_lsync_continuation, &env);
}
/// @}

/// The asynchronous memput operation.
///
/// We;re going to satisfy this operation using pwc, but that only lets us deal
/// with lsync and rsync LCO addresses at either the destination or the
/// source. If the LCO is at a third location we'll allocate a local parcel to
/// encode that, and use a resume command to launch it for the appropriate
/// event. This requires an extra hop for the rsync operation.
///
/// If we wanted to be fancy we could use a parcel if the size of the data is
/// small. This would trade off the extra hop for a bit of extra bandwidth.
/// @{
void
PGASNetwork::memput(hpx_addr_t to, const void *from, size_t size,
                    hpx_addr_t lsync, hpx_addr_t rsync)
{
  auto lcmd = Command::Nop();
  auto rcmd = Command::Nop();

  if (lsync) {
    if (gpa_to_rank(lsync) == here->rank) {
      lcmd = Command::SetLCO(lsync);
    }
    else {
      hpx_parcel_t *p = action_new_parcel(hpx_lco_set_action, lsync, 0, 0, 0);
      dbg_assert(p);
      lcmd = Command::ResumeParcel(p);
    }
  }

  if (rsync) {
    if (gpa_to_rank(rsync) == here->rank) {
      rcmd = Command::SetLCOAtSource(rsync);
    }
    else if (gpa_to_rank(rsync) == gpa_to_rank(to)) {
      rcmd = Command::SetLCO(rsync);
    }
    else {
      hpx_parcel_t *p = action_new_parcel(hpx_lco_set_action, rsync, 0, 0, 0);
      rcmd = Command::ResumeParcelAtSource(p);
    }
  }

  put(to, from, size, lcmd, rcmd);
}
/// @}

/// The lsync memput implementation.
///
/// This can't return until the local buffer can be rewritten. We're using
/// put/pwc under the hood though, which requires a local command. We'll do a
/// simple resume continuation locally to handle this.
///
/// This operation is complicated by the fact that the remote sync LCO is set by
/// the user, and may be anywhere in the system. The pwc interface gives us the
/// ability to set LCOs at the source or the destination relatively directly,
/// but if it's in a third place we need to do something special about it.
///
/// We will allocate a local parcel to store the continuation and attach an
/// rsync resume operation, which requires an extra hop for the continuation but
/// is the easiest option. If we wanted to branch here we could use a parcel if
/// the put is small.
/// @{
namespace {
struct _pwc_memput_lsync_continuation_env_t {
  hpx_addr_t    to;
  const void *from;
  size_t         n;
  hpx_addr_t rsync;
};
}

static void
_pwc_memput_lsync_continuation(hpx_parcel_t *p, void *env)
{
  auto e = static_cast<_pwc_memput_lsync_continuation_env_t*>(env);
  auto rcmd = Command::Nop();

  if (e->rsync) {
    if (gpa_to_rank(e->rsync) == here->rank) {
      rcmd = Command::SetLCOAtSource(e->rsync);
    }
    else if (gpa_to_rank(e->rsync) == gpa_to_rank(e->to)) {
      rcmd = Command::SetLCO(e->rsync);
    }
    else {
      hpx_action_t set = hpx_lco_set_action;
      hpx_parcel_t *p = action_new_parcel(set, e->rsync, 0, 0, 0);
      rcmd = Command::ResumeParcelAtSource(p);
    }
  }

  auto lcmd = Command::ResumeParcel(p);
  PWCNetwork::Instance().put(e->to, e->from, e->n, lcmd, rcmd);
}

void
PGASNetwork::memput(hpx_addr_t to, const void *from, size_t n, hpx_addr_t rsync)
{
  _pwc_memput_lsync_continuation_env_t env = {
    .to = to,
    .from = from,
    .n = n,
    .rsync = rsync
  };
  self->suspend(_pwc_memput_lsync_continuation, &env);
}
/// @}

/// The rsync memput operation.
///
/// This shouldn't return until the remote operation has completed. We can just
/// use pwc() to do this, with a remote command that wakes us up.
/// @{
namespace {
struct _pwc_memput_rsync_continuation_env_t {
  hpx_addr_t    to;
  const void *from;
  size_t         n;
};
}

static void
_pwc_memput_rsync_continuation(hpx_parcel_t *p, void *env)
{
  auto e = static_cast<_pwc_memput_rsync_continuation_env_t*>(env);
  auto lcmd = Command::Nop();
  auto rcmd = Command::ResumeParcelAtSource(p);
  PWCNetwork::Instance().put(e->to, e->from, e->n, lcmd, rcmd);
}

void
PGASNetwork::memput(hpx_addr_t to, const void *from, size_t n)
{
  _pwc_memput_rsync_continuation_env_t env = {
    .to = to,
    .from = from,
    .n = n
  };
  self->suspend(_pwc_memput_rsync_continuation, &env);
}
/// @}

///
static int
_pwc_memcpy_handler(const void *from, hpx_addr_t to, size_t n, hpx_addr_t sync)
{
  here->net->memput(to, from, n, sync);
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, _pwc_memcpy, _pwc_memcpy_handler,
                       HPX_POINTER, HPX_ADDR, HPX_SIZE_T, HPX_ADDR);

void
PGASNetwork::memcpy(hpx_addr_t to, hpx_addr_t from, size_t n, hpx_addr_t sync)
{
  action_call_lsync(_pwc_memcpy, from, 0, 0, 3, &to, &n, &sync);
}

///
static int
_pwc_memcpy_rsync_handler(const void *from, hpx_addr_t to, size_t n)
{
  here->net->memput(to, from, n);
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, _pwc_memcpy_rsync,
                       _pwc_memcpy_rsync_handler, HPX_POINTER, HPX_ADDR,
                       HPX_SIZE_T);

void
PGASNetwork::memcpy(hpx_addr_t to, hpx_addr_t from, size_t n)
{
  action_call_rsync(_pwc_memcpy_rsync, from, NULL, 0, 2, &to, &n);
}
