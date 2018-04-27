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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>
#include <unistd.h>
#include "hpx/hpx.h"

// Macros for timing
struct tms t;
#define WSEC() (times(&t) / (double)sysconf(_SC_CLK_TCK))
#define CPUSEC() (clock() / (double)CLOCKS_PER_SEC)

// Random number generator
#define POLY 0x0000000000000007UL
#define PERIOD 1317624576693539401L

// Log size of main table
// (suggested: half of global memory)
#ifndef LTABSIZE
//#define LTABSIZE 25L
#define LTABSIZE 10L
#endif
#define TABSIZE (1L << LTABSIZE)

// Number of updates to table
// (suggested: 4x number of table entries)
#define NUPDATE (4L * TABSIZE)
//#define NUPDATE 134217728

#define BLOCK_SIZE sizeof(uint64_t)

typedef struct guppie_config {
  long       ltabsize;           // local table size
  long       tabsize;            // global table size
  long       nupdate;            // number of updates
  hpx_addr_t table;              // global address of the table
} guppie_config_t;

static int _move = 0;
static int _rebalance = 0;

static hpx_action_t _update_table = 0;
static hpx_action_t _init_table   = 0;
static hpx_action_t _bitwiseor    = 0;
static hpx_action_t _main         = 0;
static hpx_action_t _mover        = 0;

// table get is synchronous and returns the value
uint64_t table_get(hpx_addr_t table, long i) {
  uint64_t val;
  size_t n = sizeof(val);
  hpx_addr_t there = hpx_addr_add(table, i*BLOCK_SIZE, BLOCK_SIZE);
  hpx_gas_memget_sync(&val, there, n);
  return val;
}

// table set is asynchronous and uses an LCO for synchronization.
void table_set(hpx_addr_t table, long i, uint64_t val,
               hpx_addr_t lco) {
  hpx_addr_t there = hpx_addr_add(table, i*BLOCK_SIZE, BLOCK_SIZE);
  hpx_gas_memput(there, &val, sizeof(val), HPX_NULL, lco);
}


static int _bitwiseor_action(uint64_t *args, size_t size) {
  uint64_t value = *args;
  hpx_addr_t target = hpx_thread_current_target();
  uint64_t *local;
  if (!hpx_gas_try_pin(target, (void**)&local))
    return HPX_RESEND;

  value ^= *local;
  memcpy(local, &value, sizeof(value));
  hpx_gas_unpin(target);
  return HPX_SUCCESS;
}


static int _init_table_action(guppie_config_t *cfg, size_t size) {
  hpx_addr_t target = hpx_thread_current_target();
  uint64_t *local;
  if (!hpx_gas_try_pin(target, (void**)&local))
    return HPX_RESEND;

  int me = HPX_LOCALITY_ID;
  int nranks = HPX_LOCALITIES;
  long r = cfg->tabsize % nranks;
  long blocks = cfg->tabsize / nranks + ((me < r) ? 1 : 0);
  hpx_addr_t and = hpx_lco_and_new(blocks);
  for (long b = 0, i = me; b < blocks; ++b, i += nranks)
    table_set(cfg->table, i, i, and);

  hpx_lco_wait(and);
  hpx_lco_delete(and, HPX_NULL);
  hpx_gas_unpin(target);
  return HPX_SUCCESS;
}


// Utility routine to start random number generator at Nth step
uint64_t startr(long n)
{
  int i, j;
  uint64_t m2[64];
  uint64_t temp, ran;

  while (n < 0) n += PERIOD;
  while (n > PERIOD) n -= PERIOD;
  if (n == 0) return 0x1;

  temp = 0x1;
  for (i=0; i<64; i++) {
    m2[i] = temp;
    temp = (temp << 1) ^ ((long) temp < 0 ? POLY : 0);
    temp = (temp << 1) ^ ((long) temp < 0 ? POLY : 0);
  }

  for (i=62; i>=0; i--)
    if ((n >> i) & 1)
      break;

  ran = 0x2;
  while (i > 0) {
    temp = 0;
    for (j=0; j<64; j++)
      if ((ran >> j) & 1)
        temp ^= m2[j];
    ran = temp;
    i -= 1;
    if ((n >> i) & 1)
      ran = (ran << 1) ^ ((long) ran < 0 ? POLY : 0);
  }

  return ran;
}

