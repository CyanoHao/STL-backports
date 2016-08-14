#pragma once

#include <cassert>
#define VERIFY(fn) assert(fn)

#define GCC_GE(major, minor)                                                   \
  (defined(__GNUC__) && !defined(__clang__) &&                                 \
   (__GNUC__ > major || __GNUC__ == major && __GNUC_MINOR__ >= minor))

#define GCC_LT(major, minor)                                                   \
  (defined(__GNUC__) && !defined(__clang__) &&                                 \
   (__GNUC__ < major || __GNUC__ == major && __GNUC_MINOR__ < minor))

#define CLANG_GE(major, minor)                                                 \
  (defined(__clang__) &&                                                       \
   (__clang_major__ > major ||                                                 \
    __clang_major__ == major && __clang_minor__ >= minor))

#define CLANG_LT(major, minor)                                                 \
  (defined(__clang__) &&                                                       \
   (__clang_major__ < major ||                                                 \
    __clang_major__ == major && __clang_minor__ < minor))

#define MSC_GE(version) (defined(_MSC_VER) && _MSC_VER >= version)

#define MSC_LT(version) (defined(_MSC_VER) && _MSC_VER < version)
