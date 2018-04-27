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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>

#include "util.h"
#include "logging.h"
#include "photon_exchange.h"
#include "verbs_connect.h"
#include "verbs_util.h"
#include "verbs_ud.h"

#define DEF_SUB_WR          16
#define RDMA_CMA_BASE_PORT  18000
#define RDMA_CMA_FORW_OFFS  1000
#define RDMA_CMA_TIMEOUT    2000
#define RDMA_CMA_RETRIES    10

static pthread_t cma_listener;

static verbs_cnct_info **__exch_cnct_info(verbs_cnct_ctx *ctx,
					  verbs_cnct_info **local_info,
					  int num_qp);
static int __verbs_connect_qps_cma(verbs_cnct_ctx *ctx,
				   verbs_cnct_info *local_info,
				   verbs_cnct_info *remote_info,
				   int pindex, int num_qp);
static int __verbs_connect_qps(verbs_cnct_ctx *ctx,
			       verbs_cnct_info *local_info,
			       verbs_cnct_info *remote_info,
			       int pindex, int num_qp);
static int __verbs_init_context_cma(verbs_cnct_ctx *ctx,
				    struct rdma_cm_id *cm_id,
				    int pindex);
static int __verbs_create_connect_info(verbs_cnct_ctx *ctx);
static void *__rdma_cma_listener_thread(void *arg);

// data that gets exchanged during RDMA CMA connect
// for now we simply identify remote peers by address (rank)
struct rdma_cma_priv {
  uint64_t address;
};

struct rdma_cma_thread_args {
  verbs_cnct_ctx *ctx;
  int pindex;
  int num_listeners;
};

int __verbs_alloc_ctx(verbs_cnct_ctx *ctx) {
  int i;

  // adjust queue sizes based on nprocs and device characteristics
  if (__verbs_adjust_ctx(ctx) != PHOTON_OK) {
    dbg_err("Could not set appropriate context values");
    return PHOTON_ERROR;
  }
  
  ctx->ib_pd = ibv_alloc_pd(ctx->ib_context);
  if (!ctx->ib_pd) {
    dbg_err("Could not create protection domain");
    return PHOTON_ERROR;
  }

  // create completion queues
  ctx->ib_cq = (struct ibv_cq **)malloc(ctx->num_cq * sizeof(struct ibv_cq *));
  if (!ctx->ib_cq) {
    log_err("Could not create send CQ pointer array of size %d", ctx->num_cq);
    return PHOTON_ERROR;
  }

  if (ctx->use_rcq) {
    ctx->ib_rq = (struct ibv_cq **)malloc(ctx->num_cq * sizeof(struct ibv_cq *));
    if (!ctx->ib_rq) {
      log_err("Could not create recv CQ pointer array of size %d", ctx->num_cq);
      return PHOTON_ERROR;
    }
  }    

  for (i = 0; i < ctx->num_cq; i++) {
    ctx->ib_cq[i] = ibv_create_cq(ctx->ib_context, ctx->max_cqe, ctx, NULL,  0);
    if (!ctx->ib_cq[i]) {
      dbg_err("Could not create send completion queue %d of %d", i, ctx->num_cq);
      return PHOTON_ERROR;
    }
    if (ctx->use_rcq) {
      ctx->ib_rq[i] = ibv_create_cq(ctx->ib_context, ctx->max_cqe, ctx, NULL,  0);
      if (!ctx->ib_rq[i]) {
	dbg_err("Could not create recv completion queue %d of %d", i, ctx->num_cq);
	return PHOTON_ERROR;
      }
    }
  }
  
  dbg_trace("created %d CQs", ctx->num_cq);
  
  // create shared receive queues if requested
  if (ctx->use_rcq && (ctx->num_srq > 0)) {
    struct ibv_srq_init_attr attr = {
      .attr = {
	.max_wr  = ctx->max_srq_wr,
	.max_sge = 1
      }
    };
    
    ctx->ib_srq = (struct ibv_srq **)malloc(ctx->num_srq * sizeof(struct ibv_srq *));
    if (!ctx->ib_srq) {
      log_err("Could not create SRQ pointer array of size %d", ctx->num_srq);
      return PHOTON_ERROR;
    }
    
    for (i = 0; i < ctx->num_srq; i++) {
      ctx->ib_srq[i] = ibv_create_srq(ctx->ib_pd, &attr);
      if (!ctx->ib_srq[i]) {
	dbg_err("Could not create SRQ");
	return PHOTON_ERROR;
      }
    }
  }
  
  dbg_trace("created %d SRQs", ctx->num_srq);

  if (__verbs_find_max_inline(ctx, &ctx->max_inline)) {
    log_err("Could not determine max inline data size");
    return PHOTON_ERROR;
  }
  
  return PHOTON_OK;
}

