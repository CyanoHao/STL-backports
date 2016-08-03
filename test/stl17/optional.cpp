// Copyright (C) 2013-2016 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a moved_to of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

/**
Test suite for STL Backports

optional since C++ 17

This testsuite is originally from GCC's libstdc++.

Modified by Cyano Hao <c@cyano.cn>
*/

#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <vector>

#include <stl_back/stl17/optional.hpp>

#include "../common/hooks.h"

using stl_backports::stl17::bad_optional_access;
using stl_backports::stl17::in_place;
using stl_backports::stl17::in_place_t;
using stl_backports::stl17::make_optional;
using stl_backports::stl17::nullopt;
using stl_backports::stl17::nullopt_t;
using stl_backports::stl17::optional;

namespace in_place_test {
void main() {
  {
    optional<int> o{in_place};
    VERIFY(o);
    VERIFY(*o == int());

    static_assert(!std::is_convertible<in_place_t, optional<int>>(), "");
  }

  {
    optional<int> o{in_place, 42};
    VERIFY(o);
    VERIFY(*o == 42);
  }

  {
    optional<std::vector<int>> o{in_place, 18, 4};
    VERIFY(o);
    VERIFY(o->size() == 18);
    VERIFY((*o)[17] == 4);
  }

  {
    optional<std::vector<int>> o{in_place, {18, 4}};
    VERIFY(o);
    VERIFY(o->size() == 2);
    VERIFY((*o)[0] == 18);
  }

  {
    optional<std::vector<int>> o{in_place, {18, 4}, std::allocator<int>{}};
    VERIFY(o);
    VERIFY(o->size() == 2);
    VERIFY((*o)[0] == 18);
  }
}
}

namespace make_optional_test {
struct combined {
  std::vector<int> v;
  std::tuple<int, int> t;
  template <class... Args>
  combined(std::initializer_list<int> il, Args &&... args)
      : v(il), t(std::forward<Args>(args)...) {}
};

void main() {
  const int i = 42;
  auto o = make_optional(i);
  static_assert(std::is_same<decltype(o), optional<int>>(), "");
  VERIFY(o && *o == 42);
  VERIFY(&*o != &i);
  auto o2 = make_optional<std::tuple<int, int>>(1, 2);
  static_assert(std::is_same<decltype(o2), optional<std::tuple<int, int>>>(),
                "");
  VERIFY(o2 && std::get<0>(*o2) == 1 && std::get<1>(*o2) == 2);
  auto o3 = make_optional<std::vector<int>>({42, 666});
  static_assert(std::is_same<decltype(o3), optional<std::vector<int>>>(), "");
  VERIFY(o3 && (*o3)[0] == 42 && (*o3)[1] == 666);
  auto o4 = make_optional<combined>({42, 666});
  static_assert(std::is_same<decltype(o4), optional<combined>>(), "");
  VERIFY(o4 && (o4->v)[0] == 42 && (o4->v)[1] == 666 &&
         std::get<0>(o4->t) == 0 && std::get<1>(o4->t) == 0);
  auto o5 = make_optional<combined>({1, 2}, 3, 4);
  static_assert(std::is_same<decltype(o5), optional<combined>>(), "");
  VERIFY(o4 && (o5->v)[0] == 1 && (o5->v)[1] == 2 && std::get<0>(o5->t) == 3 &&
         std::get<1>(o5->t) == 4);
}
}

namespace nullopt_test {
void main() {
  // [20.5.6] Disengaged state indicator
  static_assert(std::is_same<decltype(nullopt), const nullopt_t>(), "");
  static_assert(std::is_empty<nullopt_t>(), "");
  static_assert(std::is_literal_type<nullopt_t>(), "");
  static_assert(!std::is_default_constructible<nullopt_t>(), "");

  {
    optional<int> o = nullopt;
    VERIFY(!o);
  }

  {
    optional<int> o = {nullopt};
    VERIFY(!o);
  }

  {
    optional<int> o{nullopt};
    VERIFY(!o);
  }
}
}

namespace typedefs_test {
template <typename T> using check1_t = optional<T>;

using check2_t = in_place_t;
using check3_t = nullopt_t;
using check4_t = bad_optional_access;

static_assert(std::is_base_of<std::logic_error, check4_t>::value,
              "bad_optional_access must derive from logic_error");
}

namespace assignment_test {
namespace _1 {
struct exception {};

int counter = 0;

struct mixin_counter {
  mixin_counter() { ++counter; }
  mixin_counter(mixin_counter const &) { ++counter; }
  ~mixin_counter() { --counter; }
};

struct value_type : private mixin_counter {
  enum state_type {
    zero,
    moved_from,
    throwing_construction,
    throwing_copy,
    throwing_copy_assignment,
    throwing_move,
    throwing_move_assignment,
    threw,
  };

  value_type() = default;

  explicit value_type(state_type state_) : state(state_) {
    throw_if(throwing_construction);
  }

  value_type(value_type const &other) : mixin_counter(), state(other.state) {
    throw_if(throwing_copy);
  }

  value_type &operator=(value_type const &other) {
    state = other.state;
    throw_if(throwing_copy_assignment);
    return *this;
  }

  value_type(value_type &&other) : state(other.state) {
    other.state = moved_from;
    throw_if(throwing_move);
  }

  value_type &operator=(value_type &&other) {
    state = other.state;
    other.state = moved_from;
    throw_if(throwing_move_assignment);
    return *this;
  }

  void throw_if(state_type match) {
    if (state == match) {
      state = threw;
      throw exception{};
    }
  }

  state_type state = zero;
};

void main() {
  using O = optional<value_type>;
  using S = value_type::state_type;
  auto const make = [](S s = S::zero) { return O{in_place, s}; };

  enum outcome_type { nothrow, caught, bad_catch };

  // Check copy/move assignment for disengaged optional

  // From disengaged optional
  {
    O o;
    VERIFY(!o);
    O p;
    o = p;
    VERIFY(!o);
    VERIFY(!p);
  }

  {
    O o;
    VERIFY(!o);
    O p;
    o = std::move(p);
    VERIFY(!o);
    VERIFY(!p);
  }

  {
    O o;
    VERIFY(!o);
    o = {};
    VERIFY(!o);
  }

  // From engaged optional
  {
    O o;
    VERIFY(!o);
    O p = make(S::throwing_copy_assignment);
    o = p;
    VERIFY(o && o->state == S::throwing_copy_assignment);
    VERIFY(p && p->state == S::throwing_copy_assignment);
  }

  {
    O o;
    VERIFY(!o);
    O p = make(S::throwing_move_assignment);
    o = std::move(p);
    VERIFY(o && o->state == S::throwing_move_assignment);
    VERIFY(p && p->state == S::moved_from);
  }

  {
    outcome_type outcome{};
    O o;
    VERIFY(!o);
    O p = make(S::throwing_copy);

    try {
      o = p;
    } catch (exception const &) {
      outcome = caught;
    } catch (...) {
      outcome = bad_catch;
    }

    VERIFY(outcome == caught);
    VERIFY(!o);
    VERIFY(p && p->state == S::throwing_copy);
  }

  {
    outcome_type outcome{};
    O o;
    VERIFY(!o);
    O p = make(S::throwing_move);

    try {
      o = std::move(p);
    } catch (exception const &) {
      outcome = caught;
    } catch (...) {
      outcome = bad_catch;
    }

    VERIFY(outcome == caught);
    VERIFY(!o);
    VERIFY(p && p->state == S::moved_from);
  }

  VERIFY(counter == 0);
}
}
namespace _2 {
struct exception {};

int counter = 0;

struct mixin_counter {
  mixin_counter() { ++counter; }
  mixin_counter(mixin_counter const &) { ++counter; }
  ~mixin_counter() { --counter; }
};

struct value_type : private mixin_counter {
  enum state_type {
    zero,
    moved_from,
    throwing_construction,
    throwing_copy,
    throwing_copy_assignment,
    throwing_move,
    throwing_move_assignment,
    threw,
  };

