#pragma once

#include <cstddef>
#include <exception>

namespace stl_backports {
namespace stl17 {

struct in_place_tag {
  in_place_tag() = delete;
};

struct __in_place;
template <typename _Tp> struct __in_place_type;
template <std::size_t _Idx> struct __in_place_index;

using in_place_t = in_place_tag (&)(__in_place *);
template <class _Tp>
using in_place_type_t = in_place_tag (&)(__in_place_type<_Tp> *);
template <std::size_t _Idx>
using in_place_index_t = in_place_tag (&)(__in_place_index<_Idx> *);

inline in_place_tag in_place(__in_place *) { std::terminate(); }
template <class _Tp> in_place_tag in_place(__in_place_type<_Tp> *) {
  std::terminate();
}
template <std::size_t _Idx> in_place_tag in_place(__in_place_index<_Idx> *) {
  std::terminate();
}
}
}