// divide up total size (loop iters or space amount) in a blocked way
void Block(int mype, int npes, long totalsize, long *start,
           long *stop, long *size)
{
  long div;
  long rem;

  div = totalsize / npes;
  rem = totalsize % npes;

  if (mype < rem) {
    *start = mype * (div + 1);
    *stop   = *start + div;
    *size  = div + 1;
  } else {
    *start = mype * div + rem;
    *stop  = *start + div - 1;
    *size  = div;
  }
}

static int _mover_action(guppie_config_t *cfg, size_t n) {
  uint64_t src;
  uint64_t nmoves = ((double)_move/100) * (cfg->nupdate);
  hpx_addr_t done = hpx_lco_and_new(nmoves);
  hpx_time_t start = hpx_time_now();

  for (uint64_t i = 0; i < nmoves; ++i) {
    // get a random number
    src = i % (cfg->tabsize-1);
    hpx_addr_t dst = HPX_THERE(rand() % HPX_LOCALITIES);

    // get the random address into the table.
    hpx_addr_t there = hpx_addr_add(cfg->table, src * BLOCK_SIZE, BLOCK_SIZE);
    // initiate a move
    hpx_gas_move(there, dst, done);
  }

  hpx_lco_wait(done);
  double move_time = hpx_time_elapsed_ms(start)/1e3;
  printf("move time: %.7f s (%"PRIu64" moves)\n", move_time, nmoves);
  hpx_lco_delete(done, HPX_NULL);
  return HPX_SUCCESS;
}

static int _update_table_action(guppie_config_t *cfg, size_t n) {
#define VLEN 32
  uint64_t ran[VLEN];              /* Current random numbers */
  uint64_t t1[VLEN];
  long start, stop, size;
  long i;
  long j;
  hpx_addr_t and;

  int me = HPX_LOCALITY_ID;
  int nranks = HPX_LOCALITIES;

  Block(me, nranks, cfg->nupdate, &start, &stop, &size);

  for (j=0; j<VLEN; j++)
    ran[j] = startr(start + (j * (size/VLEN)));
  for (i=0; i<size/VLEN; i++) {
    for (j=0; j<VLEN; j++) {
      ran[j] = (ran[j] << 1) ^ ((long) ran[j] < 0 ? POLY : 0);
    }
    for (j=0; j<VLEN; j++)
      t1[j] = table_get(cfg->table, ran[j] & (cfg->tabsize-1));

    for (j=0; j<VLEN; j++)
      t1[j] ^= ran[j];

    and = hpx_lco_and_new(VLEN);
    for (j=0; j<VLEN; j++)
      table_set(cfg->table, ran[j] & (cfg->tabsize-1), t1[j], and);
    hpx_lco_wait(and);
    hpx_lco_delete(and, HPX_NULL);
  }
  return HPX_SUCCESS;
}

