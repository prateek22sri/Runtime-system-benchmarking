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

#ifndef METADATA_H
#define METADATA_H

#include <stddef.h>
#include <stdint.h>
#include <libhpx/instrumentation.h>
#include <libhpx/events.h>

#ifdef __cplusplus
extern "C" {
#endif

// Event data
typedef struct record {
  uint64_t ns;
  uint64_t user[];
} record_t;

// Type options currently used.  A full list is the keys of numpy.sctypeDict
typedef struct inst_type_info {
  char  code[3];
  int   width;
} inst_type_info_t;

#define METADATA_TYPE_BYTE   {.code="b", .width=1}
#define METADATA_TYPE_INT16  {.code="i2", .width=2}
#define METADATA_TYPE_INT32  {.code="i4", .width=4}
#define METADATA_TYPE_INT64  {.code="i8", .width=8}
#define METADATA_TYPE_UINT16 {.code="u2", .width=2}
#define METADATA_TYPE_UINT32 {.code="u4", .width=4}
#define METADATA_TYPE_UINT64 {.code="u8", .width=8}
#define METADATA_TYPE_FLOAT  {.code="f4", .width=4}
#define METADATA_TYPE_DOUBLE {.code="f8", .width=8}

typedef struct inst_named_value {
  uint32_t value;
  char     type[3];   //A numpy type code
  char     name[8];
} HPX_PACKED inst_named_value_t;

typedef struct inst_event_col_metadata {
  inst_type_info_t data_type;
  unsigned int offset;
  char name[256];
} inst_event_col_metadata_t;

#define METADATA_NS                           \
  { .data_type   = METADATA_TYPE_INT64,       \
    .offset      = offsetof(record_t, ns),    \
    "nanoseconds"}

#define METADATA_int(_name, off)                \
  { .data_type = METADATA_TYPE_INT64,           \
    .offset = offsetof(record_t, user)+(off*8), \
    _name }


//TODO: WHY are all values packaged as int64?  There are other data types...
#define METADATA_uint16_t(_name, pos) METADATA_int(_name, pos)
#define METADATA_uint32_t(_name, pos) METADATA_int(_name, pos)
#define METADATA_uint64_t(_name, pos) METADATA_int(_name, pos)

#define METADATA_size_t METADATA_uint64_t
#define METADATA_hpx_addr_t METADATA_uint64_t
#define METADATA_hpx_action_t METADATA_uint16_t
#define METADATA_uintptr_t METADATA_uint64_t

/// Event metadata struct.
/// In theory the number of columns need not match the number of fields in
/// an event. In practice, right now they do.
typedef struct inst_event_metadata {
  int num_cols;
  inst_event_col_metadata_t *col_metadata;
} inst_event_metadata_t;

#define _ENTRY(...) {                                               \
  .num_cols = __HPX_NARGS(__VA_ARGS__)+2,                           \
  .col_metadata =                                                   \
    (inst_event_col_metadata_t[__HPX_NARGS(__VA_ARGS__)+2]) { \
    METADATA_NS,                                                    \
    __VA_ARGS__                                                     \
  }                                                                 \
}

static const inst_event_metadata_t INST_EVENT_METADATA[] =
{
# define _MD(t,n,pos) METADATA_##t(_HPX_XSTR(n), pos)
# define _ARGS0() _ENTRY()
# define _ARGS2(t0,n0)                                  \
  _ENTRY(_MD(t0,n0,0))
# define _ARGS4(t0,n0,t1,n1)                            \
  _ENTRY(_MD(t0,n0,0),_MD(t1,n1,1))
# define _ARGS6(t0,n0,t1,n1,t2,n2)                      \
  _ENTRY(_MD(t0,n0,0),_MD(t1,n1,1),_MD(t2,n2,2))
# define _ARGS8(t0,n0,t1,n1,t2,n2,t3,n3)                \
  _ENTRY(_MD(t0,n0,0),_MD(t1,n1,1),_MD(t2,n2,2),        \
         _MD(t3,n3,3))
# define _ARGS10(t0,n0,t1,n1,t2,n2,t3,n3,t4,n4)         \
  _ENTRY(_MD(t0,n0,0),_MD(t1,n1,1),_MD(t2,n2,2),        \
         _MD(t3,n3,3),_MD(t4,n4,4))
# define _ARGSN(...) _HPX_CAT2(_ARGS, __HPX_NARGS(__VA_ARGS__))(__VA_ARGS__)
# define LIBHPX_EVENT(class, event, ...) _ARGSN(__VA_ARGS__),
# include <libhpx/events.def>
#undef LIBHPX_EVENT
# undef _ARGS0
# undef _ARGS2
# undef _ARGS4
# undef _ARGS6
# undef _ARGS8
# undef _ARGS10
# undef _ARGSN
# undef _MD
};

#ifdef __cplusplus
}
#endif

#endif
