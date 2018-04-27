#include "ph.h"
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define ZERO_RANK 0
#define ZERO_ERANK -100

bool __initialized = false;

struct photon_config_t nbc_cfg = {.nproc = 0,
                                  .address = 0,
                                  .forwarder = {.use_forwarder = 0},
                                  .ibv =
                                      {
                                          .use_cma = 0,
                                          .use_ud = 0,
                                          .num_srq = 0,
                                          .eth_dev = "roce0",
                                          /*.ib_dev  = "mlx4_0+qib0",*/
                                          .ib_dev = "qib0",
                                      },
                                  .ugni =
                                      {
                                          .bte_thresh = -1,
                                      },
                                  .cap = {.small_msg_size = -1,
                                          .small_pwc_size = -1,
                                          .eager_buf_size = -1,
                                          .ledger_entries = -1,
                                          .max_rd = -1,
                                          .default_rd = -1,
                                          .num_cq = -1,
                                          .use_rcq = 0},
                                  .exch = {.allgather = NULL, .barrier = NULL},
                                  .meta_exch = PHOTON_EXCH_MPI,
                                  .comm = NULL,
                                  .backend = PHOTON_BACKEND_VERBS};

static void _encode_tag(uint64_t *tag, uint64_t handle_id) {
  *tag = *tag + (handle_id << 4);
}

int init_photon_backend(int my_address, int total_nodes) {

  if (!__initialized &&
      __sync_bool_compare_and_swap(&__initialized, false, true)) {
    if (!photon_initialized()) {
      nbc_cfg.address = my_address;
      nbc_cfg.nproc = total_nodes;
      photon_init(&nbc_cfg);
      printf("===[%d] rank, init complete \n", my_address);
    } else {
      printf("===[%d] rank, photon already initialized \n", my_address);
    }
  }
  return 0;
}

static void _cleanup_photon_buffers(void *obj, int nproc) {
  int i;
  buffer_pool_t *bp = (buffer_pool_t *)obj;
  // release all registered buffers
  for (i = 0; i < nproc; ++i) {
    photon_unregister_buffer(bp->send[i], BASE_BUF_LENGTH);
    photon_unregister_buffer(bp->recv[i],
                             BASE_BUF_LENGTH * NUM_BUFFERS_PER_DEST);
    free(bp->send[i]);

    /*for (j = 0; j < NUM_BUFFERS_PER_DEST; ++j) {*/
    /*photon_unregister_buffer(bp->recv[i][j], BASE_BUF_LENGTH);*/
    /*}*/
    free(bp->recv[i]);
  }
  free(bp->send);
  free(bp->recv);

#ifdef PHOTON_2SIDED
  // allocate on stack
  free(bp->sendReq);
  free(bp->recvReq);
#endif

  free(bp->credits);
  free(bp->rselect);
  free(bp->sselect);
  free(bp->rbuf_list);
  /*destroy(bp->ordered_recvs);*/
}

static void CHECK(int e, char *msg) {
  if (e != PHOTON_OK) {
    fprintf(stderr, "PHOTON Error returned in %s : Error code : %d\n", msg, e);
    exit(1);
  }
}

#include <unistd.h>

static char *_RBUF(buffer_pool_t *bp, int src, int slot) {
  char *recv_ptr = bp->recv[src];
  recv_ptr = recv_ptr + BASE_BUF_LENGTH * slot;
  return recv_ptr;
}

