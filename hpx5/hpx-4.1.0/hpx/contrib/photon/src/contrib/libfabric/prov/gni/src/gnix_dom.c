/*
 * Copyright (c) 2015-2016 Los Alamos National Security, LLC.
 *                         All rights reserved.
 * Copyright (c) 2015-2016 Cray Inc. All rights reserved.
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

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <assert.h>

#include "gnix.h"
#include "gnix_nic.h"
#include "gnix_util.h"
#include "gnix_xpmem.h"

gni_cq_mode_t gnix_def_gni_cq_modes = GNI_CQ_PHYS_PAGES;

static char *__gnix_mr_type_to_str[GNIX_MR_MAX_TYPE] = {
		[GNIX_MR_TYPE_INTERNAL] = "internal",
		[GNIX_MR_TYPE_UDREG] = "udreg",
		[GNIX_MR_TYPE_NONE] = "none",
};

/*******************************************************************************
 * Forward declaration for ops structures.
 ******************************************************************************/

static struct fi_ops gnix_stx_ops;
static struct fi_ops gnix_domain_fi_ops;
static struct fi_ops_mr gnix_domain_mr_ops;
static struct fi_ops_domain gnix_domain_ops;

static void __domain_destruct(void *obj)
{
	int ret = FI_SUCCESS;
	struct gnix_nic *p, *next;
	struct gnix_fid_domain *domain = (struct gnix_fid_domain *) obj;

	GNIX_TRACE(FI_LOG_DOMAIN, "\n");

	ret = _gnix_close_cache(domain);
	if (ret != FI_SUCCESS)
		GNIX_FATAL(FI_LOG_MR, "failed to close memory registration cache\n");
	/*
	 * Drop a reference to each NIC used by this domain.
	 */

	dlist_for_each_safe(&domain->nic_list, p, next, dom_nic_list) {
		dlist_remove(&p->dom_nic_list);
		_gnix_ref_put(p);
	}

	/*
	 * remove from the list of cdms attached to fabric
	 */
	dlist_remove_init(&domain->list);

	_gnix_ref_put(domain->fabric);

	memset(domain, 0, sizeof *domain);
	free(domain);

}

static void __stx_destruct(void *obj)
{
	struct gnix_fid_stx *stx = (struct gnix_fid_stx *) obj;

	GNIX_TRACE(FI_LOG_DOMAIN, "\n");

	memset(stx, 0, sizeof(*stx));
	free(stx);
}

/*******************************************************************************
 * API function implementations.
 ******************************************************************************/

/**
 * Creates a shared transmit context.
 * @note This is basically a no-op for the GNI provider.
 *
 * @param[in]  val  value to be sign extended
 * @param[in]  len  length to sign extend the value
 * @return     FI_SUCCESS if shared tx context successfully created
 * @return     -FI_EINVAL if invalid arg(s) supplied
 * @return     -FI_ENOMEM insufficient memory
 */
DIRECT_FN STATIC int gnix_stx_open(struct fid_domain *dom,
				   struct fi_tx_attr *tx_attr,
				   struct fid_stx **stx, void *context)
{
	int ret = FI_SUCCESS;
	struct gnix_fid_domain *domain;
	struct gnix_fid_stx *stx_priv;

	GNIX_TRACE(FI_LOG_DOMAIN, "\n");

	domain = container_of(dom, struct gnix_fid_domain, domain_fid.fid);
	if (domain->domain_fid.fid.fclass != FI_CLASS_DOMAIN) {
		ret = -FI_EINVAL;
		goto err;
	}

	stx_priv = calloc(1, sizeof(*stx_priv));
	if (!stx_priv) {
		ret = -FI_ENOMEM;
		goto err;
	}

	stx_priv->domain = domain;
	_gnix_ref_init(&stx_priv->ref_cnt, 1, __stx_destruct);

	_gnix_ref_get(stx_priv->domain);

	stx_priv->stx_fid.fid.fclass = FI_CLASS_STX_CTX;
	stx_priv->stx_fid.fid.context = context;
	stx_priv->stx_fid.fid.ops = &gnix_stx_ops;
	stx_priv->stx_fid.ops = NULL;

	*stx = &stx_priv->stx_fid;

err:
	return ret;
}

