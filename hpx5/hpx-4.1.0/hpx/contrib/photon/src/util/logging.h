// =============================================================================
//  Photon RDMA Library (libphoton)
//
//  Copyright (c) 2016, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifndef LOGGING_H
#define LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <assert.h>
#include "config.h"

extern int _photon_nproc, _photon_myrank;
#if defined(ENABLE_DEBUG) || defined(ENABLE_CALLTRACE)
extern int _photon_start_debugging;
#endif

#if defined(ENABLE_CALLTRACE)
extern FILE *_phot_ofp;
#define _photon_open_ofp() { if(_phot_ofp == NULL){char name[10]; sprintf(name,"out.%05d",_photon_myrank);_phot_ofp=fopen(name,"w"); } }
#endif

void photon_logging_msg(FILE *f, const char *pre, unsigned line, const char *func, const char *fmt, ...);

#define print_msg(p)    photon_logging_msg(stdout, p, __LINE__, __FUNCTION__, NULL)
#define log_msg(p,...)  photon_logging_msg(stdout, p, __LINE__, __FUNCTION__, __VA_ARGS__)
#define err_msg(p,...)  photon_logging_msg(stderr, p, __LINE__, __FUNCTION__, __VA_ARGS__)
#define file_msg(p,...) photon_logging_msg(_phot_ofp, p, __LINE__, __FUNCTION__, __VA_ARGS__)

#ifdef ENABLE_DEBUG
#define dbg_info(...)  do{ if(!_photon_start_debugging){break;} log_msg("ALL:DBG",__VA_ARGS__); } while(0)
#define dbg_err(...)   do{ if(!_photon_start_debugging){break;} err_msg("ALL:ERR",__VA_ARGS__); } while(0)
#define dbg_warn(...)  do{ if(!_photon_start_debugging){break;} log_msg("ALL:WRN",__VA_ARGS__); } while(0)
#else
#define dbg_info(...)
#define dbg_err(...)
#define dbg_warn(...)
#endif
#ifdef ENABLE_CALLTRACE
#define dbg_trace(...) do{ if(!_photon_start_debugging){break;} _photon_open_ofp(); file_msg("ALL:TRACE",__VA_ARGS__); } while(0)
#else
#define dbg_trace(...)
#endif

#ifdef ENABLE_DEBUG
#define one_debug(...) do{ if (_photon_myrank == 0) { log_msg("ONE:DBG",__VA_ARGS__); } } while (0)
#else
#define one_debug(...)
#endif

#define one_info(...)  do{ if (_photon_myrank == 0) { log_msg("ONE:INF",__VA_ARGS__); } } while (0)
#define one_stat(...)  do{ if (_photon_myrank == 0) { log_msg("ONE:STT",__VA_ARGS__); } } while (0)
#define one_warn(...)  do{ if (_photon_myrank == 0) { log_msg("ONE:WRN",__VA_ARGS__); } } while (0)
#define one_err(...)   do{ if (_photon_myrank == 0) { err_msg("ONE:ERR",__VA_ARGS__); } } while (0)

#define log_err(...)  err_msg("ALL:ERR",__VA_ARGS__)
#define log_info(...) log_msg("ALL:INF",__VA_ARGS__)
#define log_warn(...) log_msg("ALL:WRN",__VA_ARGS__)
#define init_err()    print_msg("ALL:ERR: Library not initialized. Call photon_init() first")

#ifdef __cplusplus
}
#endif

#endif
