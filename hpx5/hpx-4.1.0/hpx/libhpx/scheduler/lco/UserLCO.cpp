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

/// @file libhpx/scheduler/user_lco.c
/// @brief A user-defined LCO.

#include "LCO.h"
#include "Condition.h"
#include "libhpx/action.h"
#include "libhpx/debug.h"
#include "libhpx/memory.h"
#include "libhpx/padding.h"
#include <mutex>
#include <cstring>

namespace {
using libhpx::scheduler::Condition;
using libhpx::scheduler::LCO;

class UserLCO final : public LCO {
 public:
  /// Allocate a user lco.
  ///
  /// This will serialize the initialization data into the end of the user
  /// data. This should only be called using a placement new operation into a
  /// buffer of size GetPaddedBytes(size, initSize).
  UserLCO(size_t size, hpx_action_t id, hpx_action_t op, hpx_action_t predicate,
          size_t initSize, const void *init);

  /// Delete a user lco.
  ///
  /// We don't expose any sort of user-programmable destructor, and all of the
  /// data has been serialized, so we don't really have anything extra to do
  /// here.
  ~UserLCO() {
    lock();
  }

  /// Set a user LCO.
  ///
  /// This will call the provided operation to reduce the data locally, and then
  /// it will check the predicate to see if the set operation triggered the LCO.
  int set(size_t size, const void *value);

  /// Get a user LCO's data.
  hpx_status_t get(size_t size, void *value, int reset);

  /// Get a reference to the user-lco data. For local operations this returns an
  /// internal pointer to the user data buffer, so we use the @p unpin parameter
  /// to tell the user if they can unpin the lco or not.
  hpx_status_t getRef(size_t size, void **out, int *unpin);

  /// Release a reference to the user lco data.
  bool release(void *out);

  /// Attach a continuation parcel to the user lco.
  hpx_status_t attach(hpx_parcel_t *p);

  /// Wait for the lco.
  hpx_status_t wait(int reset) {
    return get(0, nullptr, reset);
  }

  void error(hpx_status_t code) {
    std::lock_guard<LCO> _(*this);
    setTriggered();
    cvar_.signalError(code);
  }

  void reset() {
    std::lock_guard<LCO> _(*this);
    resetState();
  }

  size_t size(size_t size) const {
    return GetPaddedBytes(size_, initSize_);
  }

  void* getUserData() {
    return userData_;
  }

  static size_t GetPaddedBytes(size_t size, size_t initSize) {
    return sizeof(UserLCO) + size + _BYTES(8, size) + initSize;
  }

 private:
  void resetState() {
    resetTriggered();
    cvar_.reset();
    id();
  }

  void id() {
    auto f = (void (*)(void*, size_t, const void*, size_t))actions[id_].handler;
    f(userData_, size_, initData_, initSize_);
  }

  void op(const void* rhs, size_t size) {
    auto f = (hpx_monoid_op_t)actions[op_].handler;
    f(userData_, rhs, size);
  }

  bool predicate() const {
    auto f = (bool (*)(const void*, size_t))actions[predicate_].handler;
    return f(userData_, size_);
  }

  Condition               cvar_;                //<! The single user condition
  const size_t            size_;                //<! The size of the user data
  const char* const   initData_;                //<! Address of init data
  const size_t        initSize_;                //<! Size of the init data
  const hpx_action_t        id_;                //<! The id handler
  const hpx_action_t        op_;                //<! The operation
  const hpx_action_t predicate_;                //<! The predicate
  alignas(8) char     userData_[];              //<! The user data and init data
};

/// Function object class to deal with the variable-length nature of the
/// UserLCO initialization data.
class NewOp {
 public:
  NewOp(size_t size, hpx_action_t id, hpx_action_t op, hpx_action_t pred,
        size_t initSize, const void* init)
      : size_(size), initSize_(initSize), id_(id), op_(op), pred_(pred)
  {
    memcpy(init_, init, initSize);
  }

  /// Perform a construction call on a target address.
  int operator()(hpx_addr_t gva) const;

  /// Perform a construction call on a target address.
  int operator()(hpx_addr_t gva, hpx_addr_t sync) const;

  /// Remote entry point for the construct() handler.
  static int Handler(char* buffer, const NewOp& args, size_t) {
    return args.constructIn(buffer);
  }

 private:
  int constructIn(char* buffer) const {
    auto lva = new(buffer) UserLCO(size_, id_, op_, pred_, initSize_, init_);
    LCO_LOG_NEW(hpx_thread_current_target(), lva);
    return HPX_SUCCESS;
  }

  size_t size_;
  size_t initSize_;
  hpx_action_t id_;
  hpx_action_t op_;
  hpx_action_t pred_;
  char init_[];
};

LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, New, NewOp::Handler,
              HPX_POINTER, HPX_POINTER, HPX_SIZE_T);
}

