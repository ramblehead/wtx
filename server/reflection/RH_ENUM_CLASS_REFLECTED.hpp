// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __RH_ENUM_CLASS_REFLECTED_hpp__
#define __RH_ENUM_CLASS_REFLECTED_hpp__

#include "RH_STRINGIFY.h"

#define RH_ENUM_CLASS_REFLECTED(RH_EnumClassReflected, ...)        \
  class RH_EnumClassReflected {                                    \
   public:                                                         \
    enum Item {__VA_ARGS__};                                       \
    static const char* enumName() {                                \
      return RH_STRINGIFY(EnumClassReflected);                     \
    }                                                              \
    static const char* itemName(Item value) {                      \
      const char* enumNames[] {RH_STRINGIFY_LIST(__VA_ARGS__)};    \
      return enumNames[static_cast<size_t>(value)];                \
    }                                                              \
    static const Item itemValue(const char* name) {                \
      const char* enumNames[] {RH_STRINGIFY_LIST(__VA_ARGS__)};    \
      for (size_t i = 0; i < sizeof(enumNames); ++i) {             \
        auto enumName = enumNames[i];                              \
        if(std::strcmp(enumName, name) == 0) {                     \
          return static_cast<Item>(i);                             \
        }                                                          \
      }                                                            \
      throw std::invalid_argument("Wrong enum 'name'");            \
    }                                                              \
    RH_EnumClassReflected(Item value = static_cast<Item>(0))       \
      : m_value(value)                                             \
    {}                                                             \
    RH_EnumClassReflected(const char* name)                        \
      : m_value(itemValue(name))                                   \
    {}                                                             \
    RH_EnumClassReflected& operator =(Item value) {                \
      m_value = value; return *this;                               \
    }                                                              \
    operator Item() const { return m_value; }                      \
    void name(const char* value) {                                 \
      m_value = itemValue(value);                                  \
    }                                                              \
    const char* name() const {                                     \
      return RH_EnumClassReflected::itemName(m_value);             \
    }                                                              \
   private:                                                        \
    Item m_value;                                                  \
  };
//#define RH_ENUM_CLASS_REFLECTED

#endif //__RH_ENUM_CLASS_REFLECTED_hpp__
