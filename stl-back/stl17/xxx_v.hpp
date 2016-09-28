/**
STL Backports headers

std::xxx_v in <type_traits> since C++17

This implemention is based on N4606.
  C++14    -- almost fully support
  C++11    -- not support
  C++03/98 -- not support

Copyright 2016  Cyano Hao <c@cyano.cn>
*/

#pragma once

#include "../__common/cxxver.h"

#if !__STL_BACKPORTS__HAVE_CXX14
#error "Not support"
#endif

#include <type_traits>

#include "conjunction.hpp"
#include "disjunction.hpp"
#include "is_swappable.hpp"
#include "negation.hpp"

namespace stl_backports {
namespace stl17 {

// primary type categories
template <class _Tp> constexpr bool is_void_v = std::is_void<_Tp>::value;
template <class _Tp>
constexpr bool is_null_pointer_v = std::is_null_pointer<_Tp>::value;
template <class _Tp>
constexpr bool is_integral_v = std::is_integral<_Tp>::value;
template <class _Tp>
constexpr bool is_floating_point_v = std::is_floating_point<_Tp>::value;
template <class _Tp> constexpr bool is_array_v = std::is_array<_Tp>::value;
template <class _Tp> constexpr bool is_pointer_v = std::is_pointer<_Tp>::value;
template <class _Tp>
constexpr bool is_lvalue_reference_v = std::is_lvalue_reference<_Tp>::value;
template <class _Tp>
constexpr bool is_rvalue_reference_v = std::is_rvalue_reference<_Tp>::value;
template <class _Tp>
constexpr bool is_member_object_pointer_v =
    std::is_member_object_pointer<_Tp>::value;
template <class _Tp>
constexpr bool is_member_function_pointer_v =
    std::is_member_function_pointer<_Tp>::value;
template <class _Tp> constexpr bool is_enum_v = std::is_enum<_Tp>::value;
template <class _Tp> constexpr bool is_union_v = std::is_union<_Tp>::value;
template <class _Tp> constexpr bool is_class_v = std::is_class<_Tp>::value;
template <class _Tp>
constexpr bool is_function_v = std::is_function<_Tp>::value;

// composite type categories
template <class _Tp>
constexpr bool is_reference_v = std::is_reference<_Tp>::value;
template <class _Tp>
constexpr bool is_arithmetic_v = std::is_arithmetic<_Tp>::value;
template <class _Tp>
constexpr bool is_fundamental_v = std::is_fundamental<_Tp>::value;
template <class _Tp> constexpr bool is_object_v = std::is_object<_Tp>::value;
template <class _Tp> constexpr bool is_scalar_v = std::is_scalar<_Tp>::value;
template <class _Tp>
constexpr bool is_compound_v = std::is_compound<_Tp>::value;
template <class _Tp>
constexpr bool is_member_pointer_v = std::is_member_pointer<_Tp>::value;

// type properties
template <class _Tp> constexpr bool is_const_v = std::is_const<_Tp>::value;
template <class _Tp>
constexpr bool is_volatile_v = std::is_volatile<_Tp>::value;
template <class _Tp> constexpr bool is_trivial_v = std::is_trivial<_Tp>::value;
template <class _Tp>
constexpr bool is_trivially_copyable_v = std::is_trivially_copyable<_Tp>::value;
template <class _Tp>
constexpr bool is_standard_layout_v = std::is_standard_layout<_Tp>::value;
template <class _Tp> constexpr bool is_pod_v = std::is_pod<_Tp>::value;
template <class _Tp> constexpr bool is_empty_v = std::is_empty<_Tp>::value;
template <class _Tp>
constexpr bool is_polymorphic_v = std::is_polymorphic<_Tp>::value;
template <class _Tp>
constexpr bool is_abstract_v = std::is_abstract<_Tp>::value;
template <class _Tp> constexpr bool is_final_v = std::is_final<_Tp>::value;
template <class _Tp> constexpr bool is_signed_v = std::is_signed<_Tp>::value;
template <class _Tp>
constexpr bool is_unsigned_v = std::is_unsigned<_Tp>::value;
template <class _Tp, class... _Args>
constexpr bool is_constructible_v = std::is_constructible<_Tp, _Args...>::value;
template <class _Tp>
constexpr bool is_default_constructible_v =
    std::is_default_constructible<_Tp>::value;
template <class _Tp>
constexpr bool is_copy_constructible_v = std::is_copy_constructible<_Tp>::value;
template <class _Tp>
constexpr bool is_move_constructible_v = std::is_move_constructible<_Tp>::value;
template <class _Tp, class _Up>
constexpr bool is_assignable_v = std::is_assignable<_Tp, _Up>::value;
template <class _Tp>
constexpr bool is_copy_assignable_v = std::is_copy_assignable<_Tp>::value;
template <class _Tp>
constexpr bool is_move_assignable_v = std::is_move_assignable<_Tp>::value;
template <class _Tp, class _Up>
constexpr bool is_swappable_with_v = is_swappable_with<_Tp, _Up>::value;
template <class _Tp> constexpr bool is_swappable_v = is_swappable<_Tp>::value;
template <class _Tp>
constexpr bool is_destructible_v = std::is_destructible<_Tp>::value;
template <class _Tp, class... _Args>
constexpr bool is_trivially_constructible_v =
    std::is_trivially_constructible<_Tp, _Args...>::value;
template <class _Tp>
constexpr bool is_trivially_default_constructible_v =
    std::is_trivially_default_constructible<_Tp>::value;
template <class _Tp>
constexpr bool is_trivially_copy_constructible_v =
    std::is_trivially_copy_constructible<_Tp>::value;
template <class _Tp>
constexpr bool is_trivially_move_constructible_v =
    std::is_trivially_move_constructible<_Tp>::value;
template <class _Tp, class _Up>
constexpr bool is_trivially_assignable_v =
    std::is_trivially_assignable<_Tp, _Up>::value;
template <class _Tp>
constexpr bool is_trivially_copy_assignable_v =
    std::is_trivially_copy_assignable<_Tp>::value;
template <class _Tp>
constexpr bool is_trivially_move_assignable_v =
    std::is_trivially_move_assignable<_Tp>::value;
template <class _Tp>
constexpr bool is_trivially_destructible_v =
    std::is_trivially_destructible<_Tp>::value;
template <class _Tp, class... _Args>
constexpr bool is_nothrow_constructible_v =
    std::is_nothrow_constructible<_Tp, _Args...>::value;
template <class _Tp>
constexpr bool is_nothrow_default_constructible_v =
    std::is_nothrow_default_constructible<_Tp>::value;
template <class _Tp>
constexpr bool is_nothrow_copy_constructible_v =
    std::is_nothrow_copy_constructible<_Tp>::value;
template <class _Tp>
constexpr bool is_nothrow_move_constructible_v =
    std::is_nothrow_move_constructible<_Tp>::value;
template <class _Tp, class _Up>
constexpr bool is_nothrow_assignable_v =
    std::is_nothrow_assignable<_Tp, _Up>::value;
template <class _Tp>
constexpr bool is_nothrow_copy_assignable_v =
    std::is_nothrow_copy_assignable<_Tp>::value;
template <class _Tp>
constexpr bool is_nothrow_move_assignable_v =
    std::is_nothrow_move_assignable<_Tp>::value;
template <class _Tp, class _Up>
constexpr bool is_nothrow_swappable_with_v =
    is_nothrow_swappable_with<_Tp, _Up>::value;
template <class _Tp>
constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<_Tp>::value;
template <class _Tp>
constexpr bool is_nothrow_destructible_v =
    std::is_nothrow_destructible<_Tp>::value;
template <class _Tp>
constexpr bool has_virtual_destructor_v =
    std::has_virtual_destructor<_Tp>::value;

// type property queries
template <class _Tp>
constexpr std::size_t alignment_of_v = std::alignment_of<_Tp>::value;
template <class _Tp> constexpr std::size_t rank_v = std::rank<_Tp>::value;
template <class _Tp, unsigned _I = 0>
constexpr std::size_t extent_v = std::extent<_Tp, _I>::value;

// type relations
template <class _Tp, class _Up>
constexpr bool is_same_v = std::is_same<_Tp, _Up>::value;
template <class _Base, class _Derived>
constexpr bool is_base_of_v = std::is_base_of<_Base, _Derived>::value;
template <class _From, class _To>
constexpr bool is_convertible_v = std::is_convertible<_From, _To>::value;
// no std::is_callable<T, Args..., R> in C++14
// no std::is_nothrow_callable<T, Args..., R> in C++14

// logical operator traits
template <class... _B> constexpr bool conjunction_v = conjunction<_B...>::value;
template <class... _B> constexpr bool disjunction_v = disjunction<_B...>::value;
template <class _B> constexpr bool negation_v = negation<_B>::value;
}
}
