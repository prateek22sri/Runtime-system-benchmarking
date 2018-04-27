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

#ifndef HPX_CXX_LCO_H
#define HPX_CXX_LCO_H

#include <vector>
#include <algorithm>

#include <hpx/addr.h>
#include <hpx/lco.h>
#include <hpx/cxx/errors.h>
#include <hpx/cxx/global_ptr.h>

namespace hpx {
namespace lco {
/// This class is provides the lco interface common to all types of LCOs
/// extended by all LCO classes such as Future, Reduce, AndGate etc.
class LCO {
 protected:
  LCO() {}

  ~LCO() = delete;
};

template <typename T>
struct is_lco : public std::is_base_of<LCO, T> {
};

template <>
struct is_lco<void> {
  using value = std::integral_constant<bool, true>;
};

template <typename T>
inline void
reset(const global_ptr<T>& lco)
{
  static_assert(is_lco<T>::value, "LCO type required");
  hpx_lco_reset(lco.get());
}

template <typename T>
inline void
wait(const global_ptr<T>& lco, bool reset = true)
{
  static_assert(is_lco<T>::value, "LCO type required");
  const hpx_addr_t& gva = lco.get();
  if (int e = (reset) ? hpx_lco_wait_reset(gva) : hpx_lco_wait(gva)) {
    throw Error(e);
  }
}

template <typename T, typename U>
inline void
get(const global_ptr<T>& lco, U& out, bool reset = false)
{
  static_assert(is_lco<T>::value, "LCO type required");
  const hpx_addr_t& gva = lco.get();
  if (int e = (reset) ? hpx_lco_get_reset(gva, sizeof(out), &out) :
                        hpx_lco_get(gva, sizeof(out), &out)) {
    throw Error(e);
  }
}

template <typename T, template <typename> class LCO>
inline T
get(const global_ptr<LCO<T>>& lco, bool reset = false)
{
  T out;
  get(lco, out, reset);
  return out;
}

template <template <typename> class LCO>
inline void
get(const global_ptr<LCO<void>>& lco, bool reset = false)
{
  wait(lco, reset);
}

/// Get values for all of the LCOs.
///
/// This admits some parallelism in the implementation, and is preferable to
/// using hpx_lco_get() in a loop. The calling thread will block until all of
/// the LCOs are available. Entries in @p lcos that are set to HPX_NULL are
/// ignored, their corresponding values in @p values will not be written to.
///
/// @param          lcos an array of @p n global LCO addresses
/// @param[out]   values an array of @p n local buffers with sizes corresponding
///                      to @p sizes
///
/// @returns statuses an array of statuses, pass NULL if statuses are not
///                      required
template <typename T, template <typename> class LCO>
inline std::vector<hpx_status_t>
get_all(const std::vector<hpx::global_ptr<LCO<T>>>& lcos,
    std::vector<T>& values)
{
  assert(lcos.size() == values.size());

  std::vector<hpx_addr_t> _lcos;
  std::transform(lcos.begin(), lcos.end(), std::back_inserter(_lcos),
    [](const hpx::global_ptr<LCO<T>>& f) {return f.get();});
  std::vector<std::size_t> _sizes;
  std::transform(lcos.begin(), lcos.end(), std::back_inserter(_sizes),
    [](const hpx::global_ptr<LCO<T>>& f) {return sizeof(T);});
  std::vector<T*> _values;
  std::transform(values.begin(), values.end(), std::back_inserter(_values),
    [](T& val) {return &val;});

  std::vector<hpx_status_t> statuses;
  hpx_lco_get_all(_lcos.size(), _lcos.data(), _sizes.data(), (void**) _values.data(), statuses.data());

  return statuses;
}

template <typename T, typename U>
inline void
set(const global_ptr<T>& lco, U&& in)
{
  static_assert(is_lco<T>::value, "LCO type required");
  hpx_lco_set_rsync(lco.get(), sizeof(in), std::forward<U>(in));
}

template <typename T>
inline void
set(const global_ptr<T>& lco)
{
  static_assert(is_lco<T>::value, "LCO type required");
  hpx_lco_set_rsync(lco.get(), 0, NULL);
}

template <typename T>
inline void
set(const global_ptr<T>& lco, std::nullptr_t)
{
  static_assert(is_lco<T>::value, "LCO type required");
  hpx_lco_set(lco.get(), 0, NULL, HPX_NULL, HPX_NULL);
}

template <typename T>
inline void
dealloc(const global_ptr<T>& lco)
{
  static_assert(is_lco<T>::value, "LCO type required");
  hpx_lco_delete_sync(lco.get());
}

template <typename T, typename U>
inline void
dealloc(const global_ptr<T>& lco, const global_ptr<U>& sync)
{
  static_assert(is_lco<T>::value, "LCO type required");
  static_assert(is_lco<T>::value, "LCO type required");
  hpx_lco_delete(lco.get(), sync.get());
}

template <typename T>
inline void
dealloc(const global_ptr<T>& lco, std::nullptr_t)
{
  static_assert(is_lco<T>::value, "LCO type required");
  hpx_lco_delete_sync(lco.get());
}

template <typename T>
class Future : public LCO {
 public:
  Future() = delete;
  ~Future() = delete;