int init_photon_buffers(int nproc, buffer_pool_t *bp) {
  int i, j ;

  bp->cleanup = &_cleanup_photon_buffers;

  photon_rid *recvReq;
  photon_rid *sendReq;
  recvReq = (photon_rid *)malloc(sizeof(photon_rid) * nproc);
  sendReq = (photon_rid *)malloc(sizeof(photon_rid) * nproc);

  // init buffer pool data
  bp->send = (char **)malloc(sizeof(char *) * nproc);
  bp->recv = (char **)malloc(sizeof(char *) * nproc);

#ifdef PHOTON_2SIDED
  // allocate on stack
  bp->sendReq = (photon_rid *)malloc(sizeof(photon_rid) * nproc);
  bp->recvReq = (photon_rid *)malloc(sizeof(photon_rid) * nproc);
#endif

  bp->rbuf_list = (struct photon_buffer_t *)malloc(
      sizeof(struct photon_buffer_t) * nproc * NUM_BUFFERS_PER_DEST);

  // init buffer pool for temporary requests
  /*_NBC_photon_init_queue(&bp->temp_pwc_reqs);*/

  // init credits for each destination put op
  bp->credits = (int *)malloc(sizeof(int) * nproc);
  bp->rselect = (int *)malloc(sizeof(int) * nproc);
  bp->sselect = (int *)malloc(sizeof(int) * nproc);
  bp->credits_sources = sync_two_lock_queue_new();
  bp->credits_released = 0;

  for (i = 0; i < nproc; ++i) {
    // we give enough credit to start with for each initiator
    bp->credits[i] = NUM_BUFFERS_PER_DEST;
    bp->rselect[i] = 0;
    bp->sselect[i] = 0;

    /*recvReq[i] = malloc(sizeof(photon_rid)*NUM_BUFFERS_PER_DEST);*/
    /*sendReq[i] = malloc(sizeof(photon_rid)*NUM_BUFFERS_PER_DEST);*/
  }

  // register send buffers on photon - a buffer per each outgoing rank -
  //        ie:- there can be cases where we send 2/more "different" messages to
  //        outgoing ranks
  // register recv buffers on photon - a buffer per each incoming rank
  // this is important since we will use these for RDMA transfer
  for (i = 0; i < nproc; ++i) {
    posix_memalign((void **)&bp->send[i], 64,
                   BASE_BUF_LENGTH * sizeof(uint8_t));
    posix_memalign((void **)&bp->recv[i], 64,
                   BASE_BUF_LENGTH * NUM_BUFFERS_PER_DEST * sizeof(uint8_t));
    photon_register_buffer(bp->send[i], BASE_BUF_LENGTH);
    photon_register_buffer(bp->recv[i], BASE_BUF_LENGTH * NUM_BUFFERS_PER_DEST);
  }

  // wait for RDMA buffers to be posted
  for (i = 0; i < nproc; i++) {
    int ret = 0;
    do {
      if (ret == PHOTON_ERROR)
        usleep(30000);
      // everyone posts their recv buffers
      /*ret = photon_post_recv_buffer_rdma(i, bp->recv[i][j], BASE_BUF_LENGTH,
       * PHOTON_LOCAL_TAG_1, &recvReq[i][j]);*/
      ret = photon_post_recv_buffer_rdma(i, bp->recv[i],
                                         BASE_BUF_LENGTH * NUM_BUFFERS_PER_DEST,
                                         PHOTON_LOCAL_TAG_1, &recvReq[i]);
      /*CHECK(ret, "photon_post_recv_buffer_rdma()");*/
    } while (ret == PHOTON_ERROR);
  }

  // finalize RDMA registration at initiators side
  for (i = 0; i < nproc; i++) {
    int ret;
    ret = photon_wait_recv_buffer_rdma(i, PHOTON_ANY_SIZE, PHOTON_LOCAL_TAG_1,
                                       &sendReq[i]);
    CHECK(ret, "photon_wait_recv_buffer_rdma()");
    struct photon_buffer_t photon_rbuf;
    photon_get_buffer_remote(sendReq[i], &photon_rbuf);
    photon_send_FIN(sendReq[i], i, PHOTON_REQ_COMPLETED);
    photon_wait(recvReq[i]);

    for (j = 0; j < NUM_BUFFERS_PER_DEST; ++j) {
      bp->rbuf_list[i * NUM_BUFFERS_PER_DEST + j].addr =
          photon_rbuf.addr + BASE_BUF_LENGTH * j;
      bp->rbuf_list[i * NUM_BUFFERS_PER_DEST + j].size = BASE_BUF_LENGTH;
      bp->rbuf_list[i * NUM_BUFFERS_PER_DEST + j].priv = photon_rbuf.priv;
    }
  }

  return PH_OK;
}

static void _release_credits(uint64_t src, buffer_pool_t *bp) {
  // fix bug related to zero rank
  // use a different version of credit queue ?
  if (src == ZERO_RANK) {
    uint64_t temp = ZERO_ERANK;
    sync_two_lock_queue_enqueue(bp->credits_sources, (void *)temp);
  } else {
    sync_two_lock_queue_enqueue(bp->credits_sources, (void *)src);
  }
  bp->credits_released++;
}

typedef struct nbc_photon_rstate {
  int send_pending;
  int send_not_ready;
  int recv_pending;
} nbc_photon_rstate_t;

