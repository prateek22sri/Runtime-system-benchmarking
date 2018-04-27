#ifndef UTILITY_FUNCTIONS
#define UTILITY_FUNCTIONS

#include <inttypes.h>
#include "gni_pub.h"
#include "photon.h"

PHOTON_INTERNAL unsigned int get_gni_nic_address(int device_id);

PHOTON_INTERNAL uint32_t get_cookie(void);
PHOTON_INTERNAL uint8_t get_ptag(void);

PHOTON_INTERNAL int get_cq_event(gni_cq_handle_t cq_handle, unsigned int source_cq, unsigned int retry, gni_cq_entry_t *next_event);

#endif