int __verbs_init_context(verbs_cnct_ctx *ctx) {
  struct ibv_device **dev_list;
  struct ibv_context **ctx_list;
  int i, iproc, num_devs, cqind, srqind;
  int rc, found = 0;
  
  // initialize the QP array
  ctx->num_qp = MAX_QP;
  ctx->qp = (struct ibv_qp**)malloc((_photon_nproc + _photon_nforw) * sizeof(struct ibv_qp*));
  if (!ctx->qp) {
    log_err("Could not allocate QP memory");
    return PHOTON_ERROR;
  }
  // initialize the QP array that gets set later
  for (i=0; i<(_photon_nproc + _photon_nforw); i++) {
    ctx->qp[i] = NULL;
  }
  
  if (ctx->use_cma) {
    ctx->cm_id = (struct rdma_cm_id**)malloc((_photon_nproc + _photon_nforw) * sizeof(struct rdma_cm_id*));
    if (!ctx->cm_id) {
      log_err("Could not allocate CM_ID memory");
      return PHOTON_ERROR;
    }
    // initialize the CM_ID array that gets set later
    for (i=0; i<(_photon_nproc + _photon_nforw); i++) {
      ctx->qp[i] = NULL;
    }

    ctx->cm_schannel = rdma_create_event_channel();
    if (!ctx->cm_schannel) {
      log_err("Could not create RDMA event channel");
      return PHOTON_ERROR;
    }

    // make sure there are some devices we can use
    ctx_list = rdma_get_devices(&num_devs);
    if (!num_devs && !ctx_list) {
      log_err("No RDMA CMA devices found");
      return PHOTON_ERROR;
    }

    // start the RDMA CMA listener
    // first rank only connects to itself
    // forwarders will listen for a self-connection to start
    struct rdma_cma_thread_args *args;
    if (!_forwarder) {
      args = malloc(sizeof(struct rdma_cma_thread_args));
      args->ctx = ctx;
      args->pindex = -1;
      args->num_listeners = _photon_myrank + 1;
      pthread_create(&cma_listener, NULL, __rdma_cma_listener_thread, (void*)args);
    }
    else if (_forwarder) {
      args = malloc(sizeof(struct rdma_cma_thread_args));
      args->ctx = ctx;
      args->pindex = -1;
      args->num_listeners = 1;
      pthread_create(&cma_listener, NULL, __rdma_cma_listener_thread, (void*)args);
    }
  }
  else {
    dev_list = ibv_get_device_list(&num_devs);
    if (!num_devs) {
      log_err("No IB HCAs found");
      return PHOTON_ERROR;
    }
    
    photon_dev_list *dlist;
    rc = photon_parse_devstr(ctx->ib_dev, &dlist);
    if (rc < 0) {
      dbg_err("Could not parse HCA device filter: %s", ctx->ib_dev);
      return PHOTON_ERROR;
    }
    
    // just match the first port that's active and in the filter (if set)
    for (i=0; i<num_devs; i++) {
      const char *dev_name = ibv_get_device_name(dev_list[i]);
      struct ibv_context *dev_ctx;
      struct ibv_device_attr dattr;
      struct ibv_port_attr attr;
      int cport;

      if (!photon_match_dev(dlist, dev_name, 0)) {
	dbg_info("Skipping HCA: %s - no match", dev_name);
	continue;
      }
      // now we have a dev name match
      dev_ctx = ibv_open_device(dev_list[i]);
      if (dev_ctx) {
	dbg_info("Found HCA: %s", dev_name);
      }
      else {
	dbg_err("Could not open HCA: %s", dev_name);
	continue;
      }

      if (ibv_query_device(dev_ctx, &dattr)) {
	dbg_err("Could not query HCA: %s", dev_name);
	continue;
      }
      
      for (cport = 1; cport <= dattr.phys_port_cnt; cport++) {
	if (!photon_match_dev(dlist, dev_name, cport)) {
	  dbg_info("Skipping HCA: %s, port: %d - no match", dev_name, cport);
	  continue;
	}

	memset(&attr, 0, sizeof(attr));
	if (ibv_query_port(dev_ctx, cport, &attr)) {
	  dbg_err("Could not query port: %d (%s)", cport, dev_name);
	  continue;
	}

	// we found an active HCA port to use
	if (attr.state == IBV_PORT_ACTIVE) {
	  ctx->ib_context = dev_ctx;
	  ctx->ib_lid = attr.lid;
	  ctx->ib_mtu = 1 << (attr.active_mtu + 7);
	  ctx->ib_mtu_attr = attr.active_mtu;
	  ctx->max_qp_wr = dattr.max_qp_wr;
	  ctx->max_srq_wr = dattr.max_srq_wr;
	  ctx->ib_dev = strdup(dev_name);
	  ctx->ib_port = cport;

	  dbg_info("Found HCA: %s, port: %d", dev_name, cport);
	  found = 1;
	  break;
	}
      }
      if (found) {
	break;
      }
      else {
	ibv_close_device(dev_ctx);
      }
    }

    photon_free_devlist(dlist);
    ibv_free_device_list(dev_list);

    if (!found) {
      log_err("Could not find a suitable IB HCA (filter='%s')", ctx->ib_dev);
      return PHOTON_ERROR;
    }
    
    if (__verbs_alloc_ctx(ctx) != PHOTON_OK) {
      return PHOTON_ERROR;
    }

    // create QPs in the non-CMA case and transition to INIT state
    // RDMA CMA does this transition for us when we connect
    for (iproc = 0; iproc < (_photon_nproc + _photon_nforw); ++iproc) {
      cqind = PHOTON_GET_CQ_IND(ctx->num_cq, iproc);
      struct ibv_qp_init_attr attr = {
        .qp_context     = ctx,
        .send_cq        = ctx->ib_cq[cqind],
        .recv_cq        = (ctx->use_rcq) ? ctx->ib_rq[cqind] : ctx->ib_cq[cqind],
        .cap            = {
	  // qlogic HCAs appear to "eat" some WR entries so we add
	  // two extra to make our TX accounting safe
          .max_send_wr	   = ctx->tx_depth + 2,
          .max_recv_wr     = ctx->rx_depth + 2,
          .max_send_sge    = ctx->max_sge,
          .max_recv_sge    = ctx->max_sge,
          .max_inline_data = ctx->max_inline
        },
        .qp_type        = IBV_QPT_RC
      };

      if (ctx->use_rcq && (ctx->num_srq > 0)) {
	srqind = PHOTON_GET_CQ_IND(ctx->num_srq, iproc);
	dbg_trace("srqind for rank %d: %d", iproc, srqind);
	attr.srq = ctx->ib_srq[srqind];
      } 
      else {
	attr.srq = NULL;
      }
      
      ctx->qp[iproc] = ibv_create_qp(ctx->ib_pd, &attr);
      if (!(ctx->qp[iproc])) {
        dbg_err("Could not create QP for task:%d", iproc);
        return PHOTON_ERROR;
      }

      {
        struct ibv_qp_attr attr;

        attr.qp_state    = IBV_QPS_INIT;
        attr.pkey_index	 = 0;
        attr.port_num	 = ctx->ib_port;
        attr.qp_access_flags = IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_REMOTE_READ;

        if (ibv_modify_qp(ctx->qp[iproc], &attr,
                          IBV_QP_STATE		 |
                          IBV_QP_PKEY_INDEX	 |
                          IBV_QP_PORT		 |
                          IBV_QP_ACCESS_FLAGS)) {
          dbg_err("Failed to modify QP for task:%d to INIT: %s", iproc, strerror(errno));
          return PHOTON_ERROR;
        }
      }
    }

    // create a UD QP as well if requested
    if (ctx->use_ud) {
      __verbs_ud_create_qp(ctx);
    }
  }

  // init context also creates connect info for all procs
  return __verbs_create_connect_info(ctx);
}

