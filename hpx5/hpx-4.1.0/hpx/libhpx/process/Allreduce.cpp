// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "Allreduce.h"
#include "libhpx/action.h"
#include "libhpx/collective.h"
#include "libhpx/debug.h"
#include "libhpx/parcel.h"
#include "libhpx/Network.h"
#include <string.h>
#include <mutex>

namespace {
using libhpx::process::Allreduce;
}

HPX_ACTION_DECL(Allreduce::Init);
HPX_ACTION_DECL(Allreduce::Fini);
HPX_ACTION_DECL(Allreduce::Add);
HPX_ACTION_DECL(Allreduce::Remove);
HPX_ACTION_DECL(Allreduce::Join);
HPX_ACTION_DECL(Allreduce::BCast);
HPX_ACTION_DECL(Allreduce::BCastComm);

void
Allreduce::InitializeActions()
{
  LIBHPX_REGISTER_ACTION(HPX_INTERRUPT, HPX_PINNED, Allreduce::Init,
                         Allreduce::InitHandler, HPX_POINTER,
                         HPX_SIZE_T, HPX_ADDR, HPX_ACTION_T, HPX_ACTION_T);
  LIBHPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, Fini, Allreduce::FiniHandler,
                         HPX_POINTER);
  LIBHPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, Allreduce::Add,
                         Allreduce::AddHandler, HPX_POINTER, HPX_ACTION_T,
                         HPX_ADDR);
  LIBHPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, Allreduce::Remove,
                         Allreduce::RemoveHandler, HPX_POINTER, HPX_UINT);
  LIBHPX_REGISTER_ACTION(HPX_INTERRUPT, HPX_PINNED | HPX_MARSHALLED,
                         Allreduce::Join, Allreduce::JoinHandler, HPX_POINTER,
                         HPX_POINTER, HPX_SIZE_T);
  LIBHPX_REGISTER_ACTION(HPX_INTERRUPT, HPX_PINNED | HPX_MARSHALLED,
                         Allreduce::BCast, Allreduce::BCastHandler, HPX_POINTER,
                         HPX_POINTER, HPX_SIZE_T);
  LIBHPX_REGISTER_ACTION(HPX_INTERRUPT, HPX_PINNED | HPX_MARSHALLED,
                         Allreduce::BCastComm, Allreduce::BCastCommHandler,
                         HPX_POINTER, HPX_POINTER, HPX_SIZE_T);
}

Allreduce::Allreduce(size_t bytes, hpx_addr_t parent,
                     hpx_monoid_id_t id, hpx_monoid_op_t op)
    : bytes_(bytes),
      parent_(parent),
      lock_(1),
      continuation_(bytes),
      reduce_(new(bytes) Reduce(bytes, id, op)),
      id_(-1),
      ctx_(nullptr)
{
  // allocate memory for data structure plus for rank data
  // optimistic allocation for ranks - for all localities
  int ctx_bytes = sizeof(coll_t) + sizeof(int32_t) * HPX_LOCALITIES;
  coll_t *ctx = static_cast<coll_t*>(malloc(ctx_bytes));
  ctx->group_bytes = sizeof(int32_t) * HPX_LOCALITIES;
  memset(ctx, 0, sizeof(coll_t) + ctx->group_bytes);
  ctx->comm_bytes = 0;
  ctx->group_sz = 0;
  ctx->recv_count = bytes;
  ctx->type = ALL_REDUCE;
  ctx->op = op;
  ctx_ = ctx;
}

Allreduce::~Allreduce() {
  free(ctx_);
  delete reduce_;
}

unsigned
Allreduce::add(hpx_action_t op, hpx_addr_t addr)
{
  // acquire the semaphore
  std::lock_guard<Sema> _(lock_);

  // extend the local continuation structure and get and id for this input
  unsigned i = continuation_.insert(op, addr);

  // if i am the root then add leaf node into to active locations
  if (!parent_) {
    coll_t *ctx = static_cast<coll_t*>(ctx_);
    int i = ctx->group_sz++;
    int32_t *ranks = reinterpret_cast<int32_t *>(ctx->data);
    if (here->ranks > 1) {
      ranks[i] = here->gas->ownerOf(addr);
    } else {
      // smp mode
      ranks[i] = 0;
    }
  }

  // extend the local reduction, if this is the first input then we need to
  // recursively tell our parent (if we have one) that we exist, and that we
  // need to have our bcast action run as a continuation
  if (reduce_->add() && parent_) {
    hpx_addr_t allreduce = hpx_thread_current_target();
    dbg_check( hpx_call_sync(parent_, Add, &id_, sizeof(id_), &BCast,
                             &allreduce) );
  }

  return i;
}