  value_type() = default;

  explicit value_type(state_type state_) : state(state_) {
    throw_if(throwing_construction);
  }

  value_type(value_type const &other) : mixin_counter(), state(other.state) {
    throw_if(throwing_copy);
  }

  value_type &operator=(value_type const &other) {
    state = other.state;
    throw_if(throwing_copy_assignment);
    return *this;
  }

  value_type(value_type &&other) : state(other.state) {
    other.state = moved_from;
    throw_if(throwing_move);
  }

  value_type &operator=(value_type &&other) {
    state = other.state;
    other.state = moved_from;
    throw_if(throwing_move_assignment);
    return *this;
  }

  void throw_if(state_type match) {
    if (state == match) {
      state = threw;
      throw exception{};
    }
  }

  state_type state = zero;
};

void main() {
  using O = optional<value_type>;
  using S = value_type::state_type;
  auto const make = [](S s = S::zero) { return O{in_place, s}; };

  enum outcome_type { nothrow, caught, bad_catch };

  // Check copy/move assignment for engaged optional

  // From disengaged optional
  {
    O o = make(S::zero);
    VERIFY(o);
    O p;
    o = p;
    VERIFY(!o);
    VERIFY(!p);
  }

  {
    O o = make(S::zero);
    VERIFY(o);
    O p;
    o = std::move(p);
    VERIFY(!o);
    VERIFY(!p);
  }

  {
    O o = make(S::zero);
    VERIFY(o);
    o = {};
    VERIFY(!o);
  }

  // From engaged optional
  {
    O o = make(S::zero);
    VERIFY(o);
    O p = make(S::throwing_copy);
    o = p;
    VERIFY(o && o->state == S::throwing_copy);
    VERIFY(p && p->state == S::throwing_copy);
  }

  {
    O o = make(S::zero);
    VERIFY(o);
    O p = make(S::throwing_move);
    o = std::move(p);
    VERIFY(o && o->state == S::throwing_move);
    VERIFY(p && p->state == S::moved_from);
  }

  {
    outcome_type outcome{};
    O o = make(S::zero);
    VERIFY(o);
    O p = make(S::throwing_copy_assignment);

    try {
      o = p;
    } catch (exception const &) {
      outcome = caught;
    } catch (...) {
      outcome = bad_catch;
    }

    VERIFY(o && o->state == S::threw);
    VERIFY(p && p->state == S::throwing_copy_assignment);
  }

  {
    outcome_type outcome{};
    O o = make(S::zero);
    VERIFY(o);
    O p = make(S::throwing_move_assignment);

    try {
      o = std::move(p);
    } catch (exception const &) {
      outcome = caught;
    } catch (...) {
      outcome = bad_catch;
    }

    VERIFY(o && o->state == S::threw);
    VERIFY(p && p->state == S::moved_from);
  }

  VERIFY(counter == 0);
}
}
namespace _3 {
struct exception {};

int counter = 0;

struct mixin_counter {
  mixin_counter() { ++counter; }
  mixin_counter(mixin_counter const &) { ++counter; }
  ~mixin_counter() { --counter; }
};

struct value_type : private mixin_counter {
  enum state_type {
    zero,
    moved_from,
    throwing_construction,
    throwing_copy,
    throwing_copy_assignment,
    throwing_move,
    throwing_move_assignment,
    threw,
  };

  value_type() = default;

  explicit value_type(state_type state_) : state(state_) {
    throw_if(throwing_construction);
  }

  value_type(value_type const &other) : mixin_counter(), state(other.state) {
    throw_if(throwing_copy);
  }

  value_type &operator=(value_type const &other) {
    state = other.state;
    throw_if(throwing_copy_assignment);
    return *this;
  }

  value_type(value_type &&other) : state(other.state) {
    other.state = moved_from;
    throw_if(throwing_move);
  }

  value_type &operator=(value_type &&other) {
    state = other.state;
    other.state = moved_from;
    throw_if(throwing_move_assignment);
    return *this;
  }

  void throw_if(state_type match) {
    if (state == match) {
      state = threw;
      throw exception{};
    }
  }

  state_type state = zero;
};

void main() {
  using O = optional<value_type>;
  using S = value_type::state_type;
  auto const make = [](S s = S::zero) { return value_type{s}; };

  enum outcome_type { nothrow, caught, bad_catch };

  // Check value assignment for disengaged optional

  {
    O o;
    value_type v = make(S::throwing_copy_assignment);
    o = v;
    VERIFY(o && o->state == S::throwing_copy_assignment);
  }

  {
    O o;
    value_type v = make(S::throwing_move_assignment);
    o = std::move(v);
    VERIFY(o && o->state == S::throwing_move_assignment);
  }

  {
    outcome_type outcome{};
    O o;
    value_type v = make(S::throwing_copy);

    try {
      o = v;
    } catch (exception const &) {
      outcome = caught;
    } catch (...) {
      outcome = bad_catch;
    }

    VERIFY(!o);
  }

  {
    outcome_type outcome{};
    O o;
    value_type v = make(S::throwing_move);

    try {
      o = std::move(v);
    } catch (exception const &) {
      outcome = caught;
    } catch (...) {
      outcome = bad_catch;
    }

    VERIFY(!o);
  }

  VERIFY(counter == 0);
}
}
namespace _4 {
struct exception {};

int counter = 0;

struct mixin_counter {
  mixin_counter() { ++counter; }
  mixin_counter(mixin_counter const &) { ++counter; }
  ~mixin_counter() { --counter; }
};

struct value_type : private mixin_counter {
  enum state_type {
    zero,
    moved_from,
    throwing_construction,
    throwing_copy,
    throwing_copy_assignment,
    throwing_move,
    throwing_move_assignment,
    threw,
  };

  value_type() = default;

  explicit value_type(state_type state_) : state(state_) {
    throw_if(throwing_construction);
  }

  value_type(value_type const &other) : mixin_counter(), state(other.state) {
    throw_if(throwing_copy);
  }

  value_type &operator=(value_type const &other) {
    state = other.state;
    throw_if(throwing_copy_assignment);
    return *this;
  }

  value_type(value_type &&other) : state(other.state) {
    other.state = moved_from;
    throw_if(throwing_move);
  }

  value_type &operator=(value_type &&other) {
    state = other.state;
    other.state = moved_from;
    throw_if(throwing_move_assignment);
    return *this;
  }

