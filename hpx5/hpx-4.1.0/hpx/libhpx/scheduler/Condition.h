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

#ifndef LIBHPX_SCHEDULER_CVAR_H
#define LIBHPX_SCHEDULER_CVAR_H

#include <hpx/hpx.h>

namespace libhpx {
namespace scheduler {
class LCO;

class Condition final {
 public:
  Condition();
  ~Condition();

  /// Reset a condition variable.
  void reset();

  /// Check if a condition variable is empty---there are not waiting
  /// threads associated with the condition variable.
  bool empty() const;

  /// Set the condition with an error code.
  ///
  /// This records the error in the condition, and returns the list of parcels
  /// that were waiting. The error code will overwrite this list, so it must be
  /// processed here.
  ///
  /// @param         code The user-defined error code.
  ///
  /// @returns            The previous list of waiting parcels.
  hpx_parcel_t *setError(hpx_status_t code);

  /// Retrieve an error code from the condition variable.
  ///
  /// This will check to see if an error is set on a condition variable, and
  /// return it if there is one.
  ///
  /// @returns            The error state for the condition variable.
  hpx_status_t getError() const;

  /// Clear an error condition.
  ///
  /// If there is no error condition set, then this does not modify the condition
  /// variable, otherwise it clears the error.
  ///
  /// @param         cvar The condition variable to update.
  void clearError();

  /// Push a parcel onto a condition variable directly.
  ///
  /// If the condition is in an error condition, this will return that error
  /// condition without pushing the parcel onto the condition's queue, otherwise
  /// it will push the parcel and return HPX_SUCCESS. The parcel may be
  /// associated with an executing thread or simply an attached parcel.
  ///
  /// @param       parcel The parcel to push.
  ///
  /// @returns            HPX_SUCCESS or an error code
  hpx_status_t push(struct hpx_parcel *parcel);

  /// Pop the top parcel from a condition variable.
  ///
  /// @param         cvar The condition to pop.
  ///
  /// @return             The top parcel, or NULL if the condition is empty or has
  ///                       an error.
  hpx_parcel_t *pop();

  /// Pop the parcel list from a condition variable.
  ///
  /// @return             The list of continuation parcels (NULL if there are
  ///                       none).
  hpx_parcel_t *popAll();

  /// Signal a condition.
  ///
  /// The calling thread must hold the lock protecting the condition. This call is
  /// synchronous (MESA style) and one waiting thread will be woken up.
  void signal();

  /// Signal a condition.
  ///
  /// The calling thread must hold the lock protecting the condition. This call is
  /// synchronous (MESA style) and all waiting threads will be woken up.
  void signalAll();

  /// Signal an error condition.
  ///
  /// The calling thread must hold the lock on protecting the condition. This call
  /// is synchronous (MESA style) and all of the waiting threads will be woken up,
  /// with the return value of HPX_LCO_ERROR. The user-supplied error can be
  /// retrieved from the condition.
  ///
  /// @param         code The error code to set in the condition.
  void signalError(hpx_status_t code);

 private:

  uintptr_t hasError() const;

  hpx_parcel_t *top_;
};

} // namespace scheduler
} // namespace libhpx
#endif // LIBHPX_SCHEDULER_CVAR_H
