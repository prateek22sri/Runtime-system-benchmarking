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

#include "photon_pwc_nbc.h"
#include "photon_request.h"
#include <string.h>

static int nbc_coll_init(photonConfig cfg);
static int nbc_comm_create(void *c, void *active, int num_active, int total);
static photon_coll_comm nbc_comm_get();
static int nbc_coll_probe(int proc, int *flag, photon_cid *c);
static int nbc_cid_handler(int proc, pwc_command cmd, photon_cid cid,
                           void *data, int size);
static int nbc_allreduce(void *sendbuf, void *out, int count, void *datatype,
                         void *op, void *comm);
static int nbc_iallreduce(void *sendbuf, void *out, int count, void *datatype,
                          void *op, void *comm, photonRequest req);
static int _nbc_setup_comm(void *c);

static void _nbc_map_handle(photonRequest req, NBC_Handle *handle);
static NBC_Handle *_nbc_get_handle(photon_cid cid);

static void _decode_handle_tag(uint64_t intag, uint64_t *handle_id_out) {
  *handle_id_out = (intag >> 4);
}

static void _decode_comp_tag(uint64_t *intag) {
  *intag = ((*intag) & 0xf);
}

/*************** Mempool Implementation Start ****************/
/// Mempool Internal
static int init_mempool(MemPool_t *p) {
  p->array_sz = MEMPOOL_INIT_SZ;
  p->ptr_array = malloc(sizeof(void *) * p->array_sz);
  return p->ptr_array == NULL;
}

static int reallocate_mempool(MemPool_t *p) {
  void **temp = realloc(p->ptr_array, 2 * p->array_sz * sizeof(void *));
  if (temp) {
    p->array_sz *= 2;
    p->ptr_array = temp;
    // printf("realloc success :%ld \n", p->array_sz);
  } else {
    // realloc failed
    return 1;
  }
  return 0;
}

static void put_mempool(MemPool_t *p, size_t idx, void *ptr) {
  if (idx >= p->array_sz && reallocate_mempool(p)) {
    fprintf(
        stderr,
        "MemPool put operation [idx : %ld, p : %p] failed, realloc() failure !",
        idx, ptr);
  }
  if (idx < p->array_sz) {
    p->ptr_array[idx] = ptr;
  } else {
    fprintf(stderr, "MemPool put operation reached array out of bounds [idx : "
                    "%ld, p : %p] !",
            idx, ptr);
  }
}

static void *get_mempool(MemPool_t *p, size_t idx) {
  if (idx < p->array_sz) {
    return p->ptr_array[idx];
  } else {
    fprintf(stderr,
            "MemPool get operation reached array out of bounds [idx : %ld] !",
            idx);
  }
  return NULL;
}

static void clean_mempool(MemPool_t *p) {
  if (p->ptr_array) {
    free(p->ptr_array);
  }
}
/// Mempool External interface for inititialization
void Init_MemPool(MemPool_t *p) {
  if (!p) {
    fprintf(stderr, "MemPool initialization failed [mempool : %p] !", p);
  }
  p->put = put_mempool;
  p->get = get_mempool;
  p->clean = clean_mempool;

  // init mempool
  int ret = init_mempool(p);
  if (ret) {
    fprintf(stderr,
            "MemPool initialization/allocation failed [mempool array : %p] !",
            p->ptr_array);
  }
}
/*************** Mempool Implementation End ****************/

static inline void _photon_to_nbc_optype(photon_coll_op *ph_op,
                                         MPI_Op *nbc_op) {
  switch (*ph_op) {
    case PHOTON_COLL_OP_SUM: {
      *nbc_op = MPI_SUM;
      break;
    }
    case PHOTON_COLL_OP_MIN: {
      *nbc_op = MPI_MIN;
      break;
    }
    case PHOTON_COLL_OP_MAX: {
      *nbc_op = MPI_MAX;
      break;
    }
    default: {
      *nbc_op = MPI_MAX;
      log_err("Could not find a nbc operation type conversion!");
      break;
    }
  }
}

static inline void _photon_to_nbc_datatype(photon_datatype *ph_dt,
                                           MPI_Datatype *nbct) {
  switch (*ph_dt) {
    case PHOTON_INT32: {
      *nbct = MPI_INT;
      break;
    }
    case PHOTON_FLOAT: {
      *nbct = MPI_FLOAT;
      break;
    }
    case PHOTON_DOUBLE: {
      *nbct = MPI_DOUBLE;
      break;
    }
    case PHOTON_INT8: {
      *nbct = MPI_BYTE;
      break;
    }
    case PHOTON_INT16: {
      *nbct = MPI_SHORT;
      break;
    }
    case PHOTON_INT64: {
      *nbct = MPI_LONG;
      break;
    }
    default: {
      *nbct = MPI_INT;
      log_err("Could not find a nbc data type conversion\n");
      break;
    }
  }
}

