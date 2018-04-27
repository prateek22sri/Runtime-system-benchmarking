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

#ifndef HPX_CXX_GLOBAL_PTR_H
#define HPX_CXX_GLOBAL_PTR_H

#include <cstddef>
#include <memory>
#include <ostream>
#include <exception>
#include <hpx/addr.h>
#include <hpx/gas.h>

namespace hpx {

/// We throw this exception if a pin operation fails.
struct NotLocal : public std::exception {
  virtual const char* what() const throw() {
    return "Pin operation failed\n";
  }
};

template <typename T>
class global_ptr;

/// Special case global pointers to void with a template specialization.
///
/// These serve the same roll as void* pointers do. Other pointers can be cast
/// to global_ptr<void>, however we can't perform address arithmetic on them,
/// because don't know their type.
///
/// This specialization is defined first because we use it in the implementation
/// of the generic global_ptr template below (as part of an explicit cast
/// operation).
template <>
class global_ptr<void> {
 public:
  /// Default constructor.
  ///
  /// This constructs a global_ptr<void> to NULL.
  global_ptr<void>() noexcept : _impl(HPX_NULL), _bsize(0) {
  }

  global_ptr<void>(std::nullptr_t) noexcept : _impl(HPX_NULL), _bsize(0) {
  }

  /// Construct a generic global pointer from a generic HPX address.
  global_ptr<void>(hpx_addr_t addr) noexcept : _impl(addr), _bsize(0) {
  }

  /// Construct a generic global pointer from a generic HPX address.
  global_ptr<void>(hpx_addr_t addr, size_t bsize) noexcept : _impl(addr),
    _bsize(bsize) {
  }

  /// Construct a generic global pointer from a pointer to any other type---this
  /// serves to handle pointer casts as well.
  template <typename U>
  global_ptr<void>(const global_ptr<U>& ptr) noexcept : _impl(ptr.get()),
     _bsize(ptr.get_block_size()) {
  }

  /// Allow smart pointers to be used in (!ptr) style tests.
  operator bool() const noexcept {
    return (_impl != HPX_NULL);
  }

  /// Support any user that wants to get the underlying HPX address.
  hpx_addr_t get() const noexcept {
    return _impl;
  }

  /// Return the block size that this smart pointer encapsulates.
  size_t get_block_size() const noexcept {
    return _bsize;
  }

  /// Pin the global pointer.
  ///
  /// @param[out] local True if the pin succeeds, false otherwise.
  /// @return           The local pointer associated with the
  void* pin(bool &local) const noexcept {
    void *lva = nullptr;
    local = hpx_gas_try_pin(_impl, &lva);
    return lva;
  }

  /// @overload pin()
  ///
  /// @return           The local pointer associated with the global pointer.
  /// @throw   NotLocal If the pointer is not local.
  void* pin() const noexcept(false) {
    bool local;
    void* lva = pin(local);
    if (!local) {
      throw NotLocal();
    }
    return lva;
  }

  /// Unpin a global pointer.
  ///
  /// The unpin() operation must match a pin() operation, but this requirement
  /// is not enforced by the runtime. A pin_guard object can help ensure that
  /// lexically-scoped use can be properly pinned and unpinned.
  void unpin() const noexcept {
    hpx_gas_unpin(_impl);
  }

 private:
  const hpx_addr_t _impl;
  const size_t _bsize;
};

/// An HPX global address ptr.
///
/// Basically wraps hpx_addr_t and provides address arithmetic and pin/unpin
/// methods.
template <typename T>
class global_ptr {
  /// This helper class will allow users to use &ptr[x] to perform address
  /// arithmetic, without exposing the underlying T to manipulation.
  class reference {
   public:
    reference() = delete;

    /// The only thing we can do with a reference is to get its global address.
    global_ptr<T> operator&() const noexcept {
      return _gp;
    }

    ~reference() noexcept {
    }

   private:
    /// Global pointers create references in their array index operator.
    friend class global_ptr<T>;
    reference(const global_ptr<T>& gp) noexcept : _gp(gp) {
    }

    global_ptr<T> _gp;
  };

 public:
  typedef T value_type;

  /// The default constructor for a global_ptr initializes the pointer to null.
  global_ptr() noexcept : _gbl_ptr(HPX_NULL), _elems_per_blk(0) {
  }

  /// The nullptr can be implicitly converted to a global_ptr.
  global_ptr(std::nullptr_t) noexcept : _gbl_ptr(HPX_NULL), _elems_per_blk(0) {
  }