// we make connect info for all procs, including any forwarders
int __verbs_create_connect_info(verbs_cnct_ctx *ctx) {
  struct ifaddrs *ifaddr, *ifa;
  int i, j, iproc;

  ctx->local_ci = (verbs_cnct_info **)malloc((_photon_nproc + _photon_nforw) * sizeof(verbs_cnct_info *) );
  if( !ctx->local_ci ) {
    goto error_exit;
  }

  if (getifaddrs(&ifaddr) == -1) {
    dbg_err("Cannot get interface addrs");
    goto error_exit;
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL)
      continue;

    if (!strcmp(ifa->ifa_name, ctx->eth_dev) &&
        ifa->ifa_addr->sa_family == AF_INET) {
      dbg_info("Found ETH dev for CMA: %s", ifa->ifa_name);
      break;
    }
  }

  if (ctx->use_cma && !ifa) {
    log_err("Could not find ETH dev for CMA: %s", ctx->eth_dev);
    goto error_exit;
  }

  for(iproc=0; iproc < (_photon_nproc + _photon_nforw); ++iproc) {

    ctx->local_ci[iproc] = (verbs_cnct_info *)calloc(MAX_QP, sizeof(verbs_cnct_info));
    if( !ctx->local_ci[iproc] ) {
      goto error_exit;
    }

    for(i=0; i<MAX_QP; ++i) {
      
      memset(&(ctx->local_ci[iproc][i].gid.raw), 0, sizeof(union ibv_gid));
      
      if (ctx->use_cma) {
        ctx->local_ci[iproc][i].qpn = 0x0;
      }
      else {
        // can only query gid in in non-CMA mode, CMA will exchange this for us
        if (ibv_query_gid(ctx->ib_context, ctx->ib_port, 0, &(ctx->local_ci[iproc][i].gid))) {
          dbg_trace("Could not get local gid for gid index 0");
        }
        ctx->local_ci[iproc][i].qpn = ctx->qp[iproc]->qp_num;
      }

      if (ifa) {
        ctx->local_ci[iproc][i].ip = ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
        ctx->local_ci[iproc][i].cma_port = RDMA_CMA_BASE_PORT + _photon_myrank;
      }

      ctx->local_ci[iproc][i].lid = ctx->ib_lid;
      ctx->local_ci[iproc][i].psn = (lrand48() & 0xfff000) + _photon_myrank+1;
    }
  }

  // also allocate space for the remote info
  ctx->remote_ci = (verbs_cnct_info **)malloc((_photon_nproc + _photon_nforw) * sizeof(verbs_cnct_info *) );
  for (iproc = 0; iproc < (_photon_nproc + _photon_nforw); ++iproc) {

    ctx->remote_ci[iproc] = (verbs_cnct_info *)malloc(MAX_QP * sizeof(verbs_cnct_info));
    if (!ctx->remote_ci) {
      for (j = 0; j < iproc; j++) {
        free(ctx->remote_ci[j]);
      }
      free(ctx->remote_ci);
      goto error_exit;
    }
  }

  freeifaddrs(ifaddr);
  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

