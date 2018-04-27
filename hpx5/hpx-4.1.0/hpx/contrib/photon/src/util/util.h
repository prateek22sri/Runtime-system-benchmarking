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

#ifndef util_H
#define util_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "photon.h"
#include "config.h"

#define ALIGN(s, b) (((s) + ((b) - 1)) & ~((b) - 1))
#define TEST_ALIGN(t, a) (((uintptr_t)t % a) == 0)

#define CHECK_INIT(p)                             \
  if(_photon_initialized() != PHOTON_OK || !#p) { \
    init_err();                                   \
    return PHOTON_ERROR_NOINIT;                   \
  }                                               

typedef struct photon_dev_list_t {
  struct photon_dev_list_t *next;
  char                     *dev;
  int                       port;
} photon_dev_list;

int photon_scan(const char *s, int c, int len) ;
int photon_parse_devstr(const char *devstr, photon_dev_list **ret_list);
int photon_match_dev(photon_dev_list *dlist, const char *dev, int port);
void photon_free_devlist(photon_dev_list *d);
void photon_gettime_(double *s);
const char *photon_addr_str(photon_addr *addr, int af);

static inline __attribute__((const))
unsigned int next_power_of_2(unsigned int n)
{
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;
  return n;
}

static inline __attribute__((const))
bool is_power_of_2(unsigned long n)
{
  return (n != 0 && ((n & (n - 1)) == 0));
}

#ifdef __cplusplus
}
#endif

#endif
