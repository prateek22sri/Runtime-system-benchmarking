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

#ifndef LIBHPX_STRING_H
#define LIBHPX_STRING_H

#include <hpx/hpx.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int (*memget)(void *obj, void *to, hpx_addr_t from, size_t size,
                hpx_addr_t lsync, hpx_addr_t rsync);

  int (*memget_rsync)(void *obj, void *to, hpx_addr_t from, size_t size,
                      hpx_addr_t lsync);

  int (*memget_lsync)(void *obj, void *to, hpx_addr_t from, size_t size);

  int (*memput)(void *obj, hpx_addr_t to, const void *from, size_t size,
                hpx_addr_t lsync, hpx_addr_t rsync);

  int (*memput_lsync)(void *obj, hpx_addr_t to, const void *from, size_t size,
                      hpx_addr_t rsync);

  int (*memput_rsync)(void *obj, hpx_addr_t to, const void *from, size_t size);

  int (*memcpy)(void *obj, hpx_addr_t to, hpx_addr_t from, size_t size,
                hpx_addr_t sync);

  int (*memcpy_sync)(void *obj, hpx_addr_t to, hpx_addr_t from, size_t size);
} class_string_t;

#ifdef __cplusplus
}
#endif

#endif // LIBHPX_STRING_H