int __verbs_connect_single(verbs_cnct_ctx *ctx, verbs_cnct_info *local_info,
			   verbs_cnct_info *remote_info, int pindex,
                           verbs_cnct_info **ret_ci, int *ret_len,
			   photon_connect_mode_t mode) {
  switch (mode) {
  case PHOTON_CONN_ACTIVE:
    if (ctx->use_cma) {
      return __verbs_connect_qps_cma(ctx, local_info, remote_info, pindex, MAX_QP);
    }
    else {
      return __verbs_connect_qps(ctx, local_info, remote_info, pindex, MAX_QP);
    }
    break;
  case PHOTON_CONN_PASSIVE:
    if (ctx->use_cma) {
      pthread_t cma_thread;
      struct rdma_cma_thread_args *args;
      args = malloc(sizeof(struct rdma_cma_thread_args));
      args->ctx = ctx;
      args->pindex = pindex;
      args->num_listeners = 1;
      pthread_create(&cma_thread, NULL, __rdma_cma_listener_thread, (void*)args);

      // if we are some external forwarder, return which port we're listening on for the peer
      if (_forwarder && ret_ci && ret_len) {
        *ret_ci = malloc(sizeof(verbs_cnct_info));
        memcpy(*ret_ci, local_info, sizeof(verbs_cnct_info));
        (*ret_ci)->cma_port = RDMA_CMA_BASE_PORT + RDMA_CMA_FORW_OFFS + pindex;
        *ret_len = sizeof(verbs_cnct_info);
      }
      return PHOTON_OK;
    }
    else {
      return __verbs_connect_qps(ctx, local_info, remote_info, pindex, MAX_QP);
    }
  default:
    return PHOTON_ERROR;
  }
}

