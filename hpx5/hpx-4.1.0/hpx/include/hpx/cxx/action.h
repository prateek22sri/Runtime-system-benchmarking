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

#ifndef HPX_CXX_ACTION_H
#define HPX_CXX_ACTION_H

#include <type_traits>

#include <hpx/action.h>
#include <hpx/addr.h>
#include <hpx/cxx/lco.h>
#include <hpx/cxx/runtime.h>
#include <hpx/par.h>
#include <hpx/rpc.h>
#include <hpx/types.h>

namespace hpx {

namespace detail {

// list of types(type pack), meta functions defined on this
template <typename... Ts> struct tlist { using type = tlist<Ts...>; };

// reference: https://functionalcpp.wordpress.com/2013/08/05/function-traits/
template <class F> struct function_traits;

// function pointer
template <class R, class... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)> {};

template <class R, class... Args>
struct function_traits<R (&)(Args...)> : public function_traits<R(Args...)> {};

template <class R, class... Args> struct function_traits<R(Args...)> {
  using return_type = R;
  static constexpr std::size_t arity = sizeof...(Args);
  using arg_types = tlist<Args...>;
}; // template struct function_traits

/// template meta (Scheme-like) functions on tlist
/// Reduce_Right takes binary op BOP, initial value I (returned if tlist is
/// empty), and a tlist L
/// and reduces from the 'right'
template <template <typename, typename> class BOP, typename I, typename L>
struct Reduce_Right;
template <template <typename, typename> class BOP, typename I>
struct Reduce_Right<BOP, I, tlist<>> : I {};
template <template <typename, typename> class BOP, typename I, typename T>
struct Reduce_Right<BOP, I, tlist<T>> : T {};
template <template <typename, typename> class BOP, typename I, typename H,
          typename... TL>
struct Reduce_Right<BOP, I, tlist<H, TL...>>
    : BOP<H, typename Reduce_Right<BOP, I, tlist<TL...>>::type> {};

/// Map takes binary op BOP, and two tlists, applies BOP on elements of L1 and
/// L2 and produces another tlist
/// L1 and L2 must be of the same length
template <template <typename, typename> class BOP, typename L1, typename L2>
struct Map;
template <template <typename, typename> class BOP, typename... T1s,
          typename... T2s>
struct Map<BOP, tlist<T1s...>, tlist<T2s...>>
    : tlist<typename BOP<T1s, T2s>::type...> {};

/// Combine Map and Reduce_Right
template <template <typename, typename> class M,
          template <typename, typename> class R, typename I, typename L1,
          typename L2>
struct Map_Reduce : Reduce_Right<R, I, typename Map<M, L1, L2>::type> {};

template <bool B> using bool_constant = std::integral_constant<bool, B>;

// ops on {std::true_type, std::false_type}
template <typename B1, typename B2>
struct And : std::conditional<B1::value, B2, B1> {};
template <typename B1, typename B2>
struct Or : std::conditional<B1::value, B1, B2> {};
template <class B> struct Not : bool_constant<!B::value> {};

template <typename L1, typename L2> struct Same_Length;
template <typename... T1s, typename... T2s>
struct Same_Length<tlist<T1s...>, tlist<T2s...>>
    : bool_constant<(sizeof...(T1s) == sizeof...(T2s))> {};

/// needed to conditionally evaluate operands (std::conditional not enough)
template <bool C, typename T, typename F> struct delayed_conditional;
template <typename T, typename F> struct delayed_conditional<true, T, F> : T {};
template <typename T, typename F>
struct delayed_conditional<false, T, F> : F {};

/// checks if L1 and L2 are of equal lengths and then performs Map_Reduce
/// arguments are M: map op, R: reduce op, I: returned by reduce for empty lists
/// L1, L2: tlists that M operates on and converts to result tlist which is then
/// operated on by reduce
template <template <typename, typename> class M,
          template <typename, typename> class R, typename I, typename L1,
          typename L2>
struct Checked_Map_Reduce {
  using Cond = typename Same_Length<L1, L2>::type;
  using type =
      typename delayed_conditional<Cond::value, Map_Reduce<M, R, I, L1, L2>,
                                   std::false_type>::type;
};

namespace conversions {
template <typename T>
struct type2type;

/// Template specialization for integral types.
#define HPX_PLUS_PLUS_TYPE_MAP(ctype, hpxtype)  \
  template <>                                   \
  struct type2type<ctype>                       \
  {                                             \
    constexpr static auto type = hpxtype;       \
  };
#include <hpx/cxx/types.def>
#undef HPX_PLUS_PLUS_TYPE_MAP

// Partial specialization for global pointers
template <typename T>
struct type2type<hpx::global_ptr<T>>
{
  constexpr static auto type = HPX_ADDR;
};

// Partial specialization for local pointers
template <typename T>
struct type2type<T*>
{
  constexpr static auto type = HPX_POINTER;
};
}

template <hpx_action_type_t Type, uint32_t Attr, typename Alist>
struct typecheck_action_args;

/// marshalled actions only have two arguments
/// first is a pointer to the marshalled buffer and second is (convertible to)
/// std::size
template <hpx_action_type_t Type, typename T1, typename T2>
struct typecheck_action_args<Type, HPX_MARSHALLED, tlist<T1, T2>> {
  using decayed_t1 = typename std::decay<T1>::type;
  using decayed_t2 = typename std::decay<T2>::type;
  using cond1 = tlist<typename std::is_pointer<decayed_t1>::type,
                      typename std::is_unsigned<decayed_t2>::type>;
  using type = typename Reduce_Right<And, std::false_type, cond1>::type;
};
/// no checks on the types of formal arguments of the action function
template <hpx_action_type_t Type, typename Alist>
struct typecheck_action_args<Type, HPX_ATTR_NONE, Alist> : std::true_type {};
template <hpx_action_type_t Type, typename Alist>
struct typecheck_action_args<Type, HPX_PINNED, Alist>
    : typecheck_action_args<Type, HPX_ATTR_NONE, Alist> {};
template <hpx_action_type_t Type, typename Alist>
struct typecheck_action_args<Type, HPX_INTERNAL, Alist>
    : typecheck_action_args<Type, HPX_ATTR_NONE, Alist> {};
template <hpx_action_type_t Type, typename Alist>
struct typecheck_action_args<Type, HPX_VECTORED, Alist>
    : typecheck_action_args<Type, HPX_ATTR_NONE, Alist> {};
template <hpx_action_type_t Type, typename Alist>
struct typecheck_action_args<Type, HPX_COALESCED, Alist>
    : typecheck_action_args<Type, HPX_ATTR_NONE, Alist> {};
template <hpx_action_type_t Type, typename Alist>
struct typecheck_action_args<Type, HPX_COMPRESSED, Alist>
    : typecheck_action_args<Type, HPX_ATTR_NONE, Alist> {};

} // namespace detail
} // namspace hpx

