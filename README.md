# STL Backports
Backporting STL to previous C++ versions, as close to the standards as possible.

## Features

### STL-17

Defined in `stl_back/stl17/*.hpp`, with namespace `stl_backports::stl17`.

| Feature | C++14 | C++11 | C++03/98 | Details |
| ------- | ----- | ----- | -------- | ------- |
| class `optional<T>` | Almost full support | Not implemented | Not implemented | `operator->` is non-constexpr in C++14. |

## Compatibility

### STL-17

#### STL-17 / C++14

| Feature | GCC 6+ | GCC 5* | Clang 3.5+ | VC2015 (C1xx/C2) |
| ------- | ------ | ------ | ---------- | ---------------- |
| class `optional<T>` | Passed | Minor issues in constexpr (GCC's bug) | Passed | N/A |

+ \* First release with full C++14 support.
+ Clang 3.4 is the first release that supports C++14, however, it doesn't parse `cstdio` (libstdc++ 4.8) correctly.
+ Visual C++ doesn't support C++14 yet.
