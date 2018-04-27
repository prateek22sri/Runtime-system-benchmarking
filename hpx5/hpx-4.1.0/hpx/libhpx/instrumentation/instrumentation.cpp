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

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <inttypes.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>

#include <hpx/hpx.h>
#include <libhpx/action.h>
#include <libhpx/debug.h>
#include <libhpx/events.h>
#include <libhpx/instrumentation.h>
#include <libhpx/libhpx.h>
#include <libhpx/locality.h>
#include <libhpx/parcel.h>
#include <libhpx/Worker.h>
#include "file.h"

namespace {
using libhpx::instrumentation::Trace;
constexpr auto RELAXED = std::memory_order_relaxed;
}

Trace::Trace(const config_t* cfg) : active(!cfg->trace_off) {
}

Trace::~Trace() {
}

void
trace_start(void* obj)
{
  if (auto trace = static_cast<Trace*>(obj)) {
    trace->start();
  }
}

void
trace_destroy(void* obj)
{
  if (auto trace = static_cast<Trace*>(obj)) {
    trace->destroy();
  }
}

void
trace_vappend(void* obj, int UNUSED, int n, int id, ...)
{
  va_list vargs;
  va_start(vargs, id);
  static_cast<Trace*>(obj)->vappend(UNUSED, n, id, vargs);
  va_end(vargs);
}

void* trace_new(const config_t *cfg) {
#ifndef ENABLE_INSTRUMENTATION
  return NULL;
#endif

  // If we're not tracing at this locality, then don't do anything.
  if (!config_trace_at_isset(cfg, HPX_LOCALITY_ID) || !cfg->trace_classes) {
    return NULL;
  }

  libhpx_trace_backend_t type = cfg->trace_backend;
  if (type == HPX_TRACE_BACKEND_DEFAULT || type == HPX_TRACE_BACKEND_FILE) {
    return trace_file_new(cfg);
  } else if (type == HPX_TRACE_BACKEND_CONSOLE) {
    return trace_console_new(cfg);
  } else if (type == HPX_TRACE_BACKEND_STATS) {
    return trace_stats_new(cfg);
  } else {
    dbg_error("unknown trace backend type.\n");
  }
  return NULL;
}

void libhpx_inst_phase_begin() {
  if (auto trace = static_cast<Trace*>(here->tracer)) {
    trace->active.store(true, RELAXED);
  }
}

void libhpx_inst_phase_end() {
  if (auto trace = static_cast<Trace*>(here->tracer)) {
    trace->active.store(false, RELAXED);
  }
}

bool libhpx_inst_tracer_active() {
  dbg_assert(here && here->tracer);
  auto trace = static_cast<Trace*>(here->tracer);
  return trace->active.load(RELAXED);
}

int inst_check_vappend(int id, ...) {
  if (!here) {
    return 0;
  }

  if (!here->tracer) {
    return 0;
  }

  if (!libhpx::self) {
    return 0;
  }

  if (!libhpx_inst_tracer_active()) {
    return 0;
  }

  if (!inst_trace_class(TRACE_EVENT_TO_CLASS[id])) {
    return 0;
  }

  return 1;
}
