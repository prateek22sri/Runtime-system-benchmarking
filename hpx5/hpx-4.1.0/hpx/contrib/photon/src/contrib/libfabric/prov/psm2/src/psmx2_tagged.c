/*
 * Copyright (c) 2013-2014 Intel Corporation. All rights reserved.
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

#include "psmx2.h"

static ssize_t psmx2_tagged_peek_generic(struct fid_ep *ep,
					 void *buf, size_t len,
					 void *desc, fi_addr_t src_addr,
					 uint64_t tag, uint64_t ignore,
					 void *context, uint64_t flags)
{
	struct psmx2_fid_ep *ep_priv;
	struct psmx2_fid_av *av;
	struct psmx2_cq_event *event;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t req;
	psm2_mq_status2_t psm2_status;
	psm2_mq_tag_t psm2_tag, psm2_tagsel;
	uint32_t tag32, tagsel32;
	size_t idx;
	int err;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	if (src_addr != FI_ADDR_UNSPEC) {
		av = ep_priv->av;
		if (av && av->type == FI_AV_TABLE) {
			idx = (size_t)src_addr;
			if (idx >= av->last)
				return -FI_EINVAL;

			psm2_epaddr = av->epaddrs[idx];
			vlane = av->vlanes[idx];
		} else {
			psm2_epaddr = PSMX2_ADDR_TO_EP(src_addr);
			vlane = PSMX2_ADDR_TO_VL(src_addr);
		}
		tag32 = PSMX2_TAG32(0, vlane, ep_priv->vlane);
		tagsel32 = -1;
	} else {
		psm2_epaddr = 0;
		tag32 = PSMX2_TAG32(0, 0, ep_priv->vlane);
		tagsel32 = ~PSMX2_SRC_BITS;
	}

	PSMX2_SET_TAG(psm2_tag, tag, tag32);
	PSMX2_SET_TAG(psm2_tagsel, ~ignore, tagsel32);

	if (flags & (FI_CLAIM | FI_DISCARD))
		err = psm2_mq_improbe2(ep_priv->domain->psm2_mq,
				       psm2_epaddr, &psm2_tag,
				       &psm2_tagsel, &req, &psm2_status);
	else
		err = psm2_mq_iprobe2(ep_priv->domain->psm2_mq,
				      psm2_epaddr, &psm2_tag, &psm2_tagsel,
				      &psm2_status);
	switch (err) {
	case PSM2_OK:
		if (ep_priv->recv_cq) {
			if ((flags & FI_CLAIM) && context)
				PSMX2_CTXT_REQ((struct fi_context *)context) = req;

			tag = PSMX2_GET_TAG64(psm2_status.msg_tag);
			len = psm2_status.msg_length;
			event = psmx2_cq_create_event(
					ep_priv->recv_cq,
					context,		/* op_context */
					NULL,			/* buf */
					flags|FI_RECV|FI_TAGGED,/* flags */
					len,			/* len */
					0,			/* data */
					tag,			/* tag */
					len,			/* olen */
					0);			/* err */

			if (!event)
				return -FI_ENOMEM;

			vlane = PSMX2_TAG32_GET_SRC(psm2_status.msg_tag.tag2);
			event->source = PSMX2_EP_TO_ADDR(psm2_status.msg_peer, vlane);
			psmx2_cq_enqueue_event(ep_priv->recv_cq, event);
		}
		return 0;

	case PSM2_MQ_NO_COMPLETIONS:
		if (ep_priv->recv_cq) {
			event = psmx2_cq_create_event(
					ep_priv->recv_cq,
					context,		/* op_context */
					NULL,			/* buf */
					flags|FI_RECV|FI_TAGGED,/* flags */
					len,			/* len */
					0,			/* data */
					tag,			/* tag */
					len,			/* olen */
					-FI_ENOMSG);		/* err */

			if (!event)
				return -FI_ENOMEM;

			event->source = 0;
			psmx2_cq_enqueue_event(ep_priv->recv_cq, event);
		}
		return 0;

	default:
		return psmx2_errno(err);
	}
}

