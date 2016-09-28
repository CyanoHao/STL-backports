/**
STL Backports headers

std::is{, _nothrow}_swappable{<T>, _with<T, U>} in <type_traits> since C++17

This implemention is based on N4606.
  C++14    -- fully support
  C++11    -- fully support
  C++03/98 -- not support

Copyright 2016  Cyano Hao <c@cyano.cn>
*/

#pragma once

#include <type_traits>
#include <utility>

namespace stl_backports {
namespace stl17 {

template <class _Tp, class _Up> struct __is_swappable_with_impl {
  template <class...> struct __true { static constexpr bool __value = true; };

  static typename std::enable_if<
      __true<decltype(std::swap(std::declval<_Tp>(), std::declval<_Up>())),
             decltype(std::swap(std::declval<_Up>(),
                                std::declval<_Tp>()))>::__value,
      std::true_type>::type
  __test(int);

  static std::false_type __test(...);

  typedef decltype(__test(0)) type;
};

template <class _Tp, class _Up> struct __is_nothrow_swappable_with_impl {
  static typename std::enable_if<
      noexcept(std::swap(std::declval<_Tp>(), std::declval<_Up>())) &&
          noexcept(std::swap(std::declval<_Up>(), std::declval<_Tp>())),
      std::true_type>::type
  __test(int);

  static std::false_type __test(...);

  typedef decltype(__test(0)) type;
};

template <class _Tp, class _Up>
struct is_swappable_with : __is_swappable_with_impl<_Tp, _Up>::type {};

template <class _Tp, class _Up>
struct is_nothrow_swappable_with
    : __is_nothrow_swappable_with_impl<_Tp, _Up>::type {};

template <class _Tp> struct is_swappable : is_swappable_with<_Tp, _Tp>::type {};

template <class _Tp>
struct is_nothrow_swappable : is_nothrow_swappable_with<_Tp, _Tp>::type {};
}
}
