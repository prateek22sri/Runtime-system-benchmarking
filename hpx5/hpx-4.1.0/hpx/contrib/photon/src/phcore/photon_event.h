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

#ifndef PHOTON_EVENT_H
#define PHOTON_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "photon_backend.h"

#define PHOTON_EVENT_OK        0x00
#define PHOTON_EVENT_ERROR     0x01
#define PHOTON_EVENT_NONE      0x02
#define PHOTON_EVENT_REQCOMP   0x04
#define PHOTON_EVENT_REQFOUND  0x05
#define PHOTON_EVENT_NOTIMPL   0x06

#define PHOTON_EFLAG_LEDG      0x00
#define PHOTON_EFLAG_PACK      0x01

#define PHOTON_EFLAG_ONE       0x00
#define PHOTON_EFLAG_TWO       0x01

#define PHOTON_ETYPE_DATA      0x00
#define PHOTON_ETYPE_NTFY      0x01

#define ENCODE_RCQ_32(t,l,f,c,p)   ((((((uint32_t)t)<<15) |	\
				      (((uint16_t)l)<<14) |	\
				      (((uint16_t)f)<<13) |	\
				      ((uint16_t)c)) << 16) |	\
				    (uint16_t)p )
#define DECODE_RCQ_32_PROC(v)      ((uint32_t)v<<16>>16)
#define DECODE_RCQ_32_CURR(v)      ((uint32_t)v<<3>>19)
#define DECODE_RCQ_32_FLAG(v)      ((uint32_t)v<<2>>31)
#define DECODE_RCQ_32_LEAD(v)      ((uint32_t)v<<1>>31)
#define DECODE_RCQ_32_TYPE(v)      ((uint32_t)v>>31)

#define ENCODE_RCQ_64_CURR(v)      ((uint64_t)v<<32)
#define DECODE_RCQ_64_CURR(v)      ((uint64_t)v>>32)

PHOTON_INTERNAL int __photon_get_event(int proc, photon_rid *id);
PHOTON_INTERNAL int __photon_get_nevents(int proc, int max, photon_rid **id, int *n);
PHOTON_INTERNAL int __photon_get_revent(int proc, photon_rid *id, uint64_t *imm);
PHOTON_INTERNAL int __photon_get_nrevents(int proc, int max, photon_rid **id, uint64_t **imms, int *n);

PHOTON_INTERNAL int __photon_nbpop_event(photonRequest req);
PHOTON_INTERNAL int __photon_nbpop_sr(photonRequest req);
PHOTON_INTERNAL int __photon_nbpop_ledger(photonRequest req);
PHOTON_INTERNAL int __photon_wait_ledger(photonRequest req);
PHOTON_INTERNAL int __photon_wait_event(photonRequest req);
PHOTON_INTERNAL int __photon_try_one_event(photonRequest *rreq);

PHOTON_INTERNAL int __photon_handle_cq_special(photon_rid rid);
PHOTON_INTERNAL int __photon_handle_cq_event(photonRequest req, photon_rid rid, photonRequest *rreq);
PHOTON_INTERNAL int __photon_handle_send_event(photonRequest req, photon_rid rid);
PHOTON_INTERNAL int __photon_handle_recv_event(photon_rid id);

#ifdef __cplusplus
}
#endif

#endif
