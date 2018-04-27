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

#ifndef PHOTON_RDMA_LEDGER_H
#define PHOTON_RDMA_LEDGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "photon_buffer.h"

typedef struct photon_rdma_ledger_entry_t {
  volatile photon_rid request;
} photon_rdma_ledger_entry;

typedef struct photon_cid_recv_hdr_t {
  uintptr_t req;
  int cnt;
} photon_cid_recv_hdr;

typedef struct photon_cid_hdr_t {
  volatile uint16_t type;
  uint16_t cmd;
  uint32_t size;
} photon_cid_hdr;

typedef struct photon_cid_ledger_entry_t {
  photon_cid_hdr hdr;
  char data[];
} photon_cid_ledger_entry;

typedef struct photon_cid_recv_ledger_entry_t {
  photon_cid_recv_hdr rhdr;
  photon_cid_hdr hdr;
  char data[];
} photon_cid_recv_ledger_entry;

typedef struct photon_rdma_ledger_t {
  uint64_t prog;
  uint64_t curr;
  uint64_t tail;
  uint32_t num_entries;
  uint16_t entry_size;
  void *entries;
  struct photon_buffer_t remote;
  struct photon_acct_t   acct;
} photon_rdma_ledger;

typedef struct photon_rdma_ledger_entry_t * photonRDMALedgerEntry;
typedef struct photon_cid_ledger_entry_t * photonCIDLedgerEntry;
typedef struct photon_cid_recv_ledger_entry_t * photonCIDRecvLedgerEntry;
typedef struct photon_rdma_ledger_t * photonLedger;

#define PHOTON_CID_ENTRY_SIZE (sizeof(photon_cid_ledger_entry)+_photon_idsize)
#define PHOTON_CID_RECV_ENTRY_SIZE (sizeof(photon_cid_recv_ledger_entry)+_photon_idsize)

PHOTON_INTERNAL
photonLedger photon_rdma_ledger_create_reuse(void *ledger_buffer,
					     int num_entries, int entry_size,
					     int prefix);
PHOTON_INTERNAL
void photon_rdma_ledger_free(photonLedger ledger);
PHOTON_INTERNAL
int photon_rdma_ledger_get_next(int proc, photonLedger ledger);

#ifdef __cplusplus
}
#endif

#endif
