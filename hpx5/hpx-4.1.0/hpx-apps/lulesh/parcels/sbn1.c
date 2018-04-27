// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "lulesh-hpx.h"

// Performs the SBN1 update (allreduce)
static int _SBN1_result_action(Domain *ld, NodalArgs *args, size_t size) {
  CHECK_AFFINITY(ld);

  // prepare for the unpack, do this here to minimize the time spent holding the
  // lock
  int srcLocalIdx = args->srcLocalIdx;
  double     *src = args->buf;
  int          nx = ld->sizeX + 1;
  int          ny = ld->sizeY + 1;
  int          nz = ld->sizeZ + 1;
  recv_t   unpack = RECEIVER[srcLocalIdx];

  // 1. wait until recv is safe
  hpx_lco_wait(ld->sbn1_recv);

  CHECK_AFFINITY(ld);

  // static unsigned count[8] = {0};
  // printf("recv %u %u\n", ld->rank, count[ld->rank]++);
  // fflush(stdout);

  // 2. update the domain
  unpack(nx, ny, nz, src, ld->nodalMass, 0);

  // 3. if the recv is a semaphore, release it
#ifdef ENABLE_SERIAL_RECV
  hpx_lco_sema_v(ld->sbn1_recv, HPX_NULL);
#endif

  // 4. join the and for this epoch---the _advanceDomain action is waiting on
  //    this before it performs local computation for the epoch
  hpx_lco_and_set(ld->sbn1_and, HPX_NULL);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_PINNED,
                  _SBN1_result, _SBN1_result_action,
                  HPX_POINTER, HPX_POINTER, HPX_SIZE_T);

/// Perform a single send operation for SBN1---basically just allocate and pack
/// a parcel. Exposed here as an action so that we can perform it in a parallel
/// spawn.
static int _SBN1_sends_action(Domain *domain, int destLocalIdx) {
  CHECK_AFFINITY(domain);

  // Acquire a large-enough buffer to pack into.
  // - NULL first parameter means it comes with the parcel and is managed by
  //   the parcel and freed by the system inside of send()
  size_t bytes = sizeof(NodalArgs) + BUFSZ[destLocalIdx];
  hpx_parcel_t  *p = hpx_parcel_acquire(NULL, bytes);
  assert(p);

  // interpret the parcel's buffer as a NodalArgs, and pack into its buffer
  NodalArgs *nodal = hpx_parcel_get_data(p);
  int           nx = domain->sizeX + 1;
  int           ny = domain->sizeY + 1;
  int           nz = domain->sizeZ + 1;
  send_t      pack = SENDER[destLocalIdx];
  pack(nx, ny, nz, domain->nodalMass, nodal->buf);

  // compute the neighbor we are sending to (as byte offset in the domain)
  int    srcRemoteIdx = destLocalIdx;
  int     srcLocalIdx = 25 - srcRemoteIdx;
  int        distance = -OFFSET[srcLocalIdx];
  hpx_addr_t neighbor = get_domain_addr(domain->base, domain->rank + distance);
  count_local(neighbor);

  // pass along the source local index
  nodal->srcLocalIdx = srcLocalIdx;

  // static unsigned count[8] = {0};
  // printf("send %u %u\n", domain->rank, count[domain->rank]++);
  // fflush(stdout);

  // send the parcel--sync is fine because we packed in-place, we don't have a
  // continuation for this action since it's going to join the allreduce barrier
  // allocated at the remote domain, and we don't know it's global address here
  hpx_parcel_set_target(p, neighbor);
  hpx_parcel_set_action(p, _SBN1_result);
  return hpx_parcel_send_sync(p);
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _SBN1_sends, _SBN1_sends_action,
                  HPX_POINTER, HPX_INT);

/// Perform a parallel spawn of SBN1 messages for the domain
void SBN1(Domain *domain) {
  const int    nsTF = domain->sendTF[0];
  const int *sendTF = &domain->sendTF[1];
  hpx_addr_t  local = hpx_thread_current_target();
  hpx_addr_t   done = hpx_lco_and_new(nsTF);
  for (int i = 0; i < nsTF; ++i) {
    hpx_call(local, _SBN1_sends, done, &sendTF[i]);
  }
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);
}
