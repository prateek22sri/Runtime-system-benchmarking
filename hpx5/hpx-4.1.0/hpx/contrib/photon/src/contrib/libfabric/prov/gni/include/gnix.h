/*
 * Copyright (c) 2015-2016 Cray Inc.  All rights reserved.
 * Copyright (c) 2015-2016 Los Alamos National Security, LLC.
 *                         All rights reserved.
 * Copyright (c) 2015-2016 Cisco Systems, Inc.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _GNIX_H_
#define _GNIX_H_

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdbool.h>

#include <rdma/fabric.h>
#include <rdma/fi_atomic.h>
#include <rdma/fi_cm.h>
#include <rdma/fi_domain.h>
#include <rdma/fi_endpoint.h>
#include <rdma/fi_eq.h>
#include <rdma/fi_errno.h>
#include <rdma/providers/fi_prov.h>
#include <rdma/fi_rma.h>
#include <rdma/fi_tagged.h>
#include <rdma/fi_trigger.h>

#include <fi.h>
#include <fi_enosys.h>
#include <fi_indexer.h>
#include <fi_rbuf.h>
#include <fi_list.h>
#include <fi_file.h>

#ifdef HAVE_UDREG
#include <udreg_pub.h>
#endif

#include "gni_pub.h"
#include "gnix_util.h"
#include "gnix_freelist.h"
#include "gnix_mr.h"
#include "gnix_cq.h"
#include "fi_ext_gni.h"
#include "gnix_tags.h"
#include "gnix_mr_cache.h"
#include "gnix_mr_notifier.h"
#include "gnix_nic.h"

#define GNI_MAJOR_VERSION 1
#define GNI_MINOR_VERSION 0

/*
 * useful macros
 */
#ifndef likely
#define likely(x) __builtin_expect((x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect((x), 0)
#endif

#ifndef FLOOR
#define FLOOR(a, b) ((long long)(a) - (((long long)(a)) % (b)))
#endif

#ifndef CEILING
#define CEILING(a, b) ((long long)(a) <= 0LL ? 0 : (FLOOR((a)-1, b) + (b)))
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#ifndef compiler_barrier
#define compiler_barrier() asm volatile ("" ::: "memory")
#endif

#define GNIX_MAX_MSG_IOV_LIMIT 8
#define GNIX_MAX_RMA_IOV_LIMIT 1
#define GNIX_MAX_ATOMIC_IOV_LIMIT 1
#define GNIX_ADDR_CACHE_SIZE 5

/*
 * GNI GET alignment
 */

#define GNI_READ_ALIGN		4
#define GNI_READ_ALIGN_MASK	(GNI_READ_ALIGN - 1)

/*
 * GNI IOV GET alignment
 *
 * We always pull 4byte chucks for unaligned GETs. To prevent stomping on
 * someone else's head or tail data, each segment must be four bytes
 * (i.e. GNI_READ_ALIGN bytes).
 *
 * Note: "* 2" for head and tail
 */
#define GNIX_HTD_BUF_SZ (GNIX_MAX_MSG_IOV_LIMIT * GNI_READ_ALIGN * 2)

/*
 * Flags
 * The 64-bit flag field is used as follows:
 * 1-grow up    common (usable with multiple operations)
 * 59-grow down operation specific (used for single call/class)
 * 60 - 63      provider specific
 */

#define GNIX_SUPPRESS_COMPLETION	(1ULL << 60)	/* TX only flag */

#define GNIX_RMA_RDMA			(1ULL << 61)	/* RMA only flag */
#define GNIX_RMA_INDIRECT		(1ULL << 62)	/* RMA only flag */
#define GNIX_RMA_CHAINED		(1ULL << 63)	/* RMA only flag */

#define GNIX_MSG_RENDEZVOUS		(1ULL << 61)	/* MSG only flag */
#define GNIX_MSG_GET_TAIL		(1ULL << 62)	/* MSG only flag */
#define GNIX_MSG_MULTI_RECV_SUP		(1ULL << 63)	/* MSG only flag */

/*
 * Cray gni provider supported flags for fi_getinfo argument for now, needs
 * refining (see fi_getinfo.3 man page)
 */
#define GNIX_SUPPORTED_FLAGS (FI_NUMERICHOST | FI_SOURCE)

#define GNIX_DEFAULT_FLAGS (0)

