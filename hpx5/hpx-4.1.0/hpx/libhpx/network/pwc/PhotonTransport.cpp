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

#include "PhotonTransport.h"
#include "registered.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/libhpx.h"
#include "libhpx/locality.h"
#include "libhpx/parcel.h"
#include "libhpx/boot/Network.h"
#include <cstdlib>

namespace {
using libhpx::network::pwc::PhotonTransport;
using libhpx::network::pwc::Command;
using Op = libhpx::network::pwc::PhotonTransport::Op;
using Key = libhpx::network::pwc::PhotonTransport::Key;
}

/// A barrier for photon that binds to the local bootstrap network.
static int
_boot_barrier(void)
{
  here->boot->barrier();
  return 0;
}

/// An allgather for photon that binds to the local bootstrap network.
static int
_boot_allgather(void *obj, const void * src, void * dest, int n)
{
  here->boot->allgather(src, dest, n);
  return 0;
}


static void
_init_photon_config(const config_t *cfg, int rank, int ranks,
                    struct photon_config_t *pcfg) {
  static char _gid_cxx_hack[] = "ff0e::ffff:0000:0000\0";

  pcfg->meta_exch               = PHOTON_EXCH_EXTERNAL;
  pcfg->nproc                   = ranks;
  pcfg->address                 = rank;
  pcfg->comm                    = NULL;
  pcfg->fi.provider             = cfg->photon_fiprov;
  pcfg->fi.eth_dev              = cfg->photon_fidev;
  pcfg->fi.node                 = NULL;
  pcfg->fi.service              = NULL;
  pcfg->fi.domain               = NULL;
  pcfg->ibv.use_cma             = cfg->photon_usecma;
  pcfg->ibv.eth_dev             = cfg->photon_ethdev;
  pcfg->ibv.ib_dev              = cfg->photon_ibdev;
  pcfg->ibv.num_srq             = cfg->photon_ibsrq;
  pcfg->ugni.eth_dev            = cfg->photon_ethdev;
  pcfg->ugni.bte_thresh         = cfg->photon_btethresh;
  pcfg->cap.eager_buf_size      = cfg->photon_eagerbufsize;
  pcfg->cap.pwc_buf_size        = cfg->photon_pwcbufsize;
  pcfg->cap.small_pwc_size      = cfg->photon_smallpwcsize;
  pcfg->cap.ledger_entries      = cfg->photon_ledgersize;
  pcfg->cap.max_rd              = cfg->photon_maxrd;
  pcfg->cap.default_rd          = cfg->photon_defaultrd;
  pcfg->cap.num_cq              = cfg->photon_numcq;
  pcfg->cap.use_rcq             = cfg->photon_usercq;
  // static config relevant to HPX
  pcfg->cap.max_cid_size        =  8;  // 64 bit completion IDs (commands)
  // static config not relevant for current HPX usage
  pcfg->forwarder.use_forwarder =  0;
  pcfg->cap.small_msg_size      = -1;  // default 4096 - not used for PWC
  pcfg->ibv.use_ud              =  0;  // don't enable this unless we're doing
                                       // HW GAS
  pcfg->ibv.ud_gid_prefix       = _gid_cxx_hack;
  pcfg->exch.allgather      = (__typeof__(pcfg->exch.allgather))_boot_allgather;
  pcfg->exch.barrier        = (__typeof__(pcfg->exch.barrier))_boot_barrier;
  pcfg->backend             = cfg->photon_backend;
  pcfg->coll                = cfg->photon_coll;
  pcfg->attr.comp_order     = cfg->photon_comporder;
}

void
PhotonTransport::Initialize(const config_t *cfg, int rank, int ranks) {
  if (photon_initialized()) {
    return;
  }

  struct photon_config_t pcfg;
  _init_photon_config(cfg, rank, ranks, &pcfg);
  if (photon_init(&pcfg) != PHOTON_OK) {
    dbg_error("failed to initialize transport.\n");
  }
  registered_allocator_init();
}

