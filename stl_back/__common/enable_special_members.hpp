#pragma once

namespace stl_backports {
namespace __common {

template <bool _Switch, typename _Tag = void>
struct __enable_default_constructor {};

template <typename _Tag> struct __enable_default_constructor<false, _Tag> {
  constexpr __enable_default_constructor() noexcept = delete;
};

template <bool _Switch, typename _Tag = void> struct __enable_destructor {};

template <typename _Tag> struct __enable_destructor<false, _Tag> {
  ~__enable_destructor() noexcept = delete;
};

template <bool _Copy, bool _CopyAssignment, bool _Move, bool _MoveAssignment,
          class _Tag = void>
struct __enable_copy_move {};

template <typename _Tag>
struct __enable_copy_move<false, true, true, true, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = delete;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = default;
};

template <typename _Tag>
struct __enable_copy_move<true, false, true, true, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = default;
};

template <typename _Tag>
struct __enable_copy_move<false, false, true, true, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = delete;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = default;
};

template <typename _Tag>
struct __enable_copy_move<true, true, false, true, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = default;
};

template <typename _Tag>
struct __enable_copy_move<false, true, false, true, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = delete;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = default;
};

template <typename _Tag>
struct __enable_copy_move<true, false, false, true, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = default;
};

template <typename _Tag>
struct __enable_copy_move<false, false, false, true, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = delete;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = default;
};

template <typename _Tag>
struct __enable_copy_move<true, true, true, false, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = delete;
};

template <typename _Tag>
struct __enable_copy_move<false, true, true, false, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = delete;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = delete;
};

template <typename _Tag>
struct __enable_copy_move<true, false, true, false, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = delete;
};

template <typename _Tag>
struct __enable_copy_move<false, false, true, false, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = delete;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = delete;
};

template <typename _Tag>
struct __enable_copy_move<true, true, false, false, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = delete;
};

template <typename _Tag>
struct __enable_copy_move<false, true, false, false, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = delete;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = default;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = delete;
};

template <typename _Tag>
struct __enable_copy_move<true, false, false, false, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = delete;
};

template <typename _Tag>
struct __enable_copy_move<false, false, false, false, _Tag> {
  constexpr __enable_copy_move() noexcept = default;
  constexpr __enable_copy_move(__enable_copy_move const &) noexcept = delete;
  constexpr __enable_copy_move(__enable_copy_move &&) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move const &) noexcept = delete;
  __enable_copy_move &operator=(__enable_copy_move &&) noexcept = delete;
};

template <bool _Default, bool _Destructor, bool _Copy, bool _CopyAssignment,
          bool _Move, bool _MoveAssignment, typename _Tag = void>
struct _Enable_special_members
    : private __enable_default_constructor<_Default, _Tag>,
      private __enable_destructor<_Destructor, _Tag>,
      private __enable_copy_move<_Copy, _CopyAssignment, _Move, _MoveAssignment,
                                 _Tag> {};
}
}
