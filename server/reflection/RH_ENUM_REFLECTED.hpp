// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __RH_ENUM_REFLECTED_hpp__
#define __RH_ENUM_REFLECTED_hpp__

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

#define RH_ENUM_REFLECTED(RH_EnumClassReflected,...)                    \
  class RH_EnumClassReflected {                                         \
   public:                                                              \
    enum Item {__VA_ARGS__};                                            \
   private:                                                             \
    constexpr static int maxItemNameLength = 2000;                      \
    class ItemProxy {                                                   \
     public:                                                            \
      constexpr ItemProxy(                                              \
        typename std::underlying_type<Item>::type item) noexcept        \
        : item_(static_cast<Item>(item)) {}                             \
      constexpr operator Item() const noexcept {return item_;}          \
     private:                                                           \
      Item item_;                                                       \
    };                                                                  \
   public:                                                              \
    constexpr static const char* itemsEnumName() noexcept  {            \
      return RH_ENUM_STRINGIFY(RH_EnumClassReflected);                  \
    }                                                                   \
    constexpr static int itemsCount() noexcept {                        \
      constexpr const char* itemSymbols[] {                             \
        RH_ENUM_STRINGIFY_LIST(__VA_ARGS__)                             \
      };                                                                \
      return sizeof(itemSymbols) / sizeof(itemSymbols[0]);              \
    }                                                                   \
    static std::string itemName(int index) noexcept {                   \
      constexpr const char* itemSymbols[] {                             \
        RH_ENUM_STRINGIFY_LIST(__VA_ARGS__)                             \
      };                                                                \
      if(index < 0 || index >= itemsCount()) return std::string();      \
      const char* symbol = itemSymbols[index];                          \
      return std::string(symbol, itemNameLength(symbol));               \
    }                                                                   \
    static std::string itemName(Item item) noexcept {                   \
      return itemName(itemIndex(item));                                 \
    }                                                                   \
    static void itemName(                                               \
      Item item, std::function<void(std::string)> callback)             \
    {                                                                   \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValue(i) == item) callback(itemName(i));                 \
      }                                                                 \
    }                                                                   \
    constexpr static int itemIndex(const char* name) noexcept {         \
      constexpr const char* itemSymbols[] {                             \
        RH_ENUM_STRINGIFY_LIST(__VA_ARGS__)                             \
      };                                                                \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemHasName(itemSymbols[i], name)) return i;                 \
      }                                                                 \
      return -1;                                                        \
    }                                                                   \
    constexpr static int itemIndex(Item item) noexcept {                \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValue(i) == item) return i;                              \
      }                                                                 \
      return -1;                                                        \
    }                                                                   \
    static void itemIndex(                                              \
      Item item, std::function<void(int)> callback)                     \
    {                                                                   \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValue(i) == item) callback(i);                           \
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
    constexpr RH_EnumClassReflected(Item item = itemValue(0)) noexcept  \
      : item_(item)                                                     \
    {}                                                                  \
    constexpr RH_EnumClassReflected(const char* name)                   \
      : item_(itemValue(name))                                          \
    {}                                                                  \
    RH_EnumClassReflected(std::string name)                             \
      : item_(itemValue(name.c_str()))                                  \
    {}                                                                  \
    constexpr RH_EnumClassReflected&                                    \
    operator =(Item value)  noexcept                                    \
    {                                                                   \
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
    constexpr operator Item() const noexcept {return item_;}            \
    constexpr void itemName(const char* name) {                         \
      item_ = itemValue(name);                                          \
    }                                                                   \
    void itemName(std::string name) {                                   \
      item_ = itemValue(name.c_str());                                  \
    }                                                                   \
    std::string itemName() const noexcept {                             \
      return RH_EnumClassReflected::itemName(item_);                    \
    }                                                                   \
    void itemName(std::function<void(std::string)> callback) const {    \
      RH_EnumClassReflected::itemName(item_, callback);                 \
    }                                                                   \
   private:                                                             \
    constexpr static size_t                                             \
    itemNameLength(const char* symbol) noexcept                         \
    {                                                                   \
      int i = 0;                                                        \
      do {                                                              \
        if(symbol[i] == ' ' || symbol[i] == '=') return i;              \
        ++i;                                                            \
      }                                                                 \
      while(symbol[i] != '\0' && i < maxItemNameLength);                \
      return i;                                                         \
    }                                                                   \
    constexpr static bool                                               \
    itemHasName(const char* symbol, const char* name) noexcept          \
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
//#define RH_ENUM_REFLECTED

// itemThrowInvalidArgument() constexpr is a workaround for gcc bug:
// http://stackoverflow.com/questions/34280729/throw-in-constexpr-function

#endif //__RH_ENUM_REFLECTED_hpp__