  void throw_if(state_type match) {
    if (state == match) {
      state = threw;
      throw exception{};
    }
  }

  state_type state = zero;
};

void main() {
  using O = optional<value_type>;
  using S = value_type::state_type;
  auto const make = [](S s = S::zero) { return value_type{s}; };

  enum outcome_type { nothrow, caught, bad_catch };

  // Check value assignment for engaged optional

  {
    O o = make();
    value_type v = make(S::throwing_copy);
    o = v;
    VERIFY(o && o->state == S::throwing_copy);
  }

  {
    O o = make();
    value_type v = make(S::throwing_move);
    o = std::move(v);
    VERIFY(o && o->state == S::throwing_move);
  }

  {
    outcome_type outcome{};
    O o = make();
    value_type v = make(S::throwing_copy_assignment);

    try {
      o = v;
    } catch (exception const &) {
      outcome = caught;
    } catch (...) {
      outcome = bad_catch;
    }

    VERIFY(o && o->state == S::threw);
  }

  {
    outcome_type outcome{};
    O o = make();
    value_type v = make(S::throwing_move_assignment);

    try {
      o = std::move(v);
    } catch (exception const &) {
      outcome = caught;
    } catch (...) {
      outcome = bad_catch;
    }

    VERIFY(o && o->state == S::threw);
  }

  VERIFY(counter == 0);
}
}
namespace _5 {
int counter = 0;

struct mixin_counter {
  mixin_counter() { ++counter; }
  mixin_counter(mixin_counter const &) { ++counter; }
  ~mixin_counter() { --counter; }
};

struct value_type : private mixin_counter {};

void main() {
  using O = optional<value_type>;

  // Check nullopt_t and 'default' (= {}) assignment

  {
    O o;
    o = nullopt;
    VERIFY(!o);
  }

  {
    O o{in_place};
    o = nullopt;
    VERIFY(!o);
  }

  {
    O o;
    o = {};
    VERIFY(!o);
  }

  {
    O o{in_place};
    o = {};
    VERIFY(!o);
  }

  VERIFY(counter == 0);
}
}
namespace _6 {
int counter = 0;

struct mixin_counter {
  mixin_counter() { ++counter; }
  mixin_counter(mixin_counter const &) { ++counter; }
  ~mixin_counter() { --counter; }
};

struct value_type : private mixin_counter {
  value_type() = default;
  value_type(int) : state(1) {}
  value_type(std::initializer_list<char>, const char *) : state(2) {}
  int state = 0;
};

void main() {
  using O = optional<value_type>;

  // Check emplace

  {
    O o;
    o.emplace();
    VERIFY(o && o->state == 0);
  }
  {
    O o{in_place, 0};
    o.emplace();
    VERIFY(o && o->state == 0);
  }

  {
    O o;
    o.emplace(0);
    VERIFY(o && o->state == 1);
  }
  {
    O o{in_place};
    o.emplace(0);
    VERIFY(o && o->state == 1);
  }

  {
    O o;
    o.emplace({'a'}, "");
    VERIFY(o && o->state == 2);
  }
  {
    O o{in_place};
    o.emplace({'a'}, "");
    VERIFY(o && o->state == 2);
  }

  static_assert(!std::is_constructible<O, std::initializer_list<int>, int>(),
                "");

  VERIFY(counter == 0);
}
}
}

namespace cons_test {
namespace copy {
struct tracker {
  tracker(int value) : value(value) { ++count; }
  ~tracker() { --count; }

  tracker(tracker const &other) : value(other.value) { ++count; }
  tracker(tracker &&other) : value(other.value) {
    other.value = -1;
    ++count;
  }

  tracker &operator=(tracker const &) = default;
  tracker &operator=(tracker &&) = default;

  int value;

  static int count;
};

int tracker::count = 0;

struct exception {};

struct throwing_copy {
  throwing_copy() = default;
  throwing_copy(throwing_copy const &) { throw exception{}; }
};

void main() {
  // [20.5.4.1] Constructors

  {
    optional<long> o;
    auto copy = o;
    VERIFY(!copy);
    VERIFY(!o);
  }

  {
    const long val = 0x1234ABCD;
    optional<long> o{in_place, val};
    auto copy = o;
    VERIFY(copy);
    VERIFY(*copy == val);
    VERIFY(o && o == val);
  }

  {
    optional<tracker> o;
    auto copy = o;
    VERIFY(!copy);
    VERIFY(tracker::count == 0);
    VERIFY(!o);
  }

  {
    optional<tracker> o{in_place, 333};
    auto copy = o;
    VERIFY(copy);
    VERIFY(copy->value == 333);
    VERIFY(tracker::count == 2);
    VERIFY(o && o->value == 333);
  }

  enum outcome { nothrow, caught, bad_catch };

  {
    outcome result = nothrow;
    optional<throwing_copy> o;

    try {
      auto copy = o;
    } catch (exception const &) {
      result = caught;
    } catch (...) {
      result = bad_catch;
    }

    VERIFY(result == nothrow);
  }

  {
    outcome result = nothrow;
    optional<throwing_copy> o{in_place};

    try {
      auto copy = o;
    } catch (exception const &) {
      result = caught;
    } catch (...) {
      result = bad_catch;
    }

    VERIFY(result == caught);
  }

  VERIFY(tracker::count == 0);
}
}
namespace default_ {
struct tracker {
  tracker() { ++count; }
  ~tracker() { --count; }

  tracker(tracker const &) { ++count; }
  tracker(tracker &&) { ++count; }

  tracker &operator=(tracker const &) = default;
  tracker &operator=(tracker &&) = default;

  static int count;
};

int tracker::count = 0;

void main() {
  // [20.5.4.1] Constructors

  {
    optional<tracker> o;
    VERIFY(!o);
  }

  {
    optional<tracker> o{};
    VERIFY(!o);
  }

  {
    optional<tracker> o = {};
    VERIFY(!o);
  }

  VERIFY(tracker::count == 0);
}
}
namespace move {
struct tracker {
  tracker(int value) : value(value) { ++count; }
  ~tracker() { --count; }

  tracker(tracker const &other) : value(other.value) { ++count; }
  tracker(tracker &&other) : value(other.value) {
    other.value = -1;
    ++count;
  }

  tracker &operator=(tracker const &) = default;
  tracker &operator=(tracker &&) = default;

  int value;

