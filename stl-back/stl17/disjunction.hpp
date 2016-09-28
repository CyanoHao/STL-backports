/**
STL Backports headers

std::disjunction<B> in <type_traits> since C++17

This implemention is based on N4606.
  C++14    -- fully support
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
namespace stl17 {

template <class...> struct disjunction : std::false_type {};
template <class _B1> struct disjunction<_B1> : _B1 {};
template <class _B1, class... _Bn>
struct disjunction<_B1, _Bn...>
    : std::conditional_t<_B1::value != false, _B1, disjunction<_Bn...>> {};
}
}
