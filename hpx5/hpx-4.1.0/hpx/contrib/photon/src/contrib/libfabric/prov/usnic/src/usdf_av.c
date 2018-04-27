/*
 * Copyright (c) 2014-2016, Cisco Systems, Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "rdma/fi_errno.h"
#include "fi_enosys.h"
#include "fi.h"

#include "usnic_direct.h"
#include "usnic_ip_utils.h"
#include "libnl_utils.h"
#include "usd.h"
#include "usd_queue.h"

#include "usdf.h"
#include "usdf_av.h"
#include "usdf_timer.h"
#include "usdf_rdm.h"

#include "fi_ext_usnic.h"

static int usdf_av_alloc_dest(struct usdf_dest **dest_o)
{
	struct usdf_dest *dest;

	dest = calloc(1, sizeof(**dest_o));
	if (dest == NULL)
		return -errno;

	SLIST_INIT(&dest->ds_rdm_rdc_list);

	*dest_o = dest;
	return 0;
}

static void usdf_av_free_dest(struct usdf_dest *dest)
{
	struct usdf_rdm_connection *rdc = NULL;

	LIST_REMOVE(dest, ds_addresses_entry);

	while (!SLIST_EMPTY(&dest->ds_rdm_rdc_list)) {
		rdc = SLIST_FIRST(&dest->ds_rdm_rdc_list);
		rdc->dc_dest = NULL;

		SLIST_REMOVE(&dest->ds_rdm_rdc_list, rdc, usdf_rdm_connection,
			     dc_addr_link);
		if (rdc)
			rdc->dc_dest = NULL;
	}

	free(dest);
}

static int usdf_av_close_(struct usdf_av *av)
{
	struct usdf_dest *entry;

	USDF_TRACE_SYS(AV, "\n");

	pthread_spin_lock(&av->av_lock);

	if (av->av_eq)
		atomic_dec(&av->av_eq->eq_refcnt);

	atomic_dec(&av->av_domain->dom_refcnt);

	while (!LIST_EMPTY(&av->av_addresses)) {
		entry = LIST_FIRST(&av->av_addresses);
		usdf_av_free_dest(entry);
	}

	pthread_spin_destroy(&av->av_lock);
	free(av);

	USDF_DBG_SYS(AV, "AV successfully destroyed\n");

	return 0;
}

static int usdf_av_close(struct fid *fid)
{
	struct usdf_av *av;
	int pending;

	USDF_TRACE_SYS(AV, "\n");

	av = container_of(fid, struct usdf_av, av_fid.fid);
	if (atomic_get(&av->av_refcnt) > 0)
		return -FI_EBUSY;

	pending = atomic_get(&av->av_active_inserts);
	assert(pending >= 0);

	if (pending) {
		USDF_DBG_SYS(AV, "%d pending inserts, defer closing\n",
			     pending);
		atomic_set(&av->av_closing, 1);
	} else {
		usdf_av_close_(av);
	}

	return 0;
}

static void
usdf_av_insert_async_complete(struct usdf_av_insert *insert)
{
	struct fi_eq_entry entry;
	struct usdf_av *av;
	int pending;
	int closing;

	av = insert->avi_av;

	entry.fid = &av->av_fid.fid;
	entry.context = insert->avi_context;
	entry.data = insert->avi_successes;
	usdf_eq_write_internal(av->av_eq,
		FI_AV_COMPLETE, &entry, sizeof(entry), 0);

	usdf_timer_free(av->av_domain->dom_fabric, insert->avi_timer);

	pending = atomic_dec(&av->av_active_inserts);
	USDF_DBG_SYS(AV, "new active insert value: %d\n", pending);
	assert(pending >= 0);

	closing = atomic_get(&av->av_closing);

	if (!pending && closing)
		usdf_av_close_(av);

	free(insert);
}

/*
 * A request failed, post an error event to the EQ
 */
static void
usdf_post_insert_request_error(struct usdf_av_insert *insert,
		struct usdf_av_req *req)
{
	struct fi_eq_err_entry err_entry;
	struct usdf_av *av;

	av = insert->avi_av;

