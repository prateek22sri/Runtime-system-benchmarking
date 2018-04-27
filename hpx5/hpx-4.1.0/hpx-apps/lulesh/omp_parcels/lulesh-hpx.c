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

#include <inttypes.h>
#include "lulesh-hpx.h"
#include <hpx/lco.h>

static hpx_action_t _main          = 0;
static hpx_action_t _advanceDomain = 0;
static hpx_action_t _initDomain    = 0;

hpx_action_t _SBN1_sends = 0;
hpx_action_t _SBN1_result = 0;
hpx_action_t _SBN3_sends = 0;
hpx_action_t _SBN3_result = 0;
hpx_action_t _PosVel_sends = 0;
hpx_action_t _PosVel_result = 0;
hpx_action_t _MonoQ_sends = 0;
hpx_action_t _MonoQ_result = 0;

global_config_t global_config = {
  .core_major = true,
  .num_domains = -1
};

static hpx_action_t _init_global_config;
static int _init_global_config_action(global_config_t *cfg, size_t size) {
  if (hpx_get_my_rank() != 0)
    global_config = *cfg;
  return HPX_SUCCESS;
}

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

static hpx_action_t _get_comms;
static HPX_ACTION_DEF(HPX_DEFAULT, 0, _get_comms, _get_comms_action, HPX_ADDR, HPX_ADDR);
#endif

// perform one epoch of the algorithm
static int _advanceDomain_action(unsigned long *epoch, size_t size) {
  hpx_time_t start_time;
  double time_in_SBN3 = 0.0;
  double time_in_PosVel = 0.0;
  double time_in_MonoQ = 0.0;
  hpx_time_t t_s;

  unsigned long n = *epoch;
  hpx_addr_t local = hpx_thread_current_target();
  Domain *domain = NULL;
  if (!hpx_gas_try_pin(local, (void**)&domain))
    return HPX_RESEND;

  while (true) {

  // 0. If I've run enough cycles locally, then I want to join the global
  //    complete barrier (stored in my local domain as domain->complete)---this
  //    is the barrier the _main_action(, size_t size) thread is waiting on.
  if ( (domain->time >= domain->stoptime) || (domain->cycle >= domain->maxcycles)) {
    double elapsed_time_local = hpx_time_elapsed_ms(start_time);
    hpx_lco_set(domain->elapsed_ar, sizeof(double), &elapsed_time_local, HPX_NULL, HPX_NULL);

    if ( domain->rank == 0 ) {
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

      printf("time_in_SBN3 = %e\n", time_in_SBN3/1000.0);
      printf("time_in_PosVel = %e\n", time_in_PosVel/1000.0);
      printf("time_in_MonoQ = %e\n", time_in_MonoQ/1000.0);
    }

    hpx_gas_unpin(local);
    hpx_lco_set(domain->complete, 0, NULL, HPX_NULL, HPX_NULL);
    return HPX_SUCCESS;
  }
  // on the very first cycle, exchange nodalMass information
  if ( domain->cycle == 0 ) {
    // Send our allreduce messages for epoch n
    SBN1(local, domain, n);

    //  update the domain's epoch, this releases any pending the
    //    _SBN1_result_action messages, which will all acquire and release the
    //    domain lock and then join the sbn1_and reduction for the nth epoch
    hpx_lco_sema_v_sync(domain->sem_sbn1);

    //  wait for the allreduce for this epoch to complete locally
    hpx_lco_wait(domain->sbn1_and);
    hpx_lco_sema_p(domain->sem_sbn1);
    hpx_lco_delete(domain->sbn1_and, HPX_NULL);


    start_time = hpx_time_now();
  }


  // 4. Perform the local computation for epoch n
  double targetdt = domain->stoptime - domain->time;
  if ((domain->dtfixed <= 0.0) && (domain->cycle != 0)) {
    double gnewdt = 1.0e+20;
    if (domain->dtcourant < gnewdt)
      gnewdt = domain->dtcourant/2.0;
    if (domain->dthydro < gnewdt)
      gnewdt = domain->dthydro*2.0/3.0;

    // allreduce on gnewdt
    hpx_lco_set(domain->newdt,sizeof(double),&gnewdt,HPX_NULL,HPX_NULL);
  }

  domain->sbn3_and[(n + 1) % 2] = hpx_lco_and_new(domain->recvTF[0]);

  // send messages for epoch n
  t_s = hpx_time_now();
  CalcForceForNodes(local,domain,domain->rank,n);
  hpx_lco_sema_v_sync(domain->sem_sbn3);
  hpx_lco_wait(domain->sbn3_and[n % 2]);
  //  printf("Done with SBN3 on %d\n", domain->rank);
  hpx_lco_sema_p(domain->sem_sbn3);
  hpx_lco_delete(domain->sbn3_and[n % 2], HPX_NULL);
  time_in_SBN3 += hpx_time_elapsed_ms(t_s);

  CalcAccelerationForNodes(domain->xdd, domain->ydd, domain->zdd,
                             domain->fx, domain->fy, domain->fz,
                             domain->nodalMass, domain->numNode);

  ApplyAccelerationBoundaryConditionsForNodes(domain->xdd, domain->ydd, domain->zdd,
                                              domain->symmX, domain->symmY, domain->symmZ,
                                              domain->sizeX);

  if ((domain->dtfixed <= 0.0) && (domain->cycle != 0)) {
    double newdt;
    hpx_lco_get(domain->newdt,sizeof(double),&newdt);
    double olddt = domain->deltatime;
    double ratio = newdt/olddt;
    if (ratio >= 1.0) {
      if (ratio < domain->deltatimemultlb) {
        newdt = olddt;
      } else if (ratio > domain->deltatimemultub) {
        newdt = olddt*domain->deltatimemultub;
      }
    }

    if (newdt > domain->dtmax) {
      newdt = domain->dtmax;
    }

    domain->deltatime = newdt;
  }

  if ((targetdt > domain->deltatime) && (targetdt < 4.0*domain->deltatime/3.0)) {
    targetdt = 2.0*domain->deltatime/3.0;
  }

  if (targetdt < domain->deltatime) {
    domain->deltatime = targetdt;
  }

  CalcVelocityForNodes(domain->xd, domain->yd, domain->zd,
                         domain->xdd, domain->ydd, domain->zdd,
                         domain->deltatime, domain->u_cut, domain->numNode);

  CalcPositionForNodes(domain->x, domain->y, domain->z,
                         domain->xd, domain->yd, domain->zd,
                         domain->deltatime, domain->numNode);

  t_s = hpx_time_now();
  domain->posvel_and[(n + 1) % 2] = hpx_lco_and_new(domain->recvFF[0]);
  PosVel(local,domain,n);
  hpx_lco_sema_v_sync(domain->sem_posvel);
  hpx_lco_wait(domain->posvel_and[n % 2]);
  hpx_lco_sema_p(domain->sem_posvel);
  hpx_lco_delete(domain->posvel_and[n % 2], HPX_NULL);
  time_in_PosVel += hpx_time_elapsed_ms(t_s);

  t_s = hpx_time_now();
  domain->monoq_and[(n + 1) % 2] = hpx_lco_and_new(domain->recvTT[0]);
  LagrangeElements(local,domain,n);
  time_in_MonoQ += hpx_time_elapsed_ms(t_s);

  CalcTimeConstraintsForElems(domain);

  domain->time += domain->deltatime;

  domain->cycle++;

  // don't need this domain to be pinned anymore---let it move
  //  hpx_gas_unpin(local);

  //  printf("============================================================== domain %d iter %d\n", domain->rank, n);

  // 5. spawn the next epoch
  n = n + 1;

  //  return hpx_call(local, _advanceDomain, HPX_NULL, &next, sizeof(next));
  } // end while(true)
  return HPX_ERROR;
}

