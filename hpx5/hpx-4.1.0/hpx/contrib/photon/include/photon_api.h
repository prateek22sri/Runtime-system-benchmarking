// =============================================================================
//  Photon RDMA Library (libphoton)
//
//  Copyright (c) 2015, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================
#ifndef PHOTON_API_H
#define PHOTON_API_H

#ifdef __cplusplus
extern "C" {
#endif

int photon_initialized();
int photon_init(photonConfig cfg);
int photon_cancel(photon_rid request, int flags);
int photon_finalize();

// Buffers
int photon_register_buffer(void *buf, uint64_t size);
int photon_unregister_buffer(void *buf, uint64_t size);
int photon_get_buffer_private(void *buf, uint64_t size, const struct photon_buffer_priv_t **pptr);
int photon_get_buffer_remote(photon_rid request, photonBuffer ret_buf);

// RDMA rendezvous
int photon_post_recv_buffer_rdma(int proc, void *ptr, uint64_t size, int tag, photon_rid *request);
int photon_post_send_buffer_rdma(int proc, void *ptr, uint64_t size, int tag, photon_rid *request);
int photon_post_send_request_rdma(int proc, uint64_t size, int tag, photon_rid *request);
int photon_wait_recv_buffer_rdma(int proc, uint64_t size, int tag, photon_rid *request);
int photon_wait_send_buffer_rdma(int proc, uint64_t size, int tag, photon_rid *request);
int photon_wait_send_request_rdma(int tag);
int photon_post_os_put(photon_rid request, int proc, void *ptr, uint64_t size, int tag, uint64_t r_offset);
int photon_post_os_get(photon_rid request, int proc, void *ptr, uint64_t size, int tag, uint64_t r_offset);
int photon_send_FIN(photon_rid request, int proc, int flags);

// RDMA one-sided
int photon_post_os_put_direct(int proc, void *ptr, uint64_t size, photonBuffer rbuf, int flags, photon_rid *request);
int photon_post_os_get_direct(int proc, void *ptr, uint64_t size, photonBuffer rbuf, int flags, photon_rid *request);

// Vectored one-sided put/get
int photon_post_os_putv_direct(int proc, void *ptr[], uint64_t size[], photonBuffer rbuf[], int nbuf,
                               int flags, photon_rid *request);
int photon_post_os_getv_direct(int proc, void *ptr[], uint64_t size[], photonBuffer rbuf[], int nbuf,
                               int flags, photon_rid *request);

// RDMA with completion
// If @p lbuf is NULL and @p size is 0, then only completion value in @p remote is sent
// The remote buffer is specified in @p rbuf along with the remote key
// Local AND remote keys can be specified in @p lbuf and @p rbuf
// If @p lbuf keys are zeroed, Photon will attempt to find matching registered buffer
//
// The default behavior is to enable all CQ events and local and
// remote rids from probe_completion() (flags=PHOTON_REQ_NIL)
int photon_put_with_completion(int proc, uint64_t size, photonBuffer lbuf, photonBuffer rbuf,
                               photon_cid local, photon_cid remote, int flags);
int photon_get_with_completion(int proc, uint64_t size, photonBuffer lbuf, photonBuffer rbuf,
                               photon_cid local, photon_cid remote, int flags);
// Can probe ANY_SOURCE but given @p proc will only poll the CQ (if available) and completion
// ledger associated with that rank
// @p remaining returns the number of requests still pending
// @p src returns the source of a ledger probe
int photon_probe_completion(int proc, int *flag, int *remaining, photon_cid *comp, int *src,
			    void (*cb)(photon_cid), int flags);

// Collectives with completion
typedef void *photon_coll_comm;

int photon_collective_comm_create(void *active, int num_active, int total, photon_coll_comm* c);
int photon_collective_init_new_comm(void *active, int num_active, int total, photon_coll ctype,
			   photon_cid local, photon_rid *request, int flags);
int photon_collective_init(photon_coll ctype,
			   photon_cid local, photon_rid *request, int flags);
int photon_collective_join(photon_rid request, void *in, void *out, int count,
			   photon_datatype datatype, void *op);

// Atomics
int photon_atomic(int proc, photonBuffer lbuf, photonBuffer rbuf, photon_datatype datatype,
		  photon_atomic_op op, photon_rid local, photon_rid remote, int flags);

// Checks
int photon_test(photon_rid request, int *flag, int *type, photonStatus status);
int photon_wait(photon_rid request);
int photon_wait_any(int *ret_proc, photon_rid *ret_req);
int photon_wait_any_ledger(int *ret_proc, photon_rid *ret_req);
int photon_probe_ledger(int proc, int *flag, int type, photonStatus status);

// ==========================
// Experimental UD interface
// ==========================
int photon_send(photonAddr addr, void *ptr, uint64_t size, int flags, photon_rid *request);
int photon_recv(photon_rid request, void *ptr, uint64_t size, int flags);

// Tell photon that we want to accept messages for certain addresses
// identified by address family af
int photon_register_addr(photonAddr addr, int af);
int photon_unregister_addr(photonAddr addr, int af);

// Fill in addr with the local device address, using af as the hint
// default will be AF_INET6 and port gid
int photon_get_dev_addr(int af, photonAddr addr);
int photon_get_dev_name(char **dev_name);

int photon_probe(photonAddr addr, int *flag, photonStatus status);

#ifdef __cplusplus
}
#endif

#endif
