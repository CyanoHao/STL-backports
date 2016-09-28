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

any since C++17

This testsuite is originally from GCC's libstdc++.

Modified by Cyano Hao <c@cyano.cn>
*/

#include <cstring>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include <stl-back/stl17/any.hpp>

#include "../common/hooks.h"

using stl_backports::stl17::any;
using stl_backports::stl17::any_cast;
using stl_backports::stl17::in_place;
using stl_backports::stl17::make_any;
using stl_backports::stl17::bad_any_cast;
using stl_backports::stl17::in_place_type_t;

namespace assign_test {
namespace _1 {
void test01() {
  any x;
  any y;
  y = x;
  VERIFY(!x.has_value());
  VERIFY(!y.has_value());

  y = std::move(x);
  VERIFY(!x.has_value());
  VERIFY(!y.has_value());
}

void test02() {
  any x(1);
  any y;
  y = x;
  VERIFY(x.has_value());
  VERIFY(y.has_value());

  x = std::move(y);
  VERIFY(x.has_value());
  VERIFY(!y.has_value());

  x = y;
  VERIFY(!x.has_value());
  VERIFY(!y.has_value());
}

void main() {
  test01();
  test02();
}
}
namespace _2 {
struct X {
  bool moved = false;
  bool moved_from = false;
  X() = default;
  X(const X &) = default;
  X(X &&x) : moved(true) { x.moved_from = true; }
};

void test01() {
  X x;
  any a1;
  a1 = x;
  VERIFY(x.moved_from == false);
  any a2;
  a2 = std::move(x);
  VERIFY(x.moved_from == true);
  VERIFY(any_cast<X &>(a2).moved == true);
}

void main() { test01(); }
}
namespace emplace {
struct combined {
  std::vector<int> v;
  std::tuple<int, int> t;
  template <class... Args>
  combined(std::initializer_list<int> il, Args &&... args)
      : v(il), t(std::forward<Args>(args)...) {}
};

void main() {
  const int i = 42;
  any o;
  o.emplace<int>(i);
  int &i2 = any_cast<int &>(o);
  VERIFY(i2 == 42);
  VERIFY(&i2 != &i);
  any o2;
  o2.emplace<std::tuple<int, int>>(1, 2);
  std::tuple<int, int> &t = any_cast<std::tuple<int, int> &>(o2);
  VERIFY(std::get<0>(t) == 1 && std::get<1>(t) == 2);
  any o3;
  o3.emplace<std::vector<int>>({42, 666});
  std::vector<int> &v = any_cast<std::vector<int> &>(o3);
  VERIFY(v[0] == 42 && v[1] == 666);
  any o4;
  o4.emplace<combined>({42, 666});
  combined &c = any_cast<combined &>(o4);
  VERIFY(c.v[0] == 42 && c.v[1] == 666 && std::get<0>(c.t) == 0 &&
         std::get<1>(c.t) == 0);
  any o5;
  o5.emplace<combined>({1, 2}, 3, 4);
  combined &c2 = any_cast<combined &>(o5);
  VERIFY(c2.v[0] == 1 && c2.v[1] == 2 && std::get<0>(c2.t) == 3 &&
         std::get<1>(c2.t) == 4);
  any o6;
  o6.emplace<const int &>(i);
  VERIFY(o6.type() == o.type());
  any o7;
  o7.emplace<void()>(nullptr);
  any o8;
  o8.emplace<void (*)()>(nullptr);
  VERIFY(o7.type() == o8.type());
  any o9;
  o9.emplace<char(&)[42]>(nullptr);
  any o10;
  o10.emplace<char *>(nullptr);
  VERIFY(o9.type() == o10.type());
}
}
namespace self {
std::set<const void *> live_objects;

struct A {
  A() { live_objects.insert(this); }
  ~A() { live_objects.erase(this); }
  A(const A &a) {
    VERIFY(live_objects.count(&a));
    live_objects.insert(this);
  }
};

void test01() {

  any a;
  a = a;
  VERIFY(!a.has_value());

  a = A{};
  a = a;
  VERIFY(a.has_value());

  a.reset();
  VERIFY(live_objects.empty());
}

void test02() {

  struct X {
    any a;
  };

  X x;
  std::swap(x, x); // results in "self-move-assignment" of X::a
  VERIFY(!x.a.has_value());

  x.a = A{};
  std::swap(x, x); // results in "self-move-assignment" of X::a
  VERIFY(x.a.has_value());

  x.a.reset();
  VERIFY(live_objects.empty());
}

void test03() {

  any a;
  a.swap(a);
  VERIFY(!a.has_value());

  a = A{};
  a.swap(a);
  VERIFY(a.has_value());

  a.reset();
  VERIFY(live_objects.empty());
}

void main() {
  test01();
  test02();
  test03();
}
}
}

