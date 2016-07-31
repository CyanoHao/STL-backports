#pragma once

#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "../../__common/constexpr_addressof.hpp"
#include "../__helper/in_place.hpp"

namespace stl_backports {
namespace stl17 {

template <class _Tp> class optional;

struct nullopt_t {
  explicit constexpr nullopt_t(int) {}
};

constexpr nullopt_t nullopt(0);

class bad_optional_access : public std::logic_error {
public:
  bad_optional_access() : std::logic_error("bad optional access") {}
  virtual ~bad_optional_access() noexcept = default;
};

template <class _Tp, bool = std::is_trivially_destructible<_Tp>::value>
class __optional_base;

template <class _Tp> class __optional_base<_Tp, true> {
public:
  constexpr __optional_base() noexcept : __null_state(0), __engaged(false) {}
  __optional_base(const __optional_base<_Tp> &__rhs)
      : __engaged(__rhs.__engaged) {
    if (__engaged)
      new (&__val) _Tp(__rhs.__val);
  }
  __optional_base(__optional_base<_Tp> &&__rhs) noexcept(
      std::is_nothrow_move_constructible<_Tp>::value)
      : __engaged(__rhs.__engaged) {
    if (__engaged)
      new (&__val) _Tp(std::move(__rhs.__val));
  }
  constexpr __optional_base(const _Tp &__value)
      : __val(__value), __engaged(true) {}
  constexpr __optional_base(_Tp &&__value)
      : __val(std::forward<_Tp>(__value)), __engaged(true) {}
  template <class... _Args>
  constexpr explicit __optional_base(in_place_t, _Args &&... __args)
      : __val(std::forward<_Args>(__args)...), __engaged(true) {}
  template <class _Up, class... _Args>
  constexpr explicit __optional_base(in_place_t,
                                     std::initializer_list<_Up> __il,
                                     _Args &&... __args)
      : __val(__il, std::forward<_Args>(__args)...), __engaged(true) {}

  ~__optional_base() = default;

protected:
  union {
    char __null_state;
    _Tp __val;
  };
  bool __engaged;
};

template <class _Tp> class __optional_base<_Tp, false> {
public:
  constexpr __optional_base() noexcept : __null_state(0), __engaged(false) {}
  __optional_base(const __optional_base<_Tp> &__rhs)
      : __engaged(__rhs.__engaged) {
    if (__engaged)
      new (&__val) _Tp(__rhs.__val);
  }
  __optional_base(__optional_base<_Tp> &&__rhs) noexcept(
      std::is_nothrow_move_constructible<_Tp>::value)
      : __engaged(__rhs.__engaged) {
    if (__engaged)
      new (&__val) _Tp(std::move(__rhs.__val));
  }
  constexpr __optional_base(const _Tp &__value)
      : __val(__value), __engaged(true) {}
  constexpr __optional_base(_Tp &&__value)
      : __val(std::forward<_Tp>(__value)), __engaged(true) {}
  template <class... _Args>
  constexpr explicit __optional_base(in_place_t, _Args &&... __args)
      : __val(std::forward<_Args>(__args)...), __engaged(true) {}
  template <class _Up, class... _Args>
  constexpr explicit __optional_base(in_place_t,
                                     std::initializer_list<_Up> __il,
                                     _Args &&... __args)
      : __val(__il, std::forward<_Args>(__args)...), __engaged(true) {}

  ~__optional_base() {
    if (__engaged)
      __val.~_Tp();
  }

protected:
  union {
    char __null_state;
    _Tp __val;
  };
  bool __engaged;
};

template <class _Tp> class optional : private __optional_base<_Tp> {
public:
  using value_type = _Tp;

  // constructors
  constexpr optional() noexcept : __optional_base<_Tp>(){};
  constexpr optional(nullopt_t) noexcept : __optional_base<_Tp>() {}
  optional(const optional<_Tp> &__rhs) = default;
  optional(optional<_Tp> &&__rhs) noexcept(
      std::is_nothrow_move_constructible<_Tp>::value)
      : __optional_base<_Tp>(std::forward<optional<_Tp>>(__rhs)){};
  constexpr optional(const _Tp &__value) : __optional_base<_Tp>(__value) {}
  constexpr optional(_Tp &&__value)
      : __optional_base<_Tp>(std::forward<_Tp>(__value)) {}
  template <class... _Args>
  constexpr explicit optional(in_place_t, _Args &&... __args)
      : __optional_base<_Tp>(in_place, std::forward<_Args>(__args)...) {}
  template <class _Up, class... _Args>
  constexpr explicit optional(in_place_t, std::initializer_list<_Up> __il,
                              _Args &&... __args)
      : __optional_base<_Tp>(in_place, __il, std::forward<_Args>(__args)...) {}

  // destructor
  ~optional() = default;

  // assignment
  optional<_Tp> &operator=(nullopt_t) noexcept {
    reset();
    return *this;
  }
  optional<_Tp> &operator=(const optional<_Tp> &__rhs) {
    if (*this) {
      if (__rhs)
        **this = *__rhs;
      else
        reset();
    } else {
      if (__rhs) {
        new (this->operator->()) _Tp(*__rhs);
        this->__engaged = true;
      } else {
      }
    }
    return *this;
  }
  optional<_Tp> &operator=(optional<_Tp> &&__rhs) noexcept(
      std::is_nothrow_move_assignable<_Tp>::value
          &&std::is_nothrow_move_constructible<_Tp>::value) {
    if (*this) {
      if (__rhs)
        **this = *std::move(__rhs);
      else
        reset();
    } else {
      if (__rhs) {
        new (this->operator->()) _Tp(*std::move(__rhs));
        __engage();
      } else {
      }
    }
    return *this;
  }
  template <class _Up,
            typename std::enable_if<
                std::is_same<typename std::decay<_Up>::type, _Tp>::value,
                bool>::type = true>
  optional<_Tp> &operator=(_Up &&__v) {
    if (*this)
      **this = std::forward<_Up>(__v);
    else {
      new (operator->()) _Tp(std::forward<_Up>(__v));
      this->__engaged = true;
    }
    return *this;
  }
  template <class... _Args> void emplace(_Args &&... __args) {
    reset();
    new (operator->()) _Tp(std::forward<_Args>(__args)...);
    __engage();
  }
  template <class _Up, class... _Args>
  void emplace(std::initializer_list<_Up> __il, _Args &&... __args) {
    reset();
    new (operator->()) _Tp(__il, std::forward<_Args>(__args)...);
    __engage();
  }

  // swap
  void swap(optional<_Tp> &__rhs) noexcept(
      std::is_nothrow_move_constructible<_Tp>::value &&noexcept(
          std::swap(std::declval<_Tp &>(), std::declval<_Tp &>()))) {
    if (*this) {
      if (__rhs) {
        std::swap(**this, *__rhs);
      } else {
        *this = *__rhs;
        __rhs.reset();
      }
    } else {
      if (__rhs) {
        __rhs = **this;
        reset();
      } else {
      }
    }
  }

  // observers
  constexpr const _Tp *operator->() const {
    return const_cast<optional<_Tp> *>(this)->operator->();
  }
  constexpr _Tp *operator->() {
    return __common::__constexpr_addressof(this->__val);
  }
  constexpr const _Tp &operator*() const & {
    return **const_cast<optional<_Tp> *>(this);
  }
  constexpr _Tp &operator*() & { return this->__val; }
  constexpr _Tp &&operator*() && { return std::move(**this); }
  constexpr const _Tp &&operator*() const && { return std::move(**this); }
  constexpr explicit operator bool() const noexcept { return has_value(); }
  constexpr bool has_value() const noexcept { return this->__engaged; }
  constexpr const _Tp &value() const & {
    return const_cast<optional<_Tp> *>(this)->value();
  }
  constexpr _Tp &value() & {
    if (*this)
      return **this;
    else
      throw bad_optional_access();
  }
  constexpr const _Tp &&value() const && { return std::move(this->value()); }
  constexpr _Tp &&value() && { return std::move(this->value()); }
  template <class _Up> constexpr _Tp value_or(_Up &&__v) const & {
    return bool(*this) ? **this : static_cast<_Tp>(std::forward<_Up>(__v));
  }
  template <class _Up> constexpr _Tp value_or(_Up &&__v) && {
    return bool(*this) ? std::move(**this)
                       : static_cast<_Tp>(std::forward<_Up>(__v));
  }

  // modifiers
  void reset() noexcept {
    if (this->__engaged) {
      this->__engaged = false;
      this->__val.~_Tp();
    }
  }

private:
  void __engage() noexcept { this->__engaged = true; }
};

// relational operators
template <class _Tp>
constexpr bool operator==(const optional<_Tp> &__x, const optional<_Tp> &__y) {
  return (!__x && !__y) || (__x && __y && *__x == *__y);
}
template <class _Tp>
constexpr bool operator!=(const optional<_Tp> &__x, const optional<_Tp> &__y) {
  return (bool(__x) != bool(__y)) || (__x && __y && *__x != *__y);
}
template <class _Tp>
constexpr bool operator<(const optional<_Tp> &__x, const optional<_Tp> &__y) {
  return !__y ? false : (!__x ? true : *__x < *__y);
}
template <class _Tp>
constexpr bool operator>(const optional<_Tp> &__x, const optional<_Tp> &__y) {
  return !__x ? false : (!__y ? true : *__x > *__y);
}
template <class _Tp>
constexpr bool operator<=(const optional<_Tp> &__x, const optional<_Tp> &__y) {
  return !__x ? true : (!__y ? false : *__x <= *__y);
}
template <class _Tp>
constexpr bool operator>=(const optional<_Tp> &__x, const optional<_Tp> &__y) {
  return !__y ? true : (!__x ? false : *__x >= *__y);
}

// comparison with nullopt
template <class _Tp>
constexpr bool operator==(const optional<_Tp> &__x, nullopt_t) noexcept {
  return !__x;
}
template <class _Tp>
constexpr bool operator==(nullopt_t, const optional<_Tp> &__x) noexcept {
  return !__x;
}
template <class _Tp>
constexpr bool operator!=(const optional<_Tp> &__x, nullopt_t) noexcept {
  return bool(__x);
}
template <class _Tp>
constexpr bool operator!=(nullopt_t, const optional<_Tp> &__x) noexcept {
  return bool(__x);
}
template <class _Tp>
constexpr bool operator<(const optional<_Tp> &, nullopt_t) noexcept {
  return false;
}
template <class _Tp>
constexpr bool operator<(nullopt_t, const optional<_Tp> &__x) noexcept {
  return bool(__x);
}
template <class _Tp>
constexpr bool operator<=(const optional<_Tp> &__x, nullopt_t) noexcept {
  return !__x;
}
template <class _Tp>
constexpr bool operator<=(nullopt_t, const optional<_Tp> &) noexcept {
  return true;
}
template <class _Tp>
constexpr bool operator>(const optional<_Tp> &__x, nullopt_t) noexcept {
  return bool(__x);
}
template <class _Tp>
constexpr bool operator>(nullopt_t, const optional<_Tp> &) noexcept {
  return false;
}
template <class _Tp>
constexpr bool operator>=(const optional<_Tp> &, nullopt_t) noexcept {
  return true;
}
template <class _Tp>
constexpr bool operator>=(nullopt_t, const optional<_Tp> &__x) noexcept {
  return !__x;
}

// comparison with T
template <class _Tp>
constexpr bool operator==(const optional<_Tp> &__x, const _Tp &__v) {
  return bool(__x) ? *__x == __v : false;
}
template <class _Tp>
constexpr bool operator==(const _Tp &__v, const optional<_Tp> &__x) {
  return bool(__x) ? __v == *__x : false;
}
template <class _Tp>
constexpr bool operator!=(const optional<_Tp> &__x, const _Tp &__v) {
  return bool(__x) ? *__x != __v : true;
}
template <class _Tp>
constexpr bool operator!=(const _Tp &__v, const optional<_Tp> &__x) {
  return bool(__x) ? __v != *__x : true;
}
template <class _Tp>
constexpr bool operator<(const optional<_Tp> &__x, const _Tp &__v) {
  return bool(__x) ? *__x < __v : true;
}
template <class _Tp>
constexpr bool operator<(const _Tp &__v, const optional<_Tp> &__x) {
  return bool(__x) ? __v < *__x : false;
}
template <class _Tp>
constexpr bool operator<=(const optional<_Tp> &__x, const _Tp &__v) {
  return bool(__x) ? *__x <= __v : true;
}
template <class _Tp>
constexpr bool operator<=(const _Tp &__v, const optional<_Tp> &__x) {
  return bool(__x) ? __v <= *__x : false;
}
template <class _Tp>
constexpr bool operator>(const optional<_Tp> &__x, const _Tp &__v) {
  return bool(__x) ? *__x > __v : false;
}
template <class _Tp>
constexpr bool operator>(const _Tp &__v, const optional<_Tp> &__x) {
  return bool(__x) ? __v > *__x : true;
}
template <class _Tp>
constexpr bool operator>=(const optional<_Tp> &__x, const _Tp &__v) {
  return bool(__x) ? *__x >= __v : false;
}
template <class _Tp>
constexpr bool operator>=(const _Tp &__v, const optional<_Tp> &__x) {
  return bool(__x) ? __v >= *__x : true;
}

template <class _Tp>
constexpr optional<typename std::decay<_Tp>::type> make_optional(_Tp &&__v) {
  return optional<typename std::decay<_Tp>::type>(std::forward<_Tp>(__v));
}
template <class _Tp, class... _Args>
constexpr optional<_Tp> make_optional(_Args &&... __args) {
  return optional<_Tp>(in_place, std::forward<_Args>(__args)...);
}
template <class _Tp, class _Up, class... _Args>
constexpr optional<_Tp> make_optional(std::initializer_list<_Up> __il,
                                      _Args &&... __args) {
  return optional<_Tp>(in_place, __il, std::forward<_Args>(__args)...);
}
}
}

namespace std {
template <class _Tp>
void swap(stl_backports::stl17::optional<_Tp> &__x,
          stl_backports::stl17::optional<_Tp>
              &__y) noexcept(noexcept(__x.swap(__y))) {
  __x.swap(__y);
}

template <class _Tp> struct hash<stl_backports::stl17::optional<_Tp>> {
  typedef _Tp argument_type;
  typedef size_t result_type;

  size_t operator()(stl_backports::stl17::optional<_Tp> __k) {
    return __k ? hash<_Tp>()(*__k) : size_t(0);
  }
};
}
