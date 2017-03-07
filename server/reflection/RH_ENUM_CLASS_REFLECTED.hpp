// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __RH_ENUM_CLASS_REFLECTED_hpp__
#define __RH_ENUM_CLASS_REFLECTED_hpp__

#include <string>
#include <functional>
#include <stdexcept>
#include <type_traits>

#include "RH_FOR_EACH.h"

#define RH__ENUM_STRINGIFY(str) #str
#define RH_ENUM_STRINGIFY(str) RH__ENUM_STRINGIFY(str)

// param1, param2, param3 ...
// to
// "param1", "param2", "param3" ...,
#define RH__ENUM_LIST_ITEM(item) RH_ENUM_STRINGIFY(item),
#define RH_ENUM_STRINGIFY_LIST(...) \
  RH_FOR_EACH(RH__ENUM_LIST_ITEM, __VA_ARGS__)

// param1, param2, param3 ...
// to
// (proxy)param1, (proxy)param2, (proxy)param3 ...,
#define RH__ENUM_PROXIFY_LIST_ITEM(item) (ItemProxy)item,
#define RH_ENUM_PROXIFY_LIST(...) \
  RH_FOR_EACH(RH__ENUM_PROXIFY_LIST_ITEM, __VA_ARGS__)

#define RH_ENUM_CLASS_REFLECTED(RH_EnumClassReflected,...)              \
  class RH_EnumClassReflected {                                         \
   public:                                                              \
    enum Item {__VA_ARGS__};                                            \
   private:                                                             \
    constexpr static int maxItemNameLength = 2000;                      \
    class ItemProxy {                                                   \
     public:                                                            \
      constexpr ItemProxy(                                              \
        typename std::underlying_type<Item>::type item)                 \
        : item_(static_cast<Item>(item)) {}                             \
      constexpr operator Item() const {return item_;}                   \
     private:                                                           \
      Item item_;                                                       \
    };                                                                  \
   public:                                                              \
    constexpr static const char* itemsEnumName() {                      \
      return RH_ENUM_STRINGIFY(RH_EnumClassReflected);                  \
    }                                                                   \
    constexpr static int itemsCount() {                                 \
      constexpr const char* enumNames[] {                               \
        RH_ENUM_STRINGIFY_LIST(__VA_ARGS__)                             \
      };                                                                \
      return sizeof(enumNames) / sizeof(enumNames[0]);                  \
    }                                                                   \
    static std::string itemName(int index) {                            \
      constexpr const char* enumNames[] {                               \
        RH_ENUM_STRINGIFY_LIST(__VA_ARGS__)                             \
      };                                                                \
      constexpr int count = itemsCount();                               \
      if(index < 0 || index >= count) return std::string();             \
      const char* itemName = enumNames[index];                          \
      const size_t length = itemNameLength(itemName);                   \
      return std::string(itemName, length);                             \
    }                                                                   \
    static std::string itemName(Item item) {                            \
      return itemName(itemIndex(item));                                 \
    }                                                                   \
    static void itemName(Item item,                                     \
                         std::function<void(std::string)> callback)     \
    {                                                                   \
      constexpr Item items[] {RH_ENUM_PROXIFY_LIST(__VA_ARGS__)};       \
      constexpr int count = itemsCount();                               \
      for(int i = 0; i < count; ++i) {                                  \
        if(items[i] == item) callback(itemName(i));                     \
      }                                                                 \
    }                                                                   \
    constexpr static int itemIndex(const char* name) {                  \
      constexpr const char* itemNames[] {                               \
        RH_ENUM_STRINGIFY_LIST(__VA_ARGS__)                             \
      };                                                                \
      constexpr int count = itemsCount();                               \
      for(int i = 0; i < count; ++i) {                                  \
        const char* itemName = itemNames[i];                            \
        if(itemHasName(itemName, name)) return i;                       \
      }                                                                 \
      return -1;                                                        \
    }                                                                   \
    constexpr static int itemIndex(Item item) {                         \
      constexpr Item items[] {RH_ENUM_PROXIFY_LIST(__VA_ARGS__)};       \
      constexpr int count = itemsCount();                               \
      for(int i = 0; i < count; ++i) {                                  \
        if(items[i] == item) return i;                                  \
      }                                                                 \
      return -1;                                                        \
    }                                                                   \
    static void itemIndex(Item item,                                    \
                          std::function<void(int)> callback)            \
    {                                                                   \
      constexpr Item items[] {RH_ENUM_PROXIFY_LIST(__VA_ARGS__)};       \
      constexpr int count = itemsCount();                               \
      for(int i = 0; i < count; ++i) {                                  \
        if(items[i] == item) callback(i);                               \
      }                                                                 \
    }                                                                   \
    constexpr static Item itemValue(int index) {                        \
      constexpr Item items[] {RH_ENUM_PROXIFY_LIST(__VA_ARGS__)};       \
      constexpr int count = itemsCount();                               \
      if(index < 0 || index >= count) {                                 \
        itemThrowInvalidArgument("Wrong enum 'index'");                 \
      }                                                                 \
      return items[index];                                              \
    }                                                                   \
    constexpr static Item itemValue(const char* name) {                 \
      int index = itemIndex(name);                                      \
      if(index < 0) {                                                   \
        itemThrowInvalidArgument("Wrong enum 'name'");                  \
      }                                                                 \
      constexpr Item items[] {RH_ENUM_PROXIFY_LIST(__VA_ARGS__)};       \
      return items[index];                                              \
    }                                                                   \
    constexpr RH_EnumClassReflected(Item item = itemValue(0))           \
      : item_(item)                                                     \
    {}                                                                  \
    constexpr RH_EnumClassReflected(const char* name)                   \
      : item_(itemValue(name))                                          \
    {}                                                                  \
    RH_EnumClassReflected(std::string name)                             \
      : item_(itemValue(name.c_str()))                                  \
    {}                                                                  \
    constexpr RH_EnumClassReflected& operator =(Item value) {           \
      item_ = value; return *this;                                      \
    }                                                                   \
    constexpr RH_EnumClassReflected& operator =(const char* name) {     \
      item_ = itemValue(name); return *this;                            \
    }                                                                   \
    RH_EnumClassReflected& operator =(std::string name) {               \
      return this->operator =(name.c_str());                            \
    }                                                                   \
    constexpr bool operator ==(const char* name) {                      \
      return item_ == itemValue(name);                                  \
    }                                                                   \
    bool operator ==(std::string name) {                                \
      return this->operator ==(name.c_str());                           \
    }                                                                   \
    constexpr bool operator !=(const char* name) {                      \
      return !(item_ == itemValue(name));                               \
    }                                                                   \
    bool operator !=(std::string name) {                                \
      return this->operator !=(name.c_str());                           \
    }                                                                   \
    constexpr operator Item() const {return item_;}                     \
    constexpr void itemName(const char* name) {                         \
      item_ = itemValue(name);                                          \
    }                                                                   \
    void itemName(std::string name) {                                   \
      item_ = itemValue(name.c_str());                                  \
    }                                                                   \
    std::string itemName() const {                                      \
      return RH_EnumClassReflected::itemName(item_);                    \
    }                                                                   \
    void itemName(std::function<void(std::string)> callback) const {    \
      RH_EnumClassReflected::itemName(item_, callback);                 \
    }                                                                   \
   private:                                                             \
    constexpr static size_t itemNameLength(const char* symbol) {        \
      int i = 0;                                                        \
      do {                                                              \
        if(symbol[i] == ' ' || symbol[i] == '=') return i;              \
        ++i;                                                            \
      }                                                                 \
      while(symbol[i] != '\0' && i < maxItemNameLength);                \
      return i;                                                         \
    }                                                                   \
    constexpr static bool itemHasName(const char* symbol,               \
                                      const char* name)                 \
    {                                                                   \
      int i = 0;                                                        \
      do {                                                              \
        if(symbol[i] != name[i]) return false;                          \
        ++i;                                                            \
      }                                                                 \
      while(name[i] != '\0' && i < maxItemNameLength);                  \
      if(symbol[i] == ' ' ||                                            \
         symbol[i] == '=' ||                                            \
         symbol[i] == '\0') return true;                                \
      else return false;                                                \
    }                                                                   \
    constexpr static void itemThrowInvalidArgument(const char* what) {  \
      what[0] != '\0' ? throw std::invalid_argument(what) : 0;          \
    }                                                                   \
    Item item_;                                                         \
  };
//#define RH_ENUM_CLASS_REFLECTED

// itemThrowInvalidArgument() constexpr is a workaround for gcc bug:
// http://stackoverflow.com/questions/34280729/throw-in-constexpr-function

#endif //__RH_ENUM_CLASS_REFLECTED_hpp__
