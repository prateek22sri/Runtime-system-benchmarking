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

#include "libhpx/ParcelStringOps.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/parcel.h"
#include <algorithm>

namespace {
using libhpx::network::ParcelStringOps;

class ParcelMemget {
  static HPX_ACTION_DECL(Request);
  static HPX_ACTION_DECL(Reply);
  static HPX_ACTION_DECL(Sync);

  struct ReplyArgs {
    char  *to;
    char from[];
  };

  static int
  ReplyHandler(const ReplyArgs& args, size_t n)
  {
    std::copy(args.from, args.from + n - sizeof(ReplyArgs), args.to);
    return HPX_SUCCESS;
  }

  static int
  RequestHandler(const char *from, char *to, size_t n, hpx_addr_t lsync)
  {
    auto current = hpx_thread_current_parcel();
    size_t bytes = sizeof(ReplyArgs) + n;
    hpx_addr_t target = HPX_THERE(current->src);
    hpx_action_t rop = hpx_lco_set_action;
    hpx_pid_t pid = current->pid;
    auto p = parcel_new(target, Reply, lsync, rop, pid, NULL, bytes);
    auto args = static_cast<ReplyArgs*>(hpx_parcel_get_data(p));
    args->to = to;
    std::copy(from, from + n, args->from);
    parcel_launch(p);
    return HPX_SUCCESS;
  }

  static int
  SyncHandler(const void *from, size_t n)
  {
    return hpx_thread_continue(from, n);
  }

 public:
  ParcelMemget() {
    LIBHPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, Reply, ReplyHandler,
                           HPX_POINTER, HPX_SIZE_T);
    LIBHPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, Request, RequestHandler,
                           HPX_POINTER, HPX_POINTER, HPX_SIZE_T, HPX_ADDR);
    LIBHPX_REGISTER_ACTION(HPX_INTERRUPT, HPX_PINNED, Sync, SyncHandler,
                           HPX_POINTER, HPX_SIZE_T);
  }

  void
  operator()(void *dest, hpx_addr_t from, size_t size, hpx_addr_t lsync,
             hpx_addr_t rsync)
  {
    hpx_action_t op  = Request;
    hpx_action_t rop = hpx_lco_set_action;
    action_call_lsync(op, from, rsync, rop, 3, &dest, &size, &lsync);
  }

  void
  operator()(void *dest, hpx_addr_t from, size_t size, hpx_addr_t lsync)
  {
    hpx_addr_t rsync = hpx_lco_future_new(0);
    operator()(dest, from, size, lsync, rsync);
    hpx_lco_wait(rsync);
    hpx_lco_delete_sync(rsync);
  }

  void
  operator()(void *dest, hpx_addr_t from, size_t size)
  {
    action_call_rsync(Sync, from, dest, size, 1, &size);
  }
} get;

HPX_ACTION_DECL(ParcelMemget::Request) = 0;
HPX_ACTION_DECL(ParcelMemget::Reply) = 0;
HPX_ACTION_DECL(ParcelMemget::Sync) = 0;

class ParcelMemput {
  static HPX_ACTION_DECL(Async);

  static int
  AsyncHandler(char *dest, const char *from, size_t n)
  {
    std::copy(from, from + n, dest);
    return HPX_SUCCESS;
  }
 public:
  ParcelMemput() {
    LIBHPX_REGISTER_ACTION(HPX_INTERRUPT, HPX_PINNED | HPX_MARSHALLED, Async,
                           AsyncHandler, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);
  }

  void
  operator()(hpx_addr_t dest, const void *from, size_t size, hpx_addr_t lsync,
             hpx_addr_t rsync)
  {
    hpx_action_t set = hpx_lco_set_action;
    action_call_async(Async, dest, lsync, set, rsync, set, 2, from, size);
  }

  void
  operator()(hpx_addr_t dest, const void *from, size_t size, hpx_addr_t rsync)
  {
    hpx_action_t set = hpx_lco_set_action;
    action_call_lsync(Async, dest, rsync, set, 2, from, size);
  }

  void
  operator()(hpx_addr_t dest, const void *from, size_t size)
  {
    action_call_rsync(Async, dest, NULL, 0, 2, from, size);
  }
} put;
HPX_ACTION_DECL(ParcelMemput::Async) = 0;

class ParcelMemcpy {
  static HPX_ACTION_DECL(Request);
  static HPX_ACTION_DECL(Reply);

  static int
  RequestHandler(const void *from, size_t n, hpx_addr_t dest)
  {
    return hpx_call_cc(dest, Reply, from, n);
  }

  static int
  ReplyHandler(char *dest, const char *data, size_t n)
  {
    std::copy(data, data + n, dest);
    return HPX_SUCCESS;
  }
 public:
  ParcelMemcpy() {
    LIBHPX_REGISTER_ACTION(HPX_DEFAULT, HPX_PINNED, Request, RequestHandler,
                           HPX_POINTER, HPX_SIZE_T, HPX_ADDR);
    LIBHPX_REGISTER_ACTION(HPX_INTERRUPT, HPX_PINNED | HPX_MARSHALLED, Reply,
                           ReplyHandler, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);
  }

  void
  operator()(hpx_addr_t dest, hpx_addr_t from, size_t n, hpx_addr_t rsync)
  {
    hpx_action_t set = hpx_lco_set_action;
    action_call_lsync(Request, from, rsync, set, 2, &n, &dest);
  }

  void
  operator()(hpx_addr_t dest, hpx_addr_t from, size_t n)
  {
    action_call_rsync(Request, from, NULL, 0, 2, &n, &dest);
  }
} copy;
HPX_ACTION_DECL(ParcelMemcpy::Request) = 0;
HPX_ACTION_DECL(ParcelMemcpy::Reply) = 0;

}

void
ParcelStringOps::memget(void *dest, hpx_addr_t from, size_t size,
                        hpx_addr_t lsync, hpx_addr_t rsync)
{
  get(dest, from, size, lsync, rsync);
}

void
ParcelStringOps::memget(void *dest, hpx_addr_t from, size_t size,
                        hpx_addr_t lsync)
{
  get(dest, from, size, lsync);
}

void
ParcelStringOps::memget(void *dest, hpx_addr_t from, size_t size)
{
  get(dest, from, size);
}

void
ParcelStringOps::memput(hpx_addr_t dest, const void *from, size_t size,
                        hpx_addr_t lsync, hpx_addr_t rsync)
{
  put(dest, from, size, lsync, rsync);
}

void
ParcelStringOps::memput(hpx_addr_t dest, const void *from, size_t size,
                        hpx_addr_t rsync)
{
  put(dest, from, size, rsync);
}

void
ParcelStringOps::memput(hpx_addr_t dest, const void *from, size_t size)
{
  put(dest, from, size);
}

void
ParcelStringOps::memcpy(hpx_addr_t dest, hpx_addr_t from, size_t size,
                        hpx_addr_t sync)
{
  copy(dest, from, size, sync);
}

void
ParcelStringOps::memcpy(hpx_addr_t dest, hpx_addr_t from, size_t size)
{
  copy(dest, from, size);
}