photon_coll_interface nbc_collectives = {.init = nbc_coll_init,
                                         .comm_create = nbc_comm_create,
                                         .comm_get = nbc_comm_get,
                                         .probe = nbc_coll_probe,
                                         .cid_handler = nbc_cid_handler,
                                         .barrier = NULL,
                                         .ibarrier = NULL,
                                         .allreduce = nbc_allreduce,
                                         .iallreduce = nbc_iallreduce,
                                         .scan = NULL,
                                         .iscan = NULL};

// handle array to test for
typedef struct HandleEntry {
  TAILQ_ENTRY(HandleEntry) next;
  NBC_Handle *val;
} HandleEntry;

typedef TAILQ_HEAD(HandleList, HandleEntry) HandleList;

static tatas_lock_t nbc_lock;
static HandleList *nbc_handle_q;
static MemPool_t free_handles;
static photon_coll_comm _nbc_default_comm;
;
static uint64_t _nbc_handle_id = 1;
static NBC_Handle _nbc_default_handle;
static two_lock_queue_t *nbc_comp_event_q;

static inline uint64_t _nbc_get_handle_tag() {
  uint64_t tag = sync_fadd(&_nbc_handle_id, 1, SYNC_RELAXED);
  return tag;
}

static int nbc_coll_init(photonConfig cfg) {
  sync_tatas_init(&nbc_lock);
  nbc_handle_q = malloc(sizeof(HandleList));
  TAILQ_INIT(nbc_handle_q);
  Init_MemPool(&free_handles);

  int rc = _nbc_setup_comm(&_nbc_default_comm);
  if (rc != PHOTON_OK) {
    log_err("Could not initialize default communicator");
    return PHOTON_ERROR;
  }

  NBC_Init_handle(&_nbc_default_handle, _nbc_default_comm);
  _nbc_default_handle.tag = 0;
  _nbc_default_handle.completed = false;
  _nbc_map_handle(NULL, &_nbc_default_handle);
  // make default handle immediately available for testing
  push_back_to_test_queue(&_nbc_default_handle);
  // setup allreduce accounting
  nbc_comp_event_q = sync_two_lock_queue_new();

  return PHOTON_OK;
}

static photon_coll_comm nbc_comm_get() {
  return _nbc_default_comm;
}

static int nbc_coll_probe(int proc, int *flag, photon_cid *c) {
  int i, event_idx = 0;
  int MAX_EVENTS = 50;
  completion_info_t *comp_event = get_nbc_comp_event();
  completion_info_t *temp_event_array[MAX_EVENTS];

  while ((comp_event != NULL) && (event_idx < MAX_EVENTS)) {
    NBC_Handle *handle = _nbc_get_handle(comp_event->req);

    if (handle == NULL) {
      // handle not initialized yet - push back the event
      temp_event_array[event_idx++] = comp_event;
      goto next;
    } else {
      // decode into actual tag here
      _decode_comp_tag(&comp_event->req.u64);
      NBC_Handle_put_event(handle, comp_event);
      // push_back_to_test_queue(handle);
    }
  next:
    comp_event = get_nbc_comp_event();
  }

  // iterate handles/ progress
  test_some_nbc_handles();

  // push back any residual events
  for (i = 0; i < event_idx; ++i) {
    add_nbc_comp_event(temp_event_array[i]);
  }

  return PHOTON_OK;
}

static int nbc_cid_handler(int proc, pwc_command cmd, photon_cid cid,
                           void *data, int size) {
  switch (cmd) {
    case PWC_COMMAND_ALLREDUCE: {
      photon_cid comp;
      // always expecting 8 bytes here
      if (cid.size) {
        comp.u64 = *(uint64_t *)cid.data;
      } else {
        comp.u64 = cid.u64;
      }
      comp.size = 0;

      completion_info_t *comp_event =
          (completion_info_t *)malloc(sizeof(completion_info_t));
      comp_event->req = comp;
      comp_event->src = proc;
      comp_event->flag = 1;
      comp_event->local = (proc == _photon_myrank) ? true : false;
      comp_event->eager_data = NULL;
      comp_event->eager_size = 0;
      comp_event->done = false;

      if (size != 0 && data != NULL) {
        comp_event->eager_data = malloc(sizeof(size));
        memcpy(comp_event->eager_data, data, size);
        comp_event->eager_size = size;
      }

      add_nbc_comp_event(comp_event);
    } break;
    default: break;
  }

  return PHOTON_OK;
}

static int nbc_comm_create(void *c, void *active, int num_active, int total) {
  if (active != NULL) {
    *((NBC_Comminfo **)c) = malloc(sizeof(NBC_Comminfo));
    NBC_Comminfo *comm = *((NBC_Comminfo **)c);

    if (comm == NULL) {
      log_err("Error allocating communicator");
      return PHOTON_ERROR;
    }

    int res = NBC_Init_comm(_photon_myrank, active, num_active, total, comm);
    // _photon_save_comm(comm);
    dbg_info("NBC Init completed");
    if (res != NBC_OK) {
      log_err("Error NBC Initialization()");
      NBC_Finalize(comm);
      return PHOTON_ERROR;
    }
  }

  return PHOTON_OK;
}