/*
 * Cray gni provider will try to support the fabric interface capabilities (see
 * fi_getinfo.3 man page)
 * for RDM and MSG (future) endpoint types.
 */

/*
 * See capabilities section in fi_getinfo.3.
 */

/* Primary capabilities.  Each must be explicitly requested (unless the full
 * set is requested by setting input hints->caps to NULL). */
#define GNIX_EP_RDM_PRIMARY_CAPS                                               \
	(FI_MSG | FI_RMA | FI_TAGGED | FI_ATOMICS |                            \
	 FI_DIRECTED_RECV | FI_READ |                                          \
	 FI_WRITE | FI_SEND | FI_RECV | FI_REMOTE_READ | FI_REMOTE_WRITE)

/* No overhead secondary capabilities.  These can be silently enabled by the
 * provider. */
#define GNIX_EP_RDM_SEC_CAPS (FI_MULTI_RECV | FI_TRIGGER | FI_FENCE)

/* Secondary capabilities that introduce overhead.  Must be requested. */
#define GNIX_EP_RDM_SEC_CAPS_OH (FI_SOURCE | FI_RMA_EVENT)

/* FULL set of capabilities for the provider.  */
#define GNIX_EP_RDM_CAPS_FULL (GNIX_EP_RDM_PRIMARY_CAPS | \
			       GNIX_EP_RDM_SEC_CAPS | \
			       GNIX_EP_RDM_SEC_CAPS_OH)

/*
 * see Operations flags in fi_endpoint.3
 */
#define GNIX_EP_OP_FLAGS	(FI_INJECT | FI_MULTI_RECV | FI_COMPLETION | \
				 FI_INJECT_COMPLETE | FI_TRANSMIT_COMPLETE | \
				 FI_DELIVERY_COMPLETE)

/*
 * Valid msg transaction input flags.  See fi_msg.3.
 */
#define GNIX_SENDMSG_FLAGS	(FI_REMOTE_CQ_DATA | FI_COMPLETION | \
				 FI_MORE | FI_INJECT | FI_INJECT_COMPLETE | \
				 FI_TRANSMIT_COMPLETE | FI_FENCE | FI_TRIGGER)
#define GNIX_RECVMSG_FLAGS	(FI_COMPLETION | FI_MORE | FI_MULTI_RECV)
#define GNIX_TRECVMSG_FLAGS \
	(GNIX_RECVMSG_FLAGS | FI_CLAIM | FI_PEEK | FI_DISCARD)

/*
 * Valid rma transaction input flags.  See fi_rma.3.
 */
#define GNIX_WRITEMSG_FLAGS	(FI_REMOTE_CQ_DATA | FI_COMPLETION | \
				 FI_MORE | FI_INJECT | FI_INJECT_COMPLETE | \
				 FI_TRANSMIT_COMPLETE | FI_FENCE | FI_TRIGGER)
#define GNIX_READMSG_FLAGS	(FI_REMOTE_CQ_DATA | FI_COMPLETION | FI_MORE | \
				 FI_FENCE | FI_TRIGGER)
#define GNIX_ATOMICMSG_FLAGS	(FI_COMPLETION | FI_MORE | FI_INJECT | \
				 FI_FENCE | FI_TRIGGER)
#define GNIX_FATOMICMSG_FLAGS	(FI_COMPLETION | FI_MORE | FI_FENCE | \
				 FI_TRIGGER)
#define GNIX_CATOMICMSG_FLAGS	(FI_COMPLETION | FI_MORE | FI_FENCE | \
				 FI_TRIGGER)

/*
 * Valid completion event flags.  See fi_cq.3.
 */
#define GNIX_RMA_COMPLETION_FLAGS	(FI_RMA | FI_READ | FI_WRITE)
#define GNIX_AMO_COMPLETION_FLAGS	(FI_ATOMIC | FI_READ | FI_WRITE)

/*
 * GNI provider fabric default values
 */
#define GNIX_TX_SIZE_DEFAULT	500
#define GNIX_RX_SIZE_DEFAULT	500

/*
 * if this has to be changed, check gnix_getinfo, etc.
 */
#define GNIX_EP_MSG_CAPS GNIX_EP_RDM_CAPS
#define GNIX_EP_MSG_SEC_CAPS GNIX_EP_RDM_SEC_CAPS

