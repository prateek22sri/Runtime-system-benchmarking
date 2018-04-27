// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2016, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifndef HPX_SYNC_GENERIC_H_
#define HPX_SYNC_GENERIC_H_

#define SYNC_FLOAT_TY(T, s)
#define SYNC_INT_TY(T, s)                                   \
  static inline T sync_load_##s(T volatile *addr, int mm) { \
    return sync_load(addr, mm);                             \
  }
#define SYNC_PTR_TY(T, s) SYNC_INT_TY(T, s)
#include "types.def"
#undef SYNC_PTR_TY
#undef SYNC_INT_TY

#define SYNC_INT_TY(T, s)                                               \
  static inline void sync_store_##s(T volatile *addr, T val, int mm) {  \
    sync_store(addr, val, mm);                                          \
  }
#define SYNC_PTR_TY(T, s) SYNC_INT_TY(T, s)
#include "types.def"
#undef SYNC_PTR_TY
#undef SYNC_INT_TY

#define SYNC_INT_TY(T, s)                                           \
  static inline T sync_swap_##s(T volatile *addr, T val, int mm) {  \
    return sync_swap(addr, val, mm);                                \
  }
#define SYNC_PTR_TY(T, s) SYNC_INT_TY(T, s)
#include "types.def"
#undef SYNC_PTR_TY
#undef SYNC_INT_TY

#define SYNC_INT_TY(T, s)                                               \
  static inline bool sync_cas_##s(T volatile *addr, T *from, T to, int  \
                                  onsuccess, int onfailure) {           \
    return sync_cas(addr, from, to, onsuccess, onfailure);              \
  }
#define SYNC_PTR_TY(T, s) SYNC_INT_TY(T, s)
#include "types.def"
#undef SYNC_PTR_TY
#undef SYNC_INT_TY

#define SYNC_INT_TY(T, s)                                           \
  static inline T sync_fadd_##s(T volatile *addr, T val, int mm) {  \
    return sync_fadd(addr, val, mm);                                \
  }
#define SYNC_PTR_TY(T, s)
#include "types.def"
#undef SYNC_PTR_TY
#undef SYNC_INT_TY

#define SYNC_INT_TY(T, s)                                           \
  static inline T sync_addf_##s(T volatile *addr, T val, int mm) {  \
    return sync_addf(addr, val, mm);                                \
  }
#define SYNC_PTR_TY(T, s)
#include "types.def"
#undef SYNC_PTR_TY
#undef SYNC_INT_TY
#undef SYNC_FLOAT_TY

#endif /* HPX_SYNC_GENERIC_H_ */
