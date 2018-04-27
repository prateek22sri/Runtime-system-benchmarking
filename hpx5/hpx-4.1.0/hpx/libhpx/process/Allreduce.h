// ==================================================================-*- C++ -*-
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

#ifndef LIBHPX_PROCESS_ALLREDUCE_H
#define LIBHPX_PROCESS_ALLREDUCE_H

#include "Continuation.h"
#include "Reduce.h"

namespace libhpx {
namespace process {
struct Allreduce {
 public:
  Allreduce(size_t bytes, hpx_addr_t parent, hpx_monoid_id_t id,
            hpx_monoid_op_t op);
  ~Allreduce();

  hpx_addr_t getParent() const {
    return parent_;
  }

  unsigned add(hpx_action_t op, hpx_addr_t addr);
  void remove(unsigned id);
  void reduce(const void *in);
  void bcast(const void *val);
  void bcast_comm(hpx_addr_t base, const void *coll);

  static HPX_ACTION_DECL(Init);
  static HPX_ACTION_DECL(Fini);
  static HPX_ACTION_DECL(Add);
  static HPX_ACTION_DECL(Remove);
  static HPX_ACTION_DECL(Join);
  static HPX_ACTION_DECL(BCast);
  static HPX_ACTION_DECL(BCastComm);

 private:
  static int InitHandler(void *buffer, size_t bytes, hpx_addr_t parent,
                        hpx_action_t id, hpx_action_t op);

  static int FiniHandler(Allreduce *r) {
    r->~Allreduce();
    return HPX_SUCCESS;
  }

  static int AddHandler(Allreduce *r, hpx_action_t op, hpx_addr_t addr) {
    unsigned i = r->add(op, addr);
    return HPX_THREAD_CONTINUE(i);
  }

  static int RemoveHandler(Allreduce *r, unsigned id) {
    r->remove(id);
    return HPX_SUCCESS;
  }

  static int JoinHandler(Allreduce *r, const void *value, size_t bytes) {
    r->reduce(value);
    return HPX_SUCCESS;
  }

  static int BCastHandler(Allreduce *r, const void *value, size_t bytes) {
    r->bcast(value);
    return HPX_SUCCESS;
  }

  static int BCastCommHandler(Allreduce* r, void *value, size_t bytes);

  [[ gnu::constructor ]] static void InitializeActions();

  class Sema {
   public:
    Sema(int i) : sema_(hpx_lco_sema_new(i)) {
    }

    ~Sema() {
      hpx_lco_delete_sync(sema_);
    }

    void lock() {
      hpx_lco_sema_p(sema_);
    }

    void unlock() {
      hpx_lco_sema_v_sync(sema_);
    }

   private:
    const hpx_addr_t sema_;
  };

  const size_t        bytes_;         // the size of the value being reduced
  const hpx_addr_t   parent_;         // our parent node
  Sema                 lock_;         // semaphore synchronizes add/remove
  Continuation continuation_;         // our continuation data
  Reduce            *reduce_;         // the local reduction
  unsigned               id_;         // our identifier for our parent
  void                 *ctx_;         // collective context info for this reduce
};
} // namespace process
} // namespace libhpx

#endif // LIBHPX_PROCESS_ALLREDUCE_H
