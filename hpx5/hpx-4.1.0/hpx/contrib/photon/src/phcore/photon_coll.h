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

#ifndef PHOTON_COLL_H
#define PHOTON_COLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libphoton.h"

typedef struct photon_coll_interface_t {
  int (*init)(photonConfig cfg);
  int (*comm_create)(void *c, void *active, int num_active, int total);
  void *(*comm_get)();
  int (*probe)(int proc, int *flag, photon_cid *c);
  int (*cid_handler)(int proc, pwc_command cmd, photon_cid cid, void *data,
                     int size);

  int (*barrier)(void *comm);
  int (*ibarrier)(void *comm, photonRequest req);
  int (*allreduce)(void *in, void *out, int count, void *datatype, void *op,
                   void *comm);
  int (*iallreduce)(void *in, void *out, int count, void *datatype, void *op,
                    void *comm, photonRequest req);
  int (*scan)(void *in, void *out, int count, void *datatype, void *op,
              void *comm);
  int (*iscan)(void *in, void *out, int count, void *datatype, void *op,
               void *comm, photonRequest req);
  // ...
} photon_coll_interface;

typedef struct photon_coll_ctx_t { void *comm; } photon_coll_ctx;

// internal initializtion of the collective interface
PHOTON_INTERNAL int photon_coll_init(photonConfig cfg);

// internal probe method not exposed via API
PHOTON_INTERNAL int photon_coll_probe(int proc, int *flag, photon_cid *comp);

// collective communicator setup method

PHOTON_INTERNAL int _photon_collective_comm_create(void *active, int num_active,
                                                   int total,
                                                   photon_coll_comm *c);

PHOTON_INTERNAL int _photon_collective_init(photon_coll_comm c,
                                            photon_coll ctype, photon_cid local,
                                            photon_rid *request, int flags);
// implementation of public API methods, invoked from libphoton.c
PHOTON_INTERNAL int
_photon_collective_init_new_comm(void *active, int num_active, int total,
                                 photon_coll ctype, photon_cid local,
                                 photon_rid *request, int flags);

PHOTON_INTERNAL int _photon_collective_join(photon_rid request, void *in,
                                            void *out, int count,
                                            photon_datatype datatype, void *op);

#ifdef __cplusplus
}
#endif

#endif