namespace cons_test {
namespace _1 {
void test01() {
  any x;
  VERIFY(!x.has_value());

  any y(x);
  VERIFY(!x.has_value());
  VERIFY(!y.has_value());

  any z(std::move(y));
  VERIFY(!y.has_value());
  VERIFY(!z.has_value());
}

void test02() {
  any x(1);
  VERIFY(x.has_value());

  any y(x);
  VERIFY(x.has_value());
  VERIFY(y.has_value());

  any z(std::move(y));
  VERIFY(!y.has_value());
  VERIFY(z.has_value());
}

void main() {
  test01();
  test02();
}
}
namespace _2 {
struct X {
  bool moved = false;
  bool moved_from = false;
  X() = default;
  X(const X &) = default;
  X(X &&x) : moved(true) { x.moved_from = true; }
};

void test01() {
  X x;
  any a1(x);
  VERIFY(x.moved_from == false);
  any a2(std::move(x));
  VERIFY(x.moved_from == true);
  VERIFY(any_cast<X &>(a2).moved == true);
}

void main() { test01(); }
}
namespace aligned {
struct alignas(2 * alignof(void *)) X {};

bool stored_internally(void *obj, const any &a) {
  std::uintptr_t a_addr = reinterpret_cast<std::uintptr_t>(&a);
  std::uintptr_t a_end = a_addr + sizeof(a);
  std::uintptr_t obj_addr = reinterpret_cast<std::uintptr_t>(obj);
  return (a_addr <= obj_addr) && (obj_addr < a_end);
}

void test01() {
  any a = X{};
  X &x = any_cast<X &>(a);
  VERIFY(!stored_internally(&x, a));

#if 0
  // FIXME: this fails currently.
  // Small-object optimization not applied.
  a = 'X';
  char &c = any_cast<char &>(a);
  VERIFY(stored_internally(&c, a));
#endif
}

void main() { test01(); }
}
namespace in_place_ {
struct combined {
  std::vector<int> v;
  std::tuple<int, int> t;
  template <class... Args>
  combined(std::initializer_list<int> il, Args &&... args)
      : v(il), t(std::forward<Args>(args)...) {}
};

void main() {
  const int i = 42;
  any o(in_place<int>, i);
  int &i2 = any_cast<int &>(o);
  VERIFY(i2 == 42);
  VERIFY(&i2 != &i);
  any o2(in_place<std::tuple<int, int>>, 1, 2);
  std::tuple<int, int> &t = any_cast<std::tuple<int, int> &>(o2);
  VERIFY(std::get<0>(t) == 1 && std::get<1>(t) == 2);
  any o3(in_place<std::vector<int>>, {42, 666});
  std::vector<int> &v = any_cast<std::vector<int> &>(o3);
  VERIFY(v[0] == 42 && v[1] == 666);
  any o4(in_place<combined>, {42, 666});
  combined &c = any_cast<combined &>(o4);
  VERIFY(c.v[0] == 42 && c.v[1] == 666 && std::get<0>(c.t) == 0 &&
         std::get<1>(c.t) == 0);
  any o5(in_place<combined>, {1, 2}, 3, 4);
  combined &c2 = any_cast<combined &>(o5);
  VERIFY(c2.v[0] == 1 && c2.v[1] == 2 && std::get<0>(c2.t) == 3 &&
         std::get<1>(c2.t) == 4);
  any o6(in_place<int &>, i);
  VERIFY(o6.type() == o.type());
  any o7(in_place<void()>, nullptr);
  any o8(in_place<void (*)()>, nullptr);
  VERIFY(o7.type() == o8.type());
  any o9(in_place<char(&)[42]>, nullptr);
  any o10(in_place<char *>, nullptr);
  VERIFY(o9.type() == o10.type());
}
}
namespace nontrivial {
struct LocationAware {
  LocationAware() {}
  ~LocationAware() { VERIFY(self == this); }
  LocationAware(const LocationAware &) {}
  LocationAware &operator=(const LocationAware &) { return *this; }
  LocationAware(LocationAware &&) noexcept {}
  LocationAware &operator=(LocationAware &&) noexcept { return *this; }