#define GNIX_MAX_MSG_SIZE ((0x1ULL << 32) - 1)
#define GNIX_CACHELINE_SIZE (64)
#define GNIX_INJECT_SIZE GNIX_CACHELINE_SIZE

/*
 * Cray gni provider will require the following fabric interface modes (see
 * fi_getinfo.3 man page)
 */
#define GNIX_FAB_MODES	0

/*
 * fabric modes that GNI provider doesn't need
 */
#define GNIX_FAB_MODES_CLEAR (FI_MSG_PREFIX | FI_ASYNC_IOV)

/*
 * gnix address format - used for fi_send/fi_recv, etc.
 */
struct gnix_address {
	uint32_t device_addr;
	uint32_t cdm_id;
};

/*
 * macro for testing whether a gnix_address value is FI_ADDR_UNSPEC
 */

#define GNIX_ADDR_UNSPEC(var) (((var).device_addr == -1) && \
				((var).cdm_id == -1))
/*
 * macro testing for gnix_address equality
 */

#define GNIX_ADDR_EQUAL(a, b) (((a).device_addr == (b).device_addr) && \
				((a).cdm_id == (b).cdm_id))


#define GNIX_EPN_TYPE_UNBOUND	0
#define GNIX_EPN_TYPE_BOUND	1

/*
 * info returned by fi_getname/fi_getpeer - has enough
 * side band info for RDM ep's to be able to connect, etc.
 */
struct gnix_ep_name {
	struct gnix_address gnix_addr;
	struct {
		uint32_t name_type : 8;
		uint32_t cm_nic_cdm_id : 24;
		uint32_t cookie;
	};
	uint64_t reserved[4];
};

/* AV address string revision. */
#define GNIX_AV_STR_ADDR_VERSION  1

/*
 * 49 is the number of characters printed out in gnix_av_straddr.
 *  1 is for the null terminator
 */
#define GNIX_AV_MAX_STR_ADDR_LEN  (49 + 1)

/*
 * 15 is the number of characters for the device addr.
 *  1 is for the null terminator
 */
#define GNIX_AV_MIN_STR_ADDR_LEN  (15 + 1)

/*
 * enum for blocking/non-blocking progress
 */
enum gnix_progress_type {
	GNIX_PRG_BLOCKING,
	GNIX_PRG_NON_BLOCKING
};

/*
 * simple struct for gnix fabric, may add more stuff here later
 */
struct gnix_fid_fabric {
	struct fid_fabric fab_fid;
	/* llist of domains's opened from fabric */
	struct dlist_entry domain_list;
	/* number of bound datagrams for domains opened from
	 * this fabric object - used by cm nic*/
	int n_bnd_dgrams;
	/* number of wildcard datagrams for domains opened from
	 * this fabric object - used by cm nic*/
	int n_wc_dgrams;
	/* timeout datagram completion - see
	 * GNI_PostdataProbeWaitById in gni_pub.h */
	uint64_t datagram_timeout;
	struct gnix_reference ref_cnt;
	struct gnix_mr_notifier mr_notifier;
};

extern struct fi_ops_cm gnix_cm_ops;

/*
 * a gnix_fid_domain is associated with one or more gnix_nic's.
 * the gni_nics are in turn associated with ep's opened off of the
 * domain.  The gni_nic's are use for data motion - sending/receivng
 * messages, rma ops, etc.
 */
struct gnix_fid_domain {
	struct fid_domain domain_fid;
	/* used for fabric object dlist of domains*/
	struct dlist_entry list;
	/* list nics this domain is attached to, TODO: thread safety */
	struct dlist_entry nic_list;
	struct gnix_fid_fabric *fabric;
	struct gnix_cm_nic *cm_nic;
	fastlock_t cm_nic_lock;
	uint8_t ptag;
	uint32_t cookie;
	uint32_t cdm_id_seed;
	/* user tunable parameters accessed via open_ops functions */
	struct gnix_ops_domain params;
	/* size of gni tx cqs for this domain */
	uint32_t gni_tx_cq_size;
	/* size of gni rx cqs for this domain */
	uint32_t gni_rx_cq_size;
	/* additional gni cq modes to use for this domain */
	gni_cq_mode_t gni_cq_modes;
	/* additional gni cq modes to use for this domain */
	enum fi_progress control_progress;
	enum fi_progress data_progress;
	enum fi_threading thread_model;
	struct gnix_reference ref_cnt;
	gnix_mr_cache_attr_t mr_cache_attr;
	gnix_mr_cache_t *mr_cache;
	fastlock_t mr_cache_lock;
	struct gnix_mr_ops *mr_ops;
	int mr_cache_type;
	/* flag to indicate that memory registration is initialized and should not
	 * be changed at this point.
	 */
	int mr_is_init;
	int udreg_reg_limit;
#ifdef HAVE_UDREG
	udreg_cache_handle_t udreg_cache;
#endif
};