void _main_action(guppie_config_t *cfg, size_t size)
{

  double icputime;               // CPU time to init table
  double is;
  double cputime;                // CPU time to update table
  double s;
  uint64_t temp;
  long i;
  long j;
  hpx_addr_t lco;
  hpx_addr_t there;
  double rebalance_time;

  printf("nThreads = %d\n", hpx_get_num_ranks());
  printf("Main table size = 2^%ld = %ld words\n", cfg->ltabsize, cfg->tabsize);
  printf("Number of updates = %ld\n", cfg->nupdate);
  fflush(stdout);

  // Allocate main table.
  cfg->table = hpx_gas_alloc_cyclic_attr(cfg->tabsize, BLOCK_SIZE, BLOCK_SIZE,
                                         _rebalance ? HPX_GAS_ATTR_LB : HPX_GAS_ATTR_NONE);

  // Begin timing here
  icputime = -CPUSEC();
  is = -WSEC();
  rebalance_time = 0.0;

  // Initialize main table
  int e = hpx_bcast_rsync(_init_table, cfg, sizeof(*cfg));
  assert(e == HPX_SUCCESS);
  (void)e;

  printf("Initialization complete.\n");
  fflush(stdout);

  // Spawn a mover.
  if (_move) {
    hpx_call(HPX_HERE, _mover, HPX_NULL, cfg, sizeof(*cfg));
  }

  // Begin timing here
  icputime += CPUSEC();
  is += WSEC();

  // Begin timing here
  cputime = -CPUSEC();
  s = -WSEC();

  // Update the table
  e = hpx_bcast_rsync(_update_table, cfg, sizeof(*cfg));
  assert(e == HPX_SUCCESS);

  printf("Completed updates.\n");
  fflush(stdout);

  if (_rebalance) {
    printf("Starting automatic rebalancing.\n");
    hpx_addr_t done = hpx_lco_future_new(0);
    rebalance_time = -WSEC();
    hpx_gas_rebalance(HPX_NULL, HPX_NULL, done);
    hpx_lco_wait(done);
    rebalance_time += WSEC();
    printf("Finished automatic rebalancing.\n");
    hpx_lco_delete(done, HPX_NULL);
  }

  // End timed section
  cputime += CPUSEC();
  s += WSEC();
  // Print timing results
  printf("init(c= %.4lf w= %.4lf) up(c= %.4lf w= %.4lf, r= %.4lf)"
         " up/sec= %.0lf\n", icputime, is, cputime, s, rebalance_time,
         ((double)cfg->nupdate / s));

  // Verification of results (in serial or "safe" mode; optional)
  hpx_exit(0, NULL);
  temp = 0x1;
  lco = hpx_lco_and_new(cfg->nupdate);
  for (i=0; i<cfg->nupdate; i++) {
    temp = (temp << 1) ^ (((long) temp < 0) ? POLY : 0);
    there = hpx_addr_add(cfg->table, (temp & (cfg->tabsize-1))* BLOCK_SIZE, BLOCK_SIZE);
    hpx_call(there, _bitwiseor, lco, &temp, sizeof(temp));
  }
  hpx_lco_wait(lco);
  hpx_lco_delete(lco, HPX_NULL);

  j = 0;
  for (i=0; i<cfg->tabsize; i++) {
    if (table_get(cfg->table, i) != i)
      j++;
  }

  printf("Found %lu errors in %lu locations (%s).\n",
         j, cfg->tabsize, (j <= 0.01*cfg->tabsize) ? "passed" : "failed");

  hpx_exit(0, NULL);
}

static void _usage(FILE *stream) {
  fprintf(stream, "Usage: guppie [options] TABSIZE NUPDATES\n"
          "\t-M <num>, enable AGAS data movement with num percent frequency\n"
          "\t-R, enable automatic AGAS-based rebalancing\n"
          "\t-h, show help\n");
  hpx_print_help();
  fflush(stream);
}

// main routine
int main(int argc, char *argv[])
{
  srand(42);
  guppie_config_t guppie_cfg = {
    .ltabsize = LTABSIZE,
    .tabsize  = TABSIZE,
    .nupdate  = NUPDATE,
    .table    = HPX_NULL,
  };

  // register the actions
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _main, _main_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _init_table, _init_table_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _bitwiseor, _bitwiseor_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _update_table, _update_table_action, HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _mover, _mover_action, HPX_POINTER, HPX_SIZE_T);

  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return e;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "M:Rh?")) != -1) {
    switch (opt) {
     case 'M':
      _move = (atoi(optarg)%100)+1;
      break;
     case 'R':
      _rebalance = 1;
      break;
     case 'h':
      _usage(stdout);
      return 0;
     case '?':
     default:
      _usage(stderr);
      return -1;
    }
  }

  argc -= optind;
  argv += optind;

  switch (argc) {
   default:
    _usage(stderr);
    return -1;
   case (2):
    guppie_cfg.nupdate = 1L << (atoi(argv[1]));
   case (1):
    guppie_cfg.ltabsize = (atoi(argv[0]));
    guppie_cfg.tabsize = 1L << guppie_cfg.ltabsize;
  };

  // run the update_table action
  e = hpx_run(&_main, NULL, &guppie_cfg, sizeof(guppie_cfg));
  hpx_finalize();
  return e;
}
