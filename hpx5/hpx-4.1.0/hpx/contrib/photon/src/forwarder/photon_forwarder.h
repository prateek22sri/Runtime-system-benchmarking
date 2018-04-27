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

#ifndef PHOTON_FORWARDER_H
#define PHOTON_FORWARDER_H

#ifdef __cplusplus
extern "C" {
#endif

struct photon_forwarder_t {
  int (*init)(photonConfig cfg, ProcessInfo *photon_processes);
  int (*io_finalize)(ProcessInfo *photon_processes);
  int (*io_init)(ProcessInfo *photon_processes, char *file, int amode, void *view, int niter);
  void (*io_print)(void *io);
  int (*exchange)(ProcessInfo *pi);
  int (*connect)(ProcessInfo *pi);
} photon_forwarder;

#ifdef HAVE_XSP
extern struct photon_forwarder_t xsp_forwarder;
#endif

#ifdef __cplusplus
}
#endif

#endif
