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

#ifndef INSTRUMENTATION_H
#define INSTRUMENTATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <hpx/attributes.h>
#include <hpx/builtins.h>
#include <libhpx/config.h>
#include <libhpx/locality.h>

/// INST will do @p stmt only if instrumentation is enabled
#ifdef ENABLE_INSTRUMENTATION
# define INST(stmt) stmt;
# define INST_IF(S) if (S)
#else
# define INST(stmt)
# define INST_IF(S) if (false)
#endif

/// Initialize tracing. This is usually called in hpx_init().
void* trace_new(const config_t *cfg)
  HPX_NON_NULL(1) HPX_MALLOC;

/// "Start" tracing. This is usually called in
/// hpx_run(). This takes
/// care of some things that must be done after initialization is
/// complete,
/// specifically action registration.
void trace_start(void *obj);

/// Delete a trace object.
///
/// @param      obj The trace object to delete.
void trace_destroy(void *obj);

/// vappend
void trace_vappend(void* obj, int UNUSED, int n, int id, ...);

static inline bool inst_trace_class(int type) {
  return config_trace_classes_isset(here->config, type);
}

/// Utility to consolidate a check to see if a trace event is currently being
/// traced.
///
/// @param           id The trace event ID.
///
/// @returns            true if the event should be appended, false otherwise
int inst_check_vappend(int id, ...);

/// Record an event to the log
///
/// @param        type Type this event is part of (see hpx_inst_class_type_t)
/// @param            n The number of user arguments to log, < 5.
/// @param           id The event id (see hpx_inst_event_type_t)
#ifdef ENABLE_INSTRUMENTATION
# define trace_append(type, ...) do {                           \
    if (inst_check_vappend(__VA_ARGS__)) {                      \
      trace_vappend(here->tracer, type, __HPX_NARGS(__VA_ARGS__) - 1,   \
          __VA_ARGS__);                                         \
    }                                                           \
  } while (0)
#else
# define trace_append(type, id, ...)
#endif

// Constructor for a "file" tracer object.
void *trace_file_new(const config_t *cfg);

// Constructor for a "console" tracer object.
void *trace_console_new(const config_t *cfg);

// Constructor for the "stats" tracer object.
void *trace_stats_new(const config_t *cfg);

#ifdef __cplusplus
}
#endif

#endif