namespace hpx {

// The action type
template <hpx_action_type_t TYPE, uint32_t ATTR, typename F, typename... ContTs>
class Action;

template <typename A> struct Action_traits;

template <typename A> struct Action_traits<A &> : public Action_traits<A> {};

template <hpx_action_type_t TYPE, uint32_t ATTR, typename F, typename... ContTs>
struct Action_traits<Action<TYPE, ATTR, F, ContTs...>> {
  using ftype = F;
  using ctypes = hpx::detail::tlist<ContTs...>;
  static constexpr std::size_t arity = sizeof...(ContTs);
};

// The action type
template <hpx_action_type_t TYPE, uint32_t ATTR, typename F, typename... ContTs>
class Action {
private:
  hpx_action_t _id;
  bool _is_registerd;

  template <typename R, typename... Args>
  int _register_helper(R (&f)(Args...)) {
    return hpx_register_action(TYPE, ATTR, __FILE__ ":" _HPX_XSTR(_id), &(_id),
                               sizeof...(Args) + 1, f,
                               hpx::detail::conversions::type2type<Args>::type...);
  }

  /// This overloaded function converts actual call arguments to pointers
  /// to be passed to the C impl
  template <typename T>
  typename std::enable_if<(ATTR != HPX_MARSHALLED), T *>::type
  _convert_arg(T &arg) {
    return &arg;
  }
  template <typename T>
  typename std::enable_if<(ATTR == HPX_MARSHALLED), T &&>::type
  _convert_arg(T &&arg) {
    return std::forward<T>(arg);
  }

public:
  Action() : _is_registerd(false) {
  }

