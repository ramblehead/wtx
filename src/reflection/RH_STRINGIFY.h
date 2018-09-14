// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __RH_STRINGIFY_h__
#define __RH_STRINGIFY_h__

// Neat trick with the safe guard against nested macro issues.
// see http://stackoverflow.com/questions/806543/c-macros-manipulating-a-parameter-specific-example
// see http://stackoverflow.com/questions/2751870/how-exactly-does-the-double-stringize-trick-work
#define RH__STRINGIFY(str) #str
#define RH_STRINGIFY(str) RH__STRINGIFY(str)

#endif //__RH_STRINGIFY_h__