  static global_ptr<Future<T>> Alloc() {
    return global_ptr<Future<T>>(hpx_lco_future_new(sizeof(T)));
  }
};

/// Future to void is a 0-sized future that only contains control information.
template <>
inline global_ptr<Future<void>>
Future<void>::Alloc()
{
  return global_ptr<Future<void>>(hpx_lco_future_new(0));
}

class And : public LCO {
 public:
  And() = delete;
  ~And() = delete;

  static global_ptr<And> Alloc(size_t n) {
    return global_ptr<And>(hpx_lco_and_new(n));
  }
};

template <typename T>
class Reduce : public LCO {
 public:
  Reduce() = delete;
  ~Reduce() = delete;

  template <template <typename> class Op>
  static global_ptr<Reduce<T>> Alloc(int inputs) {
    hpx_action_t id = Op<T>::id;
    hpx_action_t op = Op<T>::op;
    return global_ptr<Reduce<T>>(hpx_lco_reduce_new(inputs, sizeof(T), id, op));
  }
};

namespace Ops {
template <typename T>
class Sum {
  static void init(T* t, size_t) {
    *t = T(0);
  }

  static void sum(T* lhs, const T* rhs, size_t) {
    *lhs += *rhs;
  }

  static void register_id(void) {
    if (int e = hpx_register_action(HPX_FUNCTION, 0, __PRETTY_FUNCTION__, &id,
                                    1, &init)) {
      throw Error(e);
    }
  }

  static void register_op(void) {
    if (int e = hpx_register_action(HPX_FUNCTION, 0, __PRETTY_FUNCTION__, &op,
                                    1, &sum)) {
      throw Error(e);
    }
  }

 public:
  static int Init() {
    register_id();
    register_op();
    return HPX_SUCCESS;
  }

  static hpx_action_t id;
  static hpx_action_t op;
};

template <typename T>
class Product {
  static void init(T* t, size_t) {
    *t = T(1);
  }

  static void product(T* lhs, const T* rhs, size_t) {
    *lhs *= *rhs;
  }

  static void register_id(void) {
    if (int e = hpx_register_action(HPX_FUNCTION, 0, __PRETTY_FUNCTION__, &id,
                                    1, &init)) {
      throw Error(e);
    }
  }

  static void register_op(void) {
    if (int e = hpx_register_action(HPX_FUNCTION, 0, __PRETTY_FUNCTION__, &op,
                                    1, &product)) {
      throw Error(e);
    }
  }

 public:
  static int Init() {
    register_id();
    register_op();
    return HPX_SUCCESS;
  }

  static hpx_action_t id;
  static hpx_action_t op;
};

template <typename T> hpx_action_t Sum<T>::id = HPX_ACTION_NULL;
template <typename T> hpx_action_t Sum<T>::op = HPX_ACTION_NULL;
template <typename T> hpx_action_t Product<T>::id = HPX_ACTION_NULL;
template <typename T> hpx_action_t Product<T>::op = HPX_ACTION_NULL;
} // namespace ops
} // namespace lco
} // namespace hpx

#endif // HPX_CXX_LCO_H