  static int count;
};

int tracker::count = 0;

struct exception {};

struct throwing_move {
  throwing_move() = default;
  throwing_move(throwing_move const &) { throw exception{}; }
};

void main() {
  // [20.5.4.1] Constructors

  {
    optional<long> o;
    auto moved_to = std::move(o);
    VERIFY(!moved_to);
    VERIFY(!o);
  }

  {
    const long val = 0x1234ABCD;
    optional<long> o{in_place, val};
    auto moved_to = std::move(o);
    VERIFY(moved_to);
    VERIFY(*moved_to == val);
    VERIFY(o && *o == val);
  }

  {
    optional<tracker> o;
    auto moved_to = std::move(o);
    VERIFY(!moved_to);
    VERIFY(tracker::count == 0);
    VERIFY(!o);
  }

  {
    optional<tracker> o{in_place, 333};
    auto moved_to = std::move(o);
    VERIFY(moved_to);
    VERIFY(moved_to->value == 333);
    VERIFY(tracker::count == 2);
    VERIFY(o && o->value == -1);
  }

  enum outcome { nothrow, caught, bad_catch };

  {
    outcome result = nothrow;
    optional<throwing_move> o;

    try {
      auto moved_to = std::move(o);
    } catch (exception const &) {
      result = caught;
    } catch (...) {
      result = bad_catch;
    }

    VERIFY(result == nothrow);
  }

  {
    outcome result = nothrow;
    optional<throwing_move> o{in_place};

    try {
      auto moved_to = std::move(o);
    } catch (exception const &) {
      result = caught;
    } catch (...) {
      result = bad_catch;
    }

    VERIFY(result == caught);
  }

  VERIFY(tracker::count == 0);
}
}
namespace value_neg {
// Nothing to do here.  Constructing an optional<T> with a U object where
// std::is_constructible<T, U>::value == true is libstdc++'s extension.
// int main() {
//   {
//     struct X {
//       explicit X(int) {}
//     };
//     optional<X> ox{42};
//     optional<X> ox2 = 42; // { dg-error "conversion" }
//     optional<std::unique_ptr<int>> oup{new int};
//     optional<std::unique_ptr<int>> oup2 = new int; // { dg-error "conversion"
//     }
//   }
// }
}
namespace value {
struct tracker {
  tracker(int value) : value(value) { ++count; }
  ~tracker() { --count; }

  tracker(tracker const &other) : value(other.value) { ++count; }
  tracker(tracker &&other) : value(other.value) {
    other.value = -1;
    ++count;
  }

  tracker &operator=(tracker const &) = default;
  tracker &operator=(tracker &&) = default;

  int value;

  static int count;
};

int tracker::count = 0;

struct exception {};

struct throwing_construction {
  explicit throwing_construction(bool propagate) : propagate(propagate) {}

  throwing_construction(throwing_construction const &other)
      : propagate(other.propagate) {
    if (propagate)
      throw exception{};
  }

  bool propagate;
};

void main() {
  // [20.5.4.1] Constructors

  {
    auto i = 0x1234ABCD;
    optional<long> o{i};
    VERIFY(o);
    VERIFY(*o == 0x1234ABCD);
    VERIFY(i == 0x1234ABCD);
  }

  {
    auto i = 0x1234ABCD;
    optional<long> o = i;
    VERIFY(o);
    VERIFY(*o == 0x1234ABCD);
    VERIFY(i == 0x1234ABCD);
  }

  {
    auto i = 0x1234ABCD;
    optional<long> o = {i};
    VERIFY(o);
    VERIFY(*o == 0x1234ABCD);
    VERIFY(i == 0x1234ABCD);
  }

  {
    auto i = 0x1234ABCD;
    optional<long> o{std::move(i)};
    VERIFY(o);
    VERIFY(*o == 0x1234ABCD);
    VERIFY(i == 0x1234ABCD);
  }

  {
    auto i = 0x1234ABCD;
    optional<long> o = std::move(i);
    VERIFY(o);
    VERIFY(*o == 0x1234ABCD);
    VERIFY(i == 0x1234ABCD);
  }

  {
    auto i = 0x1234ABCD;
    optional<long> o = {std::move(i)};
    VERIFY(o);
    VERIFY(*o == 0x1234ABCD);
    VERIFY(i == 0x1234ABCD);
  }

  {
    std::vector<int> v = {0, 1, 2, 3, 4, 5};
    optional<std::vector<int>> o{v};
    VERIFY(!v.empty());
    VERIFY(o->size() == 6);
  }

  {
    std::vector<int> v = {0, 1, 2, 3, 4, 5};
    optional<std::vector<int>> o = v;
    VERIFY(!v.empty());
    VERIFY(o->size() == 6);
  }

  {
    std::vector<int> v = {0, 1, 2, 3, 4, 5};
    optional<std::vector<int>> o{v};
    VERIFY(!v.empty());
    VERIFY(o->size() == 6);
  }

  {
    std::vector<int> v = {0, 1, 2, 3, 4, 5};
    optional<std::vector<int>> o{std::move(v)};
    VERIFY(v.empty());
    VERIFY(o->size() == 6);
  }

  {
    std::vector<int> v = {0, 1, 2, 3, 4, 5};
    optional<std::vector<int>> o = std::move(v);
    VERIFY(v.empty());
    VERIFY(o->size() == 6);
  }

  {
    std::vector<int> v = {0, 1, 2, 3, 4, 5};
    optional<std::vector<int>> o{std::move(v)};
    VERIFY(v.empty());
    VERIFY(o->size() == 6);
  }

  {
    tracker t{333};
    optional<tracker> o = t;
    VERIFY(o->value == 333);
    VERIFY(tracker::count == 2);
    VERIFY(t.value == 333);
  }

  {
    tracker t{333};
    optional<tracker> o = std::move(t);
    VERIFY(o->value == 333);
    VERIFY(tracker::count == 2);
    VERIFY(t.value == -1);
  }

  enum outcome { nothrow, caught, bad_catch };

  {
    outcome result = nothrow;
    throwing_construction t{false};

    try {
      optional<throwing_construction> o{t};
    } catch (exception const &) {
      result = caught;
    } catch (...) {
      result = bad_catch;
    }

    VERIFY(result == nothrow);
  }

  {
    outcome result = nothrow;
    throwing_construction t{true};

    try {
      optional<throwing_construction> o{t};
    } catch (exception const &) {
      result = caught;
    } catch (...) {
      result = bad_catch;
    }

    VERIFY(result == caught);
  }

  {
    outcome result = nothrow;
    throwing_construction t{false};

    try {
      optional<throwing_construction> o{std::move(t)};
    } catch (exception const &) {
      result = caught;
    } catch (...) {
      result = bad_catch;
    }

    VERIFY(result == nothrow);
  }

  {
    outcome result = nothrow;
    throwing_construction t{true};

    try {
      optional<throwing_construction> o{std::move(t)};
    } catch (exception const &) {
      result = caught;
    } catch (...) {
      result = bad_catch;
    }

    VERIFY(result == caught);
  }
}
}
}