  /// A global pointer can be explicitly constructed from an hpx_addr_t.
  ///
  /// This constructor assumes that the block size for the allocation is simply
  /// a single element. Because this assumption is fundamentally unsafe we make
  /// this an explicit constructor so that we don't accidently get
  /// difficult-to-debug errors.
  ///
  /// @param       addr The hpx_addr_t.
  explicit global_ptr(hpx_addr_t addr) noexcept : _gbl_ptr(addr),
                                                  _elems_per_blk(1) {
  }

  /// A global pointer can be constructed from an hpx_addr_t, block size pair.
  ///
  /// This constructs a global_ptr from an underlying hpx_addr_t allocation. Of
  /// course, to be valid @p n must match the block size that @p addr was
  /// allocated with. Note that @p n is in terms of sizeof(T), while the bsize
  /// for HPX allocation is in terms of bytes.
  ///
  /// @param       addr The hpx_addr_t.
  /// @param          n The number of elements per block.
  global_ptr(hpx_addr_t addr, size_t n) noexcept : _gbl_ptr(addr),
    _elems_per_blk(n) {
  }

  /// A typed global pointer can be constructed from a global_ptr<void>, as long
  /// as the user provides a block size.
  explicit global_ptr(const global_ptr<void>& gva, size_t n) noexcept
    : _gbl_ptr(gva.get()), _elems_per_blk(n) {
  }

  /// Allow smart pointers to be constructed from compatible types.
  template <class U,
            class = typename std::enable_if<std::is_base_of<T, U>::value ||
                                            std::is_same<U, void>::value>::type>
  global_ptr(const global_ptr<U>& rhs) noexcept
    : _gbl_ptr(rhs.get()), _elems_per_blk(rhs.get_block_size()) {
  }

  /// Allow smart pointers to be converted from compatible types.
  template <class U,
            class = typename std::enable_if<std::is_base_of<T, U>::value ||
                                            std::is_same<U, void>::value>::type>
  global_ptr& operator=(const global_ptr<U>& rhs) {
    _gbl_ptr = rhs.get();
    _elems_per_blk = rhs.get_block_size();
    return *this;
  }

  /// Allow smart pointers to be used in (!ptr) style tests.
  operator bool() const noexcept {
    return (_gbl_ptr != HPX_NULL);
  }

  /// Returns the raw hpx_addr_t that this smart pointer encapsulates.
  hpx_addr_t get() const noexcept {
    return _gbl_ptr;
  }

  /// Return the block size that this smart pointer encapsulates.
  size_t get_block_size() const noexcept {
    return _elems_per_blk;
  }

  /// The array-subscript operator is an alternative to explicit pointer
  /// arithmetic.
  reference operator[](size_t index) const noexcept {
    return reference(*this + index);
  }

  /// Standard pointer arithmetic returns another global pointer.
  template <typename U>
  global_ptr<T> operator+(U n) const noexcept {
    static_assert(std::is_integral<U>::value, "integer type required");
    hpx_gas_ptrdiff_t bytes = n * sizeof(T);
    hpx_addr_t addr = hpx_addr_add(_gbl_ptr, bytes, bsize());
    return global_ptr<T>(addr, _elems_per_blk);
  }

  /// Standard self-update pointer arithmetic.
  template <typename U>
  global_ptr<T>& operator+=(U n) noexcept {
    _gbl_ptr = (*this + n).get();
    return *this;
  }

  /// Standard pointer difference operation.
  ///
  /// Pointers can only be compared between the same allocation, which implies
  /// that they will have the same block size. Without more information we can't
  /// check this constraint more carefully.
  hpx_gas_ptrdiff_t operator-(const global_ptr<T>& rhs) const noexcept {
    assert(_elems_per_blk == rhs.get_block_size());
    hpx_gas_ptrdiff_t bytes = hpx_addr_sub(_gbl_ptr, rhs.get(), bsize());
    assert(bytes % sizeof(T) == 0);
    return bytes / sizeof(T);
  }

  /// Pin the global pointer.
  ///
  /// Pinning a global pointer allows the programmer to interact with its
  /// underlying block using its native type. This operation will attempt to pin
  /// the entire block pointed to by the global_ptr<T>. If it succeeds then the
  /// block will remain pinned until unpin() is called on the global_ptr<T>.
  ///
  /// HPX++ provides the pin_guard class to provide automatic lexically-scoped
  /// unpinning.
  ///
  /// @return           The local pointer associated with the global pointer.
  /// @throw   NotLocal If the pointer is not local.
  T* pin() const noexcept(false) {
    bool local;
    T* lva = pin(local);
    if (!local) {
      throw NotLocal();
    }
    return lva;
  }