ssize_t psmx2_tagged_recv_generic(struct fid_ep *ep, void *buf,
				  size_t len, void *desc,
				  fi_addr_t src_addr, 
				  uint64_t tag, uint64_t ignore,
				  void *context, uint64_t flags)
{
	struct psmx2_fid_ep *ep_priv;
	struct psmx2_fid_av *av;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag, psm2_tagsel;
	uint32_t tag32, tagsel32;	
	struct fi_context *fi_context;
	size_t idx;
	int err;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	if (flags & FI_PEEK)
		return psmx2_tagged_peek_generic(ep, buf, len, desc,
						 src_addr, tag, ignore,
						 context, flags);

	if (flags & FI_TRIGGER) {
		struct psmx2_trigger *trigger;
		struct fi_triggered_context *ctxt = context;

		trigger = calloc(1, sizeof(*trigger));
		if (!trigger)
			return -FI_ENOMEM;

		trigger->op = PSMX2_TRIGGERED_TRECV;
		trigger->cntr = container_of(ctxt->trigger.threshold.cntr,
					     struct psmx2_fid_cntr, cntr);
		trigger->threshold = ctxt->trigger.threshold.threshold;
		trigger->trecv.ep = ep;
		trigger->trecv.buf = buf;
		trigger->trecv.len = len;
		trigger->trecv.desc = desc;
		trigger->trecv.src_addr = src_addr;
		trigger->trecv.tag = tag;
		trigger->trecv.ignore = ignore;
		trigger->trecv.context = context;
		trigger->trecv.flags = flags & ~FI_TRIGGER;

		psmx2_cntr_add_trigger(trigger->cntr, trigger);
		return 0;
	}
 
	if (flags & FI_CLAIM) {
		if (!context)
			return -FI_EINVAL;

		/* TODO: handle FI_DISCARD */

		fi_context = context;
		psm2_req = PSMX2_CTXT_REQ(fi_context);
		PSMX2_CTXT_TYPE(fi_context) = PSMX2_TRECV_CONTEXT;
		PSMX2_CTXT_USER(fi_context) = buf;
		PSMX2_CTXT_EP(fi_context) = ep_priv;

		err = psm2_mq_imrecv(ep_priv->domain->psm2_mq, 0,
				     buf, len, context, &psm2_req);
		if (err != PSM2_OK)
			return psmx2_errno(err);

		PSMX2_CTXT_REQ(fi_context) = psm2_req;
		return 0;
	}

	if (ep_priv->recv_selective_completion && !(flags & FI_COMPLETION)) {
		fi_context = psmx2_ep_get_op_context(ep_priv);
		PSMX2_CTXT_TYPE(fi_context) = PSMX2_NOCOMP_RECV_CONTEXT_ALLOC;
		PSMX2_CTXT_EP(fi_context) = ep_priv;
		PSMX2_CTXT_USER(fi_context) = buf;
		PSMX2_CTXT_SIZE(fi_context) = len;
	} else {
		if (!context)
			return -FI_EINVAL;

		fi_context = context;
		PSMX2_CTXT_TYPE(fi_context) = PSMX2_TRECV_CONTEXT;
		PSMX2_CTXT_USER(fi_context) = buf;
		PSMX2_CTXT_EP(fi_context) = ep_priv;
		PSMX2_CTXT_SIZE(fi_context) = len;
	}

	if ((ep_priv->caps & FI_DIRECTED_RECV) && src_addr != FI_ADDR_UNSPEC) {
		av = ep_priv->av;
		if (av && av->type == FI_AV_TABLE) {
			idx = (size_t)src_addr;
			if (idx >= av->last)
				return -FI_EINVAL;

			psm2_epaddr = av->epaddrs[idx];
			vlane = av->vlanes[idx];
		} else {
			psm2_epaddr = PSMX2_ADDR_TO_EP(src_addr);
			vlane = PSMX2_ADDR_TO_VL(src_addr);
		}
		tag32 = PSMX2_TAG32(0, vlane, ep_priv->vlane);
		tagsel32 = ~PSMX2_IOV_BIT;
	} else {
		psm2_epaddr = 0;
		tag32 = PSMX2_TAG32(0, 0, ep_priv->vlane);
		tagsel32 = ~(PSMX2_IOV_BIT | PSMX2_SRC_BITS);
	}

	PSMX2_SET_TAG(psm2_tag, tag, tag32);
	PSMX2_SET_TAG(psm2_tagsel, ~ignore, tagsel32);

	err = psm2_mq_irecv2(ep_priv->domain->psm2_mq, psm2_epaddr,
			     &psm2_tag, &psm2_tagsel, 0, buf, len,
			     (void *)fi_context, &psm2_req);

	if (err != PSM2_OK)
		return psmx2_errno(err);

	if (fi_context == context)
		PSMX2_CTXT_REQ(fi_context) = psm2_req;

	return 0;
}


/* op_flags=0, FI_SELECTIVE_COMPLETION not set, FI_AV_MAP */
static ssize_t
psmx2_tagged_recv_no_flag_av_map(struct fid_ep *ep, void *buf, size_t len,
				 void *desc, fi_addr_t src_addr,
				 uint64_t tag, uint64_t ignore,
				 void *context)
{
	struct psmx2_fid_ep *ep_priv;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag, psm2_tagsel;
	uint32_t tag32, tagsel32;	
	struct fi_context *fi_context;
	int err;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	fi_context = context;
	PSMX2_CTXT_TYPE(fi_context) = PSMX2_TRECV_CONTEXT;
	PSMX2_CTXT_USER(fi_context) = buf;
	PSMX2_CTXT_EP(fi_context) = ep_priv;
	PSMX2_CTXT_SIZE(fi_context) = len;

	if ((ep_priv->caps & FI_DIRECTED_RECV) && src_addr != FI_ADDR_UNSPEC) {
		psm2_epaddr = PSMX2_ADDR_TO_EP(src_addr);
		vlane = PSMX2_ADDR_TO_VL(src_addr);
		tag32 = PSMX2_TAG32(0, vlane, ep_priv->vlane);
		tagsel32 = ~PSMX2_IOV_BIT;
	} else {
		psm2_epaddr = 0;
		tag32 = PSMX2_TAG32(0, 0, ep_priv->vlane);
		tagsel32 = ~(PSMX2_IOV_BIT | PSMX2_SRC_BITS);
	}

	PSMX2_SET_TAG(psm2_tag, tag, tag32);
	PSMX2_SET_TAG(psm2_tagsel, ~ignore, tagsel32);

	err = psm2_mq_irecv2(ep_priv->domain->psm2_mq, psm2_epaddr,
			     &psm2_tag, &psm2_tagsel, 0, buf, len,
			     (void *)fi_context, &psm2_req);
	if (err != PSM2_OK)
		return psmx2_errno(err);

	PSMX2_CTXT_REQ(fi_context) = psm2_req;
	return 0;
}

