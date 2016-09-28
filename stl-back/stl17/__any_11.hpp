#pragma once

#include <initializer_list>
#include <type_traits>
#include <typeinfo>

#include "__in_place.hpp"

namespace stl_backports {
namespace stl17 {

class bad_any_cast : public std::bad_cast {
public:
  const char *what() const noexcept override { return "bad any_cast"; }
};

struct __any_value_base {
  virtual ~__any_value_base() = default;
  virtual const std::type_info &_M_type() const = 0;
  virtual __any_value_base *_M_duplicate() const = 0;
};

template <class _Tp> struct __any_value : __any_value_base {
  _Tp _M_value;

  template <class... _Args,
            class = typename std::enable_if<
                std::is_constructible<_Tp, _Args...>::value>::type>
  __any_value(_Args &&... __args) : _M_value(std::forward<_Args>(__args)...) {}
  ~__any_value() = default;
  const std::type_info &_M_type() const override { return typeid(_Tp); }
  virtual __any_value_base *_M_duplicate() const override {
    return new __any_value{_M_value};
  }
};

class any {
private:
  __any_value_base *_M_ptr;

public:
  // 20.8.3.1, construction and destruction
  constexpr any() noexcept : _M_ptr(nullptr) {}
  any(const any &__other)
      : _M_ptr(__other.has_value() ? __other._M_ptr->_M_duplicate() : nullptr) {
  }
  any(any &&__other) noexcept : _M_ptr(__other._M_ptr) {
    __other._M_ptr = nullptr;
  }
  template <
      class _ValueType,
      typename = typename std::enable_if<
          std::is_copy_constructible<
              typename std::decay<_ValueType>::type>::value &&
          !std::is_same<typename std::decay<_ValueType>::type, any>::value &&
          !__is_in_place_type_t<_ValueType>::value>::type>
  any(_ValueType &&__value)
      : _M_ptr(new __any_value<typename std::decay<_ValueType>::type>{
            std::forward<_ValueType>(__value)}) {}
  template <class _Tp, class... _Args,
            typename = typename std::enable_if<std::is_constructible<
                typename std::decay<_Tp>::type, _Args...>::value>::type>
  explicit any(in_place_type_t<_Tp>, _Args &&... __args)
      : _M_ptr(new __any_value<typename std::decay<_Tp>::type>{
            std::forward<_Args>(__args)...}) {}
  template <class _Tp, class _Up, class... _Args,
            typename = typename std::enable_if<std::is_constructible<
                typename std::decay<_Tp>::type, std::initializer_list<_Up> &,
                _Args...>::value>::type>
  explicit any(in_place_type_t<_Tp>, std::initializer_list<_Up> __il,
               _Args &&... __args)
      : _M_ptr(new __any_value<typename std::decay<_Tp>::type>{
            __il, std::forward<_Args>(__args)...}) {}
  ~any() { delete _M_ptr; }

  // 20.8.3.2, assignments
  any &operator=(const any &__rhs) {
    any(__rhs).swap(*this);
    return *this;
  }
  any &operator=(any &&__rhs) noexcept {
    any(std::move(__rhs)).swap(*this);
    return *this;
  }
  template <class _ValueType,
            typename = typename std::enable_if<
                std::is_copy_constructible<
                    typename std::decay<_ValueType>::type>::value &&
                !std::is_same<typename std::decay<_ValueType>::type,
                              any>::value>::type>
  any &operator=(_ValueType &&__rhs) {
    any{std::forward<_ValueType>(__rhs)}.swap(*this);
    return *this;
  }