  /// @overload pin()
  ///
  /// @param[out] local True if the pin succeeds, false otherwise.
  /// @return           The local pointer associated with the
  T* pin(bool &local) const noexcept {
    T *lva = nullptr;
    void **clva = static_cast<void**>(static_cast<void*>(&lva));
    local = hpx_gas_try_pin(_gbl_ptr, clva);
    return lva;
  }

  /// Unpin a global pointer.
  ///
  /// The unpin() operation must match a pin() operation, but this requirement
  /// is not enforced by the runtime. A pin_guard object can help ensure that
  /// lexically-scoped use can be properly pinned and unpinned.
  void unpin() const noexcept {
    hpx_gas_unpin(_gbl_ptr);
  }

  /// Compute the HPX block size for the pointer. The HPX block size differs
  /// from the HPX++ block size because the HPX++ block size is in terms of T,
  /// while the HPX block size is in terms of bytes.
  size_t bsize() const noexcept {
    return _elems_per_blk * sizeof(T);
  }

 private:
  hpx_addr_t _gbl_ptr;
  size_t _elems_per_blk;
}; // template class global_ptr

/// The standard global pointer operators.
///
/// These are defined in their non-member forms so that we can use them for the
/// global_ptr<void> specialization.
///
/// @{
template <typename T, typename U>
inline bool
operator==(const global_ptr<T>& lhs, const global_ptr<U> &rhs) noexcept
{
  return ((lhs - rhs) == 0);
}

template <typename T, typename U>
inline bool
operator!=(const global_ptr<T>& lhs, const global_ptr<U> &rhs) noexcept
{
  return !(lhs == rhs);
}

template <typename T, typename U>
inline bool
operator<(const global_ptr<T>& lhs, const global_ptr<U> &rhs) noexcept
{
  assert(lhs.get_block_size() == rhs.get_block_size());
  return ((lhs - rhs) < 0);
}

template <typename T, typename U>
inline bool
operator>(const global_ptr<T>& lhs, const global_ptr<U> &rhs) noexcept
{
  assert(lhs.get_block_size() == rhs.get_block_size());
  return ((lhs - rhs) > 0);
}

template <typename T, typename U>
inline bool
operator<=(const global_ptr<T>& lhs, const global_ptr<U> &rhs) noexcept
{
  assert(lhs.get_block_size() == rhs.get_block_size());
  return ((lhs - rhs) <= 0);
}

template <typename T, typename U>
inline bool
operator>=(const global_ptr<T>& lhs, const global_ptr<U> &rhs) noexcept
{
  assert(lhs.get_block_size() == rhs.get_block_size());
  return ((lhs - rhs) >= 0);
}

template <typename T>
inline bool
operator==(const global_ptr<T>& lhs, std::nullptr_t) noexcept
{
  return (lhs.get() == HPX_NULL);
}

template <typename T>
inline bool
operator==(std::nullptr_t, const global_ptr<T>& rhs) noexcept
{
  return (HPX_NULL == rhs.get());
}

template <typename T>
inline bool
operator!=(const global_ptr<T>& lhs, std::nullptr_t) noexcept
{
  return (lhs.get() != HPX_NULL);
}

template <typename T>
inline bool
operator!=(std::nullptr_t, const global_ptr<T>& rhs) noexcept
{
  return (rhs.get() != HPX_NULL);
}

template <typename T>
inline bool
operator<(const global_ptr<T>& lhs, std::nullptr_t) noexcept
{
  return std::less<T*>(lhs.get(), nullptr);
}

template <typename T>
inline bool
operator<(std::nullptr_t, const global_ptr<T>& rhs) noexcept
{
  return std::less<T*>(nullptr, rhs.get());
}

template <typename T>
inline bool
operator>(const global_ptr<T>& lhs, std::nullptr_t) noexcept
{
  return nullptr < lhs;
}

template <typename T>
inline bool
operator>(std::nullptr_t, const global_ptr<T>& rhs) noexcept
{
  return rhs < nullptr;
}

template <typename T>
inline bool
operator<=(const global_ptr<T>& lhs, std::nullptr_t) noexcept
{
  return !(nullptr < lhs);
}

template <typename T>
inline bool
operator<=(std::nullptr_t, const global_ptr<T>& rhs) noexcept
{
  return !(rhs < nullptr);
}

template <typename T>
inline bool
operator>=(const global_ptr<T>& lhs, std::nullptr_t) noexcept
{
  return !(lhs < nullptr);
}

template <typename T>
inline bool
operator>=(std::nullptr_t, const global_ptr<T>& rhs) noexcept
{
  return !(nullptr < rhs);
}

/// @}

/// The pin_guard class.
///
/// A pin_guard will automatically unpin the address when it is destroyed. This
/// allows safe lexically scoped use of a pinned global pointer in circumstances
/// where that makes sense. It is unsafe to continue to access the provided
/// local pointer after the pin_guard has been destroyed.
///
/// The pin_guard is non-copyable.
template <typename T>
class pin_guard {
 private:
  pin_guard() = delete;
  pin_guard(const pin_guard<T>&) = delete;
  pin_guard<T>& operator=(const pin_guard<T>&) = delete;