/* op_flags=0, FI_SELECTIVE_COMPLETION not set, FI_AV_TABLE */
static ssize_t
psmx2_tagged_recv_no_flag_av_table(struct fid_ep *ep, void *buf, size_t len,
				   void *desc, fi_addr_t src_addr,
				   uint64_t tag, uint64_t ignore,
				   void *context)
{
	struct psmx2_fid_ep *ep_priv;
	struct psmx2_fid_av *av;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag, psm2_tagsel;
	uint32_t tag32, tagsel32;
	struct fi_context *fi_context;
	size_t idx;
	int err;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	fi_context = context;
	PSMX2_CTXT_TYPE(fi_context) = PSMX2_TRECV_CONTEXT;
	PSMX2_CTXT_USER(fi_context) = buf;
	PSMX2_CTXT_EP(fi_context) = ep_priv;
	PSMX2_CTXT_SIZE(fi_context) = len;

	if ((ep_priv->caps & FI_DIRECTED_RECV) && src_addr != FI_ADDR_UNSPEC) {
		av = ep_priv->av;
		idx = (size_t)src_addr;
		if (idx >= av->last)
			return -FI_EINVAL;

		psm2_epaddr = av->epaddrs[idx];
		vlane = av->vlanes[idx];
		tag32 = PSMX2_TAG32(0, vlane, ep_priv->vlane);
		tagsel32 = ~PSMX2_IOV_BIT;
	} else {
		psm2_epaddr = 0;
		tag32 = PSMX2_TAG32(0, 0, ep_priv->vlane);
		tagsel32 = ~(PSMX2_IOV_BIT | PSMX2_SRC_BITS);
	}

	PSMX2_SET_TAG(psm2_tag, tag, tag32);
	PSMX2_SET_TAG(psm2_tagsel, ~ignore, tagsel32);

	err = psm2_mq_irecv2(ep_priv->domain->psm2_mq, psm2_epaddr,
			     &psm2_tag, &psm2_tagsel, 0, buf, len,
			     (void *)fi_context, &psm2_req);
	if (err != PSM2_OK)
		return psmx2_errno(err);

	PSMX2_CTXT_REQ(fi_context) = psm2_req;
	return 0;
}

/* op_flags=0, FI_SELECTIVE_COMPLETION set, FI_AV_MAP */
static ssize_t
psmx2_tagged_recv_no_event_av_map(struct fid_ep *ep, void *buf, size_t len,
				  void *desc, fi_addr_t src_addr,
				  uint64_t tag, uint64_t ignore,
				  void *context)
{
	struct psmx2_fid_ep *ep_priv;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag, psm2_tagsel;
	uint32_t tag32, tagsel32;
	struct fi_context *fi_context;
	int err;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	fi_context = psmx2_ep_get_op_context(ep_priv);
	PSMX2_CTXT_TYPE(fi_context) = PSMX2_NOCOMP_RECV_CONTEXT_ALLOC;
	PSMX2_CTXT_EP(fi_context) = ep_priv;
	PSMX2_CTXT_USER(fi_context) = buf;
	PSMX2_CTXT_SIZE(fi_context) = len;

	if ((ep_priv->caps & FI_DIRECTED_RECV) && src_addr != FI_ADDR_UNSPEC) {
		psm2_epaddr = PSMX2_ADDR_TO_EP(src_addr);
		vlane = PSMX2_ADDR_TO_VL(src_addr);
		tag32 = PSMX2_TAG32(0, vlane, ep_priv->vlane);
		tagsel32 = ~PSMX2_IOV_BIT;
	} else {
		psm2_epaddr = 0;
		tag32 = PSMX2_TAG32(0, 0, ep_priv->vlane);
		tagsel32 = ~(PSMX2_IOV_BIT | PSMX2_SRC_BITS);
	}

	PSMX2_SET_TAG(psm2_tag, tag, tag32);
	PSMX2_SET_TAG(psm2_tagsel, ~ignore, tagsel32);

	err = psm2_mq_irecv2(ep_priv->domain->psm2_mq, psm2_epaddr,
			     &psm2_tag, &psm2_tagsel, 0, buf, len,
			     (void *)fi_context, &psm2_req);

	return psmx2_errno(err);
}

/* op_flags=0, FI_SELECTIVE_COMPLETION set, FI_AV_TABLE */
static ssize_t
psmx2_tagged_recv_no_event_av_table(struct fid_ep *ep, void *buf, size_t len,
				    void *desc, fi_addr_t src_addr,
				    uint64_t tag, uint64_t ignore,
				    void *context)
{
	struct psmx2_fid_ep *ep_priv;
	struct psmx2_fid_av *av;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag, psm2_tagsel;
	uint32_t tag32, tagsel32;
	struct fi_context *fi_context;
	size_t idx;
	int err;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	fi_context = psmx2_ep_get_op_context(ep_priv);
	PSMX2_CTXT_TYPE(fi_context) = PSMX2_NOCOMP_RECV_CONTEXT_ALLOC;
	PSMX2_CTXT_EP(fi_context) = ep_priv;
	PSMX2_CTXT_USER(fi_context) = buf;
	PSMX2_CTXT_SIZE(fi_context) = len;

	if ((ep_priv->caps & FI_DIRECTED_RECV) && src_addr != FI_ADDR_UNSPEC) {
		av = ep_priv->av;
		idx = (size_t)src_addr;
		if (idx >= av->last)
			return -FI_EINVAL;

		psm2_epaddr = av->epaddrs[idx];
		vlane = av->vlanes[idx];
		tag32 = PSMX2_TAG32(0, vlane, ep_priv->vlane);
		tagsel32 = ~PSMX2_IOV_BIT;
	} else {
		psm2_epaddr = 0;
		tag32 = PSMX2_TAG32(0, 0, ep_priv->vlane);
		tagsel32 = ~(PSMX2_IOV_BIT | PSMX2_SRC_BITS);
	}

	PSMX2_SET_TAG(psm2_tag, tag, tag32);
	PSMX2_SET_TAG(psm2_tagsel, ~ignore, tagsel32);

	err = psm2_mq_irecv2(ep_priv->domain->psm2_mq, psm2_epaddr,
			     &psm2_tag, &psm2_tagsel, 0, buf, len,
			     (void *)fi_context, &psm2_req);

	return psmx2_errno(err);
}

