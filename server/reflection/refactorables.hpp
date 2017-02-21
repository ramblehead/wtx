// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __refactorables_hpp__
#define __refactorables_hpp__

#include "RH_STRINGIFY.h"

// N stands for Name
inline void RH__RefactoringNameTracker(const void*) {}
#define RH_N(name) \
  ((RH__RefactoringNameTracker(&name)), (RH__STRINGIFY(name)))

// NR stands for Name Reflection
#define RH_NR(name) (RH_STRINGIFY(name)), (name)

// RN stands for Reflection Name
#define RH_RN(name) (name), (RH_STRINGIFY(name))

#endif //__refactorables_hpp__
