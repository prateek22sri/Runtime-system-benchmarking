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

#include <inttypes.h>
#include "trace.h"
#include "lulesh-hpx.h"

#ifdef DIAGNOSTICS
uint64_t remote_comms = 0;
uint64_t total_comms = 0;
#endif

const char *map_path = NULL;

global_config_t global_config = {
  .core_major = true,
  .num_domains = -1
};

static void initdouble_handler(double *input, const size_t size) {
  assert(sizeof(double) == size);
  *input = 99999999.0;
}
static HPX_ACTION(HPX_FUNCTION, 0, initdouble, initdouble_handler);

static void mindouble_handler(double *output, const double *input, const size_t size) {
  assert(sizeof(double) == size);
  if ( *output > *input ) *output = *input;
}
static HPX_ACTION(HPX_FUNCTION, 0, mindouble, mindouble_handler);

static void initmaxdouble_handler(double *input, const size_t size) {
  assert(sizeof(double) == size);
  *input = 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, initmaxdouble, initmaxdouble_handler);

static void maxdouble_handler(double *output,const double *input, const size_t size) {
  assert(sizeof(double) == size);
  if ( *output < *input ) *output = *input;
}
static HPX_ACTION(HPX_FUNCTION, 0, maxdouble, maxdouble_handler);

#ifdef DIAGNOSTICS
static void initint_handler(uint64_t *input, const size_t size) {
  *input = 0;
}
static HPX_ACTION(HPX_FUNCTION, 0, initint, initint_handler);

static void sumint_handler(uint64_t *output, const uint64_t *input, const size_t size) {
  *output += *input;
}
static HPX_ACTION(HPX_FUNCTION, 0, sumint, sumint_handler);