int __verbs_connect_peers(verbs_cnct_ctx *ctx) {
  int iproc;

  dbg_trace("Connecting peers");

  ctx->remote_ci = __exch_cnct_info(ctx, ctx->local_ci, MAX_QP);
  if( !ctx->remote_ci ) {
    dbg_err("Cannot exchange connect info");
    goto error_exit;
  }

  photon_exchange_barrier();

  if (ctx->use_cma) {
    // in the CMA case, only connect actively for ranks greater than or equal to our rank
    for (iproc = _photon_myrank; iproc < _photon_nproc; iproc++) {
      if (__verbs_connect_qps_cma(ctx, ctx->local_ci[iproc], ctx->remote_ci[iproc], iproc, MAX_QP)) {
        dbg_err("Could not connect queue pairs using RDMA CMA");
        goto error_exit;
      }
    }
  }
  else {
    for (iproc = 0; iproc < _photon_nproc; iproc++) {
      if(__verbs_connect_qps(ctx, ctx->local_ci[iproc], ctx->remote_ci[iproc], iproc, MAX_QP)) {
        dbg_err("Cannot connect queue pairs");
        goto error_exit;
      }
    }
  }

  // make sure everyone is connected before proceeding
  if (ctx->use_cma) {
    if (_photon_myrank > 0) {
      dbg_trace("waiting for listener to finish...");
      pthread_join(cma_listener, NULL);
      dbg_trace("DONE");
    }

    photon_exchange_barrier();
  }

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

static int __verbs_init_context_cma(verbs_cnct_ctx *ctx, struct rdma_cm_id *cm_id, int pindex) {
  // assign the verbs context if this is the first connection
  if (!ctx->ib_context) {
    ctx->ib_context = cm_id->verbs;
    if (!ctx->ib_context) {
      dbg_err("could not get context from CM ID!");
      goto error_exit;
    }

    // create new PD with the new context
    ctx->ib_pd = ibv_alloc_pd(ctx->ib_context);
    if (!ctx->ib_pd) {
      dbg_err("could not create PD");
      goto error_exit;
    }
    
    struct ibv_port_attr port_attr;
    if (ibv_query_port(cm_id->verbs, cm_id->port_num, &port_attr)) {
      dbg_err("could not query port");
      goto error_exit;
    }
    ctx->ib_lid = port_attr.lid;
    ctx->ib_mtu = 1 << (port_attr.active_mtu + 7);    
    ctx->ib_mtu_attr = port_attr.active_mtu;

    struct ibv_device_attr dattr;
    if (ibv_query_device(ctx->ib_context, &dattr)) {
      log_err("Could not query IB device");
      return PHOTON_ERROR;
    }

    ctx->tx_depth = dattr.max_qp_wr - DEF_SUB_WR;
    ctx->rx_depth = dattr.max_qp_wr - DEF_SUB_WR;
    ctx->max_qp_wr = dattr.max_qp_wr;
    ctx->max_srq_wr = dattr.max_srq_wr;
    ctx->max_cqe = dattr.max_cqe;

    if (__verbs_alloc_ctx(ctx) != PHOTON_OK) {
      dbg_err("Could not allocate verbs context");
      return PHOTON_ERROR;
    }
    
    // create a UD QP as well if requested
    if (ctx->use_ud) {
      __verbs_ud_create_qp(ctx);
    }
  }
  else {
    if (ctx->ib_context != cm_id->verbs) {
      dbg_err("CM ID has a new ib_context (different adapter), not supported!");
      goto error_exit;
    }
  }

  int cqind = PHOTON_GET_CQ_IND(ctx->num_cq, pindex); 
  struct ibv_qp_init_attr attr = {
    .qp_context = ctx,
    .send_cq = ctx->ib_cq[cqind],
    .recv_cq = (ctx->use_rcq) ? ctx->ib_rq[cqind] : ctx->ib_cq[cqind],
    .cap     = {
      .max_send_wr  = ctx->tx_depth,
      .max_recv_wr  = ctx->rx_depth,
      .max_send_sge = ctx->max_sge,
      .max_recv_sge = ctx->max_sge,
      .max_inline_data = ctx->max_inline
    },
    .qp_type = IBV_QPT_RC,
  };

  if (ctx->use_rcq && (ctx->num_srq > 0)) {
    int srqind = PHOTON_GET_CQ_IND(ctx->num_srq, pindex);
    dbg_trace("srqind for rank %d: %d", pindex, srqind);
    attr.srq = ctx->ib_srq[srqind];
  } 
  else {
    attr.srq = NULL;
  }
  
  // create a new QP for each connection
  if (rdma_create_qp(cm_id, ctx->ib_pd, &attr)) {
    dbg_err("could not create QP");
    goto error_exit;
  }

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

static void *__rdma_cma_listener_thread(void *arg) {
  struct rdma_cma_thread_args *args = (struct rdma_cma_thread_args*)arg;
  verbs_cnct_ctx *ctx = args->ctx;
  int num_listeners = args->num_listeners;
  int n, num_connected = 0;
  int pindex;
  int port;
  char *service;

  struct addrinfo *res;
  struct addrinfo hints = {
    .ai_flags    = AI_PASSIVE,
    .ai_family   = AF_UNSPEC,
    .ai_socktype = SOCK_STREAM
  };
  struct rdma_cm_event *event;
  struct sockaddr_in sin;
  struct rdma_cm_id *local_cm_id;
  struct rdma_cm_id *child_cm_id;
  struct rdma_conn_param conn_param;
  struct rdma_event_channel *echannel;

  if (args->pindex >= 0) {
    port = RDMA_CMA_BASE_PORT + RDMA_CMA_FORW_OFFS + args->pindex;
  }
  else {
    port = RDMA_CMA_BASE_PORT + _photon_myrank;
  }

  if (asprintf(&service, "%d", port) < 0)
    goto error_exit;

  if ((n = getaddrinfo(NULL, service, &hints, &res)) < 0) {
    dbg_err("%s for port %d\n", gai_strerror(n), port);
    goto error_exit;
  }

  sin.sin_addr.s_addr = 0;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);

  echannel = rdma_create_event_channel();
  if (!echannel) {
    dbg_err("Could not create RDMA event channel");
    goto error_exit;
  }

  if (rdma_create_id(echannel, &local_cm_id, NULL, RDMA_PS_TCP)) {
    dbg_err("Could not create CM ID");
    goto error_exit;
  }

  if (rdma_bind_addr(local_cm_id, (struct sockaddr *)&sin)) {
    dbg_err("rdma_bind_addr failed: %s", strerror(errno));
    goto error_exit;
  }

  if (rdma_listen(local_cm_id, 0)) {
    dbg_err("rdma_listen failed: %s", strerror(errno));
    goto error_exit;
  }

  dbg_trace("Listening for %d connections on port %d", num_listeners, port);

  // accept some number of connections
  // this currently depends on rank position
  do {

    if (rdma_get_cm_event(echannel, &event)) {
      dbg_err("coult not get event %s", strerror(errno));
      goto error_exit;
    }

    switch (event->event) {
    case RDMA_CM_EVENT_CONNECT_REQUEST: {
      child_cm_id = (struct rdma_cm_id *)event->id;
      if (!child_cm_id) {
        dbg_err("could not get child CM ID");
        goto error_exit;
      }

      // figure out who just connected to us and accept
      struct rdma_cma_priv *priv;
      if (event->param.conn.private_data &&
          (event->param.conn.private_data_len > 0)) {
        priv = (struct rdma_cma_priv*)event->param.conn.private_data;
        // no need to hold onto this data
        //ctx->remote_priv[priv->address] = malloc(event->param.conn.private_data_len);
        //memcpy(ctx->remote_priv[priv->address],
        //       event->param.conn.private_data,
        //       event->param.conn.private_data_len);
      }
      else {
        // TODO: use another mechanism to identify remote peer
        dbg_err("no remote connect info found");
        goto error_exit;
      }

      dbg_trace("got connection request from %d", (int)priv->address);

      if (args->pindex >= 0) {
        pindex = args->pindex;
      }
      else {
        pindex = (int)priv->address;
      }

      if (__verbs_init_context_cma(ctx, child_cm_id, pindex) != PHOTON_OK) {
        goto error_exit;
      }
      
      dbg_trace("created context");
      
      // save the child CM_ID in our process list
      ctx->cm_id[pindex] = child_cm_id;
      // save the QP in our process list
      ctx->qp[pindex] = child_cm_id->qp;

      memset(&conn_param, 0, sizeof conn_param);
      conn_param.responder_resources = ctx->atomic_depth;
      conn_param.initiator_depth = ctx->atomic_depth;
      // don't send any private data back
      conn_param.private_data = NULL;
      conn_param.private_data_len = 0;

      if (rdma_accept(child_cm_id, &conn_param)) {
        dbg_err("rdma_accept failed: %s", strerror(errno));
        goto error_exit;
      }
    }
    break;
    case RDMA_CM_EVENT_ESTABLISHED:
      dbg_trace("connection established");
      num_connected++;
      break;
    default:
      dbg_err("bad event waiting for established %d", event->event);
      goto error_exit;
      break;
    }

    rdma_ack_cm_event(event);

  }
  while (num_connected < num_listeners);

  if (rdma_destroy_id(local_cm_id)) {
    dbg_err("Could not destroy CM ID");
  }

  if (args) {
    free(args);
  }

  dbg_trace("Listener thread done");

error_exit:
  return NULL;
}

static int __verbs_connect_qps_cma(verbs_cnct_ctx *ctx,
				   verbs_cnct_info *local_info,
				   verbs_cnct_info *remote_info,
				   int i, int num_qp) {
  struct addrinfo *res;
  struct addrinfo hints = {
    .ai_family   = AF_UNSPEC,
    .ai_socktype = SOCK_STREAM
  };

  char *service, *host;
  int n, n_retries = RDMA_CMA_RETRIES;
  int port;
  struct rdma_cm_event *event;
  struct sockaddr_in sin;
  struct rdma_conn_param conn_param;

  // TODO: allow multiple RDMA CMA connections per process
  if (num_qp > 1) {
    dbg_err("photon does not currently support more than one CM_ID/QP per process!");
    goto error_exit;
  }

  // we may get a list of hostnames in the future
  // for now all we need is the remote ip and port
  host = inet_ntoa(remote_info[0].ip);
  port = remote_info[0].cma_port;

  dbg_trace("connecting to %d (%s) on port %d", i, host, port);

  if (asprintf(&service, "%d", port) < 0)
    goto error_exit;

  if ((n = getaddrinfo(host, service, &hints, &res) < 0)) {
    dbg_err("%s for %s:%d", gai_strerror(n), host, port);
    goto error_exit;
  }

  sin.sin_addr.s_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr.s_addr;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);

  if (rdma_create_id(ctx->cm_schannel, &(ctx->cm_id[i]), NULL, RDMA_PS_TCP)) {
    dbg_err("Could not create CM ID");
    goto error_exit;
  }

retry_addr:
  if (rdma_resolve_addr(ctx->cm_id[i], NULL, (struct sockaddr *)&sin, RDMA_CMA_TIMEOUT)) {
    dbg_err("resolve addr failed: %s", strerror(errno));
    goto error_exit;
  }

  if (rdma_get_cm_event(ctx->cm_schannel, &event)) {
    goto error_exit;
  }

  if (event->event == RDMA_CM_EVENT_ADDR_ERROR && (n_retries-- > 0)) {
    rdma_ack_cm_event(event);
    goto retry_addr;
  }

  if (event->event != RDMA_CM_EVENT_ADDR_RESOLVED) {
    dbg_err("unexpected CM event (res0) %d", event->event);
    goto error_exit;
  }
  rdma_ack_cm_event(event);

retry_route:
  n_retries = RDMA_CMA_RETRIES;

  if (rdma_resolve_route(ctx->cm_id[i], RDMA_CMA_TIMEOUT)) {
    dbg_err("rdma_resolve_route failed: %s", strerror(errno));
    goto error_exit;
  }

  if (rdma_get_cm_event(ctx->cm_schannel, &event)) {
    goto error_exit;
  }

  if (event->event == RDMA_CM_EVENT_ROUTE_ERROR && (n_retries-- > 0)) {
    rdma_ack_cm_event(event);
    goto retry_route;
  }

  if (event->event != RDMA_CM_EVENT_ROUTE_RESOLVED) {
    dbg_err("unexpected CM event (res1) %d", event->event);
    goto error_exit;
  }
  rdma_ack_cm_event(event);

  if (__verbs_init_context_cma(ctx, ctx->cm_id[i], i) != PHOTON_OK) {
    goto error_exit;
  }

  // save the QP in our process list
  ctx->qp[i] = ctx->cm_id[i]->qp;

  struct rdma_cma_priv priv_data = {
    .address = _photon_myrank
  };

  memset(&conn_param, 0, sizeof conn_param);
  conn_param.responder_resources = ctx->atomic_depth;
  conn_param.initiator_depth = ctx->atomic_depth;
  conn_param.retry_count = RDMA_CMA_RETRIES;
  conn_param.private_data = &priv_data;
  conn_param.private_data_len = sizeof(struct rdma_cma_priv);

  if (rdma_connect(ctx->cm_id[i], &conn_param)) {
    dbg_err("rdma_connect failure: %s", strerror(errno));
    goto error_exit;
  }

  if (rdma_get_cm_event(ctx->cm_schannel, &event)) {
    goto error_exit;
  }

  if (event->event != RDMA_CM_EVENT_ESTABLISHED) {
    dbg_err("unexpected CM event (est) %d", event->event);
    goto error_exit;
  }

  if (event->param.conn.private_data &&
      (event->param.conn.private_data_len > 0)) {
    // we got some data back from the remote peer
    // not used
  }
  rdma_ack_cm_event(event);

  freeaddrinfo(res);

  return PHOTON_OK;

error_exit:
  return PHOTON_ERROR;
}