#define GNIX_CQS_PER_EP		8

struct gnix_fid_ep_ops_en {
	uint32_t msg_recv_allowed: 1;
	uint32_t msg_send_allowed: 1;
	uint32_t rma_read_allowed: 1;
	uint32_t rma_write_allowed: 1;
	uint32_t tagged_recv_allowed: 1;
	uint32_t tagged_send_allowed: 1;
	uint32_t atomic_read_allowed: 1;
	uint32_t atomic_write_allowed: 1;
};

#define GNIX_HTD_POOL_SIZE 128

struct gnix_htd_buf {
	struct slist_entry e;
	uint8_t *buf;
};

struct gnix_htd_pool {
	bool enabled;
	fastlock_t lock;
	struct gnix_fid_mem_desc *md;
	struct slist sl;
	void *buf_ptr;
	void *sl_ptr;
};

struct gnix_addr_cache_entry {
	fi_addr_t addr;
	struct gnix_vc *vc;
};

/*
 *   gnix endpoint structure
 *
 * A gnix_cm_nic is associated with an EP if it is of type  FI_EP_RDM.
 * The gnix_cm_nic is used for building internal connections between the
 * endpoints at different addresses.
 */
struct gnix_fid_ep {
	struct fid_ep ep_fid;
	enum fi_ep_type type;
	struct gnix_fid_domain *domain;
	uint64_t op_flags;
	uint64_t caps;
	uint32_t use_tag_hlist;
	struct gnix_fid_cq *send_cq;
	struct gnix_fid_cq *recv_cq;
	struct gnix_fid_cntr *send_cntr;
	struct gnix_fid_cntr *recv_cntr;
	struct gnix_fid_cntr *write_cntr;
	struct gnix_fid_cntr *read_cntr;
	struct gnix_fid_cntr *rwrite_cntr;
	struct gnix_fid_cntr *rread_cntr;
	struct gnix_fid_av *av;
	struct gnix_fid_stx *stx_ctx;
	struct gnix_ep_name my_name;
	struct gnix_cm_nic *cm_nic;
	struct gnix_nic *nic;
	fastlock_t vc_lock;
	union {
		struct gnix_hashtable *vc_ht;	/* FI_AV_MAP */
		struct gnix_vector *vc_table;	/* FI_AV_TABLE */
	};
	struct gnix_vc *vc;		/* used for FI_EP_MSG */
	struct dlist_entry unmapped_vcs;
	/* lock for unexp and posted recv queue */
	fastlock_t recv_queue_lock;
	/* used for unexpected receives */
	struct gnix_tag_storage unexp_recv_queue;
	/* used for posted receives */
	struct gnix_tag_storage posted_recv_queue;

	fastlock_t tagged_queue_lock;
	struct gnix_tag_storage tagged_unexp_recv_queue;
	struct gnix_tag_storage tagged_posted_recv_queue;

	/* pointer to tag matching engine */
	int (*progress_fn)(struct gnix_fid_ep *, enum gnix_progress_type);
	/* RX specific progress fn */
	int (*rx_progress_fn)(struct gnix_fid_ep *, gni_return_t *rc);
	struct gnix_xpmem_handle *xpmem_hndl;
	bool tx_enabled;
	bool rx_enabled;
	bool requires_lock;
	int last_cached;
	struct gnix_addr_cache_entry addr_cache[GNIX_ADDR_CACHE_SIZE];
	int send_selective_completion;
	int recv_selective_completion;
	int min_multi_recv;
	/* note this free list will be initialized for thread safe */
	struct gnix_freelist fr_freelist;
	struct gnix_htd_pool htd_pool;
	struct gnix_reference ref_cnt;
	struct gnix_fid_ep_ops_en ep_ops;
};

#define GNIX_EP_RDM(type)         (type == FI_EP_RDM)

#define GNIX_EP_DGM(type)         (type == FI_EP_DGRAM)

