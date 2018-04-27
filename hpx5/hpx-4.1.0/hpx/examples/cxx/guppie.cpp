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
#include "config.h"
#endif

#include <hpx/hpx++.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

namespace {

template <typename T> using global_ptr = hpx::global_ptr<T>;
using hpx::alloc_cyclic;
using hpx::gas::memget;
using hpx::gas::memput;

struct tms t;
double WSEC() {
  return (times(&t) / (double)sysconf(_SC_CLK_TCK));
}

double CPUSEC() {
  return (clock() / (double)CLOCKS_PER_SEC);
}

// Random number generator
static const unsigned long POLY = 0x0000000000000007UL;
static const long long PERIOD = 1317624576693539401L;

// Log size of main table
// (suggested: half of global memory)
#ifndef LTABSIZE
//#define LTABSIZE 25L
#define LTABSIZE 10L
#endif

static const long TABSIZE = (1L << LTABSIZE);

// Number of updates to table
// (suggested: 4x number of table entries)
static const long NUPDATE = (4L * TABSIZE);
// static const long NUPDATE = 134217728;

/*
struct {
  long       ltabsize;           // local table size
  long       tabsize;            // global table size
  long       nupdate;            // number of updates
  global_ptr<uint64_t> table;              // global address of the table
  bool       move;
} cfg;
*/

typedef struct guppie_config {
  long ltabsize;              // local table size
  long tabsize;               // global table size
  long nupdate;               // number of updates
  global_ptr<uint64_t> table; // global address of the table
  bool move;
} guppie_config_t;

// table get is synchronous and returns the value
template <typename T> T table_get(const global_ptr<T> &table, long i) {
  auto f = hpx::lco::Future<void>::Alloc();
  T val;
  memget(&val, table + i, sizeof(T), f);
  hpx::lco::get(f);
  return val;
}

// table set is asynchronous and uses an LCO for synchronization.
template <typename T, typename U>
void table_set(global_ptr<T> table, long i, const T &val,
               const global_ptr<U> &rsync) {
  memput(&table[i], &val, sizeof(T), nullptr, rsync);
}

int _bitwiseor_action(uint64_t *args, size_t size) {
  uint64_t value = *args;
  hpx_addr_t target = hpx_thread_current_target();
  uint64_t *local;
  if (!hpx_gas_try_pin(target, (void **)&local))
    return HPX_RESEND;

  value ^= *local;
  memcpy(local, &value, sizeof(value));
  hpx_gas_unpin(target);
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _bitwiseor, _bitwiseor_action,
           HPX_POINTER, HPX_SIZE_T);

int _init_table_action(guppie_config_t *cfg, size_t size) {
  hpx_addr_t target = hpx_thread_current_target();
  uint64_t *local;
  if (!hpx_gas_try_pin(target, (void **)&local))
    return HPX_RESEND;

  int me = HPX_LOCALITY_ID;
  int nranks = HPX_LOCALITIES;
  long r = cfg->tabsize % nranks;
  long blocks = cfg->tabsize / nranks + ((me < r) ? 1 : 0);
  hpx_addr_t and_lco = hpx_lco_and_new(blocks);
  for (long b = 0, i = me; b < blocks; ++b, i += nranks) {
    table_set(cfg->table, i, (uint64_t)i,
              global_ptr<hpx::lco::And>(and_lco, 1));
  }
  hpx_lco_wait(and_lco);
  hpx_lco_delete(and_lco, HPX_NULL);
  hpx_gas_unpin(target);
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _init_table, _init_table_action,
           HPX_POINTER, HPX_SIZE_T);

// Utility routine to start random number generator at Nth step
uint64_t startr(long long n) {
  int i, j;
  uint64_t m2[64];
  uint64_t temp, ran;

  while (n < 0)
    n += PERIOD;
  while (n > PERIOD)
    n -= PERIOD;
  if (n == 0)
    return 0x1;

  temp = 0x1;
  for (i = 0; i < 64; i++) {
    m2[i] = temp;
    temp = (temp << 1) ^ ((long)temp < 0 ? POLY : 0);
    temp = (temp << 1) ^ ((long)temp < 0 ? POLY : 0);
  }

  for (i = 62; i >= 0; i--) {
    if ((n >> i) & 1) {
      break;
    }
  }

  ran = 0x2;
  while (i > 0) {
    temp = 0;
    for (j = 0; j < 64; j++) {
      if ((ran >> j) & 1) {
        temp ^= m2[j];
      }
    }
    ran = temp;
    i -= 1;
    if ((n >> i) & 1) {
      ran = (ran << 1) ^ ((long)ran < 0 ? POLY : 0);
    }
  }

  return ran;
}

// divide up total size (loop iters or space amount) in a blocked way
void Block(int mype, int npes, long totalsize, long &start, long &stop,
           long &size) {
  long div;
  long rem;

  div = totalsize / npes;
  rem = totalsize % npes;

  if (mype < rem) {
    start = mype * (div + 1);
    stop = start + div;
    size = div + 1;
  } else {
    start = mype * div + rem;
    stop = start + div - 1;
    size = div;
  }
}

int _mover_action(guppie_config_t *cfg, size_t sz) {
  uint64_t src;
  int dst;
  hpx_addr_t lco;

  int size = HPX_LOCALITIES;

  while (1) {
    // get a random number
    src = (13719 * rand()) % (cfg->tabsize / sizeof(uint64_t));
    assert(src < cfg->tabsize);
    dst = (rand() % size);

    // get the random address into the table.
    //     hpx_addr_t there = hpx_addr_add(cfg->table, src * BLOCK_SIZE,
    //     BLOCK_SIZE);
    auto there = &(cfg->table[src]);
    lco = hpx_lco_future_new(0);
    // initiate a move
    hpx_gas_move(there.get(), HPX_THERE(dst), lco);
    hpx_lco_wait(lco);
    hpx_lco_delete(lco, HPX_NULL);
  }
  // gets killed at shutdown
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _mover, _mover_action, HPX_POINTER,
           HPX_SIZE_T);

int _update_table_action(guppie_config_t *cfg, size_t sz) {
  static const int VLEN = 32;
  uint64_t ran[VLEN]; /* Current random numbers */
  uint64_t t1[VLEN];
  long start, stop, size;
  long i;
  long j;

  int me = HPX_LOCALITY_ID;
  int nranks = HPX_LOCALITIES;

  Block(me, nranks, cfg->nupdate, start, stop, size);

  for (j = 0; j < VLEN; j++) {
    ran[j] = startr(start + (j * (size / VLEN)));
  }
  for (i = 0; i < size / VLEN; i++) {
    for (j = 0; j < VLEN; j++) {
      ran[j] = (ran[j] << 1) ^ ((long)ran[j] < 0 ? POLY : 0);
    }
    for (j = 0; j < VLEN; j++) {
      t1[j] = table_get(cfg->table, ran[j] & (cfg->tabsize - 1));
    }

    for (j = 0; j < VLEN; j++) {
      t1[j] ^= ran[j];
    }

    hpx_addr_t done = hpx_lco_and_new(VLEN);
    for (j = 0; j < VLEN; j++) {
      table_set(cfg->table, ran[j] & (cfg->tabsize - 1), t1[j],
                global_ptr<hpx::lco::And>(done, 1));
    }
    hpx_lco_wait(done);
    hpx_lco_delete(done, HPX_NULL);
  }
  return HPX_SUCCESS;
}
HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _update_table, _update_table_action,
           HPX_POINTER, HPX_SIZE_T);