  Action(F &f) { // Should the parameter be always a reference?
    static_assert(std::is_function<F>::value,
                  "Action constructor argument is not a function.");
    using atypes = typename hpx::detail::function_traits<F>::arg_types;
    static_assert(
        hpx::detail::typecheck_action_args<TYPE, ATTR, atypes>::type::value,
        "Type checking on function arguments failed");
    _register_helper(f); // What to do with return value? Maybe throw an
                         // exception when the registration is not successful?
    _is_registerd = true;
  }

  ~Action() {
  }

  hpx_action_t get_id() const {
    return _id;
  }

  /// Fully synchronous call interface.
  ///
  /// Performs action on @p args at @p addr, and sets @p out with the
  /// resulting value. The output value @p out can be NULL, in which case no
  /// return
  /// value is generated. The type of @p out must be the same as output type of
  /// the calling action.
  ///
  /// @param         addr The address that defines where the action is executed.
  /// @param       action The action to perform.
  /// @param          out Reference to the output variable.
  /// @param            n The number of arguments for @p action.
  ///
  /// @returns            HPX_SUCCESS, or an error code if the action generated
  /// an
  ///                     error that could not be handled remotely.
  template <typename R, typename... Args>
  int call_sync(hpx_addr_t &addr, R &out, Args &... args) {
    // TODO check the continuation type with R
    using traits = hpx::detail::function_traits<F>;
    static_assert(::std::is_same<typename traits::arg_types,
                                 hpx::detail::tlist<Args...>>::value,
                  "action and argument types do not match");
    return _hpx_call_sync(addr, _id, &out, sizeof(R), sizeof...(Args),
                          &args...);
  }

  /// Locally synchronous call interface.
  ///
  /// This is a locally-synchronous, globally-asynchronous variant of
  /// the remote-procedure call interface. If @p result is not HPX_NULL,
  /// hpx_call puts the the resulting value in @p result at some point
  /// in the future.
  ///
  /// @param         addr The address that defines where the action is executed.
  /// @param       result An address of an LCO to trigger with the result.
  ///
  /// @returns            HPX_SUCCESS, or an error code if there was a problem
  ///                     locally during the hpx_call invocation.
  template <typename... Args>
  int call(hpx_addr_t addr, hpx_addr_t result, Args &&... args) {
    using L1 = typename hpx::detail::tlist<Args...>;
    using L2 = typename hpx::detail::function_traits<F>::arg_types;
    static_assert(
        hpx::detail::typecheck_action_args<TYPE, ATTR, L1>::type::value,
        "argument typecheck failed");
    static_assert(
        hpx::detail::Checked_Map_Reduce<std::is_convertible, hpx::detail::And,
                                        std::true_type, L1, L2>::type::value,
        "action and argument types do not match");
    return _hpx_call(addr, _id, result, sizeof...(Args), _convert_arg(args)...);
  }
  template <typename T1, typename LCO, typename... Args>
  int call(const ::hpx::global_ptr<T1> &addr,
           const ::hpx::global_ptr<LCO> &result, Args &&... args) {
    using L1 = typename hpx::detail::tlist<Args...>;
    using L2 = typename hpx::detail::function_traits<F>::arg_types;
    static_assert(
        hpx::detail::typecheck_action_args<TYPE, ATTR, L1>::type::value,
        "argument typecheck failed");
    static_assert(
        hpx::detail::Checked_Map_Reduce<std::is_convertible, hpx::detail::And,
                                        std::true_type, L1, L2>::type::value,
        "action and argument types do not match");
    return _hpx_call(addr.get(), _id, result.get(), sizeof...(Args),
                     _convert_arg(args)...);
  }