  // 20.8.3.3, modifiers
  template <class _Tp, class... _Args> void emplace(_Args &&... __args) {
    reset();
    _M_ptr = new __any_value<typename std::decay<_Tp>::type>{
        std::forward<_Args>(__args)...};
  }
  template <class _Tp, class _Up, class... _Args,
            typename = typename std::enable_if<std::is_constructible<
                _Tp, std::initializer_list<_Up> &, _Args...>::value>::type>
  void emplace(std::initializer_list<_Up> __il, _Args &&... __args) {
    reset();
    _M_ptr = new __any_value<_Tp>{__il, std::forward<_Args>(__args)...};
  }
  void reset() noexcept {
    delete _M_ptr;
    _M_ptr = nullptr;
  }
  void swap(any &__rhs) noexcept { std::swap(_M_ptr, __rhs._M_ptr); }

  // 20.8.3.4, observers
  bool has_value() const noexcept { return _M_ptr; }
  const std::type_info &type() const noexcept {
    return has_value() ? _M_ptr->_M_type() : typeid(void);
  };

  template <class _ValueType>
  friend const _ValueType *any_cast(const any *operand) noexcept;

  template <class _ValueType>
  friend _ValueType *any_cast(any *operand) noexcept;
};

template <class _Tp, class... _Args> any make_any(_Args &&... __args) {
  return any(in_place<_Tp>, std::forward<_Args>(__args)...);
}
template <class _Tp, class _Up, class... _Args>
any make_any(std::initializer_list<_Up> __il, _Args &&... __args) {
  return any(in_place<_Tp>, __il, std::forward<_Args>(__args)...);
}

template <class _ValueType,
          class = typename std::enable_if<
              std::is_reference<_ValueType>::value ||
              std::is_copy_constructible<_ValueType>::value>::type>
_ValueType any_cast(const any &operand) {
  if (operand.type() !=
      typeid(typename std::remove_reference<_ValueType>::type))
    throw bad_any_cast();
  return *any_cast<typename std::add_const<
      typename std::remove_reference<_ValueType>::type>::type>(&operand);
}
template <class _ValueType,
          class = typename std::enable_if<
              std::is_reference<_ValueType>::value ||
              std::is_copy_constructible<_ValueType>::value>::type>
_ValueType any_cast(any &operand) {
  if (operand.type() !=
      typeid(typename std::remove_reference<_ValueType>::type))
    throw bad_any_cast();
  return *any_cast<typename std::remove_reference<_ValueType>::type>(&operand);
}
template <
    class _ValueType, class = typename std::enable_if<
                          std::is_reference<_ValueType>::value ||
                          std::is_copy_constructible<_ValueType>::value>::type,
    typename std::enable_if<!std::is_move_constructible<_ValueType>::value ||
                                std::is_lvalue_reference<_ValueType>::value,
                            bool>::type = true>
_ValueType any_cast(any &&operand) {
  auto __p =
      any_cast<typename std::remove_reference<_ValueType>::type>(&operand);
  if (__p)
    return *__p;
  throw bad_any_cast();
}

template <
    class _ValueType, class = typename std::enable_if<
                          std::is_reference<_ValueType>::value ||
                          std::is_copy_constructible<_ValueType>::value>::type,
    typename std::enable_if<std::is_move_constructible<_ValueType>::value &&
                                !std::is_lvalue_reference<_ValueType>::value,
                            bool>::type = false>
_ValueType any_cast(any &&operand) {
  auto __p =
      any_cast<typename std::remove_reference<_ValueType>::type>(&operand);
  if (__p)
    return std::move(*__p);
  throw bad_any_cast();
}

template <class _ValueType>
const _ValueType *any_cast(const any *operand) noexcept {
  if (operand != nullptr && operand->type() == typeid(_ValueType))
    return std::addressof(
        static_cast<const __any_value<_ValueType> *>(operand->_M_ptr)
            ->_M_value);
  return nullptr;
}
template <class _ValueType> _ValueType *any_cast(any *operand) noexcept {
  if (operand != nullptr && operand->type() == typeid(_ValueType))
    return std::addressof(
        static_cast<__any_value<_ValueType> *>(operand->_M_ptr)->_M_value);
  return nullptr;
}
}
}

namespace std {
void swap(stl_backports::stl17::any &__x,
          stl_backports::stl17::any &__y) noexcept {
  __x.swap(__y);
}
}
