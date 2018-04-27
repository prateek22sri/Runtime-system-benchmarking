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

#ifndef PHOTON_CONFIG_H
#define PHOTON_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "photon_collectives.h"

#define PHOTON_ORDER_DEFAULT   0x0000
#define PHOTON_ORDER_NONE      0x0001
#define PHOTON_ORDER_STRICT    0x0002

static const char* const PHOTON_ORDER_TO_STRING[] = {
  "default",
  "none",
  "strict",
  "INVALID_COMP_ORDER"
};

//! Defining the network backends Photon can use
typedef enum {
  PHOTON_BACKEND_DEFAULT = 0, //!< Set a default (verbs).
  PHOTON_BACKEND_VERBS,       //!< Use Verbs photon backend.
  PHOTON_BACKEND_UGNI,        //!< Use uGNI photon backend.
  PHOTON_BACKEND_FI,          //!< Use libfabric backend.
  PHOTON_BACKEND_MAX
} photon_cfg_backend_t;

static const char* const PHOTON_BACKEND_TO_STRING[] = {
  "default",
  "verbs",
  "ugni",
  "fi",
  "INVALID_BACKEND"
};

struct photon_config_t {
  int address;              // i.e. process rank
  int nproc;                // Total number of processes

  struct {                  // OPTIONS for InfiniBand Verbs backend
    const char *eth_dev;    // Device name for CMA mode
    const char *ib_dev;     // IB device filter, e.g.: 'qib0:1+mlx4_0:2'
    int use_cma;            // Use connection manager to establish RDMA context
    int use_ud;             // EXPERIMENTAL: unreliable datagram mode (uses multicast)
    int num_srq;            // Number of shared receive queues for remote completions (default 0)
    const char *ud_gid_prefix; // EXPERIMENTAL: GID prefix to use for UD multicast
  } ibv;
  
  struct {                  // OPTIONS for Cray UGNI backend (set -1 for defaults)
    const char *eth_dev;    // Device name for PE
    int bte_thresh;         // Messages >= bytes use BTE (default 8192, set 0 to disable BTE)
  } ugni;

  struct {
    const char *eth_dev;
    const char *domain;
    const char *node;
    const char *service;
    const char *provider;
  } fi;

  struct {                  // EXPERIMENTAL: forwarder interface to XSPd
    int use_forwarder;
    char **forwarder_eids;
  } forwarder;

  struct {                  // Various buffer and message sizes (-1 for defaults)
    int max_cid_size;       // Max size of completion IDs in bytes for PWC (default 8)
    int eager_buf_size;     // Size of eager buffer per rank in bytes (default 64K, set 0 to disable)
    int pwc_buf_size;       // Size of PWC packed buffer per rank in bytes (default 64K, set 0 to disable)
    int small_msg_size;     // Messages <= bytes will use eager buffers (default 1024, set 0 to disable)
    int small_pwc_size;     // Messages <= bytes will be packed in PWC buffer (default 128, set 0 to disable)
    int ledger_entries;     // The number of ledger entries (default 64)
    int max_rd;             // Max number of request descriptors, power of 2 (default 1M, set 0 for unbounded)
    int default_rd;         // Initial number of request descriptors allocated per peer (default 1024)
    int num_cq;             // Number of completion queues to assign peers (default 1)
    int use_rcq;            // Use remote completions when possible (default 0, disabled)
  } cap;

  struct {
    int comp_order;         // specify completion ordering constraints (NONE, STRICT)
  } attr;
  
  struct {
    int (*barrier)(void *);
    int (*allgather)(void *, void *, void *, int);
  } exch;
  
  void *comm;                   // Optional communicator to use for exchange
  int meta_exch;                // See PHOTON_EXCH types below (default MPI)
  photon_cfg_backend_t backend; // select a backend for photon to use
  photon_cfg_coll_iface_t coll; // select a collective interface to use
};

typedef struct photon_config_t * photonConfig;

#ifdef __cplusplus
}
#endif

#endif