/**
 * Destroy a shared transmit context.
 *
 * @param[in]  fid  fid for previously allocated gnix_fid_stx
 *                  structure
 * @return     FI_SUCCESS if shared tx context successfully closed
 * @return     -FI_EINVAL if invalid arg(s) supplied
 *
 * @note - the structure will actually not be freed till all
 *         references to the structure have released their references
 *         to the stx structure.
 */
static int gnix_stx_close(fid_t fid)
{
	struct gnix_fid_stx *stx;

	GNIX_TRACE(FI_LOG_DOMAIN, "\n");

	stx = container_of(fid, struct gnix_fid_stx, stx_fid.fid);
	if (stx->stx_fid.fid.fclass != FI_CLASS_STX_CTX)
		return -FI_EINVAL;

	_gnix_ref_put(stx->domain);
	_gnix_ref_put(stx);

	return FI_SUCCESS;
}

static int gnix_domain_close(fid_t fid)
{
	int ret = FI_SUCCESS, references_held;
	struct gnix_fid_domain *domain;

	GNIX_TRACE(FI_LOG_DOMAIN, "\n");

	domain = container_of(fid, struct gnix_fid_domain, domain_fid.fid);
	if (domain->domain_fid.fid.fclass != FI_CLASS_DOMAIN) {
		ret = -FI_EINVAL;
		goto err;
	}

	/* before checking the refcnt, flush the memory registration cache */
	if (domain->mr_cache) {
		fastlock_acquire(&domain->mr_cache_lock);
		ret = _gnix_mr_cache_flush(domain->mr_cache);
		if (ret != FI_SUCCESS) {
			GNIX_WARN(FI_LOG_DOMAIN,
				  "failed to flush memory cache on domain close\n");
			fastlock_release(&domain->mr_cache_lock);
			goto err;
		}
		fastlock_release(&domain->mr_cache_lock);
	}

	/*
	 * if non-zero refcnt, there are eps, mrs, and/or an eq associated
	 * with this domain which have not been closed.
	 */

	references_held = _gnix_ref_put(domain);

	if (references_held) {
		GNIX_INFO(FI_LOG_DOMAIN, "failed to fully close domain due to "
			  "lingering references. references=%i dom=%p\n",
			  references_held, domain);
	}

	GNIX_INFO(FI_LOG_DOMAIN, "gnix_domain_close invoked returning %d\n",
		  ret);
err:
	return ret;
}

/*
 * gnix_domain_ops provides a means for an application to better
 * control allocation of underlying aries resources associated with
 * the domain.  Examples will include controlling size of underlying
 * hardware CQ sizes, max size of RX ring buffers, etc.
 */

static const uint32_t default_msg_rendezvous_thresh = 16*1024;
static const uint32_t default_rma_rdma_thresh = 8*1024;
static const uint32_t default_ct_init_size = 64;
static const uint32_t default_ct_max_size = 16384;
static const uint32_t default_ct_step = 2;
static const uint32_t default_vc_id_table_capacity = 128;
static const uint32_t default_mbox_page_size = GNIX_PAGE_2MB;
static const uint32_t default_mbox_num_per_slab = 2048;
static const uint32_t default_mbox_maxcredit = 64;
static const uint32_t default_mbox_msg_maxsize = 16384;
/* rx cq bigger to avoid having to deal with rx overruns so much */
static const uint32_t default_rx_cq_size = 16384;
static const uint32_t default_tx_cq_size = 2048;
static const uint32_t default_max_retransmits = 5;
static const int32_t default_err_inject_count; /* static var is zeroed */

static int __gnix_string_to_mr_type(const char *name)
{
	int i;
	for (i = 0; i < GNIX_MR_MAX_TYPE; i++)
		if (strncmp(name, __gnix_mr_type_to_str[i],
				strlen(__gnix_mr_type_to_str[i])) == 0)
			return i;

	return -1;
}

static int
__gnix_dom_ops_flush_cache(struct fid *fid)
{
	struct gnix_fid_domain *domain;

	GNIX_TRACE(FI_LOG_DOMAIN, "\n");

	domain = container_of(fid, struct gnix_fid_domain, domain_fid.fid);
	if (domain->domain_fid.fid.fclass != FI_CLASS_DOMAIN) {
		GNIX_WARN(FI_LOG_DOMAIN, ("Invalid domain\n"));
		return -FI_EINVAL;
	}

	return _gnix_flush_registration_cache(domain);
}