#define GNIX_EP_RDM_DGM(type)     ((type == FI_EP_RDM) || \
				   (type == FI_EP_DGRAM))

#define GNIX_EP_RDM_DGM_MSG(type) ((type == FI_EP_RDM)   || \
				   (type == FI_EP_DGRAM) || \
				   (type == FI_EP_MSG))

/**
 * gnix_fid_stx struct
 * @note - this is essentially a dummy structure for the GNI
 *         provider.  Added as a convenience for those consumers
 *         who need to use the fid_stx construct for other
 *         providers.
 *
 * @var stx_fid              embedded struct fid_stx field
 * @var domain               pointer to domain used to create the stx instance
 * @var ref_cnt              ref cnt on this object
 */
struct gnix_fid_stx {
	struct fid_stx stx_fid;
	struct gnix_fid_domain *domain;
	struct gnix_reference ref_cnt;
};

/*
 * TODO: Support shared named AVs
 */
struct gnix_fid_av {
	struct fid_av av_fid;
	struct gnix_fid_domain *domain;
	enum fi_av_type type;
	struct gnix_av_addr_entry* table;
	int *valid_entry_vec;
	size_t addrlen;
	/* How many addresses AV can hold before it needs to be resized */
	size_t capacity;
	/* How many address are currently stored in AV */
	size_t count;
	/* Hash table for mapping FI_AV_MAP */
	struct gnix_hashtable *map_ht;
	/*
	 * linked list of blocks used for allocating entries
	 *  for FI_AV_MAP
	 */
	struct slist block_list;
	struct gnix_reference ref_cnt;
};

enum gnix_fab_req_type {
	GNIX_FAB_RQ_SEND,
	GNIX_FAB_RQ_SENDV,
	GNIX_FAB_RQ_TSEND,
	GNIX_FAB_RQ_TSENDV,
	GNIX_FAB_RQ_RDMA_WRITE,
	GNIX_FAB_RQ_RDMA_READ,
	GNIX_FAB_RQ_RECV,
	GNIX_FAB_RQ_RECVV,
	GNIX_FAB_RQ_TRECV,
	GNIX_FAB_RQ_TRECVV,
	GNIX_FAB_RQ_AMO,
	GNIX_FAB_RQ_FAMO,
	GNIX_FAB_RQ_CAMO,
	GNIX_FAB_RQ_END_NON_NATIVE,
	GNIX_FAB_RQ_START_NATIVE = GNIX_NAMO_AX,
	GNIX_FAB_RQ_NAMO_AX = GNIX_NAMO_AX,
	GNIX_FAB_RQ_NAMO_AX_S = GNIX_NAMO_AX_S,
	GNIX_FAB_RQ_NAMO_FAX = GNIX_NAMO_FAX,
	GNIX_FAB_RQ_NAMO_FAX_S = GNIX_NAMO_FAX_S,
	GNIX_FAB_RQ_MAX_TYPES,
};

struct gnix_fab_req_rma {
	uint64_t                 loc_addr;
	struct gnix_fid_mem_desc *loc_md;
	size_t                   len;
	uint64_t                 rem_addr;
	uint64_t                 rem_mr_key;
	uint64_t                 imm;
	atomic_t                 outstanding_txds;
	gni_return_t             status;
};

struct gnix_fab_req_msg {
	struct gnix_tag_list_element tle;

	struct send_info_t {
		uint64_t	 send_addr;
		size_t		 send_len;
		gni_mem_handle_t mem_hndl;
		uint32_t	 head;
		uint32_t	 tail;
	}			     send_info[GNIX_MAX_MSG_IOV_LIMIT];
	struct gnix_fid_mem_desc     *send_md[GNIX_MAX_MSG_IOV_LIMIT];
	size_t                       send_iov_cnt;
	uint64_t                     send_flags;
	size_t			     cum_send_len;

	struct recv_info_t {
		uint64_t	 recv_addr;
		size_t		 recv_len;
		gni_mem_handle_t mem_hndl;
		uint32_t	 tail_len : 2; /* If the send len is > the recv_len, we
						* need to fetch the unaligned tail into
						* the txd's int buf
						*/
		uint32_t	 head_len : 2;
	}			     recv_info[GNIX_MAX_MSG_IOV_LIMIT];
	struct gnix_fid_mem_desc     *recv_md[GNIX_MAX_MSG_IOV_LIMIT];
	size_t			     recv_iov_cnt;
	uint64_t                     recv_flags; /* protocol, API info */
	size_t			     cum_recv_len;