	*req->avr_fi_addr = FI_ADDR_NOTAVAIL;
	free(req->avr_dest);

	err_entry.fid = &av->av_fid.fid;
	err_entry.context = insert->avi_context;
	err_entry.data = req - (struct usdf_av_req *)(insert + 1);
	err_entry.err = -req->avr_status;
	err_entry.err_data = NULL;
	err_entry.err_data_size = 0;

	usdf_eq_write_internal(av->av_eq, 0,
		&err_entry, sizeof(err_entry),
		USDF_EVENT_FLAG_ERROR);
}

/*
 * Called by progression thread to look for AV completions on this domain
 */
static void
usdf_av_insert_progress(void *v)
{
	int ret;
	struct usdf_av_insert *insert;
	struct usdf_fabric *fp;
	struct usdf_dest *dest;
	struct usdf_av_req *req;
	struct usdf_av_req *tmpreq;
	struct usd_device_attrs *dap;
	uint64_t now;
	uint8_t *eth;

	insert = v;
	fp = insert->avi_av->av_domain->dom_fabric;
	dap = fp->fab_dev_attrs;

	TAILQ_FOREACH_SAFE(req, tmpreq, &insert->avi_req_list, avr_link) {

		dest = req->avr_dest;
		eth = &dest->ds_dest.ds_dest.ds_udp.u_hdr.uh_eth.ether_dhost[0];
		ret = usnic_arp_lookup(dap->uda_ifname,
				req->avr_daddr_be, fp->fab_arp_sockfd, eth);

		/* anything besides EAGAIN means request is completed */
		if (ret != EAGAIN) {
			TAILQ_REMOVE(&insert->avi_req_list, req, avr_link);
			req->avr_status = -ret;

			if (ret == 0) {
				++insert->avi_successes;
				*(struct usdf_dest **)req->avr_fi_addr = dest;

				LIST_INSERT_HEAD(&insert->avi_av->av_addresses,
						 dest, ds_addresses_entry);
			} else {
				usdf_post_insert_request_error(insert, req);
			}
		}
	}

	/* Time for a new ARP? */
	now = usdf_get_ms();
	if (now - insert->avi_last_arp_time > USDF_AV_ARP_INTERVAL) {

		/* If no more ARP requests left, fail all remaining requests */
		if (insert->avi_arps_left == 0) {
			TAILQ_FOREACH(req, &insert->avi_req_list, avr_link) {
				req->avr_status = -FI_EHOSTUNREACH;
				usdf_post_insert_request_error(insert, req);
			}
			TAILQ_INIT(&insert->avi_req_list);

		/* Trigger an ARP request for all pending requests */
		} else {
			TAILQ_FOREACH_SAFE(req, tmpreq,
					&insert->avi_req_list, avr_link) {
				ret = usnic_arp_request(req->avr_daddr_be,
						fp->fab_arp_sockfd);
				if (ret != 0) {
					req->avr_status = -ret;
					TAILQ_REMOVE(&insert->avi_req_list,
							req, avr_link);
					usdf_post_insert_request_error(insert,
							req);
				}
			}

			insert->avi_last_arp_time = now;
			--insert->avi_arps_left;
		}
	}

	/* If no more pending requests, all done! */
	if (TAILQ_EMPTY(&insert->avi_req_list)) {
		usdf_av_insert_async_complete(insert);
	} else {
		/* retry in 1 ms */
		usdf_timer_set(fp, insert->avi_timer, 1);
	}

}

