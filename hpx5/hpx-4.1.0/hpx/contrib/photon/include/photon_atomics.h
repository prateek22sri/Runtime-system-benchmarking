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

#ifndef PHOTON_ATOMICS_H
#define PHOTON_ATOMICS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PHOTON_INT8,
  PHOTON_UINT8,
  PHOTON_INT16,
  PHOTON_UINT16,
  PHOTON_INT32,
  PHOTON_UINT32,
  PHOTON_INT64,
  PHOTON_UINT64,
  PHOTON_FLOAT,
  PHOTON_DOUBLE,
  PHOTON_FLOAT_COMPLEX,
  PHOTON_DOUBLE_COMPLEX,
  PHOTON_LONG_DOUBLE,
  PHOTON_LONG_DOUBLE_COMPLEX,
  PHOTON_DATATYPE_LAST
} photon_datatype;

typedef enum {
  PHOTON_ATOMIC_MIN = 1,
  PHOTON_ATOMIC_MAX,
  PHOTON_ATOMIC_SUM,
  PHOTON_ATOMIC_LAST
} photon_atomic_op;

#ifdef __cplusplus
}
#endif

#endif