namespace constexpr_test {
namespace cons {
namespace default_ {
void main() {
  // [20.5.4.1] Constructors

  {
    constexpr optional<int> o;
    static_assert(!o, "");
  }

  {
    constexpr optional<int> o{};
    static_assert(!o, "");
  }

  {
    constexpr optional<int> o = {};
    static_assert(!o, "");
  }
}
}
namespace value {
void main() {
  // [20.5.4.1] Constructors

  {
    constexpr long i = 0x1234ABCD;
    constexpr optional<long> o{i};
    static_assert(o, "");
    static_assert(*o == 0x1234ABCD, "");
  }

  // Initialize a constexpr optional <T> in this style is GCC's extension.
  // {
  //   constexpr long i = 0x1234ABCD;
  //   constexpr optional<long> o = i;
  //   static_assert(o, "");
  //   static_assert(*o == 0x1234ABCD, "");
  // }

  {
    constexpr long i = 0x1234ABCD;
    constexpr optional<long> o = {i};
    static_assert(o, "");
    static_assert(*o == 0x1234ABCD, "");
  }

  {
    constexpr long i = 0x1234ABCD;
    constexpr optional<long> o{std::move(i)};
    static_assert(o, "");
    static_assert(*o == 0x1234ABCD, "");
  }

  // Initialize a constexpr optional <T> in this style is GCC's extension.
  // {
  //   constexpr long i = 0x1234ABCD;
  //   constexpr optional<long> o = std::move(i);
  //   static_assert(o, "");
  //   static_assert(*o == 0x1234ABCD, "");
  // }

  {
    constexpr long i = 0x1234ABCD;
    constexpr optional<long> o = {std::move(i)};
    static_assert(o, "");
    static_assert(*o == 0x1234ABCD, "");
  }
}
}
}
namespace observer {
namespace _1 {
struct value_type {
  int i;
};

void main() {
  constexpr optional<value_type> o{value_type{51}};
  static_assert((*o).i == 51, "");
}
}
namespace _2 {
// Nothing to do here. constexpr addressof is not available until C++17.
// struct value_type {
//   int i;
//
//   void *operator&() { return nullptr; } // N.B. non-const
// };
//
// void main() {
//   constexpr optional<value_type> o{value_type{51}};
//   static_assert(o->i == 51, "");
//   static_assert(o->i == (*o).i, "");
//   static_assert(&o->i == &(*o).i, "");
// }
}
namespace _3 {
struct value_type {
  int i;
};

void main() {
  constexpr optional<value_type> o{value_type{51}};
  static_assert(o.value().i == 51, "");
  static_assert(o.value().i == (*o).i, "");
  static_assert(&o.value().i == &(*o).i, "");
}
}
namespace _4 {
struct value_type {
  int i;
};

void main() {
  constexpr optional<value_type> o{value_type{51}};
  constexpr value_type fallback{3};
  static_assert(o.value_or(fallback).i == 51, "");
  static_assert(o.value_or(fallback).i == (*o).i, "");
}
}
namespace _5 {
struct value_type {
  int i;
};

void main() {
  {
    constexpr optional<value_type> o{nullopt};
    static_assert(!o, "");
  }

  {
    constexpr optional<value_type> o{value_type{51}};
    static_assert(o, "");
  }
}
}
}
namespace relop {
namespace _1 {
namespace ns {
struct value_type {
  int i;
  const char *s;
};

constexpr bool strcmp(const char *lhs, const char *rhs) {
  return *lhs == *rhs && (!*lhs || strcmp(lhs + 1, rhs + 1));
}

constexpr bool strrel(const char *lhs, const char *rhs) {
  return (*rhs && (!*lhs || (*lhs < *rhs))) ||
         ((*lhs && *rhs && !(*rhs < *lhs)) && strrel(lhs + 1, rhs + 1));
}

constexpr bool operator==(value_type const &lhs, value_type const &rhs) {
  return (lhs.i == rhs.i) && strcmp(lhs.s, rhs.s);
}

constexpr bool operator!=(value_type const &lhs, value_type const &rhs) {
  return !(lhs == rhs);
}

constexpr bool operator<(value_type const &lhs, value_type const &rhs) {
  return (lhs.i < rhs.i) || (!(rhs.i < lhs.i) && strrel(lhs.s, rhs.s));
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  {
    constexpr O o, p;
    static_assert(o == p, "");
    static_assert(!(o != p), "");
  }

  {
    constexpr O o{value_type{42, "forty-two"}}, p;
    static_assert(!(o == p), "");
    static_assert(o != p, "");
  }

  {
    constexpr O o, p{value_type{42, "forty-two"}};
    static_assert(!(o == p), "");
    static_assert(o != p, "");
  }

  {
    constexpr O o{value_type{11, "eleventy"}}, p{value_type{42, "forty-two"}};
    static_assert(!(o == p), "");
    static_assert(o != p, "");
  }

  {
    constexpr O o{value_type{42, "forty-two"}}, p{value_type{11, "eleventy"}};
    static_assert(!(o == p), "");
    static_assert(o != p, "");
  }

  {
    constexpr O o{value_type{42, "forty-two"}}, p{value_type{42, "forty-two"}};
    static_assert(o == p, "");
    static_assert(!(o != p), "");
  }
}
}
namespace _2 {
namespace ns {
struct value_type {
  int i;
  const char *s;
};

constexpr bool strcmp(const char *lhs, const char *rhs) {
  return *lhs == *rhs && (!*lhs || strcmp(lhs + 1, rhs + 1));
}

constexpr bool strrel(const char *lhs, const char *rhs) {
  return (*rhs && (!*lhs || (*lhs < *rhs))) ||
         ((*lhs && *rhs && !(*rhs < *lhs)) && strrel(lhs + 1, rhs + 1));
}

constexpr bool operator==(value_type const &lhs, value_type const &rhs) {
  return (lhs.i == rhs.i) && strcmp(lhs.s, rhs.s);
}

constexpr bool operator!=(value_type const &lhs, value_type const &rhs) {
  return !(lhs == rhs);
}

constexpr bool operator<(value_type const &lhs, value_type const &rhs) {
  return (lhs.i < rhs.i) || (!(rhs.i < lhs.i) && strrel(lhs.s, rhs.s));
}

constexpr bool operator>(value_type const &lhs, value_type const &rhs) {
  return rhs < lhs;
}

constexpr bool operator<=(value_type const &lhs, value_type const &rhs) {
  return lhs < rhs || lhs == rhs;
}

constexpr bool operator>=(value_type const &lhs, value_type const &rhs) {
  return lhs > rhs || lhs == rhs;
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  {
    constexpr O o, p;
    static_assert(!(o < p), "");
    static_assert(!(o > p), "");
    static_assert(o <= p, "");
    static_assert(o >= p, "");
  }

  {
    constexpr O o{value_type{42, "forty-two"}}, p;
    static_assert(!(o < p), "");
    static_assert(o > p, "");
    static_assert(!(o <= p), "");
    static_assert(o >= p, "");
  }

  {
    constexpr O o, p{value_type{42, "forty-two"}};
    static_assert(o < p, "");
    static_assert(!(o > p), "");
    static_assert(o <= p, "");
    static_assert(!(o >= p), "");
  }

  {
    constexpr O o{value_type{11, "eleventy"}}, p{value_type{42, "forty-two"}};
    static_assert(o < p, "");
    static_assert(!(o > p), "");
    static_assert(o <= p, "");
    static_assert(!(o >= p), "");
  }

  {
    constexpr O o{value_type{42, "forty-two"}}, p{value_type{11, "eleventy"}};
    static_assert(!(o < p), "");
    static_assert(o > p, "");
    static_assert(!(o <= p), "");
    static_assert(o >= p, "");
  }

  {
    constexpr O o{value_type{42, "forty-two"}}, p{value_type{42, "forty-two"}};
    static_assert(!(o < p), "");
    static_assert(!(o > p), "");
    static_assert(o <= p, "");
    static_assert(o >= p, "");
  }
}
}
namespace _3 {
namespace ns {
struct value_type {
  int i;
  const char *s;
};

constexpr bool strcmp(const char *lhs, const char *rhs) {
  return *lhs == *rhs && (!*lhs || strcmp(lhs + 1, rhs + 1));
}

constexpr bool strrel(const char *lhs, const char *rhs) {
  return (*rhs && (!*lhs || (*lhs < *rhs))) ||
         ((*lhs && *rhs && !(*rhs < *lhs)) && strrel(lhs + 1, rhs + 1));
}

constexpr bool operator==(value_type const &lhs, value_type const &rhs) {
  return (lhs.i == rhs.i) && strcmp(lhs.s, rhs.s);
}

constexpr bool operator!=(value_type const &lhs, value_type const &rhs) {
  return !(lhs == rhs);
}

constexpr bool operator<(value_type const &lhs, value_type const &rhs) {
  return (lhs.i < rhs.i) || (!(rhs.i < lhs.i) && strrel(lhs.s, rhs.s));
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  constexpr value_type reference{42, "forty-two"};

  {
    constexpr O o;
    static_assert(!(o == reference), "");
    static_assert(!(reference == o), "");
    static_assert(o != reference, "");
    static_assert(reference != o, "");
  }

  {
    constexpr O o{value_type{11, "eleventy"}};
    static_assert(!(o == reference), "");
    static_assert(!(reference == o), "");
    static_assert(o != reference, "");
    static_assert(reference != o, "");
  }

  {
    constexpr O o{value_type{42, "forty-two"}};
    static_assert(o == reference, "");
    static_assert(reference == o, "");
    static_assert(!(o != reference), "");
    static_assert(!(reference != o), "");
  }
}
}
namespace _4 {
namespace ns {
struct value_type {
  int i;
  const char *s;
};

constexpr bool strcmp(const char *lhs, const char *rhs) {
  return *lhs == *rhs && (!*lhs || strcmp(lhs + 1, rhs + 1));
}

constexpr bool strrel(const char *lhs, const char *rhs) {
  return (*rhs && (!*lhs || (*lhs < *rhs))) ||
         ((*lhs && *rhs && !(*rhs < *lhs)) && strrel(lhs + 1, rhs + 1));
}

constexpr bool operator==(value_type const &lhs, value_type const &rhs) {
  return (lhs.i == rhs.i) && strcmp(lhs.s, rhs.s);
}

constexpr bool operator!=(value_type const &lhs, value_type const &rhs) {
  return !(lhs == rhs);
}

constexpr bool operator<(value_type const &lhs, value_type const &rhs) {
  return (lhs.i < rhs.i) || (!(rhs.i < lhs.i) && strrel(lhs.s, rhs.s));
}

constexpr bool operator>(value_type const &lhs, value_type const &rhs) {
  return rhs < lhs;
}

constexpr bool operator<=(value_type const &lhs, value_type const &rhs) {
  return lhs < rhs || lhs == rhs;
}

constexpr bool operator>=(value_type const &lhs, value_type const &rhs) {
  return lhs > rhs || lhs == rhs;
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  constexpr value_type reference{42, "forty-two"};

  {
    constexpr O o;
    static_assert(o < reference, "");
    static_assert(!(reference < o), "");
    static_assert(!(o > reference), "");
    static_assert(reference > o, "");
    static_assert(o <= reference, "");
    static_assert(!(reference <= o), "");
    static_assert(!(o >= reference), "");
    static_assert(reference >= o, "");
  }

  {
    constexpr O o{value_type{11, "eleventy"}};
    static_assert(o < reference, "");
    static_assert(!(reference < o), "");
    static_assert(!(o > reference), "");
    static_assert(reference > o, "");
    static_assert(o <= reference, "");
    static_assert(!(reference <= o), "");
    static_assert(!(o >= reference), "");
    static_assert(reference >= o, "");
  }

  {
    constexpr O o{value_type{42, "forty-two"}};
    static_assert(!(o < reference), "");
    static_assert(!(reference < o), "");
    static_assert(!(o > reference), "");
    static_assert(!(reference > o), "");
    static_assert(o <= reference, "");
    static_assert(reference <= o, "");
    static_assert(o >= reference, "");
    static_assert(reference >= o, "");
  }
}
}
namespace _5 {
namespace ns {
struct value_type {
  int i;
  const char *s;
};

constexpr bool strcmp(const char *lhs, const char *rhs) {
  return *lhs == *rhs && (!*lhs || strcmp(lhs + 1, rhs + 1));
}

constexpr bool strrel(const char *lhs, const char *rhs) {
  return (*rhs && (!*lhs || (*lhs < *rhs))) ||
         ((*lhs && *rhs && !(*rhs < *lhs)) && strrel(lhs + 1, rhs + 1));
}

constexpr bool operator==(value_type const &lhs, value_type const &rhs) {
  return (lhs.i == rhs.i) && strcmp(lhs.s, rhs.s);
}

constexpr bool operator!=(value_type const &lhs, value_type const &rhs) {
  return !(lhs == rhs);
}

constexpr bool operator<(value_type const &lhs, value_type const &rhs) {
  return (lhs.i < rhs.i) || (!(rhs.i < lhs.i) && strrel(lhs.s, rhs.s));
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  {
    constexpr O o;
    static_assert(o == nullopt, "");
    static_assert(nullopt == o, "");
    static_assert(!(o != nullopt), "");
    static_assert(!(nullopt != o), "");
  }

  {
    constexpr O o{in_place};
    static_assert(!(o == nullopt), "");
    static_assert(!(nullopt == o), "");
    static_assert(o != nullopt, "");
    static_assert(nullopt != o, "");
  }
}
}
namespace _6 {
namespace ns {
struct value_type {
  int i;
  const char *s;
};

constexpr bool strcmp(const char *lhs, const char *rhs) {
  return *lhs == *rhs && (!*lhs || strcmp(lhs + 1, rhs + 1));
}

constexpr bool strrel(const char *lhs, const char *rhs) {
  return (*rhs && (!*lhs || (*lhs < *rhs))) ||
         ((*lhs && *rhs && !(*rhs < *lhs)) && strrel(lhs + 1, rhs + 1));
}

constexpr bool operator==(value_type const &lhs, value_type const &rhs) {
  return (lhs.i == rhs.i) && strcmp(lhs.s, rhs.s);
}

constexpr bool operator!=(value_type const &lhs, value_type const &rhs) {
  return !(lhs == rhs);
}

constexpr bool operator<(value_type const &lhs, value_type const &rhs) {
  return (lhs.i < rhs.i) || (!(rhs.i < lhs.i) && strrel(lhs.s, rhs.s));
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  {
    constexpr O o;
    static_assert(!(o < nullopt), "");
    static_assert(!(nullopt < o), "");
    static_assert(!(o > nullopt), "");
    static_assert(!(nullopt > o), "");
    static_assert(o <= nullopt, "");
    static_assert(nullopt <= o, "");
    static_assert(o >= nullopt, "");
    static_assert(nullopt >= o, "");
  }

  {
    constexpr O o{in_place};
    static_assert(!(o < nullopt), "");
    static_assert(nullopt < o, "");
    static_assert(o > nullopt, "");
    static_assert(!(nullopt > o), "");
    static_assert(!(o <= nullopt), "");
    static_assert(nullopt <= o, "");
    static_assert(o >= nullopt, "");
    static_assert(!(nullopt >= o), "");
  }
}
}
}
namespace in_place_ {
void main() {
  // [20.5.5] In-place construction
  {
    constexpr optional<int> o{in_place};
    static_assert(o, "");
    static_assert(*o == int{}, "");

    static_assert(!std::is_convertible<in_place_t, optional<int>>(), "");
  }

  {
    constexpr optional<int> o{in_place, 42};
    static_assert(o, "");
    static_assert(*o == 42, "");
  }
}
}
namespace make_optional_ {
// Initialize a constexpr optional <T> in this style is GCC's extension.
// void main() {
//   constexpr int i = 42;
//   constexpr auto o = make_optional(i);
//   static_assert(std::is_same<decltype(o), const optional<int>>(), "");
//   static_assert(o && *o == 42, "");
//   static_assert(&*o != &i, "");
// }
void main() {
  constexpr int i = 42;
  const auto o = make_optional(i);
  VERIFY((std::is_same<decltype(o), const optional<int>>()));
  VERIFY(o && *o == 42);
  VERIFY(&*o != &i);
}
}
namespace nullopt_ {
void main() {
  // [20.5.6] Disengaged state indicator
  static_assert(std::is_same<decltype(nullopt), const nullopt_t>(), "");
  static_assert(std::is_empty<nullopt_t>(), "");
  static_assert(std::is_literal_type<nullopt_t>(), "");
  static_assert(!std::is_default_constructible<nullopt_t>(), "");

  // Initialize a constexpr optional <T> in this style is GCC's extension.
  // {
  //   constexpr optional<int> o = nullopt;
  //   static_assert(!o, "");
  // }

  {
    constexpr optional<int> o = {nullopt};
    static_assert(!o, "");
  }

  {
    constexpr optional<int> o{nullopt};
    static_assert(!o, "");
  }
}
}
}

namespace observer_test {
namespace _1 {
struct value_type {
  int i;
};

void main() {
  optional<value_type> o{value_type{51}};
  VERIFY((*o).i == 51);
}
}
namespace _2 {
struct value_type {
  int i;
};

void *operator&(const value_type &) = delete;

void main() {
  optional<value_type> o{value_type{51}};
  VERIFY(o->i == 51);
  VERIFY(o->i == (*o).i);
  VERIFY(&o->i == &(*o).i);
}
}
namespace _3 {
struct value_type {
  int i;
};

void main() {
  {
    optional<value_type> o{value_type{51}};
    VERIFY(o.value().i == 51);
    VERIFY(o.value().i == (*o).i);
    VERIFY(&o.value().i == &(*o).i);
  }

  {
    enum outcome_type { nothrow, caught, bad_catch };

    outcome_type outcome{};
    optional<value_type> o = nullopt;
    bool called = false;
    auto const eat = [&called](int) { called = true; };

    try {
      eat(o.value().i);
    } catch (bad_optional_access const &) {
      outcome = caught;
    } catch (...) {
      outcome = bad_catch;
    }

    VERIFY(outcome == caught);
    VERIFY(!called);
  }
}
}
namespace _4 {
struct value_type {
  int i;
};

void main() {
  optional<value_type> o{value_type{51}};
  value_type fallback{3};
  VERIFY(o.value_or(fallback).i == 51);
  VERIFY(o.value_or(fallback).i == (*o).i);
}
}
namespace _5 {
struct value_type {
  int i;
};

void main() {
  {
    optional<value_type> o = nullopt;
    VERIFY(!o);
  }
  {
    optional<value_type> o{value_type{51}};
    VERIFY(o);
  }
}
}
}

namespace relop_test {
namespace _1 {
namespace ns {
struct value_type {
  int i;
  std::string s;
};

bool operator==(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) == std::tie(rhs.i, rhs.s);
}

bool operator!=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) != std::tie(rhs.i, rhs.s);
}

bool operator<(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) < std::tie(rhs.i, rhs.s);
}

bool operator>(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) > std::tie(rhs.i, rhs.s);
}

bool operator<=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) <= std::tie(rhs.i, rhs.s);
}

bool operator>=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) >= std::tie(rhs.i, rhs.s);
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  {
    O o, p;
    VERIFY(o == p);
    VERIFY(!(o != p));
  }

