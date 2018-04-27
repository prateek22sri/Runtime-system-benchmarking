#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <limits.h>

#include "photon_backend.h"
#include "photon_buffer.h"
#include "photon_exchange.h"
#include "photon_event.h"

#include "photon_smp.h"
#include "photon_smp_connect.h"
#include "logging.h"
#include "util.h"
#include "libsync/locks.h"

#define MAX_RETRIES    1

static int __initialized = 0;

static int smp_initialized(void);
static int smp_init(photonConfig cfg, ProcessInfo *photon_processes, photonBI ss);
static int smp_finalize(void);
static int smp_get_info(ProcessInfo *pi, int proc, void **info, int *size, photon_info_t type);
static int smp_set_info(ProcessInfo *pi, int proc, void *info, int size, photon_info_t type);
static int smp_rdma_put(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                         photonBuffer lbuf, photonBuffer rbuf, uint64_t id, uint64_t imm, int flags);
static int smp_rdma_get(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                         photonBuffer lbuf, photonBuffer rbuf, uint64_t id, int flags);
static int smp_rdma_send(photonAddr addr, uintptr_t laddr, uint64_t size,
                          photonBuffer lbuf, uint64_t id, uint64_t imm, int flags);
static int smp_rdma_recv(photonAddr addr, uintptr_t laddr, uint64_t size,
                          photonBuffer lbuf, uint64_t id, int flags);
static int smp_tx_size_left(int proc);
static int smp_rx_size_left(int proc);
static int smp_get_event(int proc, int max, photon_rid *ids, int *n);
static int smp_get_revent(int proc, int max, photon_rid *ids, uint64_t *imms, int *n);

static smp_cnct_ctx smp_ctx;

// we are now a Photon backend
static struct photon_backend_attr_t smp_attr = {
  .rdma_put_align = PHOTON_SMP_PUT_ALIGN,
  .rdma_get_align = PHOTON_SMP_GET_ALIGN,
  .comp_order     = PHOTON_SMP_COMP_ORDER
};

struct photon_backend_t photon_smp_backend = {
  .attr = &smp_attr,
  .context = &smp_ctx,
  .initialized = smp_initialized,
  .init = smp_init,
  .finalize = smp_finalize,
  .connect = NULL,
  .get_info = smp_get_info,
  .set_info = smp_set_info,
  .get_dev_addr = NULL,
  .get_dev_name = NULL,
  .register_addr = NULL,
  .unregister_addr = NULL,
  .rdma_put = smp_rdma_put,
  .rdma_get = smp_rdma_get,
  .rdma_send = smp_rdma_send,
  .rdma_recv = smp_rdma_recv,
  .tx_size_left = smp_tx_size_left,
  .rx_size_left = smp_rx_size_left,
  .get_event = smp_get_event,
  .get_revent = smp_get_revent
};

static int smp_initialized() {
  if (__initialized == 1)
    return PHOTON_OK;
  else
    return PHOTON_ERROR_NOINIT;
}

static int smp_init(photonConfig cfg, ProcessInfo *photon_processes, photonBI ss) {
  // __initialized: 0 - not; -1 - initializing; 1 - initialized
  __initialized = -1;

  
  __initialized = 1;

  dbg_trace("ended successfully =============");

  return PHOTON_OK;
}

static int smp_finalize() {

  return PHOTON_OK;
}

static int smp_get_info(ProcessInfo *pi, int proc, void **ret_info, int *ret_size, photon_info_t type) {
  switch(type) {
  case PHOTON_GET_ALIGN:
    {
      *ret_info = &smp_attr.rdma_get_align;
      *ret_size = sizeof(smp_attr.rdma_get_align);
    }
    break;
  case PHOTON_PUT_ALIGN:
    {
      *ret_info = &smp_attr.rdma_put_align;
      *ret_size = sizeof(smp_attr.rdma_put_align);
    }
    break;
  case PHOTON_COMP_ORDER:
    {
      *ret_info = &smp_attr.comp_order;
      *ret_size = sizeof(smp_attr.comp_order);
    }
    break;
  default:
    goto error_exit;
    break;
  }
  
  return PHOTON_OK;

 error_exit:
  return PHOTON_ERROR;
}

static int smp_set_info(ProcessInfo *pi, int proc, void *info, int size, photon_info_t type) {

  return PHOTON_OK;
}

static int smp_rdma_put(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                         photonBuffer lbuf, photonBuffer rbuf, uint64_t id,
			 uint64_t imm, int flags) {
  return PHOTON_OK;
}

static int smp_rdma_get(int proc, uintptr_t laddr, uintptr_t raddr, uint64_t size,
                         photonBuffer lbuf, photonBuffer rbuf, uint64_t id, int flags) {
  return PHOTON_OK;
}

static int smp_rdma_send(photonAddr addr, uintptr_t laddr, uint64_t size,
                          photonBuffer lbuf, uint64_t id, uint64_t imm, int flags) {
  return PHOTON_OK;
}

static int smp_rdma_recv(photonAddr addr, uintptr_t laddr, uint64_t size,
                          photonBuffer lbuf, uint64_t id, int flags) {
  return PHOTON_OK;
}

static int smp_tx_size_left(int proc) {
  return MAX_CQ_ENTRIES;
}

static int smp_rx_size_left(int proc) {
  return MAX_CQ_ENTRIES;
}

static int smp_get_event(int proc, int max, photon_rid *ids, int *n) {
  
  return PHOTON_EVENT_OK;
}

static int smp_get_revent(int proc, int max, photon_rid *ids, uint64_t *imms, int *n) {

  return PHOTON_EVENT_NOTIMPL;
}

