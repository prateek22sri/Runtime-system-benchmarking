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

static int _PosVel_result_action(Domain *ld, NodalArgs *args, size_t size) {
  CHECK_AFFINITY(ld);

  // prepare for the unpack, do this here to minimize the time spent holding the
  // lock
  int srcLocalIdx = args->srcLocalIdx;
  double *src = args->buf;

  int recvcnt = XFERCNT[srcLocalIdx];
  recv_t unpack = RECEIVER[srcLocalIdx];

  int nx = ld->sizeX + 1;
  int ny = ld->sizeY + 1;
  int nz = ld->sizeZ + 1;

  // 1. wait until it is safe to recv
  hpx_lco_wait(ld->posvel_recv);

  CHECK_AFFINITY(ld);

  // 2. update
  unpack(nx, ny, nz, src, ld->x, 1);
  unpack(nx, ny, nz, src + recvcnt, ld->y, 1);
  unpack(nx, ny, nz, src + recvcnt*2, ld->z, 1);
  unpack(nx, ny, nz, src + recvcnt*3, ld->xd, 1);
  unpack(nx, ny, nz, src + recvcnt*4, ld->yd, 1);
  unpack(nx, ny, nz, src + recvcnt*5, ld->zd, 1);

  // 3. if the recv is a semaphore, release it
#ifdef ENABLE_SERIAL_RECV
  hpx_lco_sema_v(ld->posvel_recv, HPX_NULL);
#endif

  // 4. join the and for this epoch---the _advanceDomain action is waiting on
  //    this before it performs local computation for the epoch
  hpx_lco_and_set(ld->posvel_and, HPX_NULL);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_PINNED,
                  _PosVel_result, _PosVel_result_action,
                  HPX_POINTER, HPX_POINTER, HPX_SIZE_T);

static int _PosVel_sends_action(Domain *domain, int destLocalIdx) {
  CHECK_AFFINITY(domain);

  // Acquire a large-enough buffer to pack into.
  // - NULL first parameter means it comes with the parcel and is managed by
  //   the parcel and freed by the system inside of send()
  size_t bytes = sizeof(NodalArgs) + BUFSZ[destLocalIdx];
  // printf("PosVel bytes %zu\n", bytes);
  hpx_parcel_t *p = hpx_parcel_acquire(NULL, bytes);
  assert(p);

  // "interpret the parcel buffer as a Nodal"
  NodalArgs *nodal = hpx_parcel_get_data(p);

  send_t pack = SENDER[destLocalIdx];

  int nx = domain->sizeX + 1;
  int ny = domain->sizeY + 1;
  int nz = domain->sizeZ + 1;
  int sendcnt = XFERCNT[destLocalIdx];

  pack(nx, ny, nz, domain->x, nodal->buf);
  pack(nx, ny, nz, domain->y, nodal->buf + sendcnt);
  pack(nx, ny, nz, domain->z, nodal->buf + sendcnt*2);
  pack(nx, ny, nz, domain->xd, nodal->buf + sendcnt*3);
  pack(nx, ny, nz, domain->yd, nodal->buf + sendcnt*4);
  pack(nx, ny, nz, domain->zd, nodal->buf + sendcnt*5);

  // the neighbor this is being sent to
  int srcRemoteIdx = destLocalIdx;
  int srcLocalIdx = 25 - srcRemoteIdx;
  int distance = -OFFSET[srcLocalIdx];
  hpx_addr_t neighbor = get_domain_addr(domain->base, domain->rank + distance);
  count_local(neighbor);

  // pass along the source local index
  nodal->srcLocalIdx = srcLocalIdx;

  hpx_parcel_set_target(p, neighbor);
  hpx_parcel_set_action(p, _PosVel_result);
  return hpx_parcel_send_sync(p);
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _PosVel_sends, _PosVel_sends_action,
                  HPX_POINTER, HPX_INT);

void PosVel(Domain *domain) {
  // doRecv = F, doSend = F, planeOnly = F, data = x, y, z, xd, yd, zd
  // pack outgoing data
  int nsFF = domain->sendFF[0];
  int *sendFF = &domain->sendFF[1];

  hpx_addr_t local = hpx_thread_current_target();
  hpx_addr_t done = hpx_lco_and_new(nsFF);
  for (int i = 0; i < nsFF; i++) {
    hpx_call(local, _PosVel_sends, done, &sendFF[i]);
  }
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);
}

