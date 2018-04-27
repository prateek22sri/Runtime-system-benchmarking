// ================================================================= -*- C++ -*-
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

#ifndef HPX_CXX_PAR_FOR_H
#define HPX_CXX_PAR_FOR_H

#include <hpx/par.h>
#include <hpx/cxx/errors.h>
#include <hpx/cxx/global_ptr.h>
#include <hpx/cxx/lco.h>

namespace hpx {

inline void
parallel_for(hpx_for_action_t f, int min, int max, void *env)
{
  if (int e = hpx_par_for_sync(f, min, max, env)) {
    throw Error(e);
  }
}

template <typename T>
inline void
parallel_for(hpx_for_action_t f, int min, int max, void *env,
             const global_ptr<T>& sync)
{
  static_assert(lco::is_lco<T>::value, "LCO type required");
  if (int e = hpx_par_for(f, min, max, env, sync.get())) {
    throw Error(e);
  }
}

template <template <typename> class LCO>
inline void
parallel_for(hpx_for_action_t f, int min, int max, void *env, std::nullptr_t)
{
  if (int e = hpx_par_for(f, min, max, env, HPX_NULL)) {
    throw Error(e);
  }
}

} // namespace hpx

#endif // HPX_CXX_PAR_FOR_H