  void *const self = this;
};
static_assert(std::is_nothrow_move_constructible<LocationAware>::value, "");
static_assert(!std::is_trivially_copyable<LocationAware>::value, "");

void test01() {

  LocationAware l;
  any a = l;
}

void test02() {
  LocationAware l;
  any a = l;
  any b = a;
  {
    any tmp = std::move(a);
    a = std::move(b);
    b = std::move(tmp);
  }
}

void test03() {
  LocationAware l;
  any a = l;
  any b = a;
  std::swap(a, b);
}

void main() {
  test01();
  test02();
  test03();
}
}
}

namespace make_any_test {
struct combined {
  std::vector<int> v;
  std::tuple<int, int> t;
  template <class... Args>
  combined(std::initializer_list<int> il, Args &&... args)
      : v(il), t(std::forward<Args>(args)...) {}
};

void main() {
  const int i = 42;
  auto o = make_any<int>(i);
  int &i2 = any_cast<int &>(o);
  VERIFY(i2 == 42);
  VERIFY(&i2 != &i);
  auto o2 = make_any<std::tuple<int, int>>(1, 2);
  std::tuple<int, int> &t = any_cast<std::tuple<int, int> &>(o2);
  VERIFY(std::get<0>(t) == 1 && std::get<1>(t) == 2);
  auto o3 = make_any<std::vector<int>>({42, 666});
  std::vector<int> &v = any_cast<std::vector<int> &>(o3);
  VERIFY(v[0] == 42 && v[1] == 666);
  auto o4 = make_any<combined>({42, 666});
  combined &c = any_cast<combined &>(o4);
  VERIFY(c.v[0] == 42 && c.v[1] == 666 && std::get<0>(c.t) == 0 &&
         std::get<1>(c.t) == 0);
  auto o5 = make_any<combined>({1, 2}, 3, 4);
  combined &c2 = any_cast<combined &>(o5);
  VERIFY(c2.v[0] == 1 && c2.v[1] == 2 && std::get<0>(c2.t) == 3 &&
         std::get<1>(c2.t) == 4);
}
}