static int
__gnix_dom_ops_get_val(struct fid *fid, dom_ops_val_t t, void *val)
{
	struct gnix_fid_domain *domain;

	GNIX_TRACE(FI_LOG_DOMAIN, "\n");

	assert(val);

	domain = container_of(fid, struct gnix_fid_domain, domain_fid.fid);
	if (domain->domain_fid.fid.fclass != FI_CLASS_DOMAIN) {
		GNIX_WARN(FI_LOG_DOMAIN, ("Invalid domain\n"));
		return -FI_EINVAL;
	}

	switch (t) {
	case GNI_MSG_RENDEZVOUS_THRESHOLD:
		*(uint32_t *)val = domain->params.msg_rendezvous_thresh;
		break;
	case GNI_RMA_RDMA_THRESHOLD:
		*(uint32_t *)val = domain->params.rma_rdma_thresh;
		break;
	case GNI_CONN_TABLE_INITIAL_SIZE:
		*(uint32_t *)val = domain->params.ct_init_size;
		break;
	case GNI_CONN_TABLE_MAX_SIZE:
		*(uint32_t *)val = domain->params.ct_max_size;
		break;
	case GNI_CONN_TABLE_STEP_SIZE:
		*(uint32_t *)val = domain->params.ct_step;
		break;
	case GNI_VC_ID_TABLE_CAPACITY:
		*(uint32_t *)val = domain->params.vc_id_table_capacity;
		break;
	case GNI_MBOX_PAGE_SIZE:
		*(uint32_t *)val = domain->params.mbox_page_size;
		break;
	case GNI_MBOX_NUM_PER_SLAB:
		*(uint32_t *)val = domain->params.mbox_num_per_slab;
		break;
	case GNI_MBOX_MAX_CREDIT:
		*(uint32_t *)val = domain->params.mbox_maxcredit;
		break;
	case GNI_MBOX_MSG_MAX_SIZE:
		*(uint32_t *)val = domain->params.mbox_msg_maxsize;
		break;
	case GNI_RX_CQ_SIZE:
		*(uint32_t *)val = domain->params.rx_cq_size;
		break;
	case GNI_TX_CQ_SIZE:
		*(uint32_t *)val = domain->params.tx_cq_size;
		break;
	case GNI_MAX_RETRANSMITS:
		*(uint32_t *)val = domain->params.max_retransmits;
		break;
	case GNI_ERR_INJECT_COUNT:
		*(int32_t *)val = domain->params.err_inject_count;
		break;
	case GNI_MR_CACHE_LAZY_DEREG:
		*(int32_t *)val = domain->mr_cache_attr.lazy_deregistration;
		break;
	case GNI_MR_CACHE:
		*(char **) val = __gnix_mr_type_to_str[domain->mr_cache_type];
		break;
	case GNI_MR_UDREG_REG_LIMIT:
		*(int32_t *)val = domain->udreg_reg_limit;
		break;
	case GNI_MR_HARD_REG_LIMIT:
		*(int32_t *)val = domain->mr_cache_attr.hard_reg_limit;
		break;
	case GNI_MR_SOFT_REG_LIMIT:
		*(int32_t *)val = domain->mr_cache_attr.soft_reg_limit;
		break;
	case GNI_MR_HARD_STALE_REG_LIMIT:
		*(int32_t *)val = domain->mr_cache_attr.hard_stale_limit;
		break;
	case GNI_XPMEM_ENABLE:
		*(bool *)val = domain->params.xpmem_enabled;
#if !HAVE_XPMEM
		GNIX_WARN(FI_LOG_DOMAIN,
			  "GNI provider XPMEM support not configured\n");
#endif
		break;
	default:
		GNIX_WARN(FI_LOG_DOMAIN, ("Invalid dom_ops_val\n"));
		return -FI_EINVAL;
	}

	return FI_SUCCESS;
}