	/* @var htd_buf->buf "head(H) tail(T) data buf" layout: '[T|T|...|H|H]' */
	struct slist_entry	     *htd_buf_e;
	uint8_t			     *htd_buf;
	gni_mem_handle_t	     htd_mdh;

	uint64_t                     tag;
	uint64_t                     ignore;
	uint64_t                     imm;
	gni_mem_handle_t             rma_mdh;
	uint64_t                     rma_id;
	atomic_t                     outstanding_txds;
	gni_return_t                 status;
};

struct gnix_fab_req_amo {
	uint64_t                 loc_addr;
	struct gnix_fid_mem_desc *loc_md;
	size_t                   len;
	uint64_t                 rem_addr;
	uint64_t                 rem_mr_key;
	uint64_t                 imm;
	enum fi_datatype         datatype;
	enum fi_op               op;
	uint64_t                 first_operand;
	uint64_t                 second_operand;
	void                     *read_buf;
};

/*
 * Check for remote peer capabilities.
 * inputs:
 *   pc        - peer capabilities
 *   ops_flags - current operation flags (FI_RMA, FI_READ, etc.)
 *
 * See capabilities section in fi_getinfo.3.
 */
static inline int gnix_rma_read_target_allowed(uint64_t pc,
					       uint64_t ops_flags)
{
	if (ops_flags & FI_RMA) {
		if (ops_flags & FI_READ) {
			if (pc & FI_RMA) {
				if (pc & FI_REMOTE_READ)
					return 1;
				if (pc & (FI_READ | FI_WRITE | FI_REMOTE_WRITE))
					return 0;
				return 1;
			}
		}
	}
	return 0;
}
static inline int gnix_rma_write_target_allowed(uint64_t pc,
						uint64_t ops_flags)
{
	if (ops_flags & FI_RMA) {
		if (ops_flags & FI_WRITE) {
			if (pc & FI_RMA) {
				if (pc & FI_REMOTE_WRITE)
					return 1;
				if (pc & (FI_READ | FI_WRITE | FI_REMOTE_READ))
					return 0;
				return 1;
			}
		}
	}
	return 0;
}

static inline int gnix_atomic_read_target_allowed(uint64_t pc,
						  uint64_t ops_flags)
{
	if (ops_flags & FI_ATOMICS) {
		if (ops_flags & FI_READ) {
			if (pc & FI_ATOMICS) {
				if (pc & FI_REMOTE_READ)
					return 1;
				if (pc & (FI_READ | FI_WRITE | FI_REMOTE_WRITE))
					return 0;
				return 1;
			}
		}
	}
	return 0;
}

static inline int gnix_atomic_write_target_allowed(uint64_t pc,
						   uint64_t ops_flags)
{
	if (ops_flags & FI_ATOMICS) {
		if (ops_flags & FI_WRITE) {
			if (pc & FI_ATOMICS) {
				if (pc & FI_REMOTE_WRITE)
					return 1;
				if (pc & (FI_READ | FI_WRITE | FI_REMOTE_READ))
					return 0;
				return 1;
			}
		}
	}
	return 0;
}

/*
 * Test if this operation is permitted based on the type of transfer
 * (encoded in the flags parameter), the endpoint capabilities and the
 * remote endpoint (peer) capabilities. Set a flag to speed up future checks.
 */

