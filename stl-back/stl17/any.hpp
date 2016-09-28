/**
STL Backports headers

std::any<T> since C++17

This implemention is based on N4606.
  C++11/14 -- almost full support
  C++03/98 -- not implemented

Copyright 2016  Cyano Hao <c@cyano.cn>
*/

#pragma once

#include "../__common/cxxver.h"

#include "__in_place.hpp"

#if __STL_BACKPORTS__HAVE_CXX11
#include "__any_11.hpp"
#else
#error "not implementd"
#endif