static ssize_t psmx2_tagged_recv(struct fid_ep *ep, void *buf, size_t len,
				 void *desc, fi_addr_t src_addr, uint64_t tag,
				 uint64_t ignore, void *context)
{
	struct psmx2_fid_ep *ep_priv;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	return psmx2_tagged_recv_generic(ep, buf, len, desc, src_addr, tag,
					 ignore, context, ep_priv->rx_flags);
}

static ssize_t psmx2_tagged_recvmsg(struct fid_ep *ep,
				    const struct fi_msg_tagged *msg,
				    uint64_t flags)
{
	void *buf;
	size_t len;

	if (!msg || (msg->iov_count && !msg->msg_iov))
		return -FI_EINVAL;

	if (msg->iov_count > 1) {
		return -FI_ENOSYS;
	} else if (msg->iov_count) {
		buf = msg->msg_iov[0].iov_base;
		len = msg->msg_iov[0].iov_len;
	} else {
		buf = NULL;
		len = 0;
	}

	return psmx2_tagged_recv_generic(ep, buf, len,
					 msg->desc ? msg->desc[0] : NULL,
					 msg->addr, msg->tag, msg->ignore,
					 msg->context, flags);
}

#define PSMX2_TAGGED_RECVV_FUNC(suffix)					\
static ssize_t								\
psmx2_tagged_recvv##suffix(struct fid_ep *ep, const struct iovec *iov,	\
			   void **desc, size_t count,			\
			   fi_addr_t src_addr, uint64_t tag,		\
			   uint64_t ignore, void *context)		\
{									\
	void *buf;							\
	size_t len;							\
	if (count && !iov)						\
		return -FI_EINVAL;					\
	if (count > 1)							\
		return -FI_ENOSYS;					\
	if (count) {							\
		buf = iov[0].iov_base;					\
		len = iov[0].iov_len;					\
	} else {							\
		buf = NULL;						\
		len = 0;						\
	}								\
	return psmx2_tagged_recv##suffix(ep, buf, len,			\
					 desc ? desc[0] : NULL,		\
				 	 src_addr, tag, ignore, 	\
					 context); 			\
}

PSMX2_TAGGED_RECVV_FUNC()
PSMX2_TAGGED_RECVV_FUNC(_no_flag_av_map)
PSMX2_TAGGED_RECVV_FUNC(_no_flag_av_table)
PSMX2_TAGGED_RECVV_FUNC(_no_event_av_map)
PSMX2_TAGGED_RECVV_FUNC(_no_event_av_table)


ssize_t psmx2_tagged_send_generic(struct fid_ep *ep,
				  const void *buf, size_t len,
				  void *desc, fi_addr_t dest_addr,
				  uint64_t tag, void *context,
				  uint64_t flags, uint64_t data)
{
	struct psmx2_fid_ep *ep_priv;
	struct psmx2_fid_av *av;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag;
	uint32_t tag32;
	struct fi_context *fi_context;
	size_t idx;
	int err;
	int no_completion = 0;
	struct psmx2_cq_event *event;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	if (flags & FI_TRIGGER) {
		struct psmx2_trigger *trigger;
		struct fi_triggered_context *ctxt = context;

		trigger = calloc(1, sizeof(*trigger));
		if (!trigger)
			return -FI_ENOMEM;

		trigger->op = PSMX2_TRIGGERED_TSEND;
		trigger->cntr = container_of(ctxt->trigger.threshold.cntr,
					     struct psmx2_fid_cntr, cntr);
		trigger->threshold = ctxt->trigger.threshold.threshold;
		trigger->tsend.ep = ep;
		trigger->tsend.buf = buf;
		trigger->tsend.len = len;
		trigger->tsend.desc = desc;
		trigger->tsend.dest_addr = dest_addr;
		trigger->tsend.tag = tag;
		trigger->tsend.context = context;
		trigger->tsend.flags = flags & ~FI_TRIGGER;
		trigger->tsend.data = data;

		psmx2_cntr_add_trigger(trigger->cntr, trigger);
		return 0;
	}

	av = ep_priv->av;
	if (av && av->type == FI_AV_TABLE) {
		idx = (size_t)dest_addr;
		if (idx >= av->last)
			return -FI_EINVAL;

		psm2_epaddr = av->epaddrs[idx];
		vlane = av->vlanes[idx];
	} else {
		psm2_epaddr = PSMX2_ADDR_TO_EP(dest_addr);
		vlane = PSMX2_ADDR_TO_VL(dest_addr);
	}

	tag32 = PSMX2_TAG32(0, ep_priv->vlane, vlane);
	PSMX2_SET_TAG(psm2_tag, tag, tag32);

	if ((flags & PSMX2_NO_COMPLETION) ||
	    (ep_priv->send_selective_completion && !(flags & FI_COMPLETION)))
		no_completion = 1;

	if (flags & FI_INJECT) {
		if (len > PSMX2_INJECT_SIZE)
			return -FI_EMSGSIZE;

		err = psm2_mq_send2(ep_priv->domain->psm2_mq, psm2_epaddr,
				    0, &psm2_tag, buf, len);

		if (err != PSM2_OK)
			return psmx2_errno(err);

		if (ep_priv->send_cntr)
			psmx2_cntr_inc(ep_priv->send_cntr);

		if (ep_priv->send_cq && !no_completion) {
			event = psmx2_cq_create_event(
					ep_priv->send_cq,
					context, (void *)buf, flags, len,
					(uint64_t) data, tag,
					0 /* olen */,
					0 /* err */);

			if (event)
				psmx2_cq_enqueue_event(ep_priv->send_cq, event);
			else
				return -FI_ENOMEM;
		}

		return 0;
	}

	if (no_completion && !context) {
		fi_context = &ep_priv->nocomp_send_context;
	} else {
		if (!context)
			return -FI_EINVAL;

		fi_context = context;
		PSMX2_CTXT_TYPE(fi_context) = PSMX2_TSEND_CONTEXT;
		PSMX2_CTXT_USER(fi_context) = (void *)buf;
		PSMX2_CTXT_EP(fi_context) = ep_priv;
	}

	err = psm2_mq_isend2(ep_priv->domain->psm2_mq, psm2_epaddr, 0,
			     &psm2_tag, buf, len, (void*)fi_context,
			     &psm2_req);

	if (err != PSM2_OK)
		return psmx2_errno(err);

	if (fi_context == context)
		PSMX2_CTXT_REQ(fi_context) = psm2_req;

	return 0;
}

