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

#ifndef LIBHPX_DEBUG_H
#define LIBHPX_DEBUG_H

#include <hpx/hpx.h>
#include <libhpx/config.h>
#include "libhpx/locality.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_DEBUG
# define DEBUG 1
# define DEBUG_IF(S) if (S)
#else
# define DEBUG 0
# define DEBUG_IF(S) if (false)
#endif

/// Initialize any configurable debug data.
int dbg_init(config_t *config)
  HPX_NON_NULL(1);

/// Finalize any debug data.
void dbg_fini(void);

/// Wait for the debugger.
void dbg_wait(void);

void dbg_error_internal(unsigned line, const char *filename, const char *func,
                        const char *fmt, ...)
  HPX_PRINTF(4, 5) HPX_NORETURN;

#define dbg_error(...)                                             \
  do {                                                             \
    dbg_error_internal(__LINE__, __FILE__, __func__, __VA_ARGS__); \
    unreachable();                                                 \
  } while (0)

#ifdef ENABLE_DEBUG

# ifdef __cplusplus
# define dbg_assert_str(e, ...) assert(e)
# define dbg_assert(e) dbg_assert_str(e, "\n")
# else
// NB: this is complex for clang's benefit, so it can tell that we're asserting
// e when doing static analysis
#  define dbg_assert_str(e, ...)                        \
  do {                                                  \
    if (!(e)) {                                         \
      dbg_error("assert failed: ("#e") "__VA_ARGS__);   \
    }                                                   \
  } while (0)

#  define dbg_assert(e) dbg_assert_str(e, "\n")
# endif
#else
# define dbg_assert_str(e, ...) assert(e)
# define dbg_assert(e) assert(e)
#endif

#ifdef NDEBUG
# define dbg_check(e, ...) (void)e
#else
# define dbg_check(e, ...) dbg_assert_str((e) == HPX_SUCCESS, __VA_ARGS__)
#endif

void log_internal(unsigned line, const char *filename, const char *func,
                  const char *fmt, ...)
  HPX_PRINTF(4, 5);

#ifdef ENABLE_LOGGING
# define log_level(level, ...)                                 \
  do {                                                         \
    if (here->config &&                                        \
        config_log_level_isset(here->config, level) &&         \
        config_log_at_isset(here->config, here->rank)) {       \
      log_internal(__LINE__, __FILE__, __func__, __VA_ARGS__); \
    }                                                          \
  } while (0)
#else
# define log_level(level, ...)
#endif

#define log_dflt(...)   log_level(HPX_LOG_DEFAULT, __VA_ARGS__)
#define log_cfg(...)    log_level(HPX_LOG_CONFIG, __VA_ARGS__)
#define log_boot(...)   log_level(HPX_LOG_BOOT, __VA_ARGS__)
#define log_sched(...)  log_level(HPX_LOG_SCHED, __VA_ARGS__)
#define log_lco(...)    log_level(HPX_LOG_LCO, __VA_ARGS__)
#define log_gas(...)    log_level(HPX_LOG_GAS, __VA_ARGS__)
#define log_mem(...)    log_level(HPX_LOG_MEMORY, __VA_ARGS__)
#define log_net(...)    log_level(HPX_LOG_NET, __VA_ARGS__)
#define log_trans(...)  log_level(HPX_LOG_TRANS, __VA_ARGS__)
#define log_parcel(...) log_level(HPX_LOG_PARCEL, __VA_ARGS__)
#define log_action(...) log_level(HPX_LOG_ACTION, __VA_ARGS__)
#define log_coll(...)   log_level(HPX_LOG_COLL, __VA_ARGS__)

int log_error_internal(unsigned line, const char *filename, const char *func,
                       const char *fmt, ...)
  HPX_PRINTF(4, 5);

#define log_error(...)                                                  \
  log_error_internal(__LINE__, __FILE__, __func__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // LIBHPX_DEBUG_H