static int
__gnix_dom_ops_set_val(struct fid *fid, dom_ops_val_t t, void *val)
{
	gni_return_t grc = GNI_RC_SUCCESS;
	struct gnix_nic *nic;
	struct gnix_fid_domain *domain;
	int ret, type;

	GNIX_TRACE(FI_LOG_DOMAIN, "\n");

	assert(val);

	domain = container_of(fid, struct gnix_fid_domain, domain_fid.fid);
	if (domain->domain_fid.fid.fclass != FI_CLASS_DOMAIN) {
		GNIX_WARN(FI_LOG_DOMAIN, ("Invalid domain\n"));
		return -FI_EINVAL;
	}

	switch (t) {
	case GNI_MSG_RENDEZVOUS_THRESHOLD:
		domain->params.msg_rendezvous_thresh = *(uint32_t *)val;
		break;
	case GNI_RMA_RDMA_THRESHOLD:
		domain->params.rma_rdma_thresh = *(uint32_t *)val;
		break;
	case GNI_CONN_TABLE_INITIAL_SIZE:
		domain->params.ct_init_size = *(uint32_t *)val;
		break;
	case GNI_CONN_TABLE_MAX_SIZE:
		domain->params.ct_max_size = *(uint32_t *)val;
		break;
	case GNI_CONN_TABLE_STEP_SIZE:
		domain->params.ct_step = *(uint32_t *)val;
		break;
	case GNI_VC_ID_TABLE_CAPACITY:
		domain->params.vc_id_table_capacity = *(uint32_t *)val;
		break;
	case GNI_MBOX_PAGE_SIZE:
		domain->params.mbox_page_size = *(uint32_t *)val;
		break;
	case GNI_MBOX_NUM_PER_SLAB:
		domain->params.mbox_num_per_slab = *(uint32_t *)val;
		break;
	case GNI_MBOX_MAX_CREDIT:
		domain->params.mbox_maxcredit = *(uint32_t *)val;
		break;
	case GNI_MBOX_MSG_MAX_SIZE:
		domain->params.mbox_msg_maxsize = *(uint32_t *)val;
		break;
	case GNI_RX_CQ_SIZE:
		domain->params.rx_cq_size = *(uint32_t *)val;
		break;
	case GNI_TX_CQ_SIZE:
		domain->params.tx_cq_size = *(uint32_t *)val;
		break;
	case GNI_MAX_RETRANSMITS:
		domain->params.max_retransmits = *(uint32_t *)val;
		dlist_for_each(&domain->nic_list, nic, dom_nic_list)
		{
			COND_ACQUIRE(nic->requires_lock, &nic->lock);
			grc = GNI_SmsgSetMaxRetrans(nic->gni_nic_hndl,
						    *(uint16_t *)val);
			fastlock_release(&nic->lock);
			if (grc != GNI_RC_SUCCESS)
				break;
		}

		if (unlikely(grc != GNI_RC_SUCCESS)) {
			GNIX_WARN(FI_LOG_DOMAIN, "failed to set smsg max "
				  "retrans, ret=%s\n", gni_err_str[grc]);
			return gnixu_to_fi_errno(grc);
		}
		break;
	case GNI_ERR_INJECT_COUNT:
		domain->params.err_inject_count = *(int32_t *)val;
		break;
	case GNI_MR_CACHE_LAZY_DEREG:
		domain->mr_cache_attr.lazy_deregistration = *(int32_t *)val;
		break;
	case GNI_MR_CACHE:
		if (val != NULL) {
			GNIX_DEBUG(FI_LOG_DOMAIN, "user provided value=%s\n",
					*(char **) val);

			type = __gnix_string_to_mr_type(*(const char **) val);
			if (type < 0 || type >= GNIX_MR_MAX_TYPE)
				return -FI_EINVAL;

			GNIX_DEBUG(FI_LOG_DOMAIN, "setting domain mr type to %s\n",
					__gnix_mr_type_to_str[type]);

			ret = _gnix_open_cache(domain, type);
			if (ret != FI_SUCCESS)
				return -FI_EINVAL;
		}
		break;
	case GNI_MR_HARD_REG_LIMIT:
		domain->mr_cache_attr.hard_reg_limit = *(int32_t *) val;
		break;
	case GNI_MR_SOFT_REG_LIMIT:
		domain->mr_cache_attr.soft_reg_limit = *(int32_t *) val;
		break;
	case GNI_MR_HARD_STALE_REG_LIMIT:
		domain->mr_cache_attr.hard_stale_limit = *(int32_t *) val;
		break;
	case GNI_MR_UDREG_REG_LIMIT:
		if (*(int32_t *) val < 0)
			return -FI_EINVAL;
		domain->udreg_reg_limit = *(int32_t *) val;
		break;
	case GNI_XPMEM_ENABLE:
#if HAVE_XPMEM
		domain->params.xpmem_enabled = *(bool *)val;
#else
		GNIX_WARN(FI_LOG_DOMAIN,
			  "GNI provider XPMEM support not configured\n");
#endif
		break;
	default:
		GNIX_WARN(FI_LOG_DOMAIN, ("Invalid dom_ops_val\n"));
		return -FI_EINVAL;
	}

	return FI_SUCCESS;
}


