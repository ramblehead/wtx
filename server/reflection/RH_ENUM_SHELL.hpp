// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __RH_ENUM_SHELL_hpp__
#define __RH_ENUM_SHELL_hpp__

#include <string>
#include <functional>
#include <stdexcept>
#include <type_traits>

#include "RH_FOR_EACH_P1.h"

#define RH__ENUM_ITEM_NAME_LENGTH_MAX 2000

#define RH__ENUM_EXPAND(x) x

#define RH__ENUM_STRINGIFY_(str) #str
#define RH__ENUM_STRINGIFY(str) RH__ENUM_STRINGIFY_(str)

// param1, param2, param3, ...
// to
// "param1", "param2", "param3", ...,
#define RH__ENUM_STRINGIFY_ITEM(ignore, item) RH__ENUM_STRINGIFY(item),
#define RH__ENUM_STRINGIFY_LIST(...) \
  RH_FOR_EACH_P1(RH__ENUM_STRINGIFY_ITEM, ~, __VA_ARGS__)

// param1, param2, param3, ...
// to
// (proxy)param1, (proxy)param2, (proxy)param3 ...,
#define RH__ENUM_PROXIFY_ITEM(proxy, item) (proxy)item,
#define RH__ENUM_PROXIFY_LIST(proxy, ...) \
  RH_FOR_EACH_P1(RH__ENUM_PROXIFY_ITEM, proxy, __VA_ARGS__)

// param1, param2, param3, ...
// to
// func("param1"), func("param2"), func("param3"), ...,
#define RH__ENUM_FUNSTRINGIFY_ITEM(func, item) func(RH__ENUM_STRINGIFY(item)),
#define RH__ENUM_FUNSTRINGIFY_LIST(func, ...) \
  RH_FOR_EACH_P1(RH__ENUM_FUNSTRINGIFY_ITEM, func, __VA_ARGS__)

#define RH__ENUM_CAT_(a, b) a ## b
#define RH__ENUM_CAT(a, b) RH__ENUM_CAT_(a, b)

#define RH__ENUM_WHEN_TYPED(type) : type
#define RH__ENUM_WHEN_UNTYPED(type)
#define RH__ENUM_TYPE(typed, type) \
  RH__ENUM_EXPAND(RH__ENUM_CAT(RH__ENUM_WHEN_, typed)(type))

#define RH__ENUM_WHEN_WITHCLASS() class
#define RH__ENUM_WHEN_NOCLASS()
#define RH__ENUM_CLASS(withClass) \
  RH__ENUM_EXPAND(RH__ENUM_CAT(RH__ENUM_WHEN_, withClass)())