 public:
  // Moving the pin guard is fine.
  constexpr pin_guard(pin_guard<T>&&) = default;
  pin_guard<T>& operator=(pin_guard<T>&&) = default;

  /// Construct a pin_guard from a global address.
  ///
  /// @param        gva The address we're trying to pin.
  explicit pin_guard(const global_ptr<T>& gva) noexcept
    : _gva(gva), _local(false), _lva(gva.pin(_local)) {
  }

  /// Construct a pin_guard from a C-API global address.
  ///
  /// @param        gva The global address.
  explicit pin_guard(hpx_addr_t gva) noexcept
    : _gva(gva), _local(false), _lva(_gva.pin(_local)) {
  }

  /// This version of the pin_guard constructor will return the success or
  /// failure of the pin operation through the @p local parameter, rather than
  /// throwing an exception.
  ///
  /// @param        gva The address we're trying to pin.
  /// @param[out] local A flag indication if the @p gva was local.
  pin_guard(const global_ptr<T>& gva, bool &local) noexcept
    : _gva(gva), _local(false), _lva(_gva.pin(_local)) {
    local = _local;
  }

  /// This version of the pin_guard constructor can be used with traditional
  /// hpx_addr_t, and will return the local address through the @p out
  /// parameter.
  ///
  /// @param        gva The C-API global address.
  /// @param[out]   lva The local virtual address of the pinned address.
  pin_guard(hpx_addr_t gva, T*& lva) noexcept
    : _gva(gva), _local(false), _lva(_gva.pin(_local)) {
    lva = _lva;
  }

  /// The pin_guard destructor.
  ///
  /// The pin_guard will unpin the underlying gva if it was successfully pinned
  /// during construction.
  ~pin_guard() noexcept {
    if (_local) {
      _gva.unpin();
    }
  }

  /// Check for success.
  operator bool() const noexcept {
    return _local;
  }

  /// Access the underlying local memory.
  T* get() const noexcept {
    assert(_local);
    return _lva;
  }

  /// Use the underlying pointer.
  T* operator->() const noexcept {
    return get();
  }

 private:
  const global_ptr<T> _gva;
  bool              _local;
  T* const            _lva;
}; // template pin_guard

template <typename T>
inline pin_guard<T>
scoped_pin(const global_ptr<T> gva) noexcept(false)
{
  return pin_guard<T>(gva);
}

template <typename T>
inline pin_guard<T>
scoped_pin(const global_ptr<T> gva, bool& local) noexcept
{
  return pin_guard<T>(gva, local);
}

template <typename T>
inline pin_guard<T>
scoped_pin(hpx_addr_t gva, T*& lva) noexcept
{
  return pin_guard<T>(gva, lva);
}

template <typename T>
inline pin_guard<T>
scoped_pin(hpx_addr_t gva) noexcept
{
  return pin_guard<T>(gva);
}

template <class U, class V,
          class = typename std::enable_if<std::is_base_of<U, V>::value>::type>
global_ptr<V> global_ptr_cast(global_ptr<U> ptr) {
  return global_ptr<V>(ptr);
}

template <class V>
global_ptr<V> global_ptr_cast(global_ptr<void> ptr) {
  return global_ptr<V>(ptr);
}

} // namespace hpx

template <class T>
std::ostream& operator<<(std::ostream& out, const hpx::global_ptr<T>& rhs) {
  return (out << rhs.get());
}

template <class T>
std::ostream&& operator<<(std::ostream&& out, const hpx::global_ptr<T>& rhs) {
  return std::move(std::move(out) << rhs.get());
}

#endif // HPX_CXX_GLOBAL_PTR_H