  /// Locally synchronous call interface when LCO is set.
  ///
  /// This is a locally-synchronous, globally-asynchronous variant of
  /// the remote-procedure call interface which implements the hpx_parcel_send_
  /// through() function. The gate must be non-HPX_NULL.
  ///
  /// @param         gate The LCO that will serve as the gate (not HPX_NULL).
  /// @param         addr The address that defines where the action is executed.
  /// @param       result An address of an LCO to trigger with the result.
  /// @param            n The number of arguments for @p action.
  ///
  /// @returns            HPX_SUCCESS, or an error code if there was a problem
  ///                     locally during the hpx_call invocation.
  template <typename... Args>
  int call_when(hpx_addr_t gate, hpx_addr_t addr, hpx_addr_t result,
                Args &... args) {
    return _hpx_call_when(gate, addr, _id, result, sizeof...(Args), &args...);
  }
  template <typename LCO, typename T1, typename T2, typename... Args>
  int call_when(const ::hpx::global_ptr<LCO> &gate,
                const ::hpx::global_ptr<T1> &addr,
                const ::hpx::global_ptr<T1> &result, Args &... args) {
    return _hpx_call_when(gate.get(), addr.get(), _id, result.get(),
                          sizeof...(Args), &args...);
  }

  /// Fully synchronous call interface which implements
  /// hpx_parcel_send_through()
  /// when an LCO is set.
  ///
  /// Performs @p action on @p args at @p addr, and sets @p out with the
  /// resulting
  /// value. The output value @p out can be NULL (or the corresponding @p olen
  /// could be zero), in which case no return value is generated.
  ///
  /// The gate must be non-HPX_NULL.
  ///
  /// @param         gate The LCO that will serve as the gate (non HPX_NULL).
  /// @param         addr The address that defines where the action is executed.
  /// @param       action The action to perform.
  /// @param          out Address of the output buffer.
  /// @param         olen The length of the @p output buffer.
  /// @param            n The number of arguments for @p action.
  ///
  /// @returns            HPX_SUCCESS, or an error code if the action generated
  /// an
  ///                     error that could not be handled remotely.
  template <typename R, typename... Args>
  int call_when_sync(hpx_addr_t gate, hpx_addr_t addr, R &out, Args &... args) {
    return _hpx_call_when_sync(gate, addr, _id, &out, sizeof(R),
                               sizeof...(Args), &args...);
  }
  template <typename LCO, typename T, typename R, typename... Args>
  int call_when_sync(const ::hpx::global_ptr<LCO> &gate,
                     const ::hpx::global_ptr<T> &addr, R &out, Args &... args) {
    return _hpx_call_when_sync(gate.get(), addr.get(), _id, &out, sizeof(R),
                               sizeof...(Args), &args...);
  }

  /// Locally synchronous call with continuation interface.
  ///
  /// This is similar to hpx_call with additional parameters to specify the
  /// continuation action @p c_action to be executed at a continuation address
  /// @p
  /// c_target.
  ///
  /// @param         addr The address that defines where the action is executed.
  /// @param     c_target The address where the continuation action is executed.
  /// @param     c_action The continuation action to perform.
  ///
  /// @returns            HPX_SUCCESS, or an error code if there was a problem
  ///                     locally during the hpx_call invocation.
  template <typename Cont, typename... Args>
  int call_with_continuation(hpx_addr_t addr, hpx_addr_t c_target,
                             Cont &&c_action, Args &... args) {
    // check if the continued types of current action (caller) match input
    // arguments of c_action (callee)
    static_assert(
        std::is_same<hpx::detail::tlist<ContTs...>,
                     hpx::detail::tlist<Args...>>::value,
        "continued types and continuation argument types must match...");
    return _hpx_call_with_continuation(addr, _id, c_target, c_action.get_id(),
                                       sizeof...(Args), &args...);
  }
  template <typename T1, typename T2, typename Cont, typename... Args>
  int call_with_continuation(const ::hpx::global_ptr<T1> &addr,
                             const ::hpx::global_ptr<T2> &c_target,
                             Cont &&c_action, Args &... args) {
    static_assert(
        std::is_same<hpx::detail::tlist<ContTs...>,
                     hpx::detail::tlist<Args...>>::value,
        "continued types and continuation argument types must match...");
    return _hpx_call_with_continuation(addr.get(), _id, c_target.get(),
                                       c_action.get_id(), sizeof...(Args),
                                       &args...);
  }