  {
    O o{value_type{42, "forty-two"}}, p;
    VERIFY(!(o == p));
    VERIFY(o != p);
  }

  {
    O o, p{value_type{42, "forty-two"}};
    VERIFY(!(o == p));
    VERIFY(o != p);
  }

  {
    O o{value_type{11, "eleventy"}}, p{value_type{42, "forty-two"}};
    VERIFY(!(o == p));
    VERIFY(o != p);
  }

  {
    O o{value_type{42, "forty-two"}}, p{value_type{11, "eleventy"}};
    VERIFY(!(o == p));
    VERIFY(o != p);
  }

  {
    O o{value_type{42, "forty-two"}}, p{value_type{42, "forty-two"}};
    VERIFY(o == p);
    VERIFY(!(o != p));
  }
}
}
namespace _2 {
namespace ns {
struct value_type {
  int i;
  std::string s;
};

bool operator==(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) == std::tie(rhs.i, rhs.s);
}

bool operator!=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) != std::tie(rhs.i, rhs.s);
}

bool operator<(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) < std::tie(rhs.i, rhs.s);
}

bool operator>(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) > std::tie(rhs.i, rhs.s);
}

bool operator<=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) <= std::tie(rhs.i, rhs.s);
}

bool operator>=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) >= std::tie(rhs.i, rhs.s);
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  {
    O o, p;
    VERIFY(!(o < p));
    VERIFY(!(o > p));
    VERIFY(o <= p);
    VERIFY(o >= p);
  }

  {
    O o{value_type{42, "forty-two"}}, p;
    VERIFY(!(o < p));
    VERIFY(o > p);
    VERIFY(!(o <= p));
    VERIFY(o >= p);
  }

  {
    O o, p{value_type{42, "forty-two"}};
    VERIFY(o < p);
    VERIFY(!(o > p));
    VERIFY(o <= p);
    VERIFY(!(o >= p));
  }

  {
    O o{value_type{11, "eleventy"}}, p{value_type{42, "forty-two"}};
    VERIFY(o < p);
    VERIFY(!(o > p));
    VERIFY(o <= p);
    VERIFY(!(o >= p));
  }

  {
    O o{value_type{42, "forty-two"}}, p{value_type{11, "eleventy"}};
    VERIFY(!(o < p));
    VERIFY(o > p);
    VERIFY(!(o <= p));
    VERIFY(o >= p);
  }

  {
    O o{value_type{42, "forty-two"}}, p{value_type{42, "forty-two"}};
    VERIFY(!(o < p));
    VERIFY(!(o > p));
    VERIFY(o <= p);
    VERIFY(o >= p);
  }
}
}
namespace _3 {
namespace ns {
struct value_type {
  int i;
  std::string s;
};

bool operator==(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) == std::tie(rhs.i, rhs.s);
}

bool operator!=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) != std::tie(rhs.i, rhs.s);
}