static int
usdf_am_insert_async(struct fid_av *fav, const void *addr, size_t count,
			  fi_addr_t *fi_addr, uint64_t flags, void *context)
{
	const struct sockaddr_in *sin;
	struct usd_device_attrs *dap;
	struct usdf_av_insert *insert;
	struct usdf_av_req *req;
	struct usdf_av *av;
	struct usdf_fabric *fp;
	struct usd_dest *u_dest;
	int ret;
	size_t i;

	USDF_TRACE_SYS(AV, "\n");

	if ((flags & ~FI_MORE) != 0) {
		return -FI_EBADFLAGS;
	}

	av = av_ftou(fav);
	fp = av->av_domain->dom_fabric;
	dap = fp->fab_dev_attrs;

	if (av->av_eq == NULL) {
		return -FI_ENOEQ;
	}

	sin = addr;

	/* allocate an insert record and N requests */
	insert = calloc(1, sizeof(*insert) + count * sizeof(*req));
	if (insert == NULL) {
		return -errno;
	}
	insert->avi_av = av;
	insert->avi_context = context;
	ret = usdf_timer_alloc(usdf_av_insert_progress, insert,
			&insert->avi_timer);
	if (ret != 0) {
		goto fail;
	}
	TAILQ_INIT(&insert->avi_req_list);
	insert->avi_arps_left = USDF_AV_MAX_ARPS;

	ret = atomic_inc(&av->av_active_inserts);
	USDF_DBG_SYS(AV, "new active insert value: %d\n", ret);

	/* If no addresses, complete now */
	if (count == 0) {
		usdf_av_insert_async_complete(insert);
		return 0;
	}

	req = (struct usdf_av_req *)(insert + 1);

	for (i = 0; i < count; i++) {
		req->avr_fi_addr = &fi_addr[i];

		/* find the address we actually need to look up */
		ret = usnic_nl_rt_lookup(dap->uda_ipaddr_be,
				sin->sin_addr.s_addr, dap->uda_ifindex,
				&req->avr_daddr_be);
		if (ret != 0) {
			if (ret == EHOSTUNREACH) {
				req->avr_status = -FI_EHOSTUNREACH;
				usdf_post_insert_request_error(insert, req);
			} else {
				ret = -ret;
				goto fail;
			}

		} else {
			if (req->avr_daddr_be == 0) {
				req->avr_daddr_be = sin->sin_addr.s_addr;
			}
			req->avr_dest = calloc(1, sizeof(*req->avr_dest));
			if (req->avr_dest == NULL) {
				ret = -FI_ENOMEM;
				goto fail;
			}
			u_dest = &req->avr_dest->ds_dest;
			usd_fill_udp_dest(u_dest, dap,
					sin->sin_addr.s_addr, sin->sin_port);
			u_dest->ds_dest.ds_udp.u_hdr.uh_ip.frag_off |=
				htons(IP_DF);

			TAILQ_INSERT_TAIL(&insert->avi_req_list, req, avr_link);
		}

		++sin;
		++req;
	}

	/* resolve all addresses we can */
	usdf_av_insert_progress(insert);

	return 0;

fail:
	if (insert != NULL) {
		if (insert->avi_timer != NULL) {
			usdf_timer_free(fp, insert->avi_timer);
		}
		free(insert);
	}
	return ret;
}

static int
usdf_am_insert_sync(struct fid_av *fav, const void *addr, size_t count,
			  fi_addr_t *fi_addr, uint64_t flags, void *context)
{
	const struct sockaddr_in *sin;
	struct usdf_av *av;
	struct usd_dest *u_dest;
	struct usdf_dest *dest;
	int ret_count;
	int ret;
	size_t i;

	USDF_TRACE_SYS(AV, "\n");

	if ((flags & ~FI_MORE) != 0) {
		return -FI_EBADFLAGS;
	}

	av = av_ftou(fav);

	ret_count = 0;
	sin = addr;

	/* XXX parallelize, this will also eliminate u_dest silliness */
	for (i = 0; i < count; i++) {
		dest = NULL;
		u_dest = NULL;
		ret = usdf_av_alloc_dest(&dest);
		if (ret == 0) {
			USDF_DBG_SYS(AV, "usd_create_dest(addr=0x%x, port=0x%x)\n",
				ntohl(sin->sin_addr.s_addr), ntohs(sin->sin_port));
			ret = usd_create_dest(av->av_domain->dom_dev,
				sin->sin_addr.s_addr, sin->sin_port,
				&u_dest);
		}
		if (ret == 0) {
			u_dest->ds_dest.ds_udp.u_hdr.uh_ip.frag_off |=
				htons(IP_DF);
			dest->ds_dest = *u_dest;
			fi_addr[i] = (fi_addr_t)dest;
			LIST_INSERT_HEAD(&av->av_addresses, dest,
					 ds_addresses_entry);
			++ret_count;
		} else {
			fi_addr[i] = FI_ADDR_NOTAVAIL;
			free(dest);
		}
		free(u_dest);
		++sin;
	}

	return ret_count;
}

