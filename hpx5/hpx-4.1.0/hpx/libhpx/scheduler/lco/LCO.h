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

#ifndef LIBHPX_SCHEDULER_LCO_H
#define LIBHPX_SCHEDULER_LCO_H

#include "TatasLock.h"
#include "libhpx/parcel.h"
#include "libhpx/events.h"

#include "hpx/hpx.h"
#include <cinttypes>
#include <memory>
#include <mutex>

/// The action used to propagate an LCO error.
///
/// Our registration macros don't work inside of namespaces, so this lives
/// external.
extern HPX_ACTION_DECL(lco_error);

namespace libhpx {
namespace scheduler {
class Condition;

/// The LCO abstract class interface.
///
/// All LCOs will implement this interface, which is accessible through the
/// hpx_lco set of generic actions. Concrete classes may implement extended
/// interfaces as well.
///
/// This interface is locally synchronous, but will be invoked externally
/// through the set of hpx_lco_* operations that may use them asynchronously.
class LCO {
 public:
  /// Virtual destructor as the LCO is designed as a superclass.
  virtual ~LCO();

  /// Operator new() overloads that allow us to allocate space for LCOs from
  /// global memory (and possibly remote global memory. These will throw a
  /// NonLocalMemory exception if the allocation succeeded but was
  /// non-local. They will throw a std::bad_alloc if the allocation failed
  /// entirely.
  ///
  /// Operator delete() is required by C++ when there's an operator new(), but
  /// we don't do anything in it because our new() operators will throw rather
  /// than returning nullptr.
  /// @{
  class NonLocalMemory : public std::exception {
  };

  static void* operator new(size_t, void* ptr);
  static void* operator new(size_t, hpx_addr_t& gva);
  static void* operator new(size_t, size_t bytes, hpx_addr_t& gva);
  static void operator delete(void* obj);       // does nothing
  /// @}

  /// The abstract LCO interface that needs to be implemented by all
  /// subclasses.
  /// @{
  virtual size_t size(size_t size) const = 0;
  virtual void error(hpx_status_t code) = 0;
  virtual int set(size_t size, const void *value) = 0;
  virtual hpx_status_t get(size_t size, void *value, int reset) = 0;
  virtual hpx_status_t wait(int reset) = 0;
  virtual hpx_status_t attach(hpx_parcel_t *p) = 0;
  virtual void reset() = 0;
  /// @}

  /// The virtual LCO interface that can be implemented in terms of the abstract
  /// LCO interface, but that can also be overridden in subclasses.
  /// @{
  virtual hpx_status_t getRef(size_t size, void **out, int *unpin);
  virtual bool release(void *out);
  /// @}

  /// Static action entry points for remote procedure call handling.
  /// @{
  static int DeleteHandler(LCO* lco);
  static int SetHandler(LCO* lco, void* data, size_t n);
  static int ErrorHandler(LCO* lco, void* args, size_t n);
  static int ResetHandler(LCO* lco);
  static int SizeHandler(const LCO* lco, int arg);
  static int GetHandler(LCO* lco, int n);
  static int WaitHandler(LCO *lco, int reset);
  static int AttachHandler(LCO *lco, hpx_parcel_t *p, size_t size);
  /// @}

  /// Lock and unlock the LCO. The owner pointer helps with debugging.
  /// @{
  void lock(hpx_parcel_t* owner);
  void unlock(hpx_parcel_t* owner);
  /// @}

  /// Implement the std BasicLockable concept for use with
  /// std::lock_guard. These forward to the explicit parcel version using
  /// self->current.
  /// @{
  void lock();
  void unlock();
  /// @}

 protected:
  /// The enumeration used for dynamically typing LCOs.
  enum Type : unsigned {
    LCO_ALLREDUCE = 0,
    LCO_ALLTOALL,
    LCO_AND,
    LCO_FUTURE,
    LCO_GATHER,
    LCO_GENCOUNT,
    LCO_REDUCE,
    LCO_SEMA,
    LCO_USER,
    LCO_DATAFLOW,
    LCO_MAX
  };

  /// Initialize the LCO with a dynamic type.
  LCO(enum Type type);

  /// Routines to interact with the LCO's state.
  /// @{
  short setTriggered();
  void resetTriggered();
  short getTriggered() const;
  void setUser();
  short getUser() const;
  /// @}

  /// Used in subclasses to wait for a condition.
  hpx_status_t waitFor(Condition& cond);

  /// Used in the operator new() context to try to pin a global address. The
  /// TryPin() operation will throw a NonLocalMemory exception if the gva
  /// represents a non-local address.
  /// @{
  static void* TryPin(hpx_addr_t gva);
  /// @}

 private:
  TatasLock<short> lock_;                       //<! The LCO's lock
  short           state_;                       //<! State bits
  Type             type_;                       //<! The LCO's dynamic type
};

/// Utility macro designed to aid in debugging.
#define LCO_LOG_NEW(gva, lva) do {                                \
    dbg_assert_str(gva, "Could not malloc global memory\n");      \
    log_lco("allocated lco %" PRIu64 " (%p)\n", gva, (void*)lva); \
  } while (0)

// Helper macros to allocate LCOs in the global address space
#define lco_alloc_local(n, size, boundary)                      \
  hpx_gas_alloc_local_attr(n, size, boundary, HPX_GAS_ATTR_LCO)

#define lco_alloc_cyclic(n, size, boundary)                      \
  hpx_gas_alloc_cyclic_attr(n, size, boundary, HPX_GAS_ATTR_LCO)

} // namespace scheduler
} // namespace libhpx

#endif // LIBHPX_SCHEDULER_LCO_H