  /// Fully asynchronous call interface.
  ///
  /// This is a completely asynchronous variant of the remote-procedure
  /// call interface. If @p result is not HPX_NULL, hpx_call puts the
  /// the resulting value in @p result at some point in the future. This
  /// function returns even before the argument buffer has been copied
  /// and is free to reuse. If @p lsync is not HPX_NULL, it is set
  /// when @p args is safe to be reused or freed.
  ///
  /// @param         addr The address that defines where the action is executed.
  /// @param        lsync The global address of an LCO to signal local
  /// completion
  ///                     (i.e., R/W access to, or free of @p args is safe),
  ///                     HPX_NULL if we don't care.
  /// @param       result The global address of an LCO to signal with the
  /// result.
  ///
  /// @returns            HPX_SUCCESS, or an error code if there was a problem
  ///                     locally during the hpx_call_async invocation.
  template <typename... Args>
  int call_async(hpx_addr_t addr, hpx_addr_t lsync, hpx_addr_t result,
                 Args &... args) {
    return _hpx_call_async(addr, _id, lsync, result, sizeof...(Args), &args...);
  }
  template <typename T, typename LSYNC, typename RES, typename... Args>
  int call_async(const ::hpx::global_ptr<T> &addr,
                 const ::hpx::global_ptr<LSYNC> &lsync,
                 const ::hpx::global_ptr<RES> &result, Args &... args) {
    return _hpx_call_async(addr, _id, lsync, result, sizeof...(Args), &args...);
  }

  /// Locally synchronous call_when with continuation interface.
  ///
  /// The gate must be non-HPX_NULL.
  ///
  /// @param         gate The LCO that will serve as the gate (not HPX_NULL).
  /// @param         addr The address that defines where the action is executed.
  /// @param     c_target The address where the continuation action is executed.
  /// @param         Cont The continuation action to perform.
  ///
  /// @returns            HPX_SUCCESS, or an error code if there was a problem
  ///                     locally during the hpx_call invocation.
  template <typename Cont, typename... Args>
  int call_when_with_continuation(hpx_addr_t gate, hpx_addr_t addr,
                                  hpx_addr_t c_target, Args &... args) {

    return _hpx_call_when_with_continuation(gate, addr, _id, c_target, Cont::id,
                                            sizeof...(Args), &args...);
  }
  template <typename LCO, typename T1, typename T2, typename Cont,
            typename... Args>
  int call_when_with_continuation(const ::hpx::global_ptr<LCO> &gate,
                                  const ::hpx::global_ptr<T1> &addr,
                                  const ::hpx::global_ptr<T2> &c_target,
                                  Cont &&c_action, Args &... args) {
    return _hpx_call_when_with_continuation(gate.get(), addr.get(), _id,
                                            c_target.get(), c_action.get_id(),
                                            sizeof...(Args), &args...);
  }

  /// Call with current continuation.
  ///
  /// This calls an action passing the currrent thread's continuation as the
  /// continuation for the called action.
  ///
  /// The gate must be non-HPX_NULL.
  ///
  /// @param         gate An LCO for a dependent call (must be non HPX_NULL).
  /// @param         addr The address where the action is executed.
  /// @param          ... The arguments for the call.
  ///
  /// @returns            HPX_SUCCESS, or an error code if there was a problem
  ///                     during the hpx_call_cc invocation.
  template <typename... Args>
  int call_when_cc(hpx_addr_t gate, hpx_addr_t addr, hpx_action_t action,
                   Args &... args) {
    return _hpx_call_when_cc(gate, addr, _id, sizeof...(Args), &args...);
  }
  template <typename LCO, typename T, typename... Args>
  int call_when_cc(const ::hpx::global_ptr<LCO> &gate,
                   const ::hpx::global_ptr<T> &addr, Args &... args) {
    return _hpx_call_when_cc(gate.get(), addr.get(), _id, sizeof...(Args),
                             &args...);
  }