UserLCO::UserLCO(size_t size, hpx_action_t id, hpx_action_t op,
                 hpx_action_t predicate, size_t initSize, const void *init)
    : LCO(LCO_USER),
      cvar_(),
      size_(size),
      initData_(userData_ + size_ + _BYTES(8, size_)),
      initSize_(initSize),
      id_(id),
      op_(op),
      predicate_(predicate)
{
  dbg_assert(size_);
  dbg_assert(id_);
  dbg_assert(op_);
  dbg_assert(predicate_);
  dbg_assert(!initData_ || init);
  memcpy(const_cast<char* const>(initData_), init, initSize);
  this->id();
}

int
UserLCO::set(size_t size, const void *from)
{
  dbg_assert(!size || from);

  std::lock_guard<LCO> _(*this);
  if (getTriggered()) {
    dbg_error("Cannot set an already set LCO\n");
  }

  op(from, size);

  if (!predicate()) {
    return 0;
  }

  setTriggered();
  cvar_.signalAll();
  return 1;
}

hpx_status_t
UserLCO::get(size_t size, void *out, int reset)
{
  dbg_assert(!size || (size == size_));
  dbg_assert(!size || out);

  std::lock_guard<LCO> _(*this);
  while (!getTriggered()) {
    if (auto status = waitFor(cvar_)) {
      return status;
    }
  }

  if (size) {
    memcpy(out, userData_, size_);
  }

  if (reset) {
    resetState();
  }

  return HPX_SUCCESS;
}

hpx_status_t
UserLCO::getRef(size_t size, void **out, int *unpin)
{
  dbg_assert(size && out);

  if (hpx_status_t status = wait(0)) {
    return status;
  }

  *out  = userData_;
  *unpin = 0;
  return HPX_SUCCESS;
}

bool
UserLCO::release(void *out)
{
  dbg_assert(out == userData_);
  return 1;
}

hpx_status_t
UserLCO::attach(hpx_parcel_t *p)
{
  std::lock_guard<LCO> _(*this);
  if (!getTriggered()) {
    return cvar_.push(p);
  }

  if (auto status = cvar_.getError()) {
    parcel_delete(p);                         // todo: send error?
    return status;
  }

  parcel_launch(p);
  return HPX_SUCCESS;
}

int
NewOp::operator()(hpx_addr_t gva) const
{
  return hpx_call_sync(gva, New, nullptr, 0, this, sizeof(*this) + initSize_);
}

int
NewOp::operator()(hpx_addr_t gva, hpx_addr_t sync) const
{
  return hpx_call(gva, New, sync, sizeof(*this) + initSize_);
}

hpx_addr_t
hpx_lco_user_new(size_t size, hpx_action_t id, hpx_action_t op,
                 hpx_action_t predicate, void *init, size_t initSize)
{
  hpx_addr_t gva = HPX_NULL;
  size_t bytes = UserLCO::GetPaddedBytes(size, initSize);
  try {
    UserLCO* lco = new(bytes, gva) UserLCO(size, id, op, predicate, initSize, init);
    hpx_gas_unpin(gva);
    LCO_LOG_NEW(gva, lco);
  }
  catch (const LCO::NonLocalMemory&) {
    void* buffer = alloca(sizeof(NewOp) + initSize);
    NewOp& f = *(new(buffer) NewOp(size, id, op, predicate, initSize, init));
    dbg_check( f(gva) );
  }
  return gva;
}

hpx_addr_t
hpx_lco_user_local_array_new(int n, size_t size, hpx_action_t id,
                             hpx_action_t op, hpx_action_t predicate,
                             void *init, size_t init_size)
{
  size_t bsize = UserLCO::GetPaddedBytes(size, init_size);
  hpx_addr_t base = lco_alloc_local(n, bsize, 0);
  if (!base) {
    throw std::bad_alloc();
  }

  void *buffer = alloca(sizeof(NewOp) + init_size);
  NewOp& f = *(new(buffer) NewOp(size, id, op, predicate, init_size, init));
  hpx_addr_t bcast = hpx_lco_and_new(n);
  for (int i = 0, e = n; i < e; ++i) {
    hpx_addr_t addr = hpx_addr_add(base, i * bsize, bsize);
    dbg_check( f(addr, bcast) );
  }
  hpx_lco_wait(bcast);
  hpx_lco_delete_sync(bcast);
  return base;
}

/// Get the user-defined LCO's user data. This allows to access the buffer
/// portion of the user-defined LCO regardless the LCO has been set or not.
void *
hpx_lco_user_get_user_data(void *lco)
{
  return static_cast<UserLCO*>(lco)->getUserData();
}