static nbc_photon_rstate_t _check_sends(int req_count,
                                        photon_nbc_req *req_array,
                                        buffer_pool_t *bp,
                                        int *target_send_reqs, uint64_t tag) {
  int i;
  nbc_photon_rstate_t rstate;
  rstate.send_pending = 0;
  rstate.send_not_ready = 0;
  rstate.recv_pending = 0;
  // request accounting
  for (i = 0; i < req_count; ++i) {
    if (req_array[i].send_req) {
      if (req_array[i].status == NBC_PHOTON_REQ_PENDING) {
        target_send_reqs[rstate.send_pending++] = i;
      } else if (req_array[i].status == NBC_PHOTON_REQ_NOT_READY) {
        // attempt to send this request
        req_array[i].status = PHOTON_remote_put_wc(&req_array[i], bp, tag);
        if (req_array[i].status == NBC_PHOTON_REQ_PENDING) {
          target_send_reqs[rstate.send_pending++] = i;
        }
        rstate.send_not_ready +=
            (req_array[i].status == NBC_PHOTON_REQ_NOT_READY) ? 1 : 0;
      }
    }
  }
  return rstate;
}

static nbc_photon_rstate_t _check_recieves(int req_count,
                                           photon_nbc_req *req_array,
                                           buffer_pool_t *bp,
                                           recv_info_t *recvs, uint64_t tag) {
  int i;
  nbc_photon_rstate_t rstate;
  rstate.send_pending = 0;
  rstate.send_not_ready = 0;
  rstate.recv_pending = 0;

  for (i = 0; i < req_count; ++i) {
    if (!req_array[i].send_req) {
      if (req_array[i].status == NBC_PHOTON_REQ_PENDING &&
          delete (req_array[i].sink, recvs->ordered_recvs)) {
        // if already a matching recieve is present then don'account that
        eager_buf_t *e = &recvs->eager_recvs[req_array[i].sink];
        if (e->size > 0) {
          // eager case
          memcpy(req_array[i].req_buf, e->data, req_array[i].buf_size);
          free(e->data);
          e->size = 0;
        } else {
          memcpy(req_array[i].req_buf,
                 _RBUF(bp, req_array[i].sink, req_array[i].rselect),
                 req_array[i].buf_size);
          _release_credits(req_array[i].sink, bp);
        }
        // int* res = req_array[i].req_buf;
        /*printf("[LAZY RECV] recv rank %d src %d  result : %d target : %p tag :
         * %d \n", _photon_myrank, req_array[i].sink, *res, res, tag);*/
        req_array[i].status = NBC_PHOTON_REQ_COMPLETED;
      } else {
        rstate.recv_pending += (req_array[i].status == NBC_PHOTON_REQ_PENDING);
      }
    }
  }
  return rstate;
}

// do majority of flow control logic here
get_remote_ret_t get_remote_photon_buffer(int dest, int buffer_size,
                                          buffer_pool_t *bp) {
  get_remote_ret_t ret;
  ret.rc = RBUF_FETCH_ERROR_ZERO_CREDIT;
  ret.rbuf = NULL;

  int cr = bp->credits[dest];
  if (cr > 0) {
    if (buffer_size < BASE_BUF_LENGTH) {
      // calculate correct buffer slot
      int idx = bp->sselect[dest];
      bp->sselect[dest] = (bp->sselect[dest] + 1) % NUM_BUFFERS_PER_DEST;
      struct photon_buffer_t *rbuf =
          &bp->rbuf_list[dest * NUM_BUFFERS_PER_DEST + idx];
      // we loose credits per transaction
      bp->credits[dest] = --cr;

      ret.rc = RBUF_FETCH_OK;
      ret.rbuf = rbuf;
      return ret;
    } else {
      // print error for now
      // TODO handle this case

      ret.rc = RBUF_FETCH_ERROR;
      printf("error in acquiring remote RDMA buffer - "
             "Not enough buffer space for message (size) : %d !!\n",
             buffer_size);
    }
  }
  return ret;
}

