// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __refactorables_hpp__
#define __refactorables_hpp__

#include "RH_STRINGIFY.h"

// N stands for Name
inline constexpr void RH__RefactoringVariableNameTracker(const void*()) {}
inline constexpr void RH__RefactoringVariableNameTracker(const void*) {}
#define RH_N(name) \
  ((RH__RefactoringVariableNameTracker(&name)), (RH__STRINGIFY(name)))

// N stands for Name
template <typename T>
inline constexpr void RH__RefactoringTypeNameTracker() {}

// TN Stands for Type Name
#define RH_TN(TypeName) \
  ((RH__RefactoringTypeNameTracker<TypeName>()), (RH__STRINGIFY(TypeName)))

// NR stands for Name Reflection
#define RH_NR(name) (RH_STRINGIFY(name)), (name)

// RN stands for Reflection Name
#define RH_RN(name) (name), (RH_STRINGIFY(name))

#endif //__refactorables_hpp__