static int __verbs_connect_qps(verbs_cnct_ctx *ctx,
			       verbs_cnct_info *local_info,
			       verbs_cnct_info *remote_info,
			       int pindex, int num_qp) {
  int i;
  int err;
#ifdef ENABLE_CALLTRACE
  char gid[40];
#endif
  
  for (i = 0; i < num_qp; ++i) {
    dbg_trace("[%d/%d], pindex=%d lid=%x qpn=%x, psn=%x, qp[i].qpn=%x, gid=%s",
	      _photon_myrank, _photon_nproc, pindex,
	      remote_info[i].lid, remote_info[i].qpn, remote_info[i].psn,
	      ctx->qp[pindex]->qp_num,
	      inet_ntop(AF_INET6, remote_info[i].gid.raw, gid, 40));
    
    struct ibv_ah_attr ah_attr = {
      .is_global     = 0,
      .dlid          = remote_info[i].lid,
      .sl            = 0,
      .src_path_bits = 0,
      .port_num      = ctx->ib_port
    };
    
    if (remote_info[i].gid.global.interface_id) {
      ah_attr.is_global = 1;
      ah_attr.grh.hop_limit = 1;
      ah_attr.grh.dgid = remote_info[i].gid;
      ah_attr.grh.sgid_index = 0;
    }
    
    struct ibv_qp_attr attr = {
      .qp_state	          = IBV_QPS_RTR,
      .path_mtu	          = ctx->ib_mtu_attr,
      .dest_qp_num        = remote_info[i].qpn,
      .rq_psn             = remote_info[i].psn,
      .max_dest_rd_atomic = 1,
      .min_rnr_timer	  = 12,
      .ah_attr = ah_attr
    };
    err=ibv_modify_qp(ctx->qp[pindex], &attr,
                      IBV_QP_STATE              |
                      IBV_QP_AV                 |
                      IBV_QP_PATH_MTU           |
                      IBV_QP_DEST_QPN           |
                      IBV_QP_RQ_PSN             |
                      IBV_QP_MAX_DEST_RD_ATOMIC |
                      IBV_QP_MIN_RNR_TIMER);
    if (err) {
      dbg_err("Failed to modify QP[%d] to RTR. Reason: %s", i, strerror(err));
      return PHOTON_ERROR;
    }

    attr.qp_state      = IBV_QPS_RTS;
    attr.timeout       = 14;
    attr.retry_cnt     = 7;
    attr.rnr_retry     = 7;
    attr.sq_psn        = local_info[i].psn;
    attr.max_rd_atomic = 1;
    err=ibv_modify_qp(ctx->qp[pindex], &attr,
                      IBV_QP_STATE     |
                      IBV_QP_TIMEOUT   |
                      IBV_QP_RETRY_CNT |
                      IBV_QP_RNR_RETRY |
                      IBV_QP_SQ_PSN    |
                      IBV_QP_MAX_QP_RD_ATOMIC);
    if (err) {
      dbg_err("Failed to modify QP[%d] to RTS. Reason: %s", i, strerror(err));
      return PHOTON_ERROR;
    }
  }

  return 0;
}

