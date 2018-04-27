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

#include "SMPNetwork.h"
#include "libhpx/debug.h"
#include "libhpx/libhpx.h"
#include <cstring>

namespace {
using libhpx::Network;
using libhpx::network::SMPNetwork;
using BootNetwork = libhpx::boot::Network;
}

SMPNetwork::SMPNetwork(const BootNetwork& boot)
    : Network()
{
  if (boot.getNRanks() > 1) {
    dbg_error("SMP network does not support multiple ranks.\n");
  }
}

void
SMPNetwork::memget(void *to, hpx_addr_t from, size_t size, hpx_addr_t lsync,
                   hpx_addr_t rsync)
{
  dbg_error("SMP string implementation called\n");
}

void
SMPNetwork::memget(void *to, hpx_addr_t from, size_t size, hpx_addr_t lsync)
{
  dbg_error("SMP string implementation called\n");
}

void
SMPNetwork::memget(void *to, hpx_addr_t from, size_t size)
{
  dbg_error("SMP string implementation called\n");
}

void
SMPNetwork::memput(hpx_addr_t to, const void *from, size_t size, hpx_addr_t lsync,
       hpx_addr_t rsync)
{
  dbg_error("SMP string implementation called\n");
}

void
SMPNetwork::memput(hpx_addr_t to, const void *from, size_t size, hpx_addr_t rsync)
{
  dbg_error("SMP string implementation called\n");
}

void
SMPNetwork::memput(hpx_addr_t to, const void *from, size_t size)
{
  dbg_error("SMP string implementation called\n");
}

void
SMPNetwork::memcpy(hpx_addr_t to, hpx_addr_t from, size_t size, hpx_addr_t lsync)
{
  dbg_error("SMP string implementation called\n");
}

void
SMPNetwork::memcpy(hpx_addr_t to, hpx_addr_t from, size_t size)
{
  dbg_error("SMP string implementation called\n");
}

int
SMPNetwork::type() const
{
  return HPX_NETWORK_SMP;
}

void
SMPNetwork::progress(int)
{
}

void
SMPNetwork::deallocate(const hpx_parcel_t* p)
{
  dbg_error("The SMP network has not network-managed parcels\n");
}

int
SMPNetwork::send(hpx_parcel_t *p, hpx_parcel_t *ssync)
{
  dbg_error("SMP network send called\n");
}

hpx_parcel_t *
SMPNetwork::probe(int nrx)
{
  return NULL;
}

void
SMPNetwork::flush()
{
}

void
SMPNetwork::pin(const void *addr, size_t n, void *key)
{
}

void
SMPNetwork::unpin(const void *addr, size_t n)
{
}

int
SMPNetwork::wait(hpx_addr_t lco, int reset)
{
  return (reset) ? hpx_lco_wait_reset(lco) : hpx_lco_wait(lco);
}

int
SMPNetwork::get(hpx_addr_t lco, size_t n, void *to, int reset)
{
  return (reset) ? hpx_lco_get_reset(lco, n, to) : hpx_lco_get(lco, n, to);
}

int
SMPNetwork::init(void **)
{
  return LIBHPX_OK;
}

int
SMPNetwork::sync(void *in, size_t in_size, void *out, void *c)
{
  void *sendbuf = in;
  int count = in_size;
  std::memcpy(out, sendbuf, count);
  return LIBHPX_OK;
}
