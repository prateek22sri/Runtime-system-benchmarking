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

static int _MonoQ_result_action(Domain *ld, NodalArgs *args, size_t size) {
  CHECK_AFFINITY(ld);

  // prepare for the unpack, do this here to minimize the time spent holding the
  // lock
  int srcLocalIdx = args->srcLocalIdx;
  double *src = args->buf;

  // 1. wait to recv
  hpx_lco_wait(ld->monoq_recv);

  CHECK_AFFINITY(ld);

  // move pointers to the ghost area
  double *delv_xi = ld->delv_xi;
  double *delv_eta = ld->delv_eta;
  double *delv_zeta = ld->delv_zeta;
  delv_xi += ld->numElem;
  delv_eta += ld->numElem;
  delv_zeta += ld->numElem;

  int nx = ld->sizeX;
  int planeElem = nx*nx;

  // 2. update
  int i = ld->reverse_recvTT[srcLocalIdx];
  memcpy(delv_xi + i*planeElem, src, sizeof(double)*planeElem);
  memcpy(delv_eta + i*planeElem, src + planeElem, sizeof(double)*planeElem);
  memcpy(delv_zeta + i*planeElem, src + planeElem*2, sizeof(double)*planeElem);

  // 3. if the recv is a semaphore, release it
#ifdef ENABLE_SERIAL_RECV
  hpx_lco_sema_v(ld->monoq_recv, HPX_NULL);
#endif

  // 4. join the and for this epoch---the _advanceDomain action is waiting on
  //    this before it performs local computation for the epoch
  hpx_lco_and_set(ld->monoq_and, HPX_NULL);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED | HPX_PINNED,
                  _MonoQ_result, _MonoQ_result_action,
                  HPX_POINTER, HPX_POINTER, HPX_SIZE_T);

static int _MonoQ_sends_action(Domain *domain, int destLocalIdx) {
  CHECK_AFFINITY(domain);

  // Acquire a large-enough buffer to pack into.
  // - NULL first parameter means it comes with the parcel and is managed by
  //   the parcel and freed by the system inside of send()
  size_t bytes = sizeof(NodalArgs) + BUFSZ[destLocalIdx];
  // printf("MonoQ bytes %zu\n", bytes);
  hpx_parcel_t *p = hpx_parcel_acquire(NULL, bytes);
  assert(p);

  // "interpret the parcel buffer as a Nodal"
  NodalArgs *nodal = hpx_parcel_get_data(p);

  send_t pack = SENDER[destLocalIdx];

  int nx = domain->sizeX;
  int ny = domain->sizeY;
  int nz = domain->sizeZ;

  double *delv_xi = domain->delv_xi;
  double *delv_eta = domain->delv_eta;
  double *delv_zeta = domain->delv_zeta;

  int planeElem = nx*nx;

  pack(nx, ny, nz, delv_xi, nodal->buf);
  pack(nx, ny, nz, delv_eta, nodal->buf + planeElem);
  pack(nx, ny, nz, delv_zeta, nodal->buf + planeElem*2);

  // the neighbor this is being sent to
  int srcRemoteIdx = destLocalIdx;
  int srcLocalIdx = 25 - srcRemoteIdx;
  int distance = -OFFSET[srcLocalIdx];
  hpx_addr_t neighbor = get_domain_addr(domain->base, domain->rank + distance);
  count_local(neighbor);

  // pass along the source local index
  nodal->srcLocalIdx = srcLocalIdx;

  hpx_parcel_set_target(p, neighbor);
  hpx_parcel_set_action(p, _MonoQ_result);
  return hpx_parcel_send_sync(p);
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _MonoQ_sends, _MonoQ_sends_action,
                  HPX_POINTER, HPX_INT);

void MonoQ(Domain *domain) {
  // pack outgoing data
  int nsTT = domain->sendTT[0];
  int *sendTT = &domain->sendTT[1];

  hpx_addr_t local = hpx_thread_current_target();
  hpx_addr_t done = hpx_lco_and_new(nsTT);
  for (int i = 0; i < nsTT; i++) {
    hpx_call(local, _MonoQ_sends, done, &sendTT[i]);
  }
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);
  // okay to release domain lock
}
