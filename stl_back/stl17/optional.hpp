/**
STL Backports headers

std::optional since C++17

This implemention is based on N4606.
  C++14    -- full support
  C++11    -- not implemented
  C++03/98 -- not implemented

Copyright 2016  Cyano Hao <c@cyano.cn>
*/

#pragma once

#include "__config.h"

#include "__helper/in_place.hpp"

#if __STL_BACKPORTS__HAVE_CXX14
#include "__version/optional14.hpp"
#else
#error "not implementd"
#endif