static int _initDomain_action(InitArgs *init, size_t size) {
  hpx_addr_t local = hpx_thread_current_target();
  Domain *ld = NULL;
  if (!hpx_gas_try_pin(local, (void**)&ld))
    return HPX_RESEND;

  int nx        = init->nx;
  int nDoms     = init->nDoms;
  int maxcycles = init->maxcycles;
  int index     = init->index;
  int tp        = (int) (cbrt(nDoms) + 0.5);

  Init(tp,nx);
  int col      = index%tp;
  int row      = (index/tp)%tp;
  int plane    = index/(tp*tp);
  ld->base = init->base;
  ld->sem_sbn1 = hpx_lco_sema_new(0);
  ld->sem_sbn3 = hpx_lco_sema_new(0);
  ld->sem_posvel = hpx_lco_sema_new(0);
  ld->sem_monoq = hpx_lco_sema_new(0);
  SetDomain(index, col, row, plane, nx, tp, nDoms, maxcycles,ld);

  ld->newdt = init->newdt;
  ld->elapsed_ar = init->elapsed_ar;

  // remember the LCO we're supposed to set when we've completed maxcycles
  ld->complete = init->complete;

  // allocate the domain's generation counter
  //
  // NB: right now, we're only doing an allreduce, so there is only ever one
  //     generation waiting---if we end up using this counter inside of the
  //     allreduce boundary (i.e., timestamp), then we need to allocate the
  //     right number of inplace generations in this constructor
  ld->epoch = hpx_lco_gencount_new(0);

  // allocate the initial allreduce and gate
  ld->sbn1_and = hpx_lco_and_new(ld->recvTF[0]);

  ld->sbn3_and[0] = hpx_lco_and_new(ld->recvTF[0]);
  ld->sbn3_and[1] = HPX_NULL;

  ld->posvel_and[0] = hpx_lco_and_new(ld->recvFF[0]);
  ld->posvel_and[1] = HPX_NULL;

  ld->monoq_and[0] = hpx_lco_and_new(ld->recvTT[0]);
  ld->monoq_and[1] = HPX_NULL;

  // set up an affinity
  int id = hpx_thread_get_tls_id();
  ld->affinity = id % HPX_THREADS;
  hpx_gas_set_affinity(local, ld->affinity);

  hpx_gas_unpin(local);
  return HPX_SUCCESS;
}