  /// Call with current continuation.
  ///
  /// This calls an action passing the currrent thread's continuation as the
  /// continuation for the called action.
  ///
  /// @param         addr The address where the action is executed.
  /// @param          ... The arguments for the call.
  ///
  /// @returns            HPX_SUCCESS, or an error code if there was a problem
  ///                     during the hpx_call_cc invocation.
  template <typename... Args>
  int _hpx_call_cc(hpx_addr_t addr, Args &... args) {
    return _hpx_call_cc(addr, _id, sizeof...(Args), &args...);
  }
  template <typename T, typename... Args>
  int _hpx_call_cc(const ::hpx::global_ptr<T> &addr, Args &... args) {
    return _hpx_call_cc(addr.get(), _id, sizeof...(Args), &args...);
  }

  //   template <typename... Args>
  //   int thread_continue(Args&... args) {
  //     return _hpx_thread_continue(sizeof...(Args), &args...);
  //   };

  template <typename T> int thread_continue(T &arg) {
    return _hpx_thread_continue(2, &arg, sizeof(T));
  };

  /// hpx parallel loop interface
  /// Perform a "for" loop in parallel.
  ///
  /// This encapsulates a simple local parallel for loop:
  ///
  /// @code
  /// for (int i = min, e = max; i < e; ++i) {
  ///   hpx_call(HPX_HERE, action, sync, &i, &args);
  /// }
  /// @endcode
  ///
  /// The work is divided in equal chunks among the number of "worker"
  /// threads available. Work is actively pushed to each worker thread
  /// but is not affinitized and can be stolen by other worker threads.
  ///
  /// @param      min The minimum index in the loop.
  /// @param      max The maximum index in the loop.
  /// @param     args The arguments to the for function @p f.
  /// @param     sync An LCO that indicates the completion of all iterations.
  ///
  //// @returns An error code, or HPX_SUCCESS.
  template <typename LCO, typename... Args>
  int par_for(const hpx::global_ptr<LCO> &sync, int min, int max,
              Args &... args) {
    return hpx_par_for(_id, min, max, &args..., sync.get());
  }
  template <typename... Args>
  int par_for_sync(int min, int max, Args &... args) {
    return hpx_par_for_sync(_id, min, max, &args...);
  }

  /// Perform a parallel call.
  ///
  /// This encapsulates a simple parallel for loop with the following semantics.
  ///
  /// @code
  /// for (int i = min, e = max; i < e; ++i) {
  ///   char args[arg_size];
  ///   arg_init(args, i, env);
  ///   hpx_call(HPX_HERE, action, sync, args, arg_size);
  /// }
  /// @endcode
  ///
  /// The loop may actually be spawned as a tree, in which case @p
  /// branching_factor controls how many chunks each range is partitioned into,
  /// and @p cutoff controls the smalled chunk that is split.
  ///
  /// @param              min The minimum index in the loop.
  /// @param              max The maximum index in the loop.
  /// @param branching_factor The tree branching factor for tree-spawn.
  /// @param           cutoff The largest chunk we won't split.
  /// @param         arg_size The size of the arguments to action.
  /// @param         arg_init A callback to initialize the arguments
  /// @param         env_size The size of the environment of arg_init.
  /// @param              env An environment to pass to arg_init.
  /// @param             sync An LCO to set as the continuation for each
  /// iteration.
  ///
  /// @returns An error code, or HPX_SUCCESS.
  template <typename LCO>
  int par_call(const int min, const int max, const int branching_factor,
               const int cutoff, const size_t arg_size,
               void (*arg_init)(void *, const int, const void *),
               const size_t env_size, const void *env,
               const hpx::global_ptr<LCO> &sync) {
    return hpx_par_call(_id, min, max, branching_factor, cutoff, arg_size,
                        arg_init, env_size, env, sync.get());
  }

  int par_call_sync(const int min, const int max, const int branching_factor,
                    const int cutoff, const size_t arg_size,
                    void (*arg_init)(void *, const int, const void *),
                    const size_t env_size, const void *env) {
    return hpx_par_call_sync(_id, min, max, branching_factor, cutoff, arg_size,
                             arg_init, env_size, env);
  }

  template <typename T>
  int count_range_call(const hpx::global_ptr<T> &addr, const size_t count,
                       const size_t increment, const uint32_t bsize,
                       const size_t arg_size, void *const arg) {
    return hpx_count_range_call(addr.get(), count, increment, bsize, arg_size,
                                arg);
  }

