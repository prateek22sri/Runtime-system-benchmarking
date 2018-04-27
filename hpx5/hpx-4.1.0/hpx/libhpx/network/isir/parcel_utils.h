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

#ifndef LIBHPX_NETWORK_ISIR_PARCEL_UTILS_H
#define LIBHPX_NETWORK_ISIR_PARCEL_UTILS_H

#include <libhpx/debug.h>
#include <libhpx/parcel.h>

static inline uint32_t isir_prefix_size(void) {
  return offsetof(hpx_parcel_t, action);
}

static inline uint32_t isir_network_size(const hpx_parcel_t *p) {
  return parcel_size(p) - isir_prefix_size();
}

static inline void *isir_network_offset(hpx_parcel_t *p) {
  return &p->action;
}

static inline int payload_size_to_isir_bytes(uint32_t payload) {
  return payload + sizeof(hpx_parcel_t) - isir_prefix_size();
}

static inline uint32_t isir_bytes_to_payload_size(int bytes) {
  dbg_assert(bytes > 0);
  return bytes + isir_prefix_size() - sizeof(hpx_parcel_t);
}

static inline uint32_t tag_to_payload_size(int tag) {
  uint32_t parcel_size = tag * HPX_CACHELINE_SIZE;
  return parcel_size - sizeof(hpx_parcel_t);
}

static inline uint32_t tag_to_isir_bytes(int tag) {
  uint32_t parcel_size = tag * HPX_CACHELINE_SIZE;
  return parcel_size - offsetof(hpx_parcel_t, action);
}

#endif // LIBHPX_NETWORK_ISIR_PARCEL_UTILS_H
