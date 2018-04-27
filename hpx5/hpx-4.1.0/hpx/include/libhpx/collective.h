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

#ifndef LIBHPX_COLLECTIVE_H
#define LIBHPX_COLLECTIVE_H

#include <hpx/hpx.h>

/// collective definitions/interfaces
typedef enum {
  ALL_REDUCE = 1000 ,
} coll_type_t;

typedef struct collective {
  coll_type_t      type;   //!< type of collective operation
  hpx_monoid_op_t    op;   //!< collective operator
  int32_t      group_sz;   //!< active group size
  int32_t    recv_count;   //!< how many bytes to be recieved
  int32_t    comm_bytes;   //!< active comm size in bytes
  int32_t   group_bytes;   //!< active group size in bytes
  char           data[];   //!< variable data: group of localities+communicator
} coll_t;

#endif // LIBHPX_COLLECTIVE_H