static int _get_comms_action(hpx_addr_t remotes, hpx_addr_t totals) {
  hpx_lco_set(remotes, sizeof(remote_comms), &remote_comms, HPX_NULL, HPX_NULL);
  hpx_lco_set(totals, sizeof(total_comms), &total_comms, HPX_NULL, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _get_comms, _get_comms_action, HPX_ADDR, HPX_ADDR);
#endif

static void _printOutput(Domain *domain, double tSBN3, double tPosVel, double tMonoQ) {
  int nx = domain->sizeX;
  double elapsed_time_max;
  hpx_lco_get(domain->elapsed_ar, sizeof(double), &elapsed_time_max);

  printf("\n\nSTART_LOG\n");
  printf("PROGNAME: lulesh-parcels\n\n");
  printf("Elapsed time = %12.6e\n\n", elapsed_time_max/1000.0);

  printf("Run completed:  \n");
  printf("  Problem size = %d \n"
         "  Iteration count = %d \n"
         "  Final Origin Energy = %12.6e\n",nx,domain->cycle,domain->e[0]);
  double MaxAbsDiff = 0.0;
  double TotalAbsDiff = 0.0;
  double MaxRelDiff = 0.0;
  int j,k;
  for (j = 0; j < nx; j++) {
    for (k = j + 1; k < nx; k++) {
      double AbsDiff = fabs(domain->e[j*nx + k] - domain->e[k*nx + j]);
      TotalAbsDiff += AbsDiff;

      if (MaxAbsDiff < AbsDiff)
        MaxAbsDiff = AbsDiff;

      double RelDiff = AbsDiff/domain->e[k*nx + j];
      if (MaxRelDiff < RelDiff)
        MaxRelDiff = RelDiff;
    }
  }
  printf("  Testing plane 0 of energy array:\n"
         "  MaxAbsDiff   = %12.6e\n"
         "  TotalAbsDiff = %12.6e\n"
         "  MaxRelDiff   = %12.6e\n\n", MaxAbsDiff, TotalAbsDiff, MaxRelDiff);


  printf("END_LOG\n\n");

  printf("time_in_SBN3 = %e\n", tSBN3/1000.0);
  printf("time_in_PosVel = %e\n", tPosVel/1000.0);
  printf("time_in_MonoQ = %e\n", tMonoQ/1000.0);
}

static double _advanceSBN3(Domain *domain, int n) {
  n = (n & 1) ? 1 : 0;
  hpx_time_t t_s = hpx_time_now();
  TRACE(domain, START_SBN3_SENDS);
  SBN3(domain, n);
  TRACE(domain, START_SBN3_RECVS);
  hpx_lco_set(domain->sbn3_recv[n], 0, NULL, HPX_NULL, HPX_NULL);
  hpx_lco_wait_reset(domain->sbn3_and[n]);
  hpx_lco_reset_sync(domain->sbn3_recv[n]);
  CHECK_AFFINITY(domain);
  TRACE(domain, END_SBN3_RECVS);
  return hpx_time_elapsed_ms(t_s);
}

static double _advancePosVel(Domain *domain) {
  hpx_time_t t_s = hpx_time_now();
  TRACE(domain, START_POSVEL_SENDS);
  PosVel(domain);
  TRACE(domain, START_POSVEL_RECVS);
  hpx_lco_set(domain->posvel_recv, 0, NULL, HPX_NULL, HPX_NULL);
  hpx_lco_wait_reset(domain->posvel_and);
  hpx_lco_reset_sync(domain->posvel_recv);
  CHECK_AFFINITY(domain);
  TRACE(domain, END_POSVEL_RECVS);
  return hpx_time_elapsed_ms(t_s);
}

static double _advanceMonoQ(Domain *domain) {
  hpx_time_t t_s = hpx_time_now();
  // LagrangeElements waits for the monoq_and internally
  TRACE(domain, START_LAGRANGE_ELEMENTS);
  LagrangeElements(domain);
  return hpx_time_elapsed_ms(t_s);
}

// perform one epoch of the algorithm
static int _advanceDomain_action(Domain *dom) {
  // on the very first cycle, exchange nodalMass information
  SBN1(dom);

  // allow any pending SBN1 messages to arrive
  hpx_lco_set(dom->sbn1_recv, 0, NULL, HPX_NULL, HPX_NULL);
  hpx_lco_wait(dom->sbn1_and);

  // keep track of times
  hpx_time_t start_time = hpx_time_now();
  double          tSBN3 = 0.0;
  double        tPosVel = 0.0;
  double         tMonoQ = 0.0;

  // Perform the local computation for epoch
  for (int n = 0, e = dom->maxcycles; n < e && dom->time < dom->stoptime; ++n) {
    TRACE(dom, START_ALLREDUCE);

    // first half of the allreduce, SBN3 can occur in parallel
    if ((dom->dtfixed <= 0.0) && (dom->cycle != 0)) {
      double gnewdt = 1.0e+20;
      if (dom->dtcourant < gnewdt) {
        gnewdt = dom->dtcourant/2.0;
      }
      if (dom->dthydro < gnewdt) {
        gnewdt = dom->dthydro*2.0/3.0;
      }
      // allreduce on gnewdt
      hpx_process_collective_allreduce_join(dom->newdt, dom->reduceid,
                                            sizeof(double), &gnewdt);
    }

    TRACE(dom, START_CALC_FORCE);
    CalcForceForNodes(dom,dom->rank);

    tSBN3 += _advanceSBN3(dom, n);

    CalcAccelerationForNodes(dom->xdd, dom->ydd, dom->zdd,
                             dom->fx, dom->fy, dom->fz,
                             dom->nodalMass, dom->numNode);

    ApplyAccelerationBoundaryConditionsForNodes(dom->xdd, dom->ydd, dom->zdd,
                                                dom->symmX, dom->symmY,
                                                dom->symmZ, dom->sizeX);

    // get the reduced dt
    TRACE(dom, START_ALLREDUCE_GET);
    if ((dom->dtfixed <= 0.0) && (dom->cycle != 0)) {
      double newdt;
      hpx_lco_get_reset(dom->allreduce,sizeof(double),&newdt);
      CHECK_AFFINITY(dom);

      double olddt = dom->deltatime;
      double ratio = newdt/olddt;
      if (ratio >= 1.0) {
        if (ratio < dom->deltatimemultlb) {
          newdt = olddt;
        } else if (ratio > dom->deltatimemultub) {
          newdt = olddt*dom->deltatimemultub;
        }
      }

      if (newdt > dom->dtmax) {
        newdt = dom->dtmax;
      }

      dom->deltatime = newdt;
    }
    TRACE(dom, END_ALLREDUCE_GET);

    double targetdt = dom->stoptime - dom->time;
    if ((targetdt > dom->deltatime) && (targetdt < 4.0*dom->deltatime/3.0)) {
      targetdt = 2.0*dom->deltatime/3.0;
    }

    if (targetdt < dom->deltatime) {
      dom->deltatime = targetdt;
    }

    TRACE(dom, START_CALC_VELOCITY);
    CalcVelocityForNodes(dom->xd, dom->yd, dom->zd,
                         dom->xdd, dom->ydd, dom->zdd,
                         dom->deltatime, dom->u_cut, dom->numNode);
    TRACE(dom, START_CALC_POSITION);
    CalcPositionForNodes(dom->x, dom->y, dom->z,
                         dom->xd, dom->yd, dom->zd,
                         dom->deltatime, dom->numNode);

    tPosVel += _advancePosVel(dom);
    tMonoQ += _advanceMonoQ(dom);

    TRACE(dom, START_CALC_TIMECONSTRAINTS);
    CalcTimeConstraintsForElems(dom);
    TRACE(dom, END_CALC_TIMECONSTRAINTS);

    dom->time += dom->deltatime;

    dom->cycle++;
    // printf("============================================================== domain %d iter %d\n", dom->rank, n);
  }

  double elapsed_time_local = hpx_time_elapsed_ms(start_time);
  hpx_lco_set(dom->elapsed_ar, sizeof(double), &elapsed_time_local, HPX_NULL, HPX_NULL);
  //printf("For domain %d worker changes: %"PRIu64"\n", dom->rank, dom->coreSwitches);

  TRACE_FINI(dom);

  if (dom->rank == 0) {
    _printOutput(dom, tSBN3, tPosVel, tMonoQ);
  }

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _advanceDomain, _advanceDomain_action, HPX_POINTER);

static int _timing_handler(Domain *ld, double value) {
  static hpx_time_t _li;
  static int iteration = 1;
  hpx_time_t now = hpx_time_now();
  printf("%d %.1fms\n", iteration++, hpx_time_diff_ms(_li, now));
  _li = now;
  hpx_lco_set_rsync(ld->allreduce, sizeof(value), &value);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_INTERRUPT, HPX_PINNED, _timing, _timing_handler,
                  HPX_POINTER, HPX_DOUBLE);

static int _initDomain_action(Domain *ld, hpx_addr_t base, int index, int nDoms,
                              int nx, int maxcycles, hpx_addr_t newdt,
                              hpx_addr_t elapsed_ar, hpx_addr_t tracelock) {
  int tp = (int) (cbrt(nDoms) + 0.5);
  Init(tp,nx);
  int col      = index%tp;
  int row      = (index/tp)%tp;
  int plane    = index/(tp*tp);
  ld->base = base;

  hpx_addr_t local = hpx_thread_current_target();
  Domain *domain = NULL;
  if (!hpx_gas_try_pin(local, (void**)&domain))
    return HPX_RESEND;

  SetDomain(index, col, row, plane, nx, tp, nDoms, maxcycles,ld);

  ld->newdt = newdt;
  ld->elapsed_ar = elapsed_ar;

  ld->tracelock = tracelock;
  ld->allreduce = hpx_lco_future_new(sizeof(double));
  if (TIMING && ld->rank == 0) {
    ld->reduceid = hpx_process_collective_allreduce_subscribe(ld->newdt, _timing,
                                                              hpx_thread_current_target());
  }
  else {
    ld->reduceid = hpx_process_collective_allreduce_subscribe(ld->newdt,
                                                              hpx_lco_set_action,
                                                              ld->allreduce);
  }

  // the recv control locks---start locked to prevent recvs
#ifdef ENABLE_SERIAL_RECV
  ld->sbn1_recv = hpx_lco_sema_new(0);
  ld->sbn3_recv[0] = hpx_lco_sema_new(0);
  ld->sbn3_recv[1] = hpx_lco_sema_new(0);
  ld->posvel_recv = hpx_lco_sema_new(0);
  ld->monoq_recv = hpx_lco_sema_new(0);
#else
  ld->sbn1_recv = hpx_lco_future_new(0);
  ld->sbn3_recv[0] = hpx_lco_future_new(0);
  ld->sbn3_recv[1] = hpx_lco_future_new(0);
  ld->posvel_recv = hpx_lco_future_new(0);
  ld->monoq_recv = hpx_lco_future_new(0);
#endif

  // allocate the initial allreduce and gate
  ld->sbn1_and = hpx_lco_and_new(ld->recvTF[0]);
  ld->sbn3_and[0] = hpx_lco_and_new(ld->recvTF[0]);
  ld->sbn3_and[1] = hpx_lco_and_new(ld->recvTF[0]);
  ld->posvel_and = hpx_lco_and_new(ld->recvFF[0]);
  ld->monoq_and = hpx_lco_and_new(ld->recvTT[0]);

  // set up an affinity
  int id = hpx_thread_get_tls_id();
  ld->affinity = id % HPX_THREADS;
  hpx_gas_set_affinity(local, ld->affinity);

  ld->lastCore = 0;
  ld->coreSwitches = 0;
  ld->actions = 0;

  // set up tracing
  TRACE_INIT(ld);

  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _initDomain, _initDomain_action,
                  HPX_POINTER, HPX_ADDR, HPX_INT, HPX_INT, HPX_INT, HPX_INT,
                  HPX_ADDR, HPX_ADDR, HPX_ADDR);

static int _finiDomain_action(Domain *ld) {
  hpx_lco_delete(ld->sbn1_and, HPX_NULL);
  hpx_lco_delete(ld->sbn3_and[0], HPX_NULL);
  hpx_lco_delete(ld->sbn3_and[1], HPX_NULL);
  hpx_lco_delete(ld->posvel_and, HPX_NULL);
  hpx_lco_delete(ld->monoq_and, HPX_NULL);

  hpx_lco_delete(ld->sbn1_recv, HPX_NULL);
  hpx_lco_delete(ld->sbn3_recv[0], HPX_NULL);
  hpx_lco_delete(ld->sbn3_recv[1], HPX_NULL);
  hpx_lco_delete(ld->posvel_recv, HPX_NULL);
  hpx_lco_delete(ld->monoq_recv, HPX_NULL);

  hpx_process_collective_allreduce_unsubscribe(ld->newdt, ld->reduceid);
  hpx_lco_delete(ld->allreduce, HPX_NULL);

  struct {
    int id;
    int rank;
  } id = { ld->reduceid, HPX_LOCALITY_ID };
  DestroyDomain(ld);
  HPX_THREAD_CONTINUE(id);
}
static HPX_ACTION(HPX_DEFAULT, HPX_PINNED, _finiDomain, _finiDomain_action, HPX_POINTER);

static int _main_action(int nDoms, int nx, int maxcycles) {
  hpx_time_t t1 = hpx_time_now();

  hpx_addr_t domain = hpx_gas_alloc_cyclic(nDoms,sizeof(Domain),0);

  // Initialize the domains
  hpx_addr_t newdt = hpx_process_collective_allreduce_new(sizeof(double),
                                                          initdouble, mindouble);
  hpx_addr_t elapsed_ar = hpx_lco_reduce_new(nDoms, sizeof(double),
                                             initmaxdouble, maxdouble);
  hpx_addr_t tracelock = hpx_lco_sema_new(1);

  hpx_addr_t init = hpx_lco_and_new(nDoms);
  for (int k = 0; k < nDoms; k++) {
    hpx_addr_t block = get_domain_addr(domain, k);
    hpx_call(block, _initDomain, init, &domain, &k, &nDoms, &nx, &maxcycles,
             &newdt, &elapsed_ar, &tracelock);
  }
  hpx_lco_wait_reset(init);

  // Spawn the first epoch, _advanceDomain will recursively spawn each epoch.
  hpx_addr_t complete = hpx_lco_and_new(nDoms);
  for (int k = 0; k < nDoms; k++) {
    hpx_addr_t block = get_domain_addr(domain, k);
    hpx_call(block, _advanceDomain, complete);
  }

  // And wait for each domain to reach the end of its simulation
  hpx_lco_wait(complete);
  hpx_lco_delete(complete, HPX_NULL);

  double elapsed = hpx_time_elapsed_ms(t1);
  printf(" Elapsed: %g\n",elapsed);

#ifdef DIAGNOSTICS
  hpx_addr_t remotes_r = hpx_lco_reduce_new(hpx_get_num_ranks(),
                                            sizeof(remote_comms),
                                            initint,
                                            sumint);
  hpx_addr_t totals_r = hpx_lco_reduce_new(hpx_get_num_ranks(),
                                           sizeof(total_comms),
                                           initint,
                                           sumint);
  hpx_bcast(_get_comms, HPX_NULL, &remotes_r, &totals_r);
  uint64_t remotes, totals;
  hpx_lco_get(remotes_r, sizeof(remotes), (void*)&remotes);
  hpx_lco_get(totals_r, sizeof(totals), (void*)&totals);
  printf("remote_comms = %"PRIu64" totals_comms = %"PRIu64" ratio = %f\n",
         remotes, totals, (double)remotes/(double)totals);
#endif
  int tp = (int) (cbrt(nDoms) + 0.5);
  FILE *map = (map_path) ? fopen(map_path, "w") : NULL;
  if (map) {
    fprintf(map, "id, x, y, z, user id, rank\n");
  }
  for (int k = 0; k < nDoms; k++) {
    hpx_addr_t block = get_domain_addr(domain, k);
    int col      = k % tp;
    int row      = (k / tp) % tp;
    int plane    = k / (tp * tp);
    struct {
      int id;
      int rank;
    } id = { -1, -1 };
    hpx_call_sync(block, _finiDomain, &id, sizeof(id));
    if (map) {
      fprintf(map, "%llu, %d, %d, %d, %d, %d\n", block, col, row, plane, id.id, id.rank);
    }
  }
  if (map) {
    fprintf(map, "%llu, 0, 0, 0, -1, -1\n", newdt);
    fclose(map);
  }
  hpx_lco_delete(init, HPX_NULL);
  hpx_lco_delete(tracelock, HPX_NULL);
  hpx_lco_delete(elapsed_ar, HPX_NULL);

  hpx_process_collective_allreduce_delete(newdt);
  hpx_gas_free(domain, HPX_NULL);
  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _main, _main_action, HPX_INT, HPX_INT,
                  HPX_INT);

static void usage(FILE *f) {
  fprintf(f, "Usage: [options]\n"
          "\t-o rank, rank-major ordering"
          "\t-c, cores\n"
          "\t-t, scheduler threads\n"
          "\t-D, all localities wait for debugger\n"
          "\t-d, wait for debugger at specific locality\n"
          "\t-n, number of domains,nDoms\n"
          "\t-m, path to output map file\n"
          "\t-x, nx\n"
          "\t-i, maxcycles\n"
          "\t-h, show help\n");
}

int main(int argc, char **argv) {
  bool core_major = true;
  int nDoms = 8;
  int nx = 15;
  int maxcycles = 10;

  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "Failed to initialize hpx\n");
    return -1;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "n:m:x:i:o:h?")) != -1) {
    switch (opt) {
     case 'o':
      if (0 == strncmp("rank", optarg, 4)) {
        core_major = false;
      }
      break;
     case 'n':
      nDoms = atoi(optarg);
      break;
     case 'x':
      nx = atoi(optarg);
      break;
     case 'i':
      maxcycles = atoi(optarg);
      break;
     case 'm':
      map_path = optarg;
      break;
     case 'h':
      usage(stdout);
      return 0;
     case '?':
     default:
      usage(stderr);
      return -1;
    }
  }

  global_config.num_domains = nDoms;
  global_config.core_major = core_major;

  if (HPX_LOCALITY_ID == 0) {
    int tp = (int) (cbrt(nDoms) + 0.5);
    if (tp*tp*tp != nDoms) {
      fprintf(stderr, "Number of domains must be a cube of an integer (1, 8, 27, ...)\n");
      return -1;
    }
    printf(" Number of domains: %d nx: %d maxcycles: %d core-major ordering: %d\n",nDoms,nx,maxcycles,core_major);
  }

  e = hpx_run(&_main, NULL, &nDoms, &nx, &maxcycles);
  hpx_finalize();
  return e;
}