/* op_flags=0, FI_SELECTIVE_COMPLETION not set, FI_AV_MAP */
static ssize_t
psmx2_tagged_send_no_flag_av_map(struct fid_ep *ep, const void *buf,
				 size_t len, void *desc,
				 fi_addr_t dest_addr, uint64_t tag,
				 void *context)
{
	struct psmx2_fid_ep *ep_priv;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag;
	uint32_t tag32;
	struct fi_context *fi_context;
	int err;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	psm2_epaddr = PSMX2_ADDR_TO_EP(dest_addr);
	vlane = PSMX2_ADDR_TO_VL(dest_addr);
	tag32 = PSMX2_TAG32(0, ep_priv->vlane, vlane);
	PSMX2_SET_TAG(psm2_tag, tag, tag32);

	fi_context = context;
	PSMX2_CTXT_TYPE(fi_context) = PSMX2_TSEND_CONTEXT;
	PSMX2_CTXT_USER(fi_context) = (void *)buf;
	PSMX2_CTXT_EP(fi_context) = ep_priv;

	err = psm2_mq_isend2(ep_priv->domain->psm2_mq, psm2_epaddr, 0,
			     &psm2_tag, buf, len, (void*)fi_context,
			     &psm2_req);

	if (err != PSM2_OK)
		return psmx2_errno(err);

	PSMX2_CTXT_REQ(fi_context) = psm2_req;
	return 0;
}

/* op_flags=0, FI_SELECTIVE_COMPLETION not set, FI_AV_TABLE */
static ssize_t
psmx2_tagged_send_no_flag_av_table(struct fid_ep *ep, const void *buf,
				   size_t len, void *desc,
				   fi_addr_t dest_addr, uint64_t tag,
				   void *context)
{
	struct psmx2_fid_ep *ep_priv;
	struct psmx2_fid_av *av;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag;
	uint32_t tag32;
	struct fi_context *fi_context;
	size_t idx;
	int err;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	av = ep_priv->av;
	idx = (size_t)dest_addr;
	if (idx >= av->last)
		return -FI_EINVAL;

	psm2_epaddr = av->epaddrs[idx];
	vlane = av->vlanes[idx];
	tag32 = PSMX2_TAG32(0, ep_priv->vlane, vlane);
	PSMX2_SET_TAG(psm2_tag, tag, tag32);

	fi_context = context;
	PSMX2_CTXT_TYPE(fi_context) = PSMX2_TSEND_CONTEXT;
	PSMX2_CTXT_USER(fi_context) = (void *)buf;
	PSMX2_CTXT_EP(fi_context) = ep_priv;

	err = psm2_mq_isend2(ep_priv->domain->psm2_mq, psm2_epaddr, 0,
			     &psm2_tag, buf, len, (void*)fi_context,
			     &psm2_req);

	if (err != PSM2_OK)
		return psmx2_errno(err);

	PSMX2_CTXT_REQ(fi_context) = psm2_req;
	return 0;
}

/* op_flags=0, FI_SELECTIVE_COMPLETION set, FI_AV_MAP */
static ssize_t
psmx2_tagged_send_no_event_av_map(struct fid_ep *ep, const void *buf,
				  size_t len, void *desc,
				  fi_addr_t dest_addr, uint64_t tag,
			          void *context)
{
	struct psmx2_fid_ep *ep_priv;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag;
	uint32_t tag32;
	struct fi_context *fi_context;
	int err;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	psm2_epaddr = PSMX2_ADDR_TO_EP(dest_addr);
	vlane = PSMX2_ADDR_TO_VL(dest_addr);
	tag32 = PSMX2_TAG32(0, ep_priv->vlane, vlane);
	PSMX2_SET_TAG(psm2_tag, tag, tag32);

	fi_context = &ep_priv->nocomp_send_context;

	err = psm2_mq_isend2(ep_priv->domain->psm2_mq, psm2_epaddr, 0,
			     &psm2_tag, buf, len, (void*)fi_context,
			     &psm2_req);

	if (err != PSM2_OK)
		return psmx2_errno(err);

	return 0;
}

