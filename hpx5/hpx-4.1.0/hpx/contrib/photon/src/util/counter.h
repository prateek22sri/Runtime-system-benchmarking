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

#ifndef PHOTON_COUNTER_H_
#define PHOTON_COUNTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#define DEFINE_COUNTER(name, type)              \
 static type name = 0;                          \
 static pthread_mutex_t name##_mtx;             \
                                                \
 static inline type get_##name() {              \
     type tmp;                                  \
     pthread_mutex_lock(&name##_mtx);           \
         tmp = name;                            \
     pthread_mutex_unlock(&name##_mtx);         \
     return tmp;                                \
 }                                              \
                                                \
 static inline type get_inc_##name() {          \
     type tmp;                                  \
     pthread_mutex_lock(&name##_mtx);           \
         tmp = name;                            \
         ++name;                                \
     pthread_mutex_unlock(&name##_mtx);         \
     return tmp;                                \
 }                                              \
                                                \
 static inline type get_dec_##name() {          \
     type tmp;                                  \
     pthread_mutex_lock(&name##_mtx);           \
         tmp = name;                            \
         --name;                                \
     pthread_mutex_unlock(&name##_mtx);         \
     return tmp;                                \
 }                                              \
                                                \
 static inline type get_set_##name(type val) {  \
     type tmp;                                  \
     pthread_mutex_lock(&name##_mtx);           \
         tmp = name;                            \
         name = val;                            \
     pthread_mutex_unlock(&name##_mtx);         \
     return tmp;                                \
 }

#define INIT_COUNTER(name, val) do {            \
    pthread_mutex_init(&name##_mtx, NULL);      \
    name = val;                                 \
} while(/*CONSTCOND*/0)

#define DESTROY_COUNTER(name) do {              \
    name = 0;                                   \
    pthread_mutex_destroy(&name##_mtx);         \
} while(/*CONSTCOND*/0)

#define GET_COUNTER(name) (get_##name())
#define INC_COUNTER(name) (get_inc_##name())
#define DEC_COUNTER(name) (get_dec_##name())

#ifdef __cplusplus
}
#endif

#endif /* PHOTON_COUNTER_H_ */
