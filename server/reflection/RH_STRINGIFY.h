// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __RH_STRINGIFY_h__
#define __RH_STRINGIFY_h__

#include "RH_FOR_EACH.h"

// Neat trick with the safe guard against nested macro issues.
// see http://stackoverflow.com/questions/806543/c-macros-manipulating-a-parameter-specific-example
// see http://stackoverflow.com/questions/2751870/how-exactly-does-the-double-stringize-trick-work
#define RH__STRINGIFY(str) #str
#define RH_STRINGIFY(str) RH__STRINGIFY(str)

// Converting the list of parameters such as:
// #define list param1, param2, param3
// into "param1", "param2", "param3"
#define RH__LIST_ITEM(item) , RH_STRINGIFY(item)
#define RH__STRINGIFY_LIST(first,...) \
  RH_STRINGIFY(first) RH_FOR_EACH(RH__LIST_ITEM, __VA_ARGS__)

// Expanding "parameters list string" into "list of parameter strings"
#define RH_STRINGIFY_LIST(...) RH__EXPAND(RH__STRINGIFY_LIST(__VA_ARGS__))

#endif //__RH_STRINGIFY_h__