static struct fi_gni_ops_domain gnix_ops_domain = {
	.set_val = __gnix_dom_ops_set_val,
	.get_val = __gnix_dom_ops_get_val,
	.flush_cache = __gnix_dom_ops_flush_cache,
};

DIRECT_FN int gnix_domain_bind(struct fid_domain *domain, struct fid *fid,
			       uint64_t flags)
{
	return -FI_ENOSYS;
}

static int
gnix_domain_ops_open(struct fid *fid, const char *ops_name, uint64_t flags,
		     void **ops, void *context)
{
	int ret = FI_SUCCESS;

	if (strcmp(ops_name, FI_GNI_DOMAIN_OPS_1) == 0)
		*ops = &gnix_ops_domain;
	else
		ret = -FI_EINVAL;

	return ret;
}

DIRECT_FN int gnix_domain_open(struct fid_fabric *fabric, struct fi_info *info,
			       struct fid_domain **dom, void *context)
{
	struct gnix_fid_domain *domain = NULL;
	int ret = FI_SUCCESS;
	uint8_t ptag;
	uint32_t cookie;
	struct gnix_fid_fabric *fabric_priv;

	GNIX_TRACE(FI_LOG_DOMAIN, "\n");

	fabric_priv = container_of(fabric, struct gnix_fid_fabric, fab_fid);
	if (!info->domain_attr->name ||
	    strncmp(info->domain_attr->name, gnix_dom_name,
		    strlen(gnix_dom_name))) {
		ret = -FI_EINVAL;
		goto err;
	}

	/*
	 * check cookie/ptag credentials - for FI_EP_MSG we may be creating a
	 * domain
	 * using a cookie supplied being used by the server.  Otherwise, we use
	 * use the cookie/ptag supplied by the job launch system.
	 */
	if (info->dest_addr) {
		ret =
		    gnixu_get_rdma_credentials(info->dest_addr, &ptag, &cookie);
		if (ret) {
			GNIX_WARN(FI_LOG_DOMAIN,
				   "gnixu_get_rdma_credentials returned ptag %u cookie 0x%x\n",
				   ptag, cookie);
			goto err;
		}
	} else {
		ret = gnixu_get_rdma_credentials(NULL, &ptag, &cookie);
	}

	GNIX_INFO(FI_LOG_DOMAIN,
		  "gnix rdma credentials returned ptag %u cookie 0x%x\n",
		  ptag, cookie);

	domain = calloc(1, sizeof *domain);
	if (domain == NULL) {
		ret = -FI_ENOMEM;
		goto err;
	}

	domain->mr_cache_attr = _gnix_default_mr_cache_attr;
	domain->mr_cache_attr.reg_context = (void *) domain;
	domain->mr_cache_attr.dereg_context = NULL;
	domain->mr_cache_attr.destruct_context = NULL;
	domain->mr_cache_attr.notifier = &fabric_priv->mr_notifier;
	domain->mr_cache = NULL;
	fastlock_init(&domain->mr_cache_lock);

	domain->udreg_reg_limit = 4096;

	dlist_init(&domain->nic_list);
	dlist_init(&domain->list);

	dlist_insert_after(&domain->list, &fabric_priv->domain_list);