/* op_flags=0, FI_SELECTIVE_COMPLETION set, FI_AV_TABLE */
static ssize_t
psmx2_tagged_send_no_event_av_table(struct fid_ep *ep, const void *buf,
				    size_t len, void *desc,
				    fi_addr_t dest_addr, uint64_t tag,
				    void *context)
{
	struct psmx2_fid_ep *ep_priv;
	struct psmx2_fid_av *av;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag;
	uint32_t tag32;
	struct fi_context *fi_context;
	size_t idx;
	int err;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	av = ep_priv->av;
	idx = (size_t)dest_addr;
	if (idx >= av->last)
		return -FI_EINVAL;

	psm2_epaddr = av->epaddrs[idx];
	vlane = av->vlanes[idx];
	tag32 = PSMX2_TAG32(0, ep_priv->vlane, vlane);
	PSMX2_SET_TAG(psm2_tag, tag, tag32);

	fi_context = &ep_priv->nocomp_send_context;

	err = psm2_mq_isend2(ep_priv->domain->psm2_mq, psm2_epaddr, 0,
			     &psm2_tag, buf, len, (void*)fi_context,
			     &psm2_req);

	if (err != PSM2_OK)
		return psmx2_errno(err);

	return 0;
}

/* op_flags=0, FI_AV_MAP */
static ssize_t
psmx2_tagged_inject_no_flag_av_map(struct fid_ep *ep, const void *buf,
				   size_t len, fi_addr_t dest_addr,
				   uint64_t tag)
{
	struct psmx2_fid_ep *ep_priv;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_tag_t psm2_tag;
	uint32_t tag32;
	int err;

	if (len > PSMX2_INJECT_SIZE)
		return -FI_EMSGSIZE;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	psm2_epaddr = PSMX2_ADDR_TO_EP(dest_addr);
	vlane = PSMX2_ADDR_TO_VL(dest_addr);
	tag32 = PSMX2_TAG32(0, ep_priv->vlane, vlane);
	PSMX2_SET_TAG(psm2_tag, tag, tag32);

	err = psm2_mq_send2(ep_priv->domain->psm2_mq, psm2_epaddr, 0,
			    &psm2_tag, buf, len);

	if (err != PSM2_OK)
		return psmx2_errno(err);

	if (ep_priv->send_cntr)
		psmx2_cntr_inc(ep_priv->send_cntr);

	return 0;
}

/* op_flags=0, FI_AV_TABLE */
static ssize_t
psmx2_tagged_inject_no_flag_av_table(struct fid_ep *ep, const void *buf,
				     size_t len, fi_addr_t dest_addr,
				     uint64_t tag)
{
	struct psmx2_fid_ep *ep_priv;
	struct psmx2_fid_av *av;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_tag_t psm2_tag;
	uint32_t tag32;
	int err;
	size_t idx;

	if (len > PSMX2_INJECT_SIZE)
		return -FI_EMSGSIZE;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	av = ep_priv->av;
	idx = (size_t)dest_addr;
	if (idx >= av->last)
		return -FI_EINVAL;

	psm2_epaddr = av->epaddrs[idx];
	vlane = av->vlanes[idx];
	tag32 = PSMX2_TAG32(0, ep_priv->vlane, vlane);
	PSMX2_SET_TAG(psm2_tag, tag, tag32);

	err = psm2_mq_send2(ep_priv->domain->psm2_mq, psm2_epaddr, 0,
			    &psm2_tag, buf, len);

	if (err != PSM2_OK)
		return psmx2_errno(err);

	if (ep_priv->send_cntr)
		psmx2_cntr_inc(ep_priv->send_cntr);

	return 0;
}