int PHOTON_remote_put_wc(photon_nbc_req *req, buffer_pool_t *bp, uint64_t tag) {
  int dest = req->sink;
  uint64_t size = req->buf_size;
  get_remote_ret_t ret = get_remote_photon_buffer(dest, size, bp);

  /*printf("######################## [TRY PHOTON_put_wc ] rank: %d dest : %d
   * value: %d  tag: [%d] \n", _photon_myrank, dest, *(int*)bp->send[dest],
   * tag);*/
  if (ret.rc == RBUF_FETCH_OK) {
    struct photon_buffer_t *rbuf_ptr = ret.rbuf;
    struct photon_buffer_t lbuf;
    lbuf.addr = (uintptr_t)bp->send[dest];
    lbuf.size = size;
    lbuf.priv = (struct photon_buffer_priv_t){0, 0};
    // send encoded tags
    uint64_t local = PHOTON_LOCAL_TAG_1;
    _encode_tag(&local, tag);
    uint64_t remote = PHOTON_REMOTE_TAG_1;
    _encode_tag(&remote, tag);

    photon_cid lid = {.u64 = local, .size = 0};
    photon_cid rid = {.u64 = remote, .size = 0};
    /*printf("######################## PHOTON_put_wc rank: %d dest : %d  value:
     * %d  tag: [%d] \n", _photon_myrank, dest, *(int*)lbuf.addr, tag);*/
    _photon_put_with_completion(dest, size, &lbuf, rbuf_ptr, lid, rid,
                                PHOTON_REQ_PWC_NO_LCE, PWC_CTYPE_COLL,
                                PWC_COMMAND_ALLREDUCE);
    return NBC_PHOTON_REQ_PENDING;
  } else if (ret.rc == RBUF_FETCH_ERROR_ZERO_CREDIT) {
    // do not do `put`
    // buffer instead ?? to use when the destination remote buffers are ready
    /*_NBC_photon_enqueue(&bp->temp_pwc_reqs, req);*/
    return NBC_PHOTON_REQ_NOT_READY;
  } else {
    printf("PHOTON_put_wc failed. error code : [%d] ! \n", ret.rc);
    return NBC_PHOTON_REQ_ERROR;
  }
}

static int _send_credits(uint64_t *sources, int n, uint64_t tag) {
  int i;
  uint64_t local = PHOTON_LOCAL_TAG_2;
  _encode_tag(&local, tag);
  uint64_t remote = PHOTON_REMOTE_TAG_2;
  _encode_tag(&remote, tag);

  photon_cid lid = {.u64 = local, .size = 0};
  photon_cid rid = {.u64 = remote, .size = 0};
  for (i = 0; i < n; i++) {
    _photon_put_with_completion(sources[i], 0, NULL, NULL, lid, rid,
                                PHOTON_REQ_PWC_NO_LCE, PWC_CTYPE_COLL,
                                PWC_COMMAND_ALLREDUCE);
  }
  return 0;
}

void PHOTON_Check_credits(buffer_pool_t *bp, completion_info_t **comp,
                          int comp_size, int tag) {
  int remaining = comp_size;
  while (remaining > 0) {
    completion_info_t *event = comp[comp_size - remaining];
    // get current probe info
    if (!event->done && event->req.u64 == PHOTON_REMOTE_TAG_2) {
      // sender side ACK from reciever 'src'
      // nice !! we have recieved credits from a reciever
      /*printf("##### I rank : %d have recieved credits from src : %d \n",
       * _photon_myrank, event->src);*/
      bp->credits[event->src] += 1;
      event->done = true;
    } else if (event->req.u64 == PHOTON_LOCAL_TAG_2) {
      // nothing to do with local completion of credit send
      event->done = true;
    }
    remaining--;
  }

  if (bp->credits_released > 0) {
    void *queue_ent = sync_two_lock_queue_dequeue(bp->credits_sources);
    while (queue_ent != NULL) {
      uint64_t src = (uint64_t)queue_ent;
      if (src == ZERO_ERANK) {
        src = ZERO_RANK;
      }
      /*printf("##### I rank : %d have credits to send to dest: %d \n",
       * _photon_myrank, src);*/
      _send_credits(&src, 1, 0);
      bp->credits_released--;

      // iterate next
      queue_ent = sync_two_lock_queue_dequeue(bp->credits_sources);
    }
  }
}

#ifdef NBC_PHOTON_DEBUG_ON
int call_iter = 0;
#endif