static int _nbc_setup_comm(void *c) {
  int i, ret;
  int active[_photon_nproc];
  for (i = 0; i < _photon_nproc; ++i) {
    active[i] = i;
  }
  ret = nbc_comm_create(c, active, _photon_nproc, _photon_nproc);
  return ret;
}

static int nbc_allreduce(void *sendbuf, void *out, int count, void *datatype,
                         void *op, void *comm) {
  return PHOTON_ERROR;
}

static int nbc_iallreduce(void *sendbuf, void *out, int count, void *datatype,
                          void *op, void *comm, photonRequest req) {
  // iallreduce requests are enqueued in the order in which they are called
  int ret;
  NBC_Comminfo *nbc_comm = (NBC_Comminfo *)comm;
  NBC_Handle *handle = malloc(sizeof(NBC_Handle));
  handle->reqid = req->id;
  handle->completed = false;

  photon_datatype *ph_dt = (photon_datatype *)datatype;
  photon_coll_op *ph_op = (photon_coll_op *)op;
  MPI_Datatype mdt;
  MPI_Op mpi_op;
  _photon_to_nbc_datatype(ph_dt, &mdt);
  _photon_to_nbc_optype(ph_op, &mpi_op);

  // set nbc handle tag
  handle->tag = _nbc_get_handle_tag();
  sync_tatas_acquire(&nbc_lock);
  { ret = NBC_Iallreduce(sendbuf, out, count, mdt, mpi_op, nbc_comm, handle); }

  sync_tatas_release(&nbc_lock);
  // TODO: map a NBC handle to this request
  push_back_to_test_queue(handle);
  _nbc_map_handle(req, handle);

  return ret;
}

// TODO important function to make nbc work!
// req index should be a function of completion id
NBC_Handle *_nbc_get_handle(photon_cid cid) {
  NBC_Handle *hnl = NULL;
  uint64_t tagid;
  _decode_handle_tag(cid.u64, &tagid);

  // note no contention for default handle
  if (tagid == 0) {
    return &_nbc_default_handle;
  }
  sync_tatas_acquire(&nbc_lock);
  {
    NBC_Comminfo *comm = _nbc_default_comm;
    if (comm != NULL) {
      hnl = NBC_Handle_get(tagid, comm);
      /*printf("NBC get handle myrank : %d  handle-id : %d \n", _photon_myrank,
       * tagid);*/
    }
  }
  sync_tatas_release(&nbc_lock);

  return hnl;
}

void _nbc_map_handle(photonRequest req, NBC_Handle *handle) {
  sync_tatas_acquire(&nbc_lock);
  {
    // req accounting
    NBC_Cache_Handle(handle);
  }
  sync_tatas_release(&nbc_lock);
}

// these are the handles to test
// insert if handle is not already on the queue (ie:-no event has been tested
// under this handle)
void push_back_to_test_queue(NBC_Handle *handle) {
  sync_tatas_acquire(&nbc_lock);
  {
    HandleEntry *np;
    np = malloc(sizeof(HandleEntry));
    np->val = handle;
    TAILQ_INSERT_TAIL(nbc_handle_q, np, next);
  }
  sync_tatas_release(&nbc_lock);
}

int test_some_nbc_handles() {
  sync_tatas_acquire(&nbc_lock);
  {
    int i, idx = 0;
    HandleEntry *np;
    TAILQ_FOREACH(np, nbc_handle_q, next) {
      NBC_Handle *handle = np->val;
      /*printf("NBC Handle test. handle : %p tag : %d \n", handle, handle->tag);
       */
      int ret = NBC_Test(handle);

      // we are done if NBC returned ok
      int flag = (ret == NBC_OK);

      if (flag && (handle->tag != 0)) {
        /*printf("NBC Handle complete... handle: %p tag : %d rank : %d \n",
         * handle, handle->tag, _photon_myrank);	    */
        if (!handle->completed) {
          photonRequest req;
          req = photon_lookup_request(handle->reqid);
          assert(req);
          photon_pwc_add_req(req);
          handle->completed = true;
        }
        free_handles.put(&free_handles, idx++, np);
      }
    }

    // free completed handles
    for (i = 0; i < idx; ++i) {
      HandleEntry *n = free_handles.get(&free_handles, i);
      TAILQ_REMOVE(nbc_handle_q, n, next);
      NBC_Cache_Handle_remove(n->val);
      free(n);
    }
  }
  sync_tatas_release(&nbc_lock);
  return PHOTON_OK;
}

void add_nbc_comp_event(completion_info_t *comp) {
  sync_two_lock_queue_enqueue(nbc_comp_event_q, comp);
}

completion_info_t *get_nbc_comp_event() {
  return sync_two_lock_queue_dequeue(nbc_comp_event_q);
}
