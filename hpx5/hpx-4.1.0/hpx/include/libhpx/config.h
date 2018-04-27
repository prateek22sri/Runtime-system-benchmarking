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

#ifndef LIBHPX_CONFIG_H
#define LIBHPX_CONFIG_H

/// @file
/// @brief Types and constants needed for configuring HPX at run-time.
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <hpx/attributes.h>

#ifdef HAVE_PHOTON
# include "photon_config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LIBHPX_OPT_BITSET_ALL UINT64_MAX
#define LIBHPX_OPT_BITSET_NONE 0
#define LIBHPX_SMALL_THRESHOLD HPX_PAGE_SIZE

//! Configuration options for which global memory model to use.
typedef enum {
  HPX_GAS_DEFAULT = 0, //!< Let HPX choose what memory model to use.
  HPX_GAS_SMP,         //!< Do not use global memory.
  HPX_GAS_PGAS,        //!< Use PGAS (i.e. global memory is fixed).
  HPX_GAS_AGAS,        //!< Use AGAS (i.e. global memory may move).
  HPX_GAS_MAX
} libhpx_gas_t;

static const char* const HPX_GAS_TO_STRING[] = {
  "DEFAULT",
  "SMP",
  "PGAS",
  "AGAS",
  "INVALID_ID"
};

//! Configuration options for the network transports HPX can use.
typedef enum {
  HPX_TRANSPORT_DEFAULT = 0, //!< Let HPX choose what transport to use.
  HPX_TRANSPORT_MPI,         //!< Use MPI for network transport.
  HPX_TRANSPORT_PHOTON,      //!< Use Photon for network transport.
  HPX_TRANSPORT_MAX
} libhpx_transport_t;

static const char* const HPX_TRANSPORT_TO_STRING[] = {
  "DEFAULT",
  "MPI",
  "PHOTON",
  "INVALID_ID"
};

//! Configuration options for networks in HPX.
typedef enum {
  HPX_NETWORK_DEFAULT = 0,
  HPX_NETWORK_SMP,
  HPX_NETWORK_PWC,
  HPX_NETWORK_ISIR,
  HPX_NETWORK_MAX
} libhpx_network_t;

static const char * const HPX_NETWORK_TO_STRING[] = {
  "DEFAULT",
  "SMP",
  "PWC",
  "ISIR",
  "INVALID_ID"
};

//! Configuration options for which bootstrappers HPX can use.
typedef enum {
  HPX_BOOT_DEFAULT = 0,      //!< Let HPX choose what bootstrapper to use.
  HPX_BOOT_SMP,              //!< Use the SMP bootstrapper.
  HPX_BOOT_MPI,              //!< Use mpirun to bootstrap HPX.
  HPX_BOOT_PMI,              //!< Use the PMI bootstrapper.
  HPX_BOOT_MAX
} libhpx_boot_t;

static const char* const HPX_BOOT_TO_STRING[] = {
  "DEFAULT",
  "SMP",
  "MPI",
  "PMI",
  "INVALID_ID"
};

//! Configuration options for the thread affinity policies.
typedef enum {
  HPX_THREAD_AFFINITY_DEFAULT = 0,  //!< Default is NONE.
  HPX_THREAD_AFFINITY_HWTHREAD,     //!< Bind to hyper-thread/slot.
  HPX_THREAD_AFFINITY_CORE,         //!< Bind to the core.
  HPX_THREAD_AFFINITY_NUMA,         //!< Bind to the numa node of the PE.
  HPX_THREAD_AFFINITY_NONE,         //!< Do not bind threads.
  HPX_THREAD_AFFINITY_MAX
} libhpx_thread_affinity_t;

static const char * const HPX_THREAD_AFFINITY_TO_STRING[] = {
  "DEFAULT",
  "HW THREAD",
  "CORE",
  "NUMA",
  "NONE",
  "INVALID_POLICY"
};

//! Configuration options for the (work-stealing) scheduling policy.
typedef enum {
  HPX_SCHED_POLICY_DEFAULT = 0, //!< The default policy is "random".
  HPX_SCHED_POLICY_RANDOM,      //!< Steal from a randomly chosen worker.
  HPX_SCHED_POLICY_HIER,        //!< A hierarchical work-stealing policy.
  HPX_SCHED_POLICY_MAX
} libhpx_sched_policy_t;

static const char * const HPX_SCHED_POLICY_TO_STRING[] = {
  "DEFAULT",
  "RANDOM",
  "HIER",
  "INVALID_POLICY"
};

//! Locality types in HPX.
#define HPX_LOCALITY_NONE  -2                   //!< Represents no locality.
#define HPX_LOCALITY_ALL   -1                   //!< Represents all localities.

