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

#include <stdlib.h>
#include <inttypes.h>
#include <hpx/hpx.h>
#include "trace.h"
#include "lulesh-hpx.h"

/// We need to print trace output from rank 0 so that the bootstrap network
/// doesn't interleave it oddly.
typedef struct {
  int rank;
  int locality;
  hpx_time_t samples[];
} _output_samples_args_t;

static int _output_samples_handler(_output_samples_args_t *args, int n) {
  int            rank = args->rank;
  int        locality = args->locality;
  hpx_time_t *samples = args->samples;
  hpx_addr_t     lock = hpx_thread_current_target();

  // serialize output to prevent local interleaving---could use plain mutex here
  hpx_lco_sema_p(lock);
  {
    int events = sizeof(TRACE_EVENTS)/sizeof(TRACE_EVENTS[0]);
    int cycles = (n - sizeof(*args)) / (sizeof(hpx_time_t) * events);
    hpx_time_t start = samples[0];
    hpx_time_t prev =  samples[0];
    for (int i = 0, e = cycles; i < e; ++i) {
      for (int j = 0, e = events; j < e; ++j) {
        if (j == END_ALLREDUCE_GET) {
          start = samples[i*e + j];
        }
        hpx_time_t curr = samples[i*e + j];
        uint64_t time = hpx_time_to_ns(curr);
        uint64_t offset = hpx_time_diff_ns(start, curr);
        uint64_t rel = hpx_time_diff_ns(prev, curr);
        printf("trace sample: %d, %d, %d, %d, %"PRIu64", %"PRIu64", %"PRIu64"\n",
               locality, rank, i, j, time, offset, rel);
        prev = curr;
      }
    }
  }
  hpx_lco_sema_v_sync(lock);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _output_samples,
                  _output_samples_handler, HPX_POINTER, HPX_INT);

void TRACE_INIT(struct Domain *d) {
  int events = sizeof(TRACE_EVENTS)/sizeof(TRACE_EVENTS[0]);
  int iterations = d->maxcycles;
  d->samples = calloc(iterations * events, sizeof(d->samples[0]));
}

void TRACE_FINI(struct Domain *d) {
  int events = sizeof(TRACE_EVENTS)/sizeof(TRACE_EVENTS[0]);
  int iterations = d->maxcycles;
  int bytes = iterations * events * sizeof(hpx_time_t);

  _output_samples_args_t *args = malloc(sizeof(*args) + bytes);
  args->rank = d->rank;
  args->locality = HPX_LOCALITY_ID;
  memcpy(args->samples, d->samples, bytes);
  hpx_call_sync(d->tracelock, _output_samples, NULL, 0, args,
                sizeof(*args) + bytes);
  free(args);
}

void TRACE(struct Domain *d, int event) {
  int events = sizeof(TRACE_EVENTS)/sizeof(TRACE_EVENTS[0]);
  d->samples[d->cycle * events + event] = hpx_time_now();
}
