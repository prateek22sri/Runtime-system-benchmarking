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
#include "hpcg.h"

void unpack(NodalArgs *args,int n,Domain *domain);

typedef int local_int_t;
typedef long long global_int_t;

hpx_action_t _exchange_sends = 0;
hpx_action_t _exchange_result = 0;

int _exchange_result_action(Domain *domain, NodalArgs *args, size_t size)
{
  int n = args->n;

  // 1. wait until it is safe to recv
  hpx_lco_wait(domain->lcos.exchange_swap[n]);
  hpx_lco_wait(domain->lcos.exchange_recv[n]);

  //printf(" Message: from %d to %d \n",args->src,domain->params.comm_rank);

  // 2. update
  unpack(args,n,domain);

  hpx_lco_sema_v(domain->lcos.exchange_recv[n], HPX_NULL);

  // 3. join
  hpx_lco_and_set(domain->lcos.exchange_and[n], HPX_NULL);

  return HPX_SUCCESS;
}

static hpx_parcel_t *_make_exchange_parcel(EHS *ld) {
  size_t bytes = sizeof(NodalArgs) + ld->n_send * sizeof(double);
  hpx_parcel_t *p = hpx_parcel_acquire(NULL, bytes);
  assert(p);

  // "interpret the parcel buffer as a Nodal"
  NodalArgs *nodal = (NodalArgs*) hpx_parcel_get_data(p);

  for (int i = 0, e = ld->n_send; i < e; ++i) {
    nodal->buf[i] = ld->xv[ld->elements[i]];
  }

  nodal->n = ld->n;
  nodal->send_length = ld->n_send;
  nodal->src = ld->src;

  hpx_addr_t block = get_domain_addr(ld->base, ld->destination);
  hpx_parcel_set_target(p, block);
  hpx_parcel_set_action(p, _exchange_result);

  return p;
}

int _exchange_sends_action(EHS *ld, size_t UNUSED) {
  return hpx_parcel_send_sync(_make_exchange_parcel(ld));
}

void exchange_halo_helper(int num_neighbors, local_int_t *sendLength,
                          local_int_t *elementsToSend, double *xv,
                          int *neighbors, int generation, Domain *domain)
{
  int n = generation & 1;
  EHS ld = {
    .src = domain->params.comm_rank,
    .destination = -1,
    .n_send = -1,
    .n = n,
    .base = domain->base,
    .xv = xv,
    .elements = elementsToSend
  };

  double tt = mytimer();
  //hpx_addr_t done = hpx_lco_and_new(num_neighbors);
  for (int i = 0, e = num_neighbors; i < e; ++i) {
    int nelements = sendLength[i];
    domain->exchange_bytes += nelements*sizeof(double) + sizeof(NodalArgs);
    ld.destination = neighbors[i];
    ld.n_send = nelements;
    // hpx_call(HPX_HERE, _exchange_sends, done, &ld, sizeof(ld));
    hpx_parcel_send_sync(_make_exchange_parcel(&ld));
    ld.elements += nelements;
  }
  // hpx_lco_wait(done);
  // hpx_lco_delete(done, HPX_NULL);
  domain->eh_t += mytimer() - tt;
  domain->exchange_messages += num_neighbors;

  hpx_lco_set(domain->lcos.exchange_recv[n], 0, NULL, HPX_NULL, HPX_NULL);
  hpx_lco_wait_reset(domain->lcos.exchange_and[n]);
  hpx_lco_reset_sync(domain->lcos.exchange_recv[n]);
  hpx_lco_reset_sync(domain->lcos.exchange_swap[n]);
}