static inline int gnix_ops_allowed(struct gnix_fid_ep *ep,
				   uint64_t peer_caps,
				   uint64_t flags)
{
	uint64_t caps = ep->caps;

	GNIX_DEBUG(FI_LOG_EP_DATA, "flags:0x%llx, %s\n", flags,
		   fi_tostr(&flags, FI_TYPE_OP_FLAGS));
	GNIX_DEBUG(FI_LOG_EP_DATA, "peer_caps:0x%llx, %s\n", peer_caps,
		   fi_tostr(&peer_caps, FI_TYPE_OP_FLAGS));
	GNIX_DEBUG(FI_LOG_EP_DATA, "caps:0x%llx, %s\n",
		   ep->caps, fi_tostr(&ep->caps, FI_TYPE_CAPS));

	if ((flags & FI_RMA) && (flags & FI_READ)) {
		if (unlikely(!ep->ep_ops.rma_read_allowed)) {
			/* check if read initiate capabilities are allowed */
			if (caps & FI_RMA) {
				if (caps & FI_READ) {
					;
				} else if (caps & (FI_WRITE |
						   FI_REMOTE_WRITE |
						   FI_REMOTE_READ)) {
					return 0;
				}
			} else {
				return 0;
			}
			/* check if read remote capabilities are allowed */
			if (gnix_rma_read_target_allowed(peer_caps, flags)) {
				ep->ep_ops.rma_read_allowed = 1;
				return 1;
			}
			return 0;
		}
		return 1;
	} else if ((flags & FI_RMA) && (flags & FI_WRITE)) {
		if (unlikely(!ep->ep_ops.rma_write_allowed)) {
			/* check if write initiate capabilities are allowed */
			if (caps & FI_RMA) {
				if (caps & FI_WRITE) {
					;
				} else if (caps & (FI_READ |
						   FI_REMOTE_WRITE |
						   FI_REMOTE_READ)) {
					return 0;
				}
			} else {
				return 0;
			}
			/* check if write remote capabilities are allowed */
			if (gnix_rma_write_target_allowed(peer_caps, flags)) {
				ep->ep_ops.rma_write_allowed = 1;
				return 1;
			}
			return 0;
		}
		return 1;
	} else if ((flags & FI_ATOMICS) && (flags & FI_READ)) {
		if (unlikely(!ep->ep_ops.atomic_read_allowed)) {
			/* check if read initiate capabilities are allowed */
			if (caps & FI_ATOMICS) {
				if (caps & FI_READ) {
					;
				} else if (caps & (FI_WRITE |
						   FI_REMOTE_WRITE |
						   FI_REMOTE_READ)) {
					return 0;
				}
			} else {
				return 0;
			}
			/* check if read remote capabilities are allowed */
			if (gnix_atomic_read_target_allowed(peer_caps, flags)) {
				ep->ep_ops.atomic_read_allowed = 1;
				return 1;
			}
			return 0;
		}
		return 1;
	} else if ((flags & FI_ATOMICS) && (flags & FI_WRITE)) {
		if (unlikely(!ep->ep_ops.atomic_write_allowed)) {
			/* check if write initiate capabilities are allowed */
			if (caps & FI_ATOMICS) {
				if (caps & FI_WRITE) {
					;
				} else if (caps & (FI_READ |
						   FI_REMOTE_WRITE |
						   FI_REMOTE_READ)) {
					return 0;
				}
			} else {
				return 0;
			}
			/* check if write remote capabilities are allowed */
			if (gnix_atomic_write_target_allowed(peer_caps,
							     flags)) {
				ep->ep_ops.atomic_write_allowed = 1;
				return 1;
			}
			return 0;
		}
		return 1;
	}

	GNIX_ERR(FI_LOG_EP_DATA, "flags do not make sense %llx\n", flags);

	return 0;
}

/**
 * Fabric request layout, there is a one to one
 * correspondence between an application's invocation of fi_send, fi_recv
 * and a gnix fab_req.
 *
 * @var dlist	     a doubly linked list entry used to queue a request in
 * either the vc's tx_queue or work_queue.
 * @var addr	     the peer's gnix_address associated with this request.
 * @var type	     the fabric request type
 * @var gnix_ep      the gni endpoint associated with this request
 * @var user_context the user context, typically the receive buffer address for
 * a send or the send buffer address for a receive.
 * @var vc	      the virtual channel or connection edge between the sender
 * and receiver.
 * @var work_fn	     the function called by the nic progress loop to initiate
 * the fabric request.
 * @var flags	      a set of bit patterns that apply to all message types
 * @var iov_txds      A list of pending Rdma/CtFma GET txds.
 * @var iov_txd_cnt   The count of outstanding iov txds.
 * @var tx_failures   tx failure bits.
 * @var rma	      GNI PostRdma request
 * @var msg	      GNI SMSG request
 * @var amo	      GNI Fma request
 */
struct gnix_fab_req {
	struct dlist_entry        dlist;
	struct gnix_address       addr;
	enum gnix_fab_req_type    type;
	struct gnix_fid_ep        *gnix_ep;
	void                      *user_context;
	struct gnix_vc            *vc;
	int                       (*work_fn)(void *);
	uint64_t                  flags;

