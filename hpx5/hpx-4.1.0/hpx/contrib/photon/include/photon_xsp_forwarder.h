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

#ifndef PHOTON_XSP_FORWARDER_H
#define PHOTON_XSP_FORWARDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libxsp_client.h>
#include <netinet/in.h>
#include <mpi.h>

#include "photon_io.h"

/* This must be synchronized with xsp/include/option_types.h */
#define XSP_PHOTON_CI     0x00
#define XSP_PHOTON_RI     0x01
#define XSP_PHOTON_SI     0x02
#define XSP_PHOTON_FI     0x03
#define XSP_PHOTON_IO     0x04

/*
 * The actual XSP message for PhotonIOInfo must have variable size.
 * This is the current format (size = length = int):
 *
 * |size_of_fileURI|fileURI...|amode|niter|combiner|nints|integers...|
 * |naddrs|addresses...|ndatatypes|datatypes...|
 */

enum PhotonMPITypes {
  PHOTON_MPI_DOUBLE = 52,
};

typedef struct photon_mpi_datatype_t {
  int combiner;
  int nints;
  int *integers;
  int naddrs;
  MPI_Aint *addresses;
  int ndatatypes;
  int *datatypes;
} PhotonMPIDatatype;

typedef struct photon_io_info_t {
  char *fileURI;
  int amode;
  int niter;
  PhotonMPIDatatype view;
} PhotonIOInfo;

int photon_xsp_register_session(libxspSess *sess);
int photon_xsp_unregister_session(libxspSess *sess);
int photon_xsp_forwarder_connect_peer(libxspSess *sess, void *remote_ci, void **ret_ci, int *ret_len);
int photon_xsp_set_info(libxspSess *sess, void *info, int size, void **ret_info, int *ret_size, photon_info_t type);
int photon_xsp_do_io(libxspSess *sess);

#ifdef __cplusplus
}
#endif

#endif
