// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __RH_ENUM_SHELL_hpp__
#define __RH_ENUM_SHELL_hpp__

#include <string>
#include <vector>

#include "RH_FOR_EACH_P1.h"

#define RH__ENUM_ITEM_NAME_LENGTH_MAX 2000
#define RH__ENUM_WHITE_SPACE " \t\n";

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
// (proxy)param1, (proxy)param2, (proxy)param3, ...,
#define RH__ENUM_PROXIFY_ITEM(proxy, item) (proxy)item,
#define RH__ENUM_PROXIFY_LIST(proxy, ...) \
  RH_FOR_EACH_P1(RH__ENUM_PROXIFY_ITEM, proxy, __VA_ARGS__)

// (item01, item02), (item11, item12), (item21, item22), ...,
// to
// {"item01", item02}, {"item11", item12}, {"item21", item22}, ...,
#define RH__ENUM_LISTIFY_PAIR(item1, item2) {RH__ENUM_STRINGIFY(item1), item2},
#define RH__ENUM_LISTIFY_PAIR_ITEM(ignore, pair) RH__ENUM_LISTIFY_PAIR pair
#define RH__ENUM_LISTIFY_PAIR_LIST(...) \
  RH_FOR_EACH_P1(RH__ENUM_LISTIFY_PAIR_ITEM, ~, __VA_ARGS__)

#define RH__ENUM_TAC(a, b) a ## b
#define RH__ENUM_CAT(a, b) RH__ENUM_TAC(a, b)

#define RH__ENUM_WHEN_TYPED(type) : type
#define RH__ENUM_WHEN_UNTYPED(type)
#define RH__ENUM_TYPE(typed, type) \
  RH__ENUM_EXPAND(RH__ENUM_CAT(RH__ENUM_WHEN_, typed)(type))

#define RH__ENUM_WHEN_WITHCLASS() class
#define RH__ENUM_WHEN_NOCLASS()
#define RH__ENUM_CLASS(withClass) \
  RH__ENUM_EXPAND(RH__ENUM_CAT(RH__ENUM_WHEN_, withClass)())

#define RH__ENUM_WHEN_RENAMED(renames) \
  RH__ENUM_LISTIFY_PAIR_LIST renames
#define RH__ENUM_WHEN_NOTRENAMED(renames)
#define RH__ENUM_RENAMES(renamed, renames) \
  RH__ENUM_EXPAND(RH__ENUM_CAT(RH__ENUM_WHEN_, renamed)(renames))

class RH_EnumShell {
 public:
  constexpr static int
  itemFindCharInString(char c, const char* s) noexcept {
    for(size_t i = 0; i < RH__ENUM_ITEM_NAME_LENGTH_MAX; ++i) {
      if(s[i] == '\0') return -1;
      if(s[i] == c) return i;
    }
    return -1;
  }
  constexpr static bool
  itemBeginsWithName(const char* item,
                     const char* name) noexcept
  {
    for(size_t i = 0; i < RH__ENUM_ITEM_NAME_LENGTH_MAX; ++i) {
      if(name[i] == '\0') return true;
      if(name[i] != item[i]) return false;
      if(item[i] == '\0') return false;
    }
    return false;
  }
  constexpr static size_t
  itemNameLength(const char* symbol) noexcept {
    // eon -> End Of Symbol
    constexpr const char* eon = ",=" RH__ENUM_WHITE_SPACE;
    size_t i = 0;
    do {
      if(itemFindCharInString(symbol[i], eon) > -1) return i;
      ++i;
    }
    while(symbol[i] != '\0' && i < RH__ENUM_ITEM_NAME_LENGTH_MAX);
    return i;
  }
};