  /// Collective calls.
  ///

  template <typename... Args>
  int process_broadcast(hpx_addr_t lsync, hpx_addr_t rsync, Args &... args) {
    return _hpx_process_broadcast(hpx_thread_current_pid(), _id, lsync, rsync,
                                  sizeof...(Args), &args...);
  }
  template <typename LSYNC, typename RSYNC, typename... Args>
  int process_broadcast(const ::hpx::global_ptr<LSYNC> &lsync,
                        const ::hpx::global_ptr<RSYNC> &rsync, Args &... args) {
    return _hpx_process_broadcast(hpx_thread_current_pid(), _id, lsync.get(),
                                  rsync.get(), sizeof...(Args), &args...);
  }
  template <typename... Args>
  int process_broadcast_lsync(hpx_addr_t rsync, Args &... args) {
    return _hpx_process_broadcast_lsync(hpx_thread_current_pid(), _id, rsync,
                                        sizeof...(Args), &args...);
  }
  template <typename RSYNC, typename... Args>
  int process_broadcast_lsync(::hpx::global_ptr<RSYNC> &rsync, Args &... args) {
    return _hpx_process_broadcast_lsync(hpx_thread_current_pid(), _id,
                                        rsync.get(), sizeof...(Args), &args...);
  }
  template <typename... Args> int process_broadcast_rsync(Args &... args) {
    return _hpx_process_broadcast_rsync(hpx_thread_current_pid(), _id,
                                        sizeof...(Args), &args...);
  }

  template <typename... Args> int run(Args &&... args) {
    using L1 = typename hpx::detail::tlist<Args...>;
    using L2 = typename hpx::detail::function_traits<F>::arg_types;
    static_assert(
        hpx::detail::typecheck_action_args<TYPE, ATTR, L1>::type::value,
        "argument typecheck failed");
    static_assert(
        hpx::detail::Checked_Map_Reduce<std::is_convertible, hpx::detail::And,
                                        std::true_type, L1, L2>::type::value,
        "action and argument types do not match");
    return _hpx_run(&_id, nullptr, sizeof...(Args), _convert_arg(args)...);
  }

  template <typename... Args> int run_spmd(Args &&... args) {
    using L1 = typename hpx::detail::tlist<Args...>;
    using L2 = typename hpx::detail::function_traits<F>::arg_types;
    static_assert(
                  hpx::detail::typecheck_action_args<TYPE, ATTR, L1>::type::value,
                  "argument typecheck failed");
    static_assert(hpx::detail::Checked_Map_Reduce<std::is_convertible, hpx::detail::And,
                  std::true_type, L1, L2>::type::value,
                  "action and argument types do not match");
    return _hpx_run_spmd(&_id, nullptr, sizeof...(Args), _convert_arg(args)...);
  }

  template <typename R, typename... Args> int _register(R (&f)(Args...)) {
    return _register_helper(f);
  }

}; // template class Action

// helper methods to create action object
template <typename R, typename T1, typename T2, typename... ContTs>
inline Action<HPX_DEFAULT, HPX_MARSHALLED, R(T1 *, T2), ContTs...>
make_action(R (&f)(T1 *, T2)) {
  //   static_assert(std::is_unsigned<T2>::value, "The second argument of a "
  //                                              "marshalled action should be
  //                                              an "
  //                                              "unsigned integer type.");
  return Action<HPX_DEFAULT, HPX_MARSHALLED, R(T1 *, T2), ContTs...>(f);
}
template <hpx_action_type_t T, uint32_t ATTR, typename F, typename... ContTs>
inline Action<T, ATTR, F, ContTs...> make_action(F &f) {
  return Action<T, ATTR, F, ContTs...>(f);
}
template <typename F, typename... ContTs>
inline Action<HPX_DEFAULT, HPX_ATTR_NONE, F, ContTs...> make_action(F &f) {
  return Action<HPX_DEFAULT, HPX_ATTR_NONE, F, ContTs...>(f);
}
}

#endif // HPX_CXX_ACTION_H