bool operator<(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) < std::tie(rhs.i, rhs.s);
}

bool operator>(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) > std::tie(rhs.i, rhs.s);
}

bool operator<=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) <= std::tie(rhs.i, rhs.s);
}

bool operator>=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) >= std::tie(rhs.i, rhs.s);
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  value_type const reference{42, "forty-two"};

  {
    O o;
    VERIFY(!(o == reference));
    VERIFY(!(reference == o));
    VERIFY(o != reference);
    VERIFY(reference != o);
  }

  {
    O o{value_type{11, "eleventy"}};
    VERIFY(!(o == reference));
    VERIFY(!(reference == o));
    VERIFY(o != reference);
    VERIFY(reference != o);
  }

  {
    O o{value_type{42, "forty-two"}};
    VERIFY(o == reference);
    VERIFY(reference == o);
    VERIFY(!(o != reference));
    VERIFY(!(reference != o));
  }
}
}
namespace _4 {
namespace ns {
struct value_type {
  int i;
  std::string s;
};

bool operator==(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) == std::tie(rhs.i, rhs.s);
}

bool operator!=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) != std::tie(rhs.i, rhs.s);
}

bool operator<(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) < std::tie(rhs.i, rhs.s);
}

bool operator>(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) > std::tie(rhs.i, rhs.s);
}