static int _main_action(int *input, size_t size)
{
  int nDoms, nx, maxcycles, tp, k;
  nDoms = input[0];
  nx = input[1];
  maxcycles = input[2];

  tp = (int) (cbrt(nDoms) + 0.5);
  if (tp*tp*tp != nDoms) {
    fprintf(stderr, "Number of domains must be a cube of an integer (1, 8, 27, ...)\n");
    return -1;
  }
  printf(" Number of domains: %d nx: %d maxcycles: %d core-major ordering: %d\n",nDoms,nx,maxcycles,global_config.core_major);

  hpx_bcast_rsync(_init_global_config, &global_config, sizeof(global_config));

  hpx_time_t t1 = hpx_time_now();

  hpx_addr_t domain = hpx_gas_alloc_cyclic(nDoms,sizeof(Domain),0);
  hpx_addr_t complete = hpx_lco_and_new(nDoms);

  // Initialize the domains
  hpx_addr_t init = hpx_lco_and_new(nDoms);
  hpx_addr_t newdt = hpx_lco_allreduce_new(nDoms, nDoms, sizeof(double),
                                           initdouble,
                                           mindouble);
  hpx_addr_t elapsed_ar;
  elapsed_ar = hpx_lco_allreduce_new(nDoms, 1, sizeof(double),
                                     initmaxdouble,
                                     maxdouble);

  for (k=0;k<nDoms;k++) {
    InitArgs args = {
      .base = domain,
      .elapsed_ar = elapsed_ar,
      .index = k,
      .nDoms = nDoms,
      .nx = nx,
      .maxcycles = maxcycles,
      .complete = complete,
      .newdt = newdt
    };
    hpx_addr_t block = get_domain_addr(domain, k);
    //    printf("Initing domain %d at %d\n", k, (block >> 48));
    hpx_call(block, _initDomain, init, &args, sizeof(args));
  }
  hpx_lco_wait(init);
  hpx_lco_delete(init, HPX_NULL);

  // Spawn the first epoch, _advanceDomain will recursively spawn each epoch.
  unsigned long epoch = 0;

  for (k=0;k<nDoms;k++) {
    hpx_addr_t block = get_domain_addr(domain, k);
    hpx_call(block, _advanceDomain, HPX_NULL, &epoch, sizeof(epoch));
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

  hpx_gas_free(domain, HPX_NULL);
  hpx_exit(0, NULL);
}

static void usage(FILE *f) {
  fprintf(f, "Usage: [options]\n"
          "\t-o rank, rank-major ordering"
          "\t-c, cores\n"
          "\t-t, scheduler threads\n"
          "\t-D, all localities wait for debugger\n"
          "\t-d, wait for debugger at specific locality\n"
          "\t-n, number of domains,nDoms\n"
          "\t-x, nx\n"
          "\t-i, maxcycles\n"
          "\t-h, show help\n");
}


int main(int argc, char **argv)
{
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _main, _main_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _init_global_config, _init_global_config_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _initDomain, _initDomain_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _advanceDomain, _advanceDomain_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _SBN1_sends, _SBN1_sends_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _SBN1_result, _SBN1_result_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _SBN3_sends, _SBN3_sends_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _SBN3_result, _SBN3_result_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _PosVel_sends, _PosVel_sends_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _PosVel_result, _PosVel_result_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _MonoQ_sends, _MonoQ_sends_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _MonoQ_result, _MonoQ_result_action, HPX_POINTER, HPX_SIZE_T);

  bool core_major = true;
  int nDoms, nx, maxcycles;
  // default
  nDoms = 8;
  nx = 15;
  maxcycles = 10;


  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "Failed to initialize hpx\n");
    return -1;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "n:x:i:o:h?")) != -1) {
    switch (opt) {
      case 'o':
        if (0 == strncmp("rank", optarg, 4));
    core_major = false;
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


  int input[3];
  input[0] = nDoms;
  input[1] = nx;
  input[2] = maxcycles;

  e = hpx_run(&_main, NULL, input, 3*sizeof(int));
  hpx_finalize();
  return e;
}