namespace misc_test {
namespace any_cast_no_rtti {
void test01() {
  any x(1);
  auto p = any_cast<double>(&x);
  VERIFY(p == nullptr);

  x = 1.0;
  p = any_cast<double>(&x);
  VERIFY(p != nullptr);

  x = any();
  p = any_cast<double>(&x);
  VERIFY(p == nullptr);

  try {
    any_cast<double>(x);
    VERIFY(false);
  } catch (const bad_any_cast &) {
  }
}

void main() { test01(); }
}
namespace any_cast_ {
void test01() {
  using std::string;
  using std::strcmp;

  // taken from example in N3804 proposal

  any x(5);                      // x holds int
  VERIFY(any_cast<int>(x) == 5); // cast to value
  any_cast<int &>(x) = 10;       // cast to reference
  VERIFY(any_cast<int>(x) == 10);

  x = "Meow"; // x holds const char*
  VERIFY(strcmp(any_cast<const char *>(x), "Meow") == 0);
  any_cast<const char *&>(x) = "Harry";
  VERIFY(strcmp(any_cast<const char *>(x), "Harry") == 0);

  x = string("Meow"); // x holds string
  string s, s2("Jane");
  s = move(any_cast<string &>(x)); // move from any
  VERIFY(s == "Meow");
  any_cast<string &>(x) = move(s2); // move to any
  VERIFY(any_cast<const string &>(x) == "Jane");

  string cat("Meow");
  const any y(cat); // const y holds string
  VERIFY(any_cast<const string &>(y) == cat);
}

void test02() {
  any x(1);
  auto p = any_cast<double>(&x);
  VERIFY(p == nullptr);

  x = 1.0;
  p = any_cast<double>(&x);
  VERIFY(p != nullptr);

  x = any();
  p = any_cast<double>(&x);
  VERIFY(p == nullptr);

  try {
    any_cast<double>(x);
    VERIFY(false);
  } catch (const bad_any_cast &) {
  }
}

static int move_count = 0;

void test03() {
  struct MoveEnabled {
    MoveEnabled(MoveEnabled &&) { ++move_count; }
    MoveEnabled() = default;
    MoveEnabled(const MoveEnabled &) = default;
  };
  MoveEnabled m;
  MoveEnabled m2 = any_cast<MoveEnabled>(any(m));
  VERIFY(move_count == 1);
  MoveEnabled &&m3 = any_cast<MoveEnabled &&>(any(m));
  VERIFY(move_count == 1);

#if 0
  // This should fail (libstdc++'s extension).
  // The standard said it should be direct initiaized.
  struct MoveDeleted {
    MoveDeleted(MoveDeleted &&) = delete;
    MoveDeleted() = default;
    MoveDeleted(const MoveDeleted &) = default;
  };
  MoveDeleted md;
  MoveDeleted &&md2 = any_cast<MoveDeleted>(any(std::move(md)));
  MoveDeleted &&md3 = any_cast<MoveDeleted &&>(any(std::move(md)));
#endif
}

void main() {
  test01();
  test02();
  test03();
}
}
namespace swap_ {
void test01() {
  any x(1);
  any y;
  std::swap(x, y);
  VERIFY(!x.has_value());
  VERIFY(y.has_value());
}

void main() { test01(); }
}
}

namespace modifires_test {
namespace _1 {
void test01() {
  any x(1);
  any y;
  x.swap(y);
  VERIFY(!x.has_value());
  VERIFY(y.has_value());
  x.swap(y);
  VERIFY(x.has_value());
  VERIFY(!y.has_value());

  x.reset();
  VERIFY(!x.has_value());
}

void main() { test01(); }
}
}

namespace observers_test {
namespace type_ {
void test01() {
  any x;
  VERIFY(x.type() == typeid(void));
  x = 1;
  VERIFY(x.type() == typeid(int));
  x = any();
  VERIFY(x.type() == typeid(void));
}

void main() { test01(); }
}
}

namespace requirements_test {
using std::unique_ptr;

static_assert(std::is_assignable<any &, int>::value, "");
static_assert(!std::is_assignable<any &, unique_ptr<int>>::value, "");
static_assert(std::is_constructible<any, int>::value, "");
static_assert(!std::is_constructible<any, unique_ptr<int>>::value, "");
static_assert(!std::is_assignable<any &, const unique_ptr<int> &>::value, "");
static_assert(!std::is_constructible<any &, const unique_ptr<int> &>::value,
              "");
static_assert(!std::is_assignable<any &, unique_ptr<int> &>::value, "");
static_assert(!std::is_constructible<any &, unique_ptr<int> &>::value, "");

struct NoDefaultCtor {
  NoDefaultCtor() = delete;
};
static_assert(
    !std::is_constructible<any, in_place_type_t<NoDefaultCtor>>::value, "");
}

namespace typedefs_test {
static_assert(std::is_base_of<std::bad_cast, bad_any_cast>::value,
              "bad_any_cast must derive from bad_cast");
}

int main() {
  assign_test::_1::main();
  assign_test::_2::main();
  assign_test::emplace::main();
  assign_test::self::main();
  cons_test::_1::main();
  cons_test::_2::main();
  cons_test::aligned::main();
  cons_test::in_place_::main();
  cons_test::nontrivial::main();
  make_any_test::main();
  modifires_test::_1::main();
  observers_test::type_::main();
  return 0;
}