ssize_t psmx2_tagged_sendv_generic(struct fid_ep *ep,
				   const struct iovec *iov, void *desc,
				   size_t count, fi_addr_t dest_addr,
				   uint64_t tag, void *context,
				   uint64_t flags, uint64_t data)
{
	struct psmx2_fid_ep *ep_priv;
	struct psmx2_fid_av *av;
	psm2_epaddr_t psm2_epaddr;
	uint8_t vlane;
	psm2_mq_req_t psm2_req;
	psm2_mq_tag_t psm2_tag;
	uint32_t tag32, tag32_base;
	struct fi_context * fi_context;
	int send_flag = 0;
	int err;
	size_t idx;
	int no_completion = 0;
	struct psmx2_cq_event *event;
	size_t real_count;
	size_t len, total_len;
	char *p;
	uint32_t *q;
	int i;
	struct psmx2_sendv_request *req;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	if (flags & FI_TRIGGER) {
		struct psmx2_trigger *trigger;
		struct fi_triggered_context *ctxt = context;

		trigger = calloc(1, sizeof(*trigger));
		if (!trigger)
			return -FI_ENOMEM;

		trigger->op = PSMX2_TRIGGERED_TSENDV;
		trigger->cntr = container_of(ctxt->trigger.threshold.cntr,
					     struct psmx2_fid_cntr, cntr);
		trigger->threshold = ctxt->trigger.threshold.threshold;
		trigger->tsendv.ep = ep;
		trigger->tsendv.iov = iov;
		trigger->tsendv.desc = desc;
		trigger->tsendv.count = count;
		trigger->tsendv.dest_addr = dest_addr;
		trigger->tsendv.tag = tag;
		trigger->tsendv.context = context;
		trigger->tsendv.flags = flags & ~FI_TRIGGER;
		trigger->tsendv.data = data;

		psmx2_cntr_add_trigger(trigger->cntr, trigger);
		return 0;
	}

	total_len = 0;
	real_count = 0;
	for (i=0; i<count; i++) {
		if (iov[i].iov_len) {
			total_len += iov[i].iov_len;
			real_count++;
		}
	}

	req = malloc(sizeof(*req));
	if (!req)
		return -FI_ENOMEM;

	if (total_len <= PSMX2_IOV_BUF_SIZE) {
		req->iov_protocol = PSMX2_IOV_PROTO_PACK;
		p = req->buf;
		for (i=0; i<count; i++) {
			if (iov[i].iov_len) {
				memcpy(p, iov[i].iov_base, iov[i].iov_len);
				p += iov[i].iov_len;
			}
		}

		tag32_base = 0;
		len = total_len;
	} else {
		req->iov_protocol = PSMX2_IOV_PROTO_MULTI;
		req->iov_done = 0;
		req->iov_info.seq_num = (++ep_priv->iov_seq_num) %
					PSMX2_IOV_MAX_SEQ_NUM + 1;
		req->iov_info.count = (uint32_t)real_count;
		req->iov_info.total_len = (uint32_t)total_len;

		q = req->iov_info.len;
		for (i=0; i<count; i++) {
			if (iov[i].iov_len)
				*q++ = (uint32_t)iov[i].iov_len;
		}

		tag32_base = PSMX2_IOV_BIT;
		len = (3 + real_count) * sizeof(uint32_t);
	}

	av = ep_priv->av;
	if (av && av->type == FI_AV_TABLE) {
		idx = (size_t)dest_addr;
		if (idx >= av->last) {
			free(req);
			return -FI_EINVAL;
		}

		psm2_epaddr = av->epaddrs[idx];
		vlane = av->vlanes[idx];
	} else  {
		psm2_epaddr = PSMX2_ADDR_TO_EP(dest_addr);
		vlane = PSMX2_ADDR_TO_VL(dest_addr);
	}

	tag32 = PSMX2_TAG32(tag32_base, ep_priv->vlane, vlane);
	PSMX2_SET_TAG(psm2_tag, tag, tag32);

	if ((flags & PSMX2_NO_COMPLETION) ||
	    (ep_priv->send_selective_completion && !(flags & FI_COMPLETION)))
		no_completion = 1;

	if (flags & FI_INJECT) {
		if (len > PSMX2_INJECT_SIZE) {
			free(req);
			return -FI_EMSGSIZE;
		}

		err = psm2_mq_send2(ep_priv->domain->psm2_mq, psm2_epaddr,
				    send_flag, &psm2_tag, req->buf, len);

		free(req);

		if (err != PSM2_OK)
			return psmx2_errno(err);

		if (ep_priv->send_cntr)
			psmx2_cntr_inc(ep_priv->send_cntr);

		if (ep_priv->send_cq && !no_completion) {
			event = psmx2_cq_create_event(
					ep_priv->send_cq,
					context, NULL, flags, len,
					(uint64_t) data,
					0 /* tag */,
					0 /* olen */,
					0 /* err */);

			if (event)
				psmx2_cq_enqueue_event(ep_priv->send_cq, event);
			else
				return -FI_ENOMEM;
		}

		return 0;
	}

	req->no_completion = no_completion;
	req->user_context = context;
	req->comp_flag = FI_TAGGED;

	fi_context = &req->fi_context;
	PSMX2_CTXT_TYPE(fi_context) = PSMX2_SENDV_CONTEXT;
	PSMX2_CTXT_USER(fi_context) = req;
	PSMX2_CTXT_EP(fi_context) = ep_priv;

	err = psm2_mq_isend2(ep_priv->domain->psm2_mq, psm2_epaddr,
			     send_flag, &psm2_tag, req->buf, len,
			     (void *)fi_context, &psm2_req);

	if (err != PSM2_OK) {
		free(req);
		return psmx2_errno(err);
	}

	PSMX2_CTXT_REQ(fi_context) = psm2_req;

	if (req->iov_protocol == PSMX2_IOV_PROTO_MULTI) {
		fi_context = &req->fi_context_iov;
		PSMX2_CTXT_TYPE(fi_context) = PSMX2_IOV_SEND_CONTEXT;
		PSMX2_CTXT_USER(fi_context) = req;
		PSMX2_CTXT_EP(fi_context) = ep_priv;
		tag32 = PSMX2_TAG32(0, ep_priv->vlane, vlane);
		PSMX2_TAG32_SET_SEQ(tag32, req->iov_info.seq_num);
		PSMX2_SET_TAG(psm2_tag, tag, tag32);
		for (i=0; i<count; i++) {
			if (iov[i].iov_len) {
				err = psm2_mq_isend2(ep_priv->domain->psm2_mq,
						     psm2_epaddr, send_flag, &psm2_tag,
						     iov[i].iov_base, iov[i].iov_len,
						     (void *)fi_context, &psm2_req);
				if (err != PSM2_OK)
					return psmx2_errno(err);
			}
		}
	}

	return 0;
}