static verbs_cnct_info **__exch_cnct_info(verbs_cnct_ctx *ctx, verbs_cnct_info **local_info, int num_qp) {
  int peer;
  char smsg[ sizeof "00000000:00000000:00000000:00000000:00000000:00000000000000000000000000000000"];
  char *rmsg;
  int i, k, iproc, rc;
  verbs_cnct_info **remote_info = ctx->remote_ci;
  int msg_size = sizeof smsg;
  char gid[sizeof "00000000000000000000000000000000" + 1];

  rmsg = (char*)calloc(_photon_nproc, msg_size);
  if (!rmsg) {
    log_err("Could not allocate exchange recv buffer");
    goto err_exit;
  }

  for (i = 0; i < num_qp; i++) {
    for (iproc=0; iproc < _photon_nproc; iproc++) {
      peer = iproc;
      
      inet_ntop(AF_INET6, local_info[peer][i].gid.raw, gid, sizeof gid);
      sprintf(smsg, "%08x:%08x:%08x:%08x:%08d:%s", local_info[peer][i].lid, local_info[peer][i].qpn,
	      local_info[peer][i].psn, local_info[peer][i].ip.s_addr, local_info[peer][i].cma_port, gid);
      
      rc = photon_exchange_allgather(smsg, rmsg, msg_size);
      if (rc != PHOTON_OK) {
	log_err("exchange failed");
	goto err_exit;
      }

      if (_photon_myrank == iproc) {
	char *tmp = rmsg;
	for (k = 0; k < _photon_nproc; k++) {
	  sscanf(tmp, "%x:%x:%x:%x:%d:%s",
		 &remote_info[k][i].lid,
		 &remote_info[k][i].qpn,
		 &remote_info[k][i].psn,
		 &remote_info[k][i].ip.s_addr,
		 &remote_info[k][i].cma_port,
		 gid);
	  inet_pton(AF_INET6, gid, &remote_info[k][i].gid.raw);
	  tmp += msg_size;
	}
      }
    }
  }
  
  free(rmsg);
  
  return remote_info;
err_exit:
  return NULL;
}