#define RH__ENUM_SHELL(RH_EnumShell, withClass,                         \
                       ItemType, itemUTyped, ItemUType, ...)            \
  class RH_EnumShell {                                                  \
   public:                                                              \
    enum RH__ENUM_CLASS(withClass) ItemType                             \
    RH__ENUM_TYPE(itemUTyped, ItemUType) {__VA_ARGS__};                 \
   private:                                                             \
    constexpr static int                                                \
    ItemType ## _nameLengthMax = RH__ENUM_ITEM_NAME_LENGTH_MAX;         \
    class ItemType ## Proxy {                                           \
     public:                                                            \
      constexpr ItemType ## Proxy(                                      \
        typename std::underlying_type<ItemType>::type item) noexcept    \
        : item_(static_cast<ItemType>(item)) {}                         \
      constexpr operator ItemType() const noexcept {return item_;}      \
     private:                                                           \
      ItemType item_;                                                   \
    };                                                                  \
   public:                                                              \
    constexpr static const char* itemsScopeName() noexcept  {           \
      return RH__ENUM_STRINGIFY(ItemType);                              \
    }                                                                   \
    constexpr static const char* itemsEnumReflectedName() noexcept {    \
      return RH__ENUM_STRINGIFY(RH_EnumShell);                          \
    }                                                                   \
    constexpr static int itemsCount() noexcept {                        \
      constexpr const char* itemSymbols[] {                             \
        RH__ENUM_STRINGIFY_LIST(__VA_ARGS__)                            \
      };                                                                \
      return sizeof(itemSymbols) / sizeof(itemSymbols[0]);              \
    }                                                                   \
    static std::string itemName(int index) noexcept {                   \
      if(index < 0 || index >= itemsCount()) return std::string();      \
      return std::string(itemSymbol(index), itemNameLength(index));     \
    }                                                                   \
    static std::string itemName(ItemType item) noexcept {               \
      return itemName(itemIndex(item));                                 \
    }                                                                   \
    static void itemName(                                               \
      ItemType item, std::function<void(std::string)> callback)         \
    {                                                                   \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValue(i) == item) callback(itemName(i));                 \
      }                                                                 \
    }                                                                   \
    constexpr static int itemIndex(const char* name) noexcept {         \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemHasName(itemSymbol(i), name)) return i;                  \
      }                                                                 \
      return -1;                                                        \
    }                                                                   \
    constexpr static int itemIndex(ItemType item) noexcept {            \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValue(i) == item) return i;                              \
      }                                                                 \
      return -1;                                                        \
    }                                                                   \
    static void itemIndex(                                              \
      ItemType item, std::function<void(int)> callback)                 \
    {                                                                   \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValue(i) == item) callback(i);                           \
      }                                                                 \
    }                                                                   \
    constexpr static ItemType itemValue(int ItemType ## _index) {       \
      if(ItemType ## _index < 0 ||                                      \
         ItemType ## _index >= itemsCount())                            \
      {                                                                 \
        itemThrowInvalidArgument("Wrong enum 'index'");                 \
      }                                                                 \
      return itemValueNoRangeCheck(ItemType ## _index);                 \
    }                                                                   \
    constexpr static ItemType                                           \
    itemValue(const char* ItemType ## _name) {                          \
      int ItemType ## _index = itemIndex(ItemType ## _name);            \
      if(ItemType ## _index < 0) {                                      \
        itemThrowInvalidArgument("Wrong enum 'name'");                  \
      }                                                                 \
      return itemValueNoRangeCheck(ItemType ## _index);                 \
    }                                                                   \
    constexpr RH_EnumShell(ItemType item = itemValue(0)) noexcept       \
      : item_(item)                                                     \
    {}                                                                  \
    constexpr RH_EnumShell(const char* name)                            \
      : item_(itemValue(name))                                          \
    {}                                                                  \
    RH_EnumShell(std::string name)                                      \
      : item_(itemValue(name.c_str()))                                  \
    {}                                                                  \
    constexpr RH_EnumShell&                                             \
    operator =(ItemType value) noexcept                                 \
    {                                                                   \
      item_ = value; return *this;                                      \
    }                                                                   \
    constexpr RH_EnumShell& operator =(const char* name) {              \
      item_ = itemValue(name); return *this;                            \
    }                                                                   \
    RH_EnumShell& operator =(std::string name) {                        \
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
    constexpr operator ItemType() const noexcept {return itemValue();}  \
    constexpr ItemType itemValue() const noexcept {return item_;}       \
    constexpr void itemName(const char* name) {                         \
      item_ = itemValue(name);                                          \
    }                                                                   \
    void itemName(std::string name) {                                   \
      item_ = itemValue(name.c_str());                                  \
    }                                                                   \
    std::string itemName() const noexcept {                             \
      return RH_EnumShell::itemName(item_);                             \
    }                                                                   \
    void itemName(std::function<void(std::string)> callback) const {    \
      RH_EnumShell::itemName(item_, callback);                          \
    }                                                                   \
   private:                                                             \
    constexpr static ItemType                                           \
    itemValueNoRangeCheck(int ItemType ## _index) {                     \
      enum RH__ENUM_TYPE(itemUTyped, ItemUType) {__VA_ARGS__};          \
      constexpr ItemType ItemType ## _array[] {                         \
        RH__ENUM_PROXIFY_LIST(ItemType ## Proxy, __VA_ARGS__)           \
      };                                                                \
      return ItemType ## _array[ItemType ## _index];                    \
    }                                                                   \
    constexpr static const char* itemSymbol(int index) noexcept {       \
      constexpr const char* itemSymbols[] {                             \
        RH__ENUM_STRINGIFY_LIST(__VA_ARGS__)                            \
      };                                                                \
      return itemSymbols[index];                                        \
    }                                                                   \
    constexpr static size_t                                             \
    itemNameLength(const char* symbol) noexcept {                       \
      size_t i = 0;                                                     \
      do {                                                              \
        if(symbol[i] == ' ' || symbol[i] == '=') return i; ++i;         \
      }                                                                 \
      while(symbol[i] != '\0' && i < ItemType ## _nameLengthMax);       \
      return i;                                                         \
    }                                                                   \
    constexpr static size_t                                             \
    itemNameLength(int index) noexcept {                                \
      constexpr size_t lengths[] {                                      \
        RH__ENUM_FUNSTRINGIFY_LIST(itemNameLength, __VA_ARGS__)         \
      };                                                                \
      return lengths[index];                                            \
    }                                                                   \
    constexpr static bool                                               \
    itemHasName(const char* symbol, const char* name) noexcept {        \
      int i = 0;                                                        \
      do {                                                              \
        if(symbol[i] != name[i]) return false; ++i;                     \
      }                                                                 \
      while(name[i] != '\0' && i < ItemType ## _nameLengthMax);         \
      if(symbol[i] == ' ' ||                                            \
         symbol[i] == '=' ||                                            \
         symbol[i] == '\0') return true;                                \
      else return false;                                                \
    }                                                                   \
    constexpr static void itemThrowInvalidArgument(const char* what) {  \
      what[0] != '\0' ? throw std::invalid_argument(what) : 0;          \
    }                                                                   \
    ItemType item_;                                                     \
  };
//#define RH_ENUM_SHELL

// itemThrowInvalidArgument() constexpr is a workaround for gcc bug:
// http://stackoverflow.com/questions/34280729/throw-in-constexpr-function

#define RH_ENUM_SHELL(RH_EnumShell, ...) \
  RH__ENUM_SHELL(RH_EnumShell, NOCLASS,         \
                 Item, UNTYPED, ~, __VA_ARGS__)

#define RH_ENUM_UTYPE_SHELL(RH_EnumShell, UType, ...) \
  RH__ENUM_SHELL(RH_EnumShell, WITHCLASS,             \
                 Item, TYPED, UType, __VA_ARGS__)

#define RH_ENUM_CLASS_SHELL(RH_EnumShell, ...) \
  RH__ENUM_SHELL(RH_EnumShell, WITHCLASS,      \
                 Item, UNTYPED, ~, __VA_ARGS__)

#define RH_ENUM_CLASS_UTYPE_SHELL(RH_EnumShell, UType, ...) \
  RH__ENUM_SHELL(RH_EnumShell, WITHCLASS,                   \
                 Item, TYPED, UType, __VA_ARGS__)

#define RH_ENUM_CLASS_ITEM_SHELL(RH_EnumShell, Item, ...)  \
  RH__ENUM_SHELL(RH_EnumShell, WITHCLASS,                  \
                 Item, UNTYPED, ~, __VA_ARGS__)

#define RH_ENUM_CLASS_ITEM_UTYPE_SHELL(RH_EnumShell, Item, UType,...) \
  RH__ENUM_SHELL(RH_EnumShell, WITHCLASS,                             \
                 Item, TYPED, UType, __VA_ARGS__)

#endif //__RH_ENUM_SHELL_hpp__