#define RH__ENUM_SHELL(RH__EnumShellName, withClass,                    \
                       ItemType, itemUTyped, ItemUType,                 \
                       itemsRenamed, itemRenames, ...)                  \
  class RH__EnumShellName : public RH_EnumShell {                       \
   public:                                                              \
    enum RH__ENUM_CLASS(withClass) ItemType                             \
    RH__ENUM_TYPE(itemUTyped, ItemUType) {__VA_ARGS__};                 \
   private:                                                             \
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
      return RH__ENUM_STRINGIFY(RH__EnumShellName);                     \
    }                                                                   \
    constexpr static int itemsCount() noexcept {                        \
      constexpr const char* itemSymbols[] {                             \
        RH__ENUM_STRINGIFY_LIST(__VA_ARGS__)                            \
      };                                                                \
      return sizeof(itemSymbols) / sizeof(itemSymbols[0]);              \
    }                                                                   \
    static std::string itemName(int index) {                            \
      if(index < 0 || index >= itemsCount()) return std::string();      \
      const char* symbol = itemSymbol(index);                           \
      const char* name = itemRenamed(symbol);                           \
      if(symbol == name) {                                              \
        size_t nameLength = itemNameLength(name);                       \
        return std::string(name, nameLength);                           \
      }                                                                 \
      else return std::string(name);                                    \
    }                                                                   \
    static std::string itemName(ItemType item) {                        \
      return itemName(itemIndex(item));                                 \
    }                                                                   \
    static std::vector<std::string> itemNames(ItemType item)            \
    {                                                                   \
      decltype(itemNames(item)) result;                                 \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValueNoRangeCheck(i) == item)                            \
          result.push_back(itemName(i));                                \
      }                                                                 \
      return result;                                                    \
    }                                                                   \
    constexpr static int itemIndex(const char* name) noexcept {         \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        const char* rsymbol = itemRenamed(itemSymbol(i));               \
        if(itemBeginsWithName(rsymbol, name)) return i;                 \
      }                                                                 \
      return -1;                                                        \
    }                                                                   \
    constexpr static int itemIndex(ItemType item) noexcept {            \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValueNoRangeCheck(i) == item) return i;                  \
      }                                                                 \
      return -1;                                                        \
    }                                                                   \
    static std::vector<int> itemIndexes(ItemType item)                  \
    {                                                                   \
      decltype(itemIndexes(item)) result;                               \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValueNoRangeCheck(i) == item) result.push_back(i);       \
      }                                                                 \
      return result;                                                    \
    }                                                                   \
    constexpr static ItemType itemValue(int index) {                    \
      if(index < 0 || index >= itemsCount()) {                          \
        itemThrowInvalidArgument("Invalid enum index");                 \
      }                                                                 \
      return itemValueNoRangeCheck(index);                              \
    }                                                                   \
    constexpr static ItemType                                           \
    itemValue(const char* name) {                                       \
      int index = itemIndex(name);                                      \
      if(index < 0) {                                                   \
        itemThrowInvalidArgument("Invalid enum name");                  \
      }                                                                 \
      return itemValueNoRangeCheck(index);                              \
    }                                                                   \
    constexpr RH__EnumShellName(ItemType item = itemValue(0)) noexcept  \
      : item_(item)                                                     \
    {}                                                                  \
    constexpr RH__EnumShellName(const char* name)                       \
      : item_(itemValue(name))                                          \
    {}                                                                  \
    RH__EnumShellName(std::string name)                                 \
      : item_(itemValue(name.c_str()))                                  \
    {}                                                                  \
    constexpr RH__EnumShellName& operator =(ItemType item) noexcept {   \
      item_ = item; return *this;                                       \
    }                                                                   \
    constexpr RH__EnumShellName& operator =(const char* name) {         \
      item_ = itemValue(name); return *this;                            \
    }                                                                   \
    RH__EnumShellName& operator =(std::string name) {                   \
      return this->operator =(name.c_str());                            \
    }                                                                   \
    constexpr bool operator ==(const char* name) const {                \
      return item_ == itemValue(name);                                  \
    }                                                                   \
    bool operator ==(std::string name) const {                          \
      return this->operator ==(name.c_str());                           \
    }                                                                   \
    constexpr bool operator !=(const char* name) const {                \
      return !(item_ == itemValue(name));                               \
    }                                                                   \
    bool operator !=(std::string name) const {                          \
      return this->operator !=(name.c_str());                           \
    }                                                                   \
    constexpr operator ItemType() const {return itemValue();}           \
    constexpr ItemType itemValue() const noexcept {return item_;}       \
    constexpr bool itemName(const char* name) noexcept {                \
      int index = itemIndex(name);                                      \
      if(index < 0) return false;                                       \
      item_ = itemValueNoRangeCheck(index);                             \
      return true;                                                      \
    }                                                                   \
    bool itemName(std::string name) noexcept {                          \
      return itemName(name.c_str());                                    \
    }                                                                   \
    std::string itemName() const {                                      \
      return RH__EnumShellName::itemName(item_);                        \
    }                                                                   \
    std::vector<std::string> itemNames() const {                        \
      return RH__EnumShellName::itemNames(item_);                       \
    }                                                                   \
   public:                                                              \
    constexpr static ItemType                                           \
    itemValueNoRangeCheck(int ItemType ## _index) noexcept {            \
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
    constexpr static const char*                                        \
    itemRenamed(const char* symbol) noexcept {                          \
      constexpr struct {                                                \
        const char* key;                                                \
        const char* value;                                              \
        } renames[] = {RH__ENUM_RENAMES(itemsRenamed, itemRenames)};    \
      for(const auto& rename : renames)                                 \
        if(itemBeginsWithName(symbol, rename.key)) return rename.value; \
      return symbol;                                                    \
    }                                                                   \
    constexpr static void itemThrowInvalidArgument(const char* what) {  \
      what[0] != '\0' ? throw std::invalid_argument(what) : 0;          \
    }                                                                   \
    ItemType item_;                                                     \
  };
