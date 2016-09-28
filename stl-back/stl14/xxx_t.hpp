/**
STL Backports headers

std::xxx_t in <type_traits> since C++14

This implemention is based on N4296.
  C++11    -- fully support
  C++03/98 -- not support

Copyright 2016  Cyano Hao <c@cyano.cn>
*/

#pragma once

#include "../__common/cxxver.h"

#if !__STL_BACKPORTS__HAVE_CXX11
#error "Not support"
#endif

#include <type_traits>

namespace stl_backports {
namespace stl14 {

// const-volatile modifications
template <class _Tp>
using remove_const_t = typename std::remove_const<_Tp>::type;
template <class _Tp>
using remove_volatile_t = typename std::remove_volatile<_Tp>::type;
template <class _Tp> using remove_cv_t = typename std::remove_cv<_Tp>::type;
template <class _Tp> using add_const_t = typename std::add_const<_Tp>::type;
template <class _Tp>
using add_volatile_t = typename std::add_volatile<_Tp>::type;
template <class _Tp> using add_cv_t = typename std::add_cv<_Tp>::type;

// reference modifications
template <class _Tp>
using remove_reference_t = typename std::remove_reference<_Tp>::type;
template <class _Tp>
using add_lvalue_reference_t = typename std::add_lvalue_reference<_Tp>::type;
template <class _Tp>
using add_rvalue_reference_t = typename std::add_rvalue_reference<_Tp>::type;

// sign modifications
template <class _Tp> using make_signed_t = typename std::make_signed<_Tp>::type;
template <class _Tp>
using make_unsigned_t = typename std::make_unsigned<_Tp>::type;

// array modifications
template <class _Tp>
using remove_extent_t = typename std::remove_extent<_Tp>::type;
template <class _Tp>
using remove_all_extents_t = typename std::remove_all_extents<_Tp>::type;

// pointer modifications
template <class _Tp>
using remove_pointer_t = typename std::remove_pointer<_Tp>::type;
template <class _Tp> using add_pointer_t = typename std::add_pointer<_Tp>::type;

template <std::size_t _Len> struct __aligned_storage_size {
  union __type {
    char __data[_Len];
    struct __attribute__((__aligned__)) {
    } __align;
  };
};

// other transformations
template <std::size_t _Len, std::size_t _Align = alignof(
                                typename __aligned_storage_size<_Len>::__type)>
using aligned_storage_t = typename std::aligned_storage<_Len, _Align>::type;
template <std::size_t _Len, class... Types>
using aligned_union_t = typename std::aligned_union<_Len, Types...>::type;
template <class _Tp> using decay_t = typename std::decay<_Tp>::type;
template <bool __b, class _Tp = void>
using enable_if_t = typename std::enable_if<__b, _Tp>::type;
template <bool __b, class _Tp, class _Fp>
using conditional_t = typename std::conditional<__b, _Tp, _Fp>::type;
template <class... _Tp>
using common_type_t = typename std::common_type<_Tp...>::type;
template <class _Tp>
using underlying_type_t = typename std::underlying_type<_Tp>::type;
template <class _Tp> using result_of_t = typename std::result_of<_Tp>::type;
template <class...> using void_t = void;
}
}
