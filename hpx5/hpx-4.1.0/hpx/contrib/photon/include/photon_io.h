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

#ifndef PHOTON_IO_H
#define PHOTON_IO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { PHOTON_CI,
	       PHOTON_RI,
	       PHOTON_SI,
	       PHOTON_FI,
	       PHOTON_IO,
	       PHOTON_MTU,
	       PHOTON_GET_ALIGN,
	       PHOTON_PUT_ALIGN,
	       PHOTON_COMP_ORDER
} photon_info_t;

int photon_io_init(char *file, int amode, void *view, int niter);
int photon_io_finalize();
void photon_io_print_info(void *io);

#ifdef __cplusplus
}
#endif

#endif
