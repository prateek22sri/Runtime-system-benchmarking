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

#ifndef PHOTON_BACKEND_H
#define PHOTON_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libphoton.h"
#include "photon_buffer.h"
#include "photon_request.h"
#include "photon_msgbuffer.h"
#include "photon_rdma_ledger.h"
#include "photon_rdma_INFO_ledger.h"
#include "photon_rdma_EAGER_buf.h"

#include "htable.h"
#include "logging.h"
#include "squeue.h"
#include "libsync/sync.h"
#include "libsync/queues.h"

#ifdef HAVE_XSP
#include "photon_xsp_forwarder.h"
#endif

#define PHOTON_GET_CQ_IND(n, i) ((n > 1) ? (i % n) : 0)

#define DEF_EAGER_BUF_SIZE   (1024*8)  // 8K bytes of space per rank
#define DEF_PWC_BUF_SIZE     (1024*8)  // 8K bytes of space per rank
#define DEF_PWC_CID_SIZE     (8)       // Default to 64 bit completion IDs
#define DEF_SMALL_MSG_SIZE   (1024)
#define DEF_LEDGER_SIZE      (64)
#define DEF_SP_SIZE          (128)
#define DEF_NUM_CQ           1
#define DEF_USE_RCQ          0

#define MAX_LEDGER_SIZE      (1<<13)   // Only 13 bits available in RCQ
#define MAX_PWC_BUF_SIZE     (1<<13)   // Only 13 bits available in RCQ
#define MIN_PWC_BUF_SIZE     (1024)    // PWC requires some internal buffer

#define UD_MASK_SIZE         (1<<6)

#define EVENT_NIL            0x00
#define LEDGER               0x01
#define EVQUEUE              0x02
#define SENDRECV             0x03

#define RDMA_FLAG_NIL        0x00
#define RDMA_FLAG_NO_CQE     (1<<1)
#define RDMA_FLAG_WITH_IMM   (1<<2)

#define LEDGER_ALL           0xff
#define LEDGER_INFO          (1<<1)
#define LEDGER_EAGER         (1<<2)
#define LEDGER_FIN           (1<<3)
#define LEDGER_PWC           (1<<4)
#define LEDGER_BUF           (1<<5)
#define LEDGER_PBUF          (1<<6)

#define PH_BE_NIL            0x00
#define PH_BE_IN_ORDER       (1<<1)

typedef enum { PHOTON_CONN_ACTIVE, PHOTON_CONN_PASSIVE } photon_connect_mode_t;

typedef struct proc_info_t {
  photonRILedger     local_snd_info_ledger;
  photonRILedger     remote_snd_info_ledger;
  photonRILedger     local_rcv_info_ledger;
  photonRILedger     remote_rcv_info_ledger;
  photonLedger       local_fin_ledger;
  photonLedger       remote_fin_ledger;
  photonLedger       local_eager_ledger;
  photonLedger       remote_eager_ledger;
  photonLedger       local_pwc_ledger;
  photonLedger       remote_pwc_ledger;

  photonEagerBuf     local_eager_buf;
  photonEagerBuf     remote_eager_buf;
  photonEagerBuf     local_pwc_buf;
  photonEagerBuf     remote_pwc_buf;
  photonMsgBuf       smsgbuf;

  photonRequestTable request_table;

#ifdef HAVE_XSP
  libxspSess *sess;
  PhotonIOInfo *io_info;
#endif
} ProcessInfo;

// photon memory registration requests
struct photon_mem_register_req {
  SLIST_ENTRY(photon_mem_register_req) list;
  void *buffer;
  uint64_t buffer_size;
};

// header for UD message
typedef struct photon_ud_hdr_t {
  uint32_t request;
  uint32_t src_addr;
  uint32_t length;
  uint16_t msn;
  uint16_t nmsg;
} photon_ud_hdr;

typedef struct photon_backend_attr_t {
  int rdma_get_align;
  int rdma_put_align;
  int comp_order;
} photon_backend_attr;