const Key *
PhotonTransport::FindKeyRef(const void *addr, size_t n) {
  const Key *found = NULL;
  int e = photon_get_buffer_private((void*)addr, n, &found);
  if (PHOTON_OK != e) {
    dbg_assert(found == NULL);
    log_net("no rdma key for range (%p, %zu)\n", addr, n);
  }
  return found;
}

void
PhotonTransport::FindKey(const void *addr, size_t n, Key *key) {
  if (const Key *found = PhotonTransport::FindKeyRef(addr, n)) {
    *key = *found;
  }
  else {
    dbg_error("failed to find rdma key for (%p, %zu)\n", addr, n);
  }
}

Key
PhotonTransport::FindKey(const void *addr, size_t n) {
  if (const Key *found = PhotonTransport::FindKeyRef(addr, n)) {
    return *found;
  }
  else {
    dbg_error("failed to find rdma key for (%p, %zu)\n", addr, n);
  }
}

void
PhotonTransport::Pin(const void *base, size_t n, Key *key)
{
  if (PHOTON_OK != photon_register_buffer((void*)base, n)) {
    dbg_error("failed to register segment with Photon\n");
  }
  else {
    static constexpr auto LEVEL= HPX_LOG_NET | HPX_LOG_MEMORY;
    log_level(LEVEL, "pinned segment (%p, %zu)\n", base, n);
  }

  if (key) {
    FindKey(base, n, key);
  }
}

void
PhotonTransport::Unpin(const void *base, size_t n)
{
  int e = photon_unregister_buffer((void*)base, n);
  if (PHOTON_OK != e) {
    dbg_error("unhandled error %d during release of segment (%p, %zu)\n", e,
              base, n);
  }
  else {
    static constexpr auto LEVEL= HPX_LOG_NET | HPX_LOG_MEMORY;
    log_level(LEVEL, "unpinned the segment (%p, %zu)\n", base, n);
  }
}

// async entry point for unpin
static int
_photon_unpin_async(const void *base, size_t n, int src, uint64_t op) {
  PhotonTransport::Unpin(base, n);
  if (hpx_parcel_t* p = Command::Unpack(op)(src)) {
    parcel_launch(p);
  }
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_INTERRUPT, 0, _unpin_async, _photon_unpin_async,
                     HPX_POINTER, HPX_SIZE_T, HPX_INT, HPX_UINT64);

/// Interpose a command to unpin a region before performing @p op.
///
/// We do not require that the local address for pwc/gwc be registered with
/// the network. When we get a region that is not yet registered we need to
/// dynamically register it, and then de-register it once the operation has
/// completed.
///
/// This operation attaches a parcel resume command to the local completion
/// event that will unpin the region, and then continue with the user-supplied
/// local command.
///
/// @param         addr The address we need to de-register.
/// @param            n The number of bytes to de-register.
/// @param           op The user's local completion operation.
///
/// @returns            The operation that should be used as the local
///                     completion event handler for the current operation
///                     (pwc/gwc instance).
static Command
_chain_unpin(const void *addr, size_t n, Command op)
{
  // we assume that this parcel doesn't need credit to run---technically it
  // not easy to account for this parcel because of the fact that pwc() can be
  // run as a scheduler_suspend() operation
  uint64_t pack = Command::Pack(op);
  hpx_parcel_t *p = action_new_parcel(_unpin_async, // action
                                      HPX_HERE,     // target
                                      0,            // continuation target
                                      0,            // continuation action
                                      4,            // nargs
                                      &addr,        // buffer to unpin
                                      &n,           // length to unpin
                                      &here->rank,  // src for command
                                      &pack);       // command

  return Command::ResumeParcel(p);
}