//! Configuration options for runtime logging in HPX.
#define HPX_LOG_DEFAULT 1               //!< The default logging level.
#define HPX_LOG_BOOT    2               //!< Log the bootstrapper execution.
#define HPX_LOG_SCHED   4               //!< Log the HPX scheduler operations.
#define HPX_LOG_GAS     8               //!< Log the Global-Address-Space ops.
#define HPX_LOG_LCO     16              //!< Log the LCO operations.
#define HPX_LOG_NET     32              //!< Turn on logging for network ops.
#define HPX_LOG_TRANS   64              //!< Log the transport operations.
#define HPX_LOG_PARCEL  128             //!< Parcel logging.
#define HPX_LOG_ACTION  256             //!< Log action registration.
#define HPX_LOG_CONFIG  512             //!< Log configuration.
#define HPX_LOG_MEMORY  1024            //!< Log memory (coarse grained)
#define HPX_LOG_COLL    2048            //!< Log collectives

static const char *const HPX_LOG_LEVEL_TO_STRING[] = {
  "DEFAULT",
  "BOOT",
  "SCHED",
  "GAS",
  "LCO",
  "NET",
  "TRANS",
  "PARCEL",
  "ACTION",
  "CONFIG",
  "MEMORY"
};

//! Configuration options for tracing backends in HPX.
typedef enum {
  HPX_TRACE_BACKEND_DEFAULT = 0,
  HPX_TRACE_BACKEND_FILE,
  HPX_TRACE_BACKEND_CONSOLE,
  HPX_TRACE_BACKEND_STATS,
  HPX_TRACE_BACKEND_MAX
} libhpx_trace_backend_t;

static const char * const HPX_TRACE_BACKEND_TO_STRING[] = {
  "DEFAULT",
  "FILE",
  "CONSOLE",
  "STATS",
  "INVALID_ID"
};

//! Configuration options for tracing in HPX.
#define HPX_TRACE_PARCEL      (UINT64_C(1) << 0)
#define HPX_TRACE_NETWORK     (UINT64_C(1) << 1)
#define HPX_TRACE_SCHED       (UINT64_C(1) << 2)
#define HPX_TRACE_LCO         (UINT64_C(1) << 3)
#define HPX_TRACE_PROCESS     (UINT64_C(1) << 4)
#define HPX_TRACE_MEMORY      (UINT64_C(1) << 5)
#define HPX_TRACE_TRACE       (UINT64_C(1) << 6)
#define HPX_TRACE_GAS         (UINT64_C(1) << 7)
#define HPX_TRACE_COLLECTIVE  (UINT64_C(1) << 8)

static const char *const HPX_TRACE_CLASS_TO_STRING[] = {
  "PARCEL",
  "NETWORK",
  "SCHED",
  "LCO",
  "PROCESS",
  "MEMORY",
  "TRACE",
  "GAS",
  "COLLECTIVE"
};

#define HPX_WAITON_NONE     (UINT64_C(0))
#define HPX_WAITON_SIGSEGV  (UINT64_C(1) << 0)
#define HPX_WAITON_SIGABRT  (UINT64_C(1) << 1)
#define HPX_WAITON_SIGFPE   (UINT64_C(1) << 2)
#define HPX_WAITON_SIGILL   (UINT64_C(1) << 3)
#define HPX_WAITON_SIGBUS   (UINT64_C(1) << 4)
#define HPX_WAITON_SIGIOT   (UINT64_C(1) << 5)
#define HPX_WAITON_SIGSYS   (UINT64_C(1) << 6)
#define HPX_WAITON_SIGTRAP  (UINT64_C(1) << 7)

static const char* const HPX_WAITON_TO_STRING[] = {
  "SIGSEGV",
  "SIGABRT",
  "SIGFPE",
  "SIGILL",
  "SIGBUS",
  "SIGIOT",
  "SIGSYS",
  "SIGTRAP"
};

//! Configuration options for GAS affinity implementations
typedef enum {
  HPX_GAS_AFFINITY_NONE = 0,
  HPX_GAS_AFFINITY_URCU,
  HPX_GAS_AFFINITY_CUCKOO,
  HPX_GAS_AFFINITY_MAX
} libhpx_gas_affinity_t;

static const char * const HPX_GAS_AFFINITY_TO_STRING[] = {
  "NONE",
  "URCU",
  "INVALID_ID"
};

/// The HPX configuration type.
///
/// This configuration is used to control some of the runtime
/// parameters for the HPX system.  The body of the struct is
/// defined in options.def, and not here in config.h.
typedef struct config {
#define LIBHPX_OPT(group, id, init, ctype) ctype group##id;
# include "options.def"
#undef LIBHPX_OPT
} config_t;

config_t *config_new(int *argc, char ***argv)
  HPX_MALLOC;

void config_delete(config_t *cfg);

void config_print(const config_t *cfg, FILE *file);

/// Add declarations to query each of the set options.
#define LIBHPX_OPT_INTSET(group, id, UNUSED2, UNUSED3, UNUSED4)     \
  int config_##group##id##_isset(const config_t *cfg, int value)    \
    HPX_NON_NULL(1);

#define LIBHPX_OPT_BITSET(group, id, UNUSED2)                       \
  static inline uint64_t                                            \
  config_##group##id##_isset(const config_t *cfg, uint64_t mask) {  \
    return (cfg->group##id & mask);                                 \
  }
# include "options.def"
#undef LIBHPX_OPT_BITSET
#undef LIBHPX_OPT_INTSET

#ifdef __cplusplus
}
#endif

#endif // LIBHPX_CONFIG_H
