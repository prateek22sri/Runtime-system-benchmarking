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

static int _SBN3_result_action(Domain *ld, NodalArgs *args, size_t size) {
  CHECK_AFFINITY(ld);

  // prepare for the unpack, do this here to minimize the time spent holding the
  // lock
  int srcLocalIdx = args->srcLocalIdx;
  int n = args->n;
  double *src = args->buf;

  int recvcnt = XFERCNT[srcLocalIdx];
  recv_t unpack = RECEIVER[srcLocalIdx];

  int nx = ld->sizeX + 1;
  int ny = ld->sizeY + 1;
  int nz = ld->sizeZ + 1;

  // 1. wait until it is safe to recv
  hpx_lco_wait(ld->sbn3_recv[n]);

  CHECK_AFFINITY(ld);

  // 2. update
  unpack(nx, ny, nz, src, ld->fx, 0);
  unpack(nx, ny, nz, src + recvcnt, ld->fy, 0);
  unpack(nx, ny, nz, src + recvcnt*2, ld->fz, 0);

  // 3. if the recv is a semaphore, release it
#ifdef ENABLE_SERIAL_RECV
  hpx_lco_sema_v(ld->sbn3_recv[n], HPX_NULL);
#endif

  // 4. join the and for this epoch---the _advanceDomain action is waiting on
  //    this before it performs local computation for the epoch
  hpx_lco_and_set(ld->sbn3_and[n], HPX_NULL);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_PINNED, _SBN3_result,
                  _SBN3_result_action, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);

static int _SBN3_sends_action(Domain *domain, int destLocalIdx, int n) {
  CHECK_AFFINITY(domain);

  // Acquire a large-enough buffer to pack into.
  // - NULL first parameter means it comes with the parcel and is managed by
  //   the parcel and freed by the system inside of send()
  size_t bytes = sizeof(NodalArgs) + BUFSZ[destLocalIdx];
  // printf("SBN3 bytes %zu\n", bytes);
  hpx_parcel_t *p = hpx_parcel_acquire(NULL, bytes);
  assert(p);

  // "interpret the parcel buffer as a Nodal"
  NodalArgs *nodal = hpx_parcel_get_data(p);

  send_t pack = SENDER[destLocalIdx];

  int nx = domain->sizeX + 1;
  int ny = domain->sizeY + 1;
  int nz = domain->sizeZ + 1;
  int sendcnt = XFERCNT[destLocalIdx];
  pack(nx, ny, nz, domain->fx, nodal->buf);
  pack(nx, ny, nz, domain->fy, nodal->buf + sendcnt);
  pack(nx, ny, nz, domain->fz, nodal->buf + sendcnt*2);

  // the neighbor this is being sent to
  int srcRemoteIdx = destLocalIdx;
  int srcLocalIdx = 25 - srcRemoteIdx;
  int distance = -OFFSET[srcLocalIdx];
  hpx_addr_t neighbor = get_domain_addr(domain->base, domain->rank + distance);
  count_local(neighbor);

  // pass along the source local index
  nodal->srcLocalIdx = srcLocalIdx;
  nodal->n = n;

  hpx_parcel_set_target(p, neighbor);
  hpx_parcel_set_action(p, _SBN3_result);

  return hpx_parcel_send_sync(p);
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _SBN3_sends, _SBN3_sends_action,
                  HPX_POINTER, HPX_INT, HPX_INT);

void SBN3(Domain *domain, int n) {
  // pack outgoing data
  int nsTF = domain->sendTF[0];
  int *sendTF = &domain->sendTF[1];

  hpx_addr_t local = hpx_thread_current_target();
  hpx_addr_t done = hpx_lco_and_new(nsTF);
  // must hold the domain lock this entire time
  for (int i = 0; i < nsTF; i++) {
    hpx_call(local, _SBN3_sends, done, &sendTF[i], &n);
  }
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);
}