static int usdf_resolve_addr(const char *node, const char *service,
		struct sockaddr_in *in)
{
	struct addrinfo *ai;
	int ret;

	struct addrinfo hints = {
		.ai_family = AF_INET,
	};

	if (!node || !service || !in)
		return -FI_EINVAL;

	ret = getaddrinfo(node, service, &hints, &ai);
	if (ret) {
		USDF_DBG("getaddrinfo: %s\n", gai_strerror(ret));
		return -FI_EINVAL;
	}

	*in = *(struct sockaddr_in *) ai->ai_addr;

	assert(ai->ai_family == AF_INET);
	assert(in->sin_family == AF_INET);

	freeaddrinfo(ai);
	return ret;
}

static int usdf_av_insertsvc(struct fid_av *fav, const char *node,
		const char *service, fi_addr_t *fi_addr, uint64_t flags,
		void *context)
{
	struct sockaddr_in *addr;
	int ret;

	USDF_TRACE_SYS(AV, "\n");

	if (!fav)
		return -FI_EINVAL;

	addr = calloc(1, sizeof(*addr));
	if (!addr) {
		USDF_DBG("address allocation failed\n");
		return -FI_ENOMEM;
	}

	ret = usdf_resolve_addr(node, service, addr);
	if (ret)
		goto fail;

	ret = fav->ops->insert(fav, addr, 1, fi_addr, flags, context);

fail:
	free(addr);
	return ret;
}

static int
usdf_am_remove(struct fid_av *fav, fi_addr_t *fi_addr, size_t count,
			  uint64_t flags)
{
	struct usdf_dest *dest;
	size_t i;

	USDF_TRACE_SYS(AV, "\n");

	for (i = 0; i < count; ++i) {
		if (fi_addr[i] != FI_ADDR_NOTAVAIL) {
			dest = (struct usdf_dest *)(uintptr_t)fi_addr[i];
			usdf_av_free_dest(dest);

			/* Mark invalid by setting to FI_ADDR_NOTAVAIL*/
			fi_addr[i] = FI_ADDR_NOTAVAIL;
		}
	}

	return 0;
}

static int
usdf_am_lookup(struct fid_av *av, fi_addr_t fi_addr, void *addr,
			  size_t *addrlen)
{
	struct usdf_dest *dest;
	struct sockaddr_in sin = { 0 };
	size_t copylen;

	USDF_TRACE_SYS(AV, "\n");

	if (fi_addr == FI_ADDR_NOTAVAIL) {
		USDF_WARN_SYS(AV, "invalid address, can't lookup\n");
		return -FI_EINVAL;
	}

	dest = (struct usdf_dest *)(uintptr_t)fi_addr;

	if (*addrlen < sizeof(sin)) {
		copylen = *addrlen;
	} else {
		copylen = sizeof(sin);
	}

	sin.sin_family = AF_INET;
	usd_expand_dest(&dest->ds_dest, &sin.sin_addr.s_addr, &sin.sin_port);
	memcpy(addr, &sin, copylen);

	*addrlen = sizeof(sin);
	return 0;
}