//#define RH_ENUM_SHELL

// itemThrowInvalidArgument() constexpr is a workaround for gcc bug:
// http://stackoverflow.com/questions/34280729/throw-in-constexpr-function


#define RH_ENUM_SHELL(EnumSell, ...) \
  RH__ENUM_SHELL(EnumSell, NOCLASS,  \
                 Item, UNTYPED, ~, NOTRENAMED, ~, __VA_ARGS__)

#define RH_ENUM_RSHELL(EnumSell, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, NOCLASS,                \
                 Item, UNTYPED, ~, RENAMED, itemRenames, __VA_ARGS__)


#define RH_ENUM_UTYPE_SHELL(EnumSell, UType, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,             \
                 Item, TYPED, UType, NOTRENAMED, ~, __VA_ARGS__)

#define RH_ENUM_UTYPE_RSHELL(EnumSell, UType, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                           \
                 Item, TYPED, UType, RENAMED, itemRenames, __VA_ARGS__)


#define RH_ENUM_CLASS_SHELL(EnumSell, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,      \
                 Item, UNTYPED, ~, NOTRENAMED, ~, __VA_ARGS__)

#define RH_ENUM_CLASS_RSHELL(EnumSell, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                    \
                 Item, UNTYPED, ~, RENAMED, itemRenames, __VA_ARGS__)


#define RH_ENUM_CLASS_UTYPE_SHELL(EnumSell, UType, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                   \
                 Item, TYPED, UType, NOTRENAMED, ~, __VA_ARGS__)

#define RH_ENUM_CLASS_UTYPE_RSHELL(EnumSell, UType, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                                 \
                 Item, TYPED, UType, RENAMED, itemRenames, __VA_ARGS__)


#define RH_ENUM_CLASS_ITEM_SHELL(EnumSell, Item, ...)  \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                  \
                 Item, UNTYPED, ~, NOTRENAMED, ~, __VA_ARGS__)

#define RH_ENUM_CLASS_ITEM_RSHELL(EnumSell, Item, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                               \
                 Item, UNTYPED, ~, RENAMED, itemRenames, __VA_ARGS__)


#define RH_ENUM_CLASS_ITEM_UTYPE_SHELL(EnumSell, Item, UType, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                              \
                 Item, TYPED, UType, NOTRENAMED, ~, __VA_ARGS__)

#define RH_ENUM_CLASS_ITEM_UTYPE_RSHELL(EnumSell, Item,          \
                                        UType, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                            \
                 Item, TYPED, UType, RENAMED, itemRenames, __VA_ARGS__)

#endif //__RH_ENUM_SHELL_hpp__

// Emacs, here are this file hints.
// Use nlinum mode instead of linum mode, as linum sometimes hangs
// over long C macro definitions parsing.
// Local Variables:
// eval: (progn (linum-mode -1) (nlinum-mode 1))
// End:
