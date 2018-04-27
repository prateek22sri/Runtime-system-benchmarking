/*
 * Copyright (c) 2013-2015 Intel Corporation, Inc.  All rights reserved.
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

#include "config.h"

#include <fi_mem.h>

#include "fi_verbs.h"
#include "ep_rdm/verbs_rdm.h"

static void fi_ibv_fini(void);

static const char *local_node = "localhost";

struct fi_provider fi_ibv_prov = {
	.name = VERBS_PROV_NAME,
	.version = VERBS_PROV_VERS,
	.fi_version = FI_VERSION(1, 3),
	.getinfo = fi_ibv_getinfo,
	.fabric = fi_ibv_fabric,
	.cleanup = fi_ibv_fini
};

int fi_ibv_sockaddr_len(struct sockaddr *addr)
{
	if (!addr)
		return 0;

	switch (addr->sa_family) {
	case AF_INET:
		return sizeof(struct sockaddr_in);
	case AF_INET6:
		return sizeof(struct sockaddr_in6);
	case AF_IB:
		return sizeof(struct sockaddr_ib);
	default:
		return 0;
	}
}

int fi_ibv_rdm_cm_bind_ep(struct fi_ibv_rdm_cm *cm, struct fi_ibv_rdm_ep *ep)
{
	char my_ipoib_addr_str[INET6_ADDRSTRLEN];

	assert(cm->ec && cm->listener);

	memcpy(&ep->my_addr, ep->domain->info->src_addr, sizeof(ep->my_addr));

	inet_ntop(ep->my_addr.sin_family,
		  &ep->my_addr.sin_addr.s_addr,
		  my_ipoib_addr_str, INET_ADDRSTRLEN);

	VERBS_INFO(FI_LOG_EP_CTRL, "My IPoIB: %s\n", my_ipoib_addr_str);

	if (!cm->is_bound) {
		errno = 0;
		if (rdma_bind_addr(cm->listener, (struct sockaddr *)&ep->my_addr)) {
			VERBS_INFO(FI_LOG_EP_CTRL,
				"Failed to bind cm listener to my IPoIB addr %s: %s\n",
				my_ipoib_addr_str, strerror(errno));
			return -FI_EOTHER;
		}
		if (rdma_listen(cm->listener, 1024)) {
			VERBS_INFO(FI_LOG_EP_CTRL, "rdma_listen failed: %s\n",
				strerror(errno));
			return -FI_EOTHER;
		}
		cm->is_bound = 1;
	}

	if (!ep->my_addr.sin_port) {
		ep->my_addr.sin_port = rdma_get_src_port(cm->listener);
	}
	assert(ep->my_addr.sin_family == AF_INET);

	VERBS_INFO(FI_LOG_EP_CTRL, "My ep_addr: %s:%u\n",
		inet_ntoa(ep->my_addr.sin_addr), ntohs(ep->my_addr.sin_port));

	return FI_SUCCESS;
}

int fi_ibv_get_rdma_rai(const char *node, const char *service, uint64_t flags,
		   const struct fi_info *hints, struct rdma_addrinfo **rai)
{
	struct rdma_addrinfo rai_hints, *_rai;
	struct rdma_addrinfo **rai_current;
	int ret = fi_ibv_fi_to_rai(hints, flags, &rai_hints);

	if (ret)
		goto out;

	if (!node && !rai_hints.ai_dst_addr) {
		if ((!rai_hints.ai_src_addr && !service) ||
		    (!rai_hints.ai_src_addr && FI_IBV_EP_TYPE_IS_RDM(hints)))
		{
			node = local_node;
		}
		rai_hints.ai_flags |= RAI_PASSIVE;
	}

	ret = rdma_getaddrinfo((char *) node, (char *) service,
				&rai_hints, &_rai);
	if (ret) {
		VERBS_INFO_ERRNO(FI_LOG_FABRIC, "rdma_getaddrinfo", errno);
		if (errno) {
			ret = -errno;
		}
		goto out;
	}

	/*
	 * If caller requested rai, remove ib_rai entries added by IBACM to
	 * prevent wrong ib_connect_hdr from being sent in connect request.
	 */
	if (rai && hints && (hints->addr_format != FI_SOCKADDR_IB)) {
		for (rai_current = &_rai; *rai_current;) {
			struct rdma_addrinfo *rai_next;
			if ((*rai_current)->ai_family == AF_IB) {
				rai_next = (*rai_current)->ai_next;
				(*rai_current)->ai_next = NULL;
				rdma_freeaddrinfo(*rai_current);
				*rai_current = rai_next;
				continue;
			}
			rai_current = &(*rai_current)->ai_next;
		}
	}

	if (rai)
		*rai = _rai;

out:
	if (rai_hints.ai_src_addr)
		free(rai_hints.ai_src_addr);
	if (rai_hints.ai_dst_addr)
		free(rai_hints.ai_dst_addr);
	return ret;
}

int fi_ibv_create_ep(const char *node, const char *service,
		     uint64_t flags, const struct fi_info *hints,
		     struct rdma_addrinfo **rai, struct rdma_cm_id **id)
{
	struct rdma_addrinfo *_rai;
	struct sockaddr *local_addr;
	int ret;

	ret = fi_ibv_get_rdma_rai(node, service, flags, hints, &_rai);
	if (ret) {
		return ret;
	}

	ret = rdma_create_ep(id, _rai, NULL, NULL);
	if (ret) {
		VERBS_INFO_ERRNO(FI_LOG_FABRIC, "rdma_create_ep", errno);
		ret = -errno;
		goto err1;
	}
	if (rai && !_rai->ai_src_addr) {
		local_addr = rdma_get_local_addr(*id);
		_rai->ai_src_len = fi_ibv_sockaddr_len(local_addr);
		if (!(_rai->ai_src_addr = malloc(_rai->ai_src_len))) {
			ret = -FI_ENOMEM;
			goto err2;
		}
		memcpy(_rai->ai_src_addr, local_addr, _rai->ai_src_len);
	}

	if (rai) {
		*rai = _rai;
	} else {
		rdma_freeaddrinfo(_rai);
	}

	return ret;
err2:
	rdma_destroy_ep(*id);
err1:
	rdma_freeaddrinfo(_rai);

	return ret;
}

void fi_ibv_destroy_ep(struct rdma_addrinfo *rai, struct rdma_cm_id **id)
{
	rdma_freeaddrinfo(rai);
	rdma_destroy_ep(*id);
}

#define VERBS_SIGNAL_SEND(ep) \
	(atomic_get(&ep->unsignaled_send_cnt) >= VERBS_SEND_SIGNAL_THRESH(ep) && \
	 !atomic_get(&ep->comp_pending))

static int fi_ibv_signal_send(struct fi_ibv_msg_ep *ep, struct ibv_send_wr *wr)
{
	struct fi_ibv_msg_epe *epe;

	fastlock_acquire(&ep->scq->lock);
	if (VERBS_SIGNAL_SEND(ep)) {
		epe = util_buf_alloc(ep->scq->domain->fab->epe_pool);
		if (!epe) {
			fastlock_release(&ep->scq->lock);
			return -FI_ENOMEM;
		}
		memset(epe, 0, sizeof(*epe));
		wr->send_flags |= IBV_SEND_SIGNALED;
		wr->wr_id = ep->ep_id;
		epe->ep = ep;
		slist_insert_tail(&epe->entry, &ep->scq->ep_list);
		atomic_inc(&ep->comp_pending);
	}
	fastlock_release(&ep->scq->lock);
	return 0;
}

static int fi_ibv_reap_comp(struct fi_ibv_msg_ep *ep)
{
	struct fi_ibv_wce *wce = NULL;
	int got_wc = 0;
	int ret = 0;

	fastlock_acquire(&ep->scq->lock);
	while (atomic_get(&ep->comp_pending) > 0) {
		if (!wce) {
			wce = util_buf_alloc(ep->scq->domain->fab->wce_pool);
			if (!wce) {
				fastlock_release(&ep->scq->lock);
				return -FI_ENOMEM;
			}
			memset(wce, 0, sizeof(*wce));
		}
		ret = fi_ibv_poll_cq(ep->scq, &wce->wc);
		if (ret < 0) {
			FI_WARN(&fi_ibv_prov, FI_LOG_EP_DATA,
				"Failed to read completion for signaled send\n");
			util_buf_release(ep->scq->domain->fab->wce_pool, wce);
			fastlock_release(&ep->scq->lock);
			return ret;
		} else if (ret > 0) {
			slist_insert_tail(&wce->entry, &ep->scq->wcq);
			got_wc = 1;
			wce = NULL;
		}
	}
	if (wce)
		util_buf_release(ep->scq->domain->fab->wce_pool, wce);

	if (got_wc && ep->scq->channel)
		ret = fi_ibv_cq_signal(&ep->scq->cq_fid);

	fastlock_release(&ep->scq->lock);
	return ret;
}