static const char *
usdf_av_straddr(struct fid_av *av, const void *addr,
				    char *buf, size_t *len)
{
	const struct sockaddr_in *sin;
	char straddr[24];
	int size;

	sin = addr;
	size = snprintf(straddr, sizeof straddr, "%s:%d",
			inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
	snprintf(buf, *len, "%s", straddr);
	*len = size + 1;
	return buf;
}

static int
usdf_av_bind(struct fid *fid, struct fid *bfid, uint64_t flags)
{
	struct usdf_av *av;

	USDF_TRACE_SYS(AV, "\n");

	av = av_fidtou(fid);

	switch (bfid->fclass) {
	case FI_CLASS_EQ:
		if (av->av_eq != NULL) {
			return -FI_EINVAL;
		}
		av->av_eq = eq_fidtou(bfid);
		atomic_inc(&av->av_eq->eq_refcnt);
		break;
	default:
		return -FI_EINVAL;
	}

	return 0;
}

static struct fi_ops usdf_av_fi_ops = {
	.size = sizeof(struct fi_ops),
	.close = usdf_av_close,
	.bind = usdf_av_bind,
	.control = fi_no_control,
	.ops_open = usdf_av_ops_open,
};

static struct fi_ops_av usdf_am_ops_async = {
	.size = sizeof(struct fi_ops_av),
	.insert = usdf_am_insert_async,
	.insertsvc = usdf_av_insertsvc,
	.insertsym = fi_no_av_insertsym,
	.remove = usdf_am_remove,
	.lookup = usdf_am_lookup,
	.straddr = usdf_av_straddr
};

static struct fi_ops_av usdf_am_ops_sync = {
	.size = sizeof(struct fi_ops_av),
	.insert = usdf_am_insert_sync,
	.insertsvc = usdf_av_insertsvc,
	.insertsym = fi_no_av_insertsym,
	.remove = usdf_am_remove,
	.lookup = usdf_am_lookup,
	.straddr = usdf_av_straddr
};

static int usdf_av_process_attr(struct fi_av_attr *attr)
{
	USDF_TRACE_SYS(AV, "\n");

	if (attr == NULL) {
		USDF_WARN_SYS(AV, "NULL AV attribute structure is invalid\n");
		return -FI_EINVAL;
	}

	if (attr->name || attr->map_addr || (attr->flags & FI_READ)) {
		USDF_WARN_SYS(AV, "named AVs are not supported\n");
		return -FI_ENOSYS;
	}

	if (attr->flags & ~FI_EVENT) {
		USDF_WARN_SYS(AV, "invalid flag, only FI_EVENT is supported\n");
		return -FI_EINVAL;
	}

	if (attr->rx_ctx_bits) {
		USDF_WARN_SYS(AV, "scalable endpoints not supported\n");
		return -FI_EINVAL;
	}

	if (attr->ep_per_node > 1)
		USDF_WARN_SYS(AV, "ep_per_node not supported, ignoring\n");

	switch (attr->type) {
	case FI_AV_UNSPEC:
		USDF_DBG_SYS(AV, "no AV type specified, using FI_AV_MAP\n");
	case FI_AV_MAP:
		break;
	case FI_AV_TABLE:
		USDF_DBG_SYS(AV, "FI_AV_TABLE is unsupported\n");
		return -FI_ENOSYS;
	default:
		USDF_WARN_SYS(AV, "unknown AV type %d, not supported",
			      attr->type);
		return -FI_EINVAL;
	}

	return FI_SUCCESS;
}

int
usdf_av_open(struct fid_domain *domain, struct fi_av_attr *attr,
		 struct fid_av **av_o, void *context)
{
	struct usdf_domain *udp;
	struct usdf_av *av;
	int ret;

	USDF_TRACE_SYS(AV, "\n");

	if (!av_o) {
		USDF_WARN_SYS(AV, "provided AV pointer can not be NULL\n");
		return -FI_EINVAL;
	}

	ret = usdf_av_process_attr(attr);
	if (ret)
		return ret;

	udp = dom_ftou(domain);

	av = calloc(1, sizeof(*av));
	if (av == NULL) {
		return -FI_ENOMEM;
	}

	if (attr->flags & FI_EVENT) {
		av->av_fid.ops = &usdf_am_ops_async;
	} else {
		av->av_fid.ops = &usdf_am_ops_sync;
	}

	LIST_INIT(&av->av_addresses);

	av->av_fid.fid.fclass = FI_CLASS_AV;
	av->av_fid.fid.context = context;
	av->av_fid.fid.ops = &usdf_av_fi_ops;
	av->av_flags = attr->flags;

	pthread_spin_init(&av->av_lock, PTHREAD_PROCESS_PRIVATE);
	atomic_initialize(&av->av_active_inserts, 0);
	atomic_initialize(&av->av_closing, 0);

	atomic_initialize(&av->av_refcnt, 0);
	atomic_inc(&udp->dom_refcnt);
	av->av_domain = udp;

	*av_o = av_utof(av);
	return 0;
}