int
PhotonTransport::Op::cmd()
{
  int flags = ((lop) ? 0 : PHOTON_REQ_PWC_NO_LCE) |
              ((rop) ? 0 : PHOTON_REQ_PWC_NO_RCE);

  photon_cid lid;
  lid.u64 = Command::Pack(lop);
  lid.size = 0;

  photon_cid rid;
  rid.u64 = Command::Pack(rop);
  rid.size = 0;
  int e = photon_put_with_completion(rank, 0, nullptr, nullptr, lid, rid, flags);
  if (PHOTON_OK == e) {
    return LIBHPX_OK;
  }

  if (PHOTON_ERROR_RESOURCE == e) {
    log_error("could not initiate command due to resource constraint\n");
    return LIBHPX_RETRY;
  }

  dbg_error("could not initiate a put-with-completion\n");
}

int
PhotonTransport::Op::put()
{
  int flags = ((lop) ? 0 : PHOTON_REQ_PWC_NO_LCE) |
              ((rop) ? 0 : PHOTON_REQ_PWC_NO_RCE);

  struct photon_buffer_t rbuf = {
    .addr = (uintptr_t)dest,
    .size = n,
    .priv = *dest_key
  };

  struct photon_buffer_t lbuf = {
    .addr = (uintptr_t)src,
    .size = n
  };

  if (src_key) {
    lbuf.priv = *src_key;
  }
  else {
    log_net("temporarily registering buffer (%p, %zu)\n", src, n);
    PhotonTransport::Pin(src, n, &lbuf.priv);
    lop = _chain_unpin(src, n, lop);
  }

  photon_cid lid;
  lid.u64 = Command::Pack(lop);
  lid.size = 0;

  photon_cid rid;
  rid.u64 = Command::Pack(rop);
  rid.size = 0;

  int e = photon_put_with_completion(rank, n, &lbuf, &rbuf, lid, rid, flags);
  if (PHOTON_OK == e) {
    return LIBHPX_OK;
  }

  if (PHOTON_ERROR_RESOURCE == e) {
    log_error("could not initiate pwc due to resource constraint\n");
    return LIBHPX_RETRY;
  }

  dbg_error("could not initiate a put-with-completion\n");
}

int
PhotonTransport::Op::get()
{
  int flags = (rop) ? 0 : PHOTON_REQ_PWC_NO_RCE;

  struct photon_buffer_t lbuf = {
    .addr = (uintptr_t)dest,
    .size = n
  };

  if (dest_key) {
    lbuf.priv = *dest_key;
  }
  else {
    log_net("temporarily registering buffer (%p, %zu)\n", dest, n);
    PhotonTransport::Pin(dest, n, &lbuf.priv);
    lop = _chain_unpin(dest, n, lop);
  }

  dbg_assert(src_key);
  struct photon_buffer_t rbuf = {
    .addr = (uintptr_t)src,
    .size = n,
    .priv = *src_key
  };

  photon_cid lid;
  lid.u64 = Command::Pack(lop);
  lid.size = 0;

  photon_cid rid;
  rid.u64 = Command::Pack(rop);
  rid.size = 0;

  int e = photon_get_with_completion(rank, n, &lbuf, &rbuf, lid, rid, flags);
  if (PHOTON_OK == e) {
    return LIBHPX_OK;
  }

  dbg_error("failed transport get operation\n");
}

int
PhotonTransport::Poll(Command *op, int *remaining, int rank, int *src, int type)
{
  photon_cid rid;
  int flag = 0;
  int prank = (rank == ANY_SOURCE) ? PHOTON_ANY_SOURCE : rank;
  int e = photon_probe_completion(prank, &flag, remaining, &rid, src, NULL, type);
  if (PHOTON_OK != e) {
    dbg_error("photon probe error\n");
  }
  *op = Command::Unpack(rid.u64);
  return flag;
}

int
PhotonTransport::Test(Command *op, int *remaining, int id, int *src)
{
  return Poll(op, remaining, id, src, PHOTON_PROBE_EVQ);
}

int
PhotonTransport::Probe(Command *op, int *remaining, int rank, int *src)
{
  return Poll(op, remaining, rank, src, PHOTON_PROBE_LEDGER);
}