bool operator<=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) <= std::tie(rhs.i, rhs.s);
}

bool operator>=(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) >= std::tie(rhs.i, rhs.s);
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  value_type const reference{42, "forty-two"};

  {
    O o;
    VERIFY(o < reference);
    VERIFY(!(reference < o));
    VERIFY(!(o > reference));
    VERIFY(reference > o);
    VERIFY(o <= reference);
    VERIFY(!(reference <= o));
    VERIFY(!(o >= reference));
    VERIFY(reference >= o);
  }

  {
    O o{value_type{11, "eleventy"}};
    VERIFY(o < reference);
    VERIFY(!(reference < o));
    VERIFY(!(o > reference));
    VERIFY(reference > o);
    VERIFY(o <= reference);
    VERIFY(!(reference <= o));
    VERIFY(!(o >= reference));
    VERIFY(reference >= o);
  }

  {
    O o{value_type{42, "forty-two"}};
    VERIFY(!(o < reference));
    VERIFY(!(reference < o));
    VERIFY(!(o > reference));
    VERIFY(!(reference > o));
    VERIFY(o <= reference);
    VERIFY(reference <= o);
    VERIFY(o >= reference);
    VERIFY(reference >= o);
  }
}
}
namespace _5 {
namespace ns {
struct value_type {
  int i;
  std::string s;
};

bool operator==(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) == std::tie(rhs.i, rhs.s);
}

bool operator<(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) < std::tie(rhs.i, rhs.s);
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  {
    O o;
    VERIFY(o == nullopt);
    VERIFY(nullopt == o);
    VERIFY(!(o != nullopt));
    VERIFY(!(nullopt != o));
  }

  {
    O o{in_place};
    VERIFY(!(o == nullopt));
    VERIFY(!(nullopt == o));
    VERIFY(o != nullopt);
    VERIFY(nullopt != o);
  }
}
}
namespace _6 {
namespace ns {
struct value_type {
  int i;
  std::string s;
};

bool operator==(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) == std::tie(rhs.i, rhs.s);
}

bool operator<(value_type const &lhs, value_type const &rhs) {
  return std::tie(lhs.i, lhs.s) < std::tie(rhs.i, rhs.s);
}

} // namespace ns

void main() {
  using ns::value_type;
  using O = optional<value_type>;

  {
    O o;
    VERIFY(!(o < nullopt));
    VERIFY(!(nullopt < o));
    VERIFY(!(o > nullopt));
    VERIFY(!(nullopt > o));
    VERIFY(o <= nullopt);
    VERIFY(nullopt <= o);
    VERIFY(o >= nullopt);
    VERIFY(nullopt >= o);
  }

  {
    O o{in_place};
    VERIFY(!(o < nullopt));
    VERIFY(nullopt < o);
    VERIFY(o > nullopt);
    VERIFY(!(nullopt > o));
    VERIFY(!(o <= nullopt));
    VERIFY(nullopt <= o);
    VERIFY(o >= nullopt);
    VERIFY(!(nullopt >= o));
  }
}
}
}

namespace swap_test {
using std::swap;
namespace _1 {
struct exception {};

int counter = 0;

struct mixin_counter {
  mixin_counter() { ++counter; }
  mixin_counter(mixin_counter const &) { ++counter; }
  ~mixin_counter() { --counter; }
};

namespace ns {

struct value_type : private mixin_counter {
  explicit value_type(int state) : state(state) {}
  int state;
};

int swaps = 0;

void swap(value_type &lhs, value_type &rhs) {
  ++swaps;
  using std::swap;
  swap(lhs.state, rhs.state);
}

} // namespace ns

void main() {
  using O = optional<ns::value_type>;

  VERIFY(ns::swaps == 0);

  {
    O o, p;
    swap(o, p);
    VERIFY(!o);
    VERIFY(!p);
  }

  {
    O o{in_place, 45}, p;
    swap(o, p);
    VERIFY(!o);
    VERIFY(p && p->state == 45);
  }

  {
    O o, p{in_place, 45};
    swap(o, p);
    VERIFY(o && o->state == 45);
    VERIFY(!p);
  }

  {
    O o{in_place, 167}, p{in_place, 999};
    VERIFY(ns::swaps == 0);

    swap(o, p);

    VERIFY(o && o->state == 999);
    VERIFY(p && p->state == 167);
    VERIFY(ns::swaps == 1);
  }

  VERIFY(counter == 0);
}
}
}

int main() {
  in_place_test::main();
  make_optional_test::main();
  assignment_test::_1::main();
  assignment_test::_2::main();
  assignment_test::_3::main();
  assignment_test::_4::main();
  assignment_test::_5::main();
  assignment_test::_6::main();
  cons_test::copy::main();
  cons_test::default_::main();
  cons_test::move::main();
  // cons_test::value_neg::main();
  cons_test::value::main();
  constexpr_test::cons::default_::main();
  constexpr_test::cons::value::main();
  constexpr_test::observer::_1::main();
  // constexpr_test::observer::_2::main();
  constexpr_test::observer::_3::main();
  constexpr_test::observer::_4::main();
  constexpr_test::observer::_5::main();
  constexpr_test::relop::_1::main();
  constexpr_test::relop::_2::main();
  constexpr_test::relop::_3::main();
  constexpr_test::relop::_4::main();
  constexpr_test::relop::_5::main();
  constexpr_test::relop::_6::main();
  constexpr_test::in_place_::main();
  constexpr_test::make_optional_::main();
  constexpr_test::nullopt_::main();
  observer_test::_1::main();
  observer_test::_2::main();
  observer_test::_3::main();
  observer_test::_4::main();
  observer_test::_5::main();
  relop_test::_1::main();
  relop_test::_2::main();
  relop_test::_3::main();
  relop_test::_4::main();
  relop_test::_5::main();
  relop_test::_6::main();
}