ssize_t fi_ibv_send(struct fi_ibv_msg_ep *ep, struct ibv_send_wr *wr, size_t len,
		    int count, void *context)
{
	struct ibv_send_wr *bad_wr;
	int ret;

	assert(ep->scq);
	wr->num_sge = count;
	wr->wr_id = (uintptr_t) context;

	if (wr->send_flags & IBV_SEND_SIGNALED) {
		assert((wr->wr_id & ep->scq->wr_id_mask) != ep->scq->send_signal_wr_id);
		atomic_set(&ep->unsignaled_send_cnt, 0);
	} else {
		if (VERBS_SIGNAL_SEND(ep)) {
			ret = fi_ibv_signal_send(ep, wr);
			if (ret)
				return ret;
		} else {
			atomic_inc(&ep->unsignaled_send_cnt);

			if (atomic_get(&ep->unsignaled_send_cnt) >=
					VERBS_SEND_COMP_THRESH(ep)) {
				ret = fi_ibv_reap_comp(ep);
				if (ret)
					return ret;
			}
		}
	}

	ret = ibv_post_send(ep->id->qp, wr, &bad_wr);
	switch (ret) {
	case ENOMEM:
		return -FI_EAGAIN;
	case -1:
		/* Deal with non-compliant libibverbs drivers which set errno
		 * instead of directly returning the error value */
		return (errno == ENOMEM) ? -FI_EAGAIN : -errno;
	default:
		return -ret;
	}
}

ssize_t fi_ibv_send_buf(struct fi_ibv_msg_ep *ep, struct ibv_send_wr *wr,
			const void *buf, size_t len, void *desc, void *context)
{
	struct ibv_sge sge;

	fi_ibv_set_sge(sge, buf, len, desc);
	wr->sg_list = &sge;

	return fi_ibv_send(ep, wr, len, 1, context);
}

ssize_t fi_ibv_send_buf_inline(struct fi_ibv_msg_ep *ep, struct ibv_send_wr *wr,
			       const void *buf, size_t len)
{
	struct ibv_sge sge;

	fi_ibv_set_sge_inline(sge, buf, len);
	wr->sg_list = &sge;

	return fi_ibv_send(ep, wr, len, 1, NULL);
}

ssize_t fi_ibv_send_iov_flags(struct fi_ibv_msg_ep *ep, struct ibv_send_wr *wr,
			      const struct iovec *iov, void **desc, int count,
			      void *context, uint64_t flags)
{
	size_t len = 0;

	if (!desc)
		fi_ibv_set_sge_iov_inline(wr->sg_list, iov, count, len);
	else
		fi_ibv_set_sge_iov(wr->sg_list, iov, count, desc, len);

	wr->send_flags = VERBS_INJECT_FLAGS(ep, len, flags) | VERBS_COMP_FLAGS(ep, flags);

	return fi_ibv_send(ep, wr, len, count, context);
}

static void fi_ibv_fini(void)
{
	fi_ibv_free_info();
}

VERBS_INI
{
	fi_param_define(&fi_ibv_prov, "iface", FI_PARAM_STRING,
			"the prefix or the full name of the network interface "
			"associated with the IB device (default: ib)");
	fi_param_define(&fi_ibv_prov, "rdm_buffer_num", FI_PARAM_INT,
			"the number of pre-registered buffers for buffered "
			"operations between the endpoints, must be a power of 2 "
			"(default: 8)");
	fi_param_define(&fi_ibv_prov, "rdm_buffer_size", FI_PARAM_INT,
			"the maximum size of a buffered operation (bytes) "
			"(default: platform specific)");
	fi_param_define(&fi_ibv_prov, "rdm_rndv_seg_size", FI_PARAM_INT,
			"the segment size for zero copy protocols (bytes)"
			"(default: 1073741824)");
	fi_param_define(&fi_ibv_prov, "rdm_cqread_bunch_size", FI_PARAM_INT,
			"the number of entries to be read from the verbs "
			"completion queue at a time (default: 8)");
	fi_param_define(&fi_ibv_prov, "rdm_thread_timeout", FI_PARAM_INT,
			"the wake up timeout of the helper thread (usec) "
			"(default: 100)");
	fi_param_define(&fi_ibv_prov, "rdm_eager_send_opcode", FI_PARAM_STRING,
			"the operation code that will be used for eager messaging. "
			"Only IBV_WR_SEND and IBV_WR_RDMA_WRITE_WITH_IMM are supported. "
			"The last one is not applicable for iWarp. "
			"(default: IBV_WR_SEND)");

	return &fi_ibv_prov;
}