	domain->fabric = fabric_priv;
	_gnix_ref_get(domain->fabric);

	domain->ptag = ptag;
	domain->cookie = cookie;
	domain->cdm_id_seed = getpid();  /* TODO: direct syscall better */

	/* user tunables */
	domain->params.msg_rendezvous_thresh = default_msg_rendezvous_thresh;
	domain->params.rma_rdma_thresh = default_rma_rdma_thresh;
	domain->params.ct_init_size = default_ct_init_size;
	domain->params.ct_max_size = default_ct_max_size;
	domain->params.ct_step = default_ct_step;
	domain->params.vc_id_table_capacity = default_vc_id_table_capacity;
	domain->params.mbox_page_size = default_mbox_page_size;
	domain->params.mbox_num_per_slab = default_mbox_num_per_slab;
	domain->params.mbox_maxcredit = default_mbox_maxcredit;
	domain->params.mbox_msg_maxsize = default_mbox_msg_maxsize;
	domain->params.max_retransmits = default_max_retransmits;
	domain->params.err_inject_count = default_err_inject_count;
#if HAVE_XPMEM
	domain->params.xpmem_enabled = true;
#else
	domain->params.xpmem_enabled = false;
#endif

	domain->gni_tx_cq_size = default_tx_cq_size;
	domain->gni_rx_cq_size = default_rx_cq_size;
	domain->gni_cq_modes = gnix_def_gni_cq_modes;
	_gnix_ref_init(&domain->ref_cnt, 1, __domain_destruct);

	domain->domain_fid.fid.fclass = FI_CLASS_DOMAIN;
	domain->domain_fid.fid.context = context;
	domain->domain_fid.fid.ops = &gnix_domain_fi_ops;
	domain->domain_fid.ops = &gnix_domain_ops;
	domain->domain_fid.mr = &gnix_domain_mr_ops;

	domain->control_progress = info->domain_attr->control_progress;
	domain->data_progress = info->domain_attr->data_progress;
	domain->thread_model = info->domain_attr->threading;
	domain->mr_is_init = 0;

	fastlock_init(&domain->cm_nic_lock);

	_gnix_open_cache(domain, GNIX_DEFAULT_CACHE_TYPE);

	*dom = &domain->domain_fid;
	return FI_SUCCESS;

err:
	if (domain != NULL) {
		free(domain);
	}
	return ret;
}

DIRECT_FN int gnix_srx_context(struct fid_domain *domain,
			       struct fi_rx_attr *attr,
			       struct fid_ep **rx_ep, void *context)
{
	return -FI_ENOSYS;
}

DIRECT_FN int gnix_scalable_ep_open(struct fid_domain *domain,
				    struct fi_info *info,
				    struct fid_ep **sep, void *context)
{
	return -FI_ENOSYS;
}

DIRECT_FN int gnix_scalable_ep_bind(fid_t fid, struct fid *bfid, uint64_t flags)
{
	return -FI_ENOSYS;
}

/*******************************************************************************
 * FI_OPS_* data structures.
 ******************************************************************************/

static struct fi_ops gnix_stx_ops = {
	.close = gnix_stx_close,
	.bind = fi_no_bind,
	.control = fi_no_control,
	.ops_open = fi_no_ops_open
};

static struct fi_ops gnix_domain_fi_ops = {
	.size = sizeof(struct fi_ops),
	.close = gnix_domain_close,
	.bind = fi_no_bind,
	.control = fi_no_control,
	.ops_open = gnix_domain_ops_open
};

static struct fi_ops_mr gnix_domain_mr_ops = {
	.size = sizeof(struct fi_ops_mr),
	.reg = gnix_mr_reg,
	.regv = fi_no_mr_regv,
	.regattr = fi_no_mr_regattr
};

static struct fi_ops_domain gnix_domain_ops = {
	.size = sizeof(struct fi_ops_domain),
	.av_open = gnix_av_open,
	.cq_open = gnix_cq_open,
	.endpoint = gnix_ep_open,
	.scalable_ep = fi_no_scalable_ep,
	.cntr_open = gnix_cntr_open,
	.poll_open = fi_no_poll_open,
	.stx_ctx = gnix_stx_open,
	.srx_ctx = fi_no_srx_context
};