struct photon_backend_t {
  photon_backend_attr *attr;
  void *context;
  int (*initialized)(void);
  int (*init)(photonConfig cfg, ProcessInfo *info, photonBI ss);
  int (*finalize)(void);
  int (*connect)(void *local_ci, void *remote_ci, int pindex, void **ret_ci, int *ret_len,
		 photon_connect_mode_t);
  int (*get_info)(ProcessInfo *pi, int proc, void **info, int *size, photon_info_t type);
  int (*set_info)(ProcessInfo *pi, int proc, void *info, int size, photon_info_t type);
  int (*get_dev_addr)(int af, photonAddr addr);
  int (*get_dev_name)(char **ib_dev);
  int (*register_addr)(photonAddr addr, int af);
  int (*unregister_addr)(photonAddr addr, int af);
  int (*rdma_put)(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                  photonBuffer lbuf, photonBuffer rbuf, uint64_t id, uint64_t imm, int flags);
  int (*rdma_get)(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                  photonBuffer lbuf, photonBuffer rbuf, uint64_t id, int flags);
  int (*rdma_send)(photonAddr addr, uintptr_t laddr, uint64_t size,
                   photonBuffer lbuf, uint64_t id, uint64_t imm, int flags);
  int (*rdma_recv)(photonAddr addr, uintptr_t laddr, uint64_t size,
                   photonBuffer lbuf, uint64_t id, int flags);
  int (*tx_size_left)(int proc);
  int (*rx_size_left)(int proc);
  int (*get_event)(int proc, int max, photon_rid *ids, int *n);
  int (*get_revent)(int proc, int max, photon_rid *ids, uint64_t *imms, int *n);
};

extern struct photon_backend_t  photon_default_backend;

extern ProcessInfo             *photon_processes;
extern photonBI                 shared_storage;

#ifdef HAVE_XSP
int photon_xsp_lookup_proc(libxspSess *sess, ProcessInfo **ret_pi, int *index);
int photon_xsp_unused_proc(ProcessInfo **ret_pi, int *index);
#endif

// Photon API methods
PHOTON_INTERNAL int _photon_initialized(void);
PHOTON_INTERNAL int _photon_init(photonConfig cfg, ProcessInfo *info, photonBI ss);
PHOTON_INTERNAL int _photon_cancel(photon_rid request, int flags);
PHOTON_INTERNAL int _photon_finalize(void);
PHOTON_INTERNAL int _photon_register_buffer(void *buffer, uint64_t size);
PHOTON_INTERNAL int _photon_unregister_buffer(void *buffer, uint64_t size);
PHOTON_INTERNAL int _photon_test(photon_rid request, int *flag, int *type, photonStatus status);
PHOTON_INTERNAL int _photon_wait(photon_rid request);
PHOTON_INTERNAL int _photon_send(photonAddr addr, void *ptr, uint64_t size, int flags, photon_rid *request);
PHOTON_INTERNAL int _photon_recv(uint64_t request, void *ptr, uint64_t size, int flags);
PHOTON_INTERNAL int _photon_post_recv_buffer_rdma(int proc, void *ptr, uint64_t size, int tag, photon_rid *request);
PHOTON_INTERNAL int _photon_post_send_buffer_rdma(int proc, void *ptr, uint64_t size, int tag, photon_rid *request);
PHOTON_INTERNAL int _photon_post_send_request_rdma(int proc, uint64_t size, int tag, photon_rid *request);
PHOTON_INTERNAL int _photon_wait_recv_buffer_rdma(int proc, uint64_t size, int tag, photon_rid *request);
PHOTON_INTERNAL int _photon_wait_send_buffer_rdma(int proc, uint64_t size, int tag, photon_rid *request);
PHOTON_INTERNAL int _photon_wait_send_request_rdma(int tag);
PHOTON_INTERNAL int _photon_post_os_put(photon_rid request, int proc, void *ptr, uint64_t size, int tag, uint64_t r_offset);
PHOTON_INTERNAL int _photon_post_os_get(photon_rid request, int proc, void *ptr, uint64_t size, int tag, uint64_t r_offset);
PHOTON_INTERNAL int _photon_post_os_put_direct(int proc, void *ptr, uint64_t size, photonBuffer rbuf, int flags, photon_rid *request);
PHOTON_INTERNAL int _photon_post_os_get_direct(int proc, void *ptr, uint64_t size, photonBuffer rbuf, int flags, photon_rid *request);
PHOTON_INTERNAL int _photon_send_FIN(photon_rid request, int proc, int flags);
PHOTON_INTERNAL int _photon_wait_any(int *ret_proc, photon_rid *ret_req);
PHOTON_INTERNAL int _photon_wait_any_ledger(int *ret_proc, photon_rid *ret_req);
PHOTON_INTERNAL int _photon_probe_ledger(int proc, int *flag, int type, photonStatus status);
PHOTON_INTERNAL int _photon_probe(photonAddr addr, int *flag, photonStatus status);
PHOTON_INTERNAL int _photon_io_init(char *file, int amode, void *view, int niter);
PHOTON_INTERNAL int _photon_io_finalize();

// util
int _photon_get_buffer_private(void *buf, uint64_t size, const struct photon_buffer_priv_t **pptr);
int _photon_get_buffer_remote(photon_rid request, photonBuffer ret_buf);
int _photon_handle_addr(photonAddr addr, photonAddr ret_addr);

#ifdef __cplusplus
}
#endif

#endif
