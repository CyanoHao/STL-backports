#pragma once

namespace stl_backports {
namespace __common {
template <class _Tp> constexpr _Tp *__constexpr_addressof(_Tp &__arg) {
  return reinterpret_cast<_Tp *>(
      &const_cast<char &>(reinterpret_cast<const volatile char &>(__arg)));
}
}
}
