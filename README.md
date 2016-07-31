# STL Backports
Backporting STL to previous C++ versions, as close to the standards as possible.

## Features

### STL-17

Defined in `stl_back/stl17/*.hpp`, with namespace `stl_backports::stl17`.

| Feature | C++14 | C++11 | C++03/98 |
| ------- | ----- | ----- | -------- |
| class `optional<T>` | Full support | Not implemented | Not implemented |

## Compatibility

### STL-17

#### STL-17 / C++14

| Feature | g++-6 | g++-5 | clang++-3.8 | c1xx/c2 (vc14) |
| ------- | ----- | ----- | ----------- | -------------- |
| class `optional<T>` | OK | Minor issues in constexpr | Minor issues in constexpr | Not tested |
