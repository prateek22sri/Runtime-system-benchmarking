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

static int psmx2_cm_getname(fid_t fid, void *addr, size_t *addrlen)
{
	struct psmx2_fid_ep *ep;
	struct psmx2_ep_name *epname = addr;

	ep = container_of(fid, struct psmx2_fid_ep, ep.fid);
	if (!ep->domain)
		return -FI_EBADF;

	if (*addrlen < sizeof(struct psmx2_ep_name)) {
		*addrlen = sizeof(struct psmx2_ep_name);
		return -FI_ETOOSMALL;
	}

	memset(epname, 0, sizeof(*epname));
	epname->epid = ep->domain->psm2_epid;
	epname->vlane = ep->vlane;
	*addrlen = sizeof(struct psmx2_ep_name);

	return 0;
}

struct fi_ops_cm psmx2_cm_ops = {
	.size = sizeof(struct fi_ops_cm),
	.getname = psmx2_cm_getname,
	.getpeer = fi_no_getpeer,
	.connect = fi_no_connect,
	.listen = fi_no_listen,
	.accept = fi_no_accept,
	.reject = fi_no_reject,
	.shutdown = fi_no_shutdown,
};

