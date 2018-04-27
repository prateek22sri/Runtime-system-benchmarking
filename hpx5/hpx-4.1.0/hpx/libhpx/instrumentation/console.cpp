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
#include <libhpx/Worker.h>
#include "metadata.h"

namespace {
/// This will output a list of action ids and names.
static void _print_actions(void) {
  if (here->rank == 0) {
    for (int i = 1, e = action_table_size(); i < e; ++i) {
      const char *tag = action_is_internal(i) ? "INTERNAL" : "USER";
      fprintf(stderr, "%d,%s,%s\n", i, actions[i].key, tag);
    }
  }
}

#define _vprint_console(c,id,fmt,...)                 \
  fprintf(stderr, "%d,%d,%" PRIu64 ",%s,%s" fmt "\n", \
          here->rank, libhpx::self->getId(),          \
          hpx_time_from_start_ns(hpx_time_now()),     \
          HPX_TRACE_CLASS_TO_STRING[ceil_log2_32(c)], \
          TRACE_EVENT_TO_STRING[id], ##__VA_ARGS__)

using libhpx::instrumentation::Trace;
class ConsoleTracer : public Trace {
 public:
  ConsoleTracer(const config_t *cfg) : Trace(cfg) {
  }

  ~ConsoleTracer() {
  }

  void vappend(int UNUSED, int n, int id, va_list& vargs) {
    int c = TRACE_EVENT_TO_CLASS[id];

    switch (n) {
     case 0:
      _vprint_console(c, id, "");
      break;
     case 1:
      _vprint_console(c, id, ",%" PRIu64, va_arg(vargs, uint64_t));
      break;
     case 2:
      _vprint_console(c, id, ",%" PRIu64 ",%" PRIu64,
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t));
      break;
     case 3:
      _vprint_console(c, id, ",%" PRIu64 ",%" PRIu64 ",%" PRIu64,
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t));
      break;
     case 4:
      _vprint_console(c, id, ",%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64,
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t));
      break;
     case 5:
      _vprint_console(c, id, ",%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64
                      ",%" PRIu64,
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t));
      break;
     case 6:
      _vprint_console(c, id, ",%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64
                      ",%" PRIu64 ",%" PRIu64,
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t));
      break;
     case 7:
     default:
      _vprint_console(c, id, ",%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64
                      ",%" PRIu64 ",%" PRIu64 ",%" PRIu64,
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t),
                      va_arg(vargs, uint64_t));
      break;
    }
  }

  void start(void) {
    if (inst_trace_class(HPX_TRACE_PARCEL)) {
      _print_actions();
    }
  }

  void destroy(void) {
  }
};
}

void *trace_console_new(const config_t *cfg) {
  return new ConsoleTracer(cfg);
}