static ssize_t psmx2_tagged_send(struct fid_ep *ep,
				 const void *buf, size_t len,
				 void *desc, fi_addr_t dest_addr,
				 uint64_t tag, void *context)
{
	struct psmx2_fid_ep *ep_priv;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	return psmx2_tagged_send_generic(ep, buf, len, desc, dest_addr,
					 tag, context, ep_priv->tx_flags, 0);
}

static ssize_t psmx2_tagged_sendmsg(struct fid_ep *ep,
				    const struct fi_msg_tagged *msg,
				    uint64_t flags)
{
	void *buf;
	size_t len;

	if (!msg || (msg->iov_count && !msg->msg_iov))
		return -FI_EINVAL;

	if (msg->iov_count > 1) {
		return psmx2_tagged_sendv_generic(ep, msg->msg_iov,
						  msg->desc, msg->iov_count,
						  msg->addr, msg->tag,
						  msg->context, flags,
						  msg->data);
	} else if (msg->iov_count) {
		buf = msg->msg_iov[0].iov_base;
		len = msg->msg_iov[0].iov_len;
	} else {
		buf = NULL;
		len = 0;
	}

	return psmx2_tagged_send_generic(ep, buf, len,
					 msg->desc ? msg->desc[0] : NULL,
					 msg->addr, msg->tag, msg->context,
					 flags, msg->data);
}

#define PSMX2_TAGGED_SENDV_FUNC(suffix)					\
static ssize_t								\
psmx2_tagged_sendv##suffix(struct fid_ep *ep, const struct iovec *iov,	\
			   void **desc,size_t count,			\
			   fi_addr_t dest_addr,	uint64_t tag,		\
			   void *context)				\
{									\
	void *buf;							\
	size_t len;							\
	if (count && !iov)						\
		return -FI_EINVAL;					\
	if (count > PSMX2_IOV_MAX_COUNT) {				\
		return -FI_EINVAL;					\
	} else if (count > 1) {						\
		struct psmx2_fid_ep *ep_priv;				\
		ep_priv = container_of(ep, struct psmx2_fid_ep, ep);	\
		return psmx2_tagged_sendv_generic(ep, iov, desc, count, \
						  dest_addr, tag,	\
						  context,		\
						  ep_priv->tx_flags, 0);\
	} else if (count) {						\
		buf = iov[0].iov_base;					\
		len = iov[0].iov_len;					\
	} else {							\
		buf = NULL;						\
		len = 0;						\
	}								\
	return psmx2_tagged_send##suffix(ep, buf, len,			\
					 desc ? desc[0] : NULL,		\
				 	 dest_addr, tag, context);	\
}

PSMX2_TAGGED_SENDV_FUNC()
PSMX2_TAGGED_SENDV_FUNC(_no_flag_av_map)
PSMX2_TAGGED_SENDV_FUNC(_no_flag_av_table)
PSMX2_TAGGED_SENDV_FUNC(_no_event_av_map)
PSMX2_TAGGED_SENDV_FUNC(_no_event_av_table)

static ssize_t psmx2_tagged_inject(struct fid_ep *ep,
				   const void *buf, size_t len,
				   fi_addr_t dest_addr, uint64_t tag)
{
	struct psmx2_fid_ep *ep_priv;

	ep_priv = container_of(ep, struct psmx2_fid_ep, ep);

	return psmx2_tagged_send_generic(ep, buf, len, NULL, dest_addr,
					 tag, NULL,
				  	 ep_priv->tx_flags | FI_INJECT | PSMX2_NO_COMPLETION,
					 0);
}

#define PSMX2_TAGGED_OPS(suffix,sendopt,recvopt,injopt)	\
struct fi_ops_tagged psmx2_tagged_ops##suffix = {	\
	.size = sizeof(struct fi_ops_tagged),		\
	.recv = psmx2_tagged_recv##recvopt,		\
	.recvv = psmx2_tagged_recvv##recvopt,		\
	.recvmsg = psmx2_tagged_recvmsg,		\
	.send = psmx2_tagged_send##sendopt,		\
	.sendv = psmx2_tagged_sendv##sendopt,		\
	.sendmsg = psmx2_tagged_sendmsg,		\
	.inject = psmx2_tagged_inject##injopt,		\
	.senddata = fi_no_tagged_senddata,		\
	.injectdata = fi_no_tagged_injectdata,		\
};

PSMX2_TAGGED_OPS(,,,)
PSMX2_TAGGED_OPS(_no_flag_av_map, _no_flag_av_map, _no_flag_av_map, _no_flag_av_map)
PSMX2_TAGGED_OPS(_no_flag_av_table, _no_flag_av_table, _no_flag_av_table, _no_flag_av_table)
PSMX2_TAGGED_OPS(_no_event_av_map, _no_event_av_map, _no_event_av_map, _no_flag_av_map)
PSMX2_TAGGED_OPS(_no_event_av_table, _no_event_av_table, _no_event_av_table, _no_flag_av_table)
PSMX2_TAGGED_OPS(_no_send_event_av_map, _no_event_av_map, _no_flag_av_map, _no_flag_av_map)
PSMX2_TAGGED_OPS(_no_send_event_av_table, _no_event_av_table, _no_flag_av_table, _no_flag_av_table)
PSMX2_TAGGED_OPS(_no_recv_event_av_map, _no_flag_av_map, _no_event_av_map, _no_flag_av_map)
PSMX2_TAGGED_OPS(_no_recv_event_av_table, _no_flag_av_table, _no_event_av_table, _no_flag_av_table)