bool PHOTON_Testall_PWC(int req_count, photon_nbc_req *req_array, int *done,
                        buffer_pool_t *bp, recv_info_t *recvs,
                        completion_info_t **comp, int comp_size, uint64_t tag) {
  /*photon_rid request;*/
  /*printf("=====PHOON TEST ALL rank %d src %d \n", _photon_myrank,
   * comp->src);*/
  photon_cid request;
  int flag, src;
  int i, remaining;

  int send_pending = 0;
  int send_not_ready = 0;
  int recv_pending = 0;
  /*int target_source_ranks[NUM_PROCS_ULIMIT];	*/
  int num_local_sends = 0;
  int target_send_reqs[req_count];
// todo remove
#ifdef NBC_PHOTON_DEBUG_ON
  int myrank = -1;
#endif
  *done = 0;

  // do flow control
  PHOTON_Check_credits(bp, comp, comp_size, tag);
  // request counting for current NBC round
  nbc_photon_rstate_t rstate;
  rstate = _check_sends(req_count, req_array, bp, target_send_reqs, tag);
  send_pending = rstate.send_pending;
  send_not_ready = rstate.send_not_ready;
  rstate = _check_recieves(req_count, req_array, bp, recvs, tag);
  recv_pending = rstate.recv_pending;

  // we progress all pending local sends - fast
  remaining = comp_size;
  while (remaining > 0) {
    completion_info_t *event = comp[comp_size - remaining];

    // get current probe info
    request = event->req;
    src = event->src;
    flag = !event->done;

#ifdef NBC_PHOTON_DEBUG_ON
    printf("probe rank [%d] call==[%d] flag [%d] rem [%d] req [%ld] src[%d] rc "
           "[%d] send_pending [%d] recv_pending [%d] send_not_ready[%d]  tag : "
           "[%d]\n",
           myrank, call_iter++, flag, remaining, request.u64, src, rc,
           send_pending, recv_pending, send_not_ready, tag);
    fflush(stdout);
#endif

    if (flag) {
      if (request.u64 == PHOTON_LOCAL_TAG_1) {
        req_array[target_send_reqs[num_local_sends++]].status =
            NBC_PHOTON_REQ_COMPLETED;
        send_pending--;
        event->done = true;
        /*printf("=====send rank %d \n", myrank);*/
      } else if (request.u64 == PHOTON_REMOTE_TAG_1) {
        add(src, recvs->ordered_recvs);
        event->done = true;
        /*printf("=====recv rank %d src %d \n", _photon_myrank, src);*/
        // reciever side - recv request complete
        bool src_matched = false;
        ;
        for (i = 0; i < req_count; ++i) {
          if (!req_array[i].send_req &&
              delete (req_array[i].sink, recvs->ordered_recvs)) {
            if (event->eager_size > 0) {
              // eager case
              assert(req_array[i].buf_size == event->eager_size);
              memcpy(req_array[i].req_buf, event->eager_data,
                     event->eager_size);
              _release_credits(req_array[i].sink, bp);
            } else {
              memcpy(req_array[i].req_buf,
                     _RBUF(bp, req_array[i].sink, req_array[i].rselect),
                     req_array[i].buf_size);
              _release_credits(req_array[i].sink, bp);
            }
            if (req_array[i].sink == src) {
              src_matched = true;
            }
            /*printf("[RECV]recv rank %d src %d  result : %d target : %p tag :
             * %d \n", _photon_myrank, src, *res, res, tag);*/
            req_array[i].status = NBC_PHOTON_REQ_COMPLETED;
            recv_pending--;
          }
        }
        if (!src_matched) {
          // we haven't seen a match for curent event/s - maybe future round
          // this case means that incase this event contains a eager buffer
          // copy that to temporary buffer in buffer pool manager for a future
          // match
          if (event->eager_size > 0) {
            int size = event->eager_size;
            recvs->eager_recvs[src].data = malloc(size);
            memcpy(recvs->eager_recvs[src].data, event->eager_data, size);
            recvs->eager_recvs[src].size = size;
            _release_credits(src, bp);
          }
        }
      }
    }
    remaining--;
  }

  /*PHOTON_Check_credits(bp, comp, comp_size, tag);*/
  // if i complete the remaining remaining recv reqs AND the non-ready puts then
  // i am done in this round
  /*printf("[NBC_TEST]######### NBC round test ==> rank : %d | send_pending : %d
   * recv_pending : %d send_not_ready : %d   tag : [%d]... \n",
   * _photon_myrank,*/
  /*send_pending, recv_pending, send_not_ready, tag);*/
  if (send_pending <= 0 && recv_pending <= 0 && send_not_ready <= 0) {
    /*printf("[NBC_TEST ROUND DONE]######### rank : %d \n", _photon_myrank);*/
    *done = 1;
  }
  return PH_OK;
}