	/* TODO: change the size of this for unaligned data? */
	struct gnix_tx_descriptor *iov_txds[GNIX_MAX_MSG_IOV_LIMIT];
	/*
	 * special value of UINT_MAX is used to indicate
	 * an unrecoverable (aka non-transient) error has occurred
	 * in one of the underlying GNI transactions
	 */
	uint32_t		  tx_failures;

	/* common to rma/amo/msg */
	union {
		struct gnix_fab_req_rma rma;
		struct gnix_fab_req_msg msg;
		struct gnix_fab_req_amo amo;
	};
	char inject_buf[GNIX_INJECT_SIZE];
};

/*
 * test whether a request is replayable
 * or not based on the value of the tx_failures field
 */

static inline bool _gnix_req_replayable(struct gnix_fab_req *req)
{
	bool ret = false;
	uint32_t tx_failures, max_retrans;

	tx_failures = req->tx_failures;
	max_retrans = req->gnix_ep->domain->params.max_retransmits;
	if ((req->tx_failures != UINT_MAX) &&
	    (++tx_failures < max_retrans))
		ret = true;

	return ret;
}
static inline int _gnix_req_inject_err(struct gnix_fab_req *req)
{
	int err_cnt = req->gnix_ep->domain->params.err_inject_count;

	if (likely(!err_cnt)) {
		return 0;
	} else if (err_cnt > 0) {
		return req->tx_failures < err_cnt;
	} else { /* (err_cnt < 0) */
		return req->tx_failures < (rand() % (-err_cnt));
	}
}

static inline int _gnix_req_inject_smsg_err(struct gnix_fab_req *req)
{
	int err_cnt = req->gnix_ep->domain->params.err_inject_count;
	int retrans_cnt = req->gnix_ep->domain->params.max_retransmits;

	if (likely(!err_cnt)) {
		return 0;
	} else if (retrans_cnt <= err_cnt) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * work queue struct, used for handling delay ops, etc. in a generic wat
 */

struct gnix_work_req {
	struct dlist_entry list;
	/* function to be invoked to progress this work queue req.
	   first element is pointer to data needec by the func, second
	   is a pointer to an int which will be set to 1 if progress
	   function is complete */
	int (*progress_fn)(void *, int *);
	/* data to be passed to the progress function */
	void *data;
	/* function to be invoked if this work element has completed */
	int (*completer_fn)(void *);
	/* data for completer function */
	void *completer_data;
};

/*
 * globals
 */
extern const char gnix_fab_name[];
extern const char gnix_dom_name[];
extern uint32_t gnix_cdm_modes;
extern atomic_t gnix_id_counter;


/*
 * linked list helpers
 */

static inline void gnix_slist_insert_tail(struct slist_entry *item,
					  struct slist *list)
{
	item->next = NULL;
	slist_insert_tail(item, list);
}

/*
 * prototypes for fi ops methods
 */
int gnix_domain_open(struct fid_fabric *fabric, struct fi_info *info,
		     struct fid_domain **domain, void *context);

int gnix_av_open(struct fid_domain *domain, struct fi_av_attr *attr,
		 struct fid_av **av, void *context);

int gnix_cq_open(struct fid_domain *domain, struct fi_cq_attr *attr,
		 struct fid_cq **cq, void *context);

int gnix_ep_open(struct fid_domain *domain, struct fi_info *info,
		 struct fid_ep **ep, void *context);

int gnix_eq_open(struct fid_fabric *fabric, struct fi_eq_attr *attr,
		 struct fid_eq **eq, void *context);

int gnix_mr_reg(struct fid *fid, const void *buf, size_t len,
		uint64_t access, uint64_t offset, uint64_t requested_key,
		uint64_t flags, struct fid_mr **mr_o, void *context);

int gnix_cntr_open(struct fid_domain *domain, struct fi_cntr_attr *attr,
		 struct fid_cntr **cntr, void *context);


/* Prepend DIRECT_FN to provider specific API functions for global visibility
 * when using fabric direct.  If the API function is static use the STATIC
 * macro to bind symbols globally when compiling with fabric direct.
 */
#ifdef FABRIC_DIRECT_ENABLED
#define DIRECT_FN __attribute__((visibility ("default")))
#define STATIC
#else
#define DIRECT_FN
#define STATIC static
#endif

#endif /* _GNIX_H_ */
