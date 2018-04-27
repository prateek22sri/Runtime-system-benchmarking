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

#include "Trace.h"

#include <cinttypes>
#include <fcntl.h>
#include <inttypes.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <hpx/hpx.h>
#include <libhpx/debug.h>
#include <libhpx/libhpx.h>
#include <libhpx/Scheduler.h>
#include "libhpx/Worker.h"
#include "metadata.h"

namespace {
using libhpx::self;
using libhpx::Worker;
using libhpx::instrumentation::Trace;

class StatsTracer : public Trace {
 public:
  StatsTracer(const config_t* cfg) : Trace(cfg) {
  }

  ~StatsTracer() {
  }

  // int type() const {
  //   return HPX_TRACE_BACKEND_STATS;
  // }

  void vappend(int UNUSED, int n, int event_id, va_list&) {
    int worker_id = self->getId();
    here->stats[worker_id][event_id]++;
  }

  void start(void) {
    const unsigned nworkers = here->sched->getNWorkers();
    here->stats = new uint64_t*[nworkers];
    for (unsigned w = 0; w < nworkers; ++w) {
      const size_t size = sizeof(uint64_t) * TRACE_NUM_EVENTS;
      if (posix_memalign((void**)&here->stats[w], HPX_CACHELINE_SIZE, size)) {
        dbg_error("could not allocate aligned buffer\n");
      }
      memset(here->stats[w], 0, size);
    }
  }

  void destroy(void) {
    for (int k = 1; k < HPX_THREADS; ++k) {
      for (unsigned i = 0; i < TRACE_NUM_EVENTS; ++i) {
        int c = TRACE_EVENT_TO_CLASS[i];
        if (inst_trace_class(c)) {
          here->stats[0][i] += here->stats[k][i];
        }
      }
      free(here->stats[k]);
      here->stats[k] = nullptr;
    }

    for (unsigned i = 0; i < TRACE_NUM_EVENTS; ++i) {
      int c = TRACE_EVENT_TO_CLASS[i];
      if (inst_trace_class(c)) {
        printf("%d,%s,%" PRIu64 "\n",
               here->rank,
               TRACE_EVENT_TO_STRING[i],
               here->stats[0][i]);
      }
    }
    free(here->stats[0]);
    here->stats[0] = nullptr;
    delete[] here->stats;
    here->stats = nullptr;
  }
};
}

void*
trace_stats_new(const config_t *cfg)
{
  return new StatsTracer(cfg);
}