void _main_action(guppie_config_t *cfg, std::size_t size) {
  double icputime; // CPU time to init table
  double is;
  double cputime; // CPU time to update table
  double s;
  uint64_t temp;
  long i;
  long j;
  hpx_addr_t lco;

  printf("nThreads = %d\n", hpx_get_num_ranks());
  printf("Main table size = 2^%ld = %ld words\n", cfg->ltabsize, cfg->tabsize);
  printf("Number of updates = %ld\n", cfg->nupdate);
  fflush(stdout);

  // Allocate main table.
  cfg->table = alloc_cyclic<uint64_t>(cfg->tabsize, 1, sizeof(uint64_t));

  // Begin timing here
  icputime = -CPUSEC();
  is = -WSEC();

  // Initialize main table
  int e = hpx_bcast_rsync(_init_table, cfg, sizeof(*cfg));
  assert(e == HPX_SUCCESS);
  (void)e;

  printf("Initialization complete.\n");
  fflush(stdout);

  // Spawn a mover.
  if (cfg->move) {
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

  // End timed section
  cputime += CPUSEC();
  s += WSEC();
  // Print timing results
  printf("init(c= %.4lf w= %.4lf) up(c= %.4lf w= %.4lf) up/sec= %.0lf\n",
         icputime, is, cputime, s, ((double)cfg->nupdate / s));

  // Verification of results (in serial or "safe" mode; optional)
  temp = 0x1;
  lco = hpx_lco_and_new(cfg->nupdate);
  for (i = 0; i < cfg->nupdate; i++) {
    temp = (temp << 1) ^ (((long)temp < 0) ? POLY : 0);
    //     there = hpx_addr_add(cfg->table, (temp & (cfg->tabsize-1))*
    //     BLOCK_SIZE, BLOCK_SIZE);
    //     hpx_call(there, _bitwiseor, lco, &temp, sizeof(temp));
    auto there = &(cfg->table[temp & (cfg->tabsize - 1)]);
    hpx_call(there.get(), _bitwiseor, lco, &temp, sizeof(temp));
  }
  hpx_lco_wait(lco);
  hpx_lco_delete(lco, HPX_NULL);

  j = 0;
  for (i = 0; i < cfg->tabsize; i++) {
    if (table_get(cfg->table, i) != i)
      j++;
  }

  printf("Found %lu errors in %lu locations (%s).\n", j, cfg->tabsize,
         (j <= 0.01 * cfg->tabsize) ? "passed" : "failed");

  hpx::free(cfg->table);
  hpx::exit();
}
// auto _main = hpx::make_action(_main_action);
auto _main = hpx::Action<HPX_DEFAULT, HPX_MARSHALLED, decltype(_main_action)>(
    _main_action);

void _usage(FILE *stream) {
  fprintf(stream, "Usage: guppie [options] TABSIZE NUPDATES\n"
                  "\t-M, enable AGAS data movement\n"
                  "\t-h, show help\n");
  hpx::print_help();
  fflush(stream);
}
} // namespace

// main routine
int main(int argc, char *argv[]) {
  guppie_config_t cfg;
  cfg.ltabsize = LTABSIZE;
  cfg.tabsize = TABSIZE;
  cfg.nupdate = NUPDATE;
  cfg.table = global_ptr<uint64_t>();
  cfg.move = false;

  int e = hpx::init(&argc, &argv);
  if (e) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return e;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "Mh?")) != -1) {
    switch (opt) {
      case 'M': cfg.move = true; break;
      case 'h': _usage(stdout); return 0;
      case '?':
      default: _usage(stderr); return -1;
    }
  }

  argc -= optind;
  argv += optind;

  switch (argc) {
    default: _usage(stderr); return -1;
    case (2): cfg.nupdate = 1L << (atoi(argv[1]));
    case (1): cfg.ltabsize = (atoi(argv[0])); cfg.tabsize = 1L << cfg.ltabsize;
  }

  // run the update_table action
  e = _main.run(&cfg, sizeof(cfg));

  hpx::finalize();
  return e;
}