void
Allreduce::remove(unsigned id)
{
  // acquire the lock
  std::lock_guard<Sema> _(lock_);

  // remove the continuation that is leaving
  continuation_.remove(id);

  // remove this input from our allreduce, if this is the last input then tell
  // our parent that we are no longer participating
  if (reduce_->remove() && parent_) {
    dbg_check( hpx_call_sync(parent_, Remove, NULL, 0, &id_) );
    id_ = -1;
  }
}


void
Allreduce::reduce(const void *val)
{
  log_coll("reducing at %p\n", this);

  // if this isn't the last local value then just continue
  if (!reduce_->join(val)) {
    return;
  }

  if (here->config->coll_network && parent_) {
    // for sw based direct collective join
    // create parcel and prepare for coll call
    // hpx_parcel_t *p = hpx_parcel_acquire(NULL, bytes_);
    // void* in =  hpx_parcel_get_data(p);
    void *output = calloc(bytes_, sizeof(char));
    void *in = calloc(bytes_, sizeof(char));
    reduce_->reset(in);

    // perform synchronized collective comm
    here->net->sync(in, bytes_, output, ctx_);

    // call all local continuations to communicate the result
    continuation_.trigger(output);
    free(output);
    free(in);
  }
  // the local continuation is done, join the parent node asynchronously
  else if (parent_) {
    hpx_parcel_t *p = hpx_parcel_acquire(NULL, bytes_);
    p->target = parent_;
    p->action = Join;
    reduce_->reset(hpx_parcel_get_data(p));
    parcel_launch(p);
  }
  else {
    // this is a root node, so turn around and run all of our continuations
    void *result = malloc(bytes_);
    reduce_->reset(result);
    bcast(result);
    free(result);
  }
}

void
Allreduce::bcast(const void *value)
{
  log_coll("broadcasting at %p\n", this);
  // just trigger the continuation stored in this node
  continuation_.trigger(value);
}

void
Allreduce::bcast_comm(hpx_addr_t base, const void *coll)
{
  log_coll("broadcasting comm ranks from root %p\n", this);

  const coll_t *ctx = static_cast<const coll_t*>(ctx_);
  size_t bytes = sizeof(coll_t) + ctx->group_bytes;

  // boradcast my comm group to all leaves
  // this is executed only on network root
  if (coll == NULL) {
    int n = here->ranks;
    hpx_addr_t target = HPX_NULL;
    hpx_addr_t lco = hpx_lco_and_new(n);
    for (int i = 0; i < n; ++i) {
      if (here->rank != unsigned(i)) {
        target = hpx_addr_add(base, i * sizeof(*this), sizeof(*this));
        hpx_call(target, BCastComm, lco, ctx, bytes);
      }
    }

    target = hpx_addr_add(base, here->rank * sizeof(*this), sizeof(*this));
    // order sends to remote rank first and local rank then; here
    // provided network is flushed ,this will facilitate blocking call
    // for a collective group creation in bcast_comm
    hpx_call(target, BCastComm, lco, ctx, bytes);

    hpx_lco_wait(lco);
    hpx_lco_delete_sync(lco);

    return;
  }

  // set the collective context in current leaf node
  // this is executed only in leaves
  const coll_t *c = static_cast<const coll_t*>(coll);
  bytes = sizeof(*c) + c->group_bytes;
  dbg_assert(ctx->group_bytes >= 0 && size_t(ctx->group_bytes) >= bytes);
  memcpy(ctx_, c, bytes);

  int32_t *ranks = (int32_t *)ctx->data;
  int32_t *copy_ranks = (int32_t *)c->data;
  for (int i = 0; i < c->group_sz; ++i) {
    ranks[i] = copy_ranks[i];
  }
  // perform collective initialization for all leaf nodes here
  dbg_check(here->net->init(&ctx_));
}

int
Allreduce::BCastCommHandler(Allreduce* r, void *value, size_t bytes)
{
  if (!r->parent_) {
    // if root network node we pass only the base address for bcast
    dbg_assert(bytes == sizeof(hpx_addr_t));
    hpx_addr_t base = *((hpx_addr_t *)value);
    r->bcast_comm(base, NULL);
  }
  else {
    coll_t *ctx = static_cast<coll_t *>(value);
    dbg_assert(bytes == (sizeof(coll_t) + ctx->group_bytes));
    r->bcast_comm(HPX_NULL, ctx);
  }
  return HPX_SUCCESS;
}

int
Allreduce::InitHandler(void *buffer, size_t bytes, hpx_addr_t parent,
                       hpx_action_t id, hpx_action_t op)
{
  hpx_monoid_id_t rid = (hpx_monoid_id_t)actions[id].handler;
  hpx_monoid_op_t rop = (hpx_monoid_op_t)actions[op].handler;
  Allreduce* r = new(buffer) Allreduce(bytes, parent, rid, rop);
  assert(r);
  return HPX_SUCCESS;
}

