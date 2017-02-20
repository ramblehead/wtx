// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __RH_CODE_POINT_hpp__
#define __RH_CODE_POINT_hpp__

#include <boost/current_function.hpp>

#define RH_CODE_POINT              \
  (std::string(__FILE__)           \
   + std::string(":")              \
   + std::to_string(__LINE__)      \
   + std::string(": ")             \
   + std::string(BOOST_CURRENT_FUNCTION))
// #define RH_CODE_POINT

#endif // __RH_CODE_POINT_hpp__
