// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __RH_ENUM_SHELL_hpp__
#define __RH_ENUM_SHELL_hpp__

// TODO: * Skip white space in quote definition prifix (e.g. "  \" ,  ...")

#include <string>
#include <functional>
#include <stdexcept>
#include <type_traits>

#include "RH_FOR_EACH_P1.h"

#define RH__ENUM_ITEM_NAME_LENGTH_MAX 2000
#define RH__ENUM_WHITE_SPACE " \t\n";
#define RH__ENUM_DEFAULT_QUOTE '\'';

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

class RH__EnumShellBase {
 public:
  // TODO: Compile-time-optimisation, similar to the used in the following
  //       commented method, is possible for all EnumShell methods operating on
  //       string literals. Such optimised methods should also work at run-time,
  //       although with performance penalties due to the recursion. To avoid
  //       such penalties, it should be possible to be able to distinguish
  //       between constexpr and run-time function calls and keep two different
  //       implementations of each method which may operate on either variables
  //       or literals.
  // Something like: http://stackoverflow.com/questions/40409323/compile-time-or-runtime-detection-within-a-constexpr-function
  // I am still looking for an elegant solution for distinguishing between
  // constexpr and run-time calls. C++17 "if constexpr" might be handy for this.
  //
  // constexpr static int
  // itemFindCharInStringImpl(char c,
  //                          const char* s,
  //                          size_t i,
  //                          size_t lengthMax) noexcept
  // {
  //   if(s[i] != '\0' && i < lengthMax) {
  //     if(s[i] == c) return i;
  //     else return itemFindCharInStringImpl(c, s, i + 1, lengthMax);
  //   }
  //   return -1;
  // }
  constexpr static int
  itemFindCharInString(char c, const char* s) noexcept {
    size_t i = 0;
    do {
      if(s[i] == c) return i; ++i;
    }
    while(s[i] != '\0' && i < RH__ENUM_ITEM_NAME_LENGTH_MAX);
    return -1;
    // return itemFindCharInStringImpl(c, s, 0, RH__ENUM_ITEM_NAME_LENGTH_MAX);
  }
  constexpr static size_t
  itemNameLength(const char* name) noexcept {
    constexpr const char* invalid = ",=" RH__ENUM_WHITE_SPACE;
    size_t i = 0;
    do {
      if(itemFindCharInString(name[i], invalid) > -1) return i;
      ++i;
    }
    while(name[i] != '\0' && i < RH__ENUM_ITEM_NAME_LENGTH_MAX);
    return i;
  }
  static const char*
  itemSkipWhiteSpace(const char* s) noexcept {
    const char* iter = s;
    constexpr const char* space = RH__ENUM_WHITE_SPACE;
    do {
      if(itemFindCharInString(*iter, space) < 0) return iter;
      ++iter;
    }
    while(*iter != '\0');
    return iter;
  }
  static const char*
  itemNextChar(const char* s, char c) noexcept {
    const char* iter = s;
    do {
      if(*iter == c) return itemSkipWhiteSpace(iter + 1); ++iter;
    }
    while(*iter != '\0');
    return iter;
  }
};

#define RH__ENUM_SHELL(RH__EnumShell, withClass,                        \
                       ItemType, itemUTyped, ItemUType,                 \
                       itemRenames, ...)                                \
  class RH__EnumShell : public RH__EnumShellBase {                      \
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
      return RH__ENUM_STRINGIFY(RH__EnumShell);                         \
    }                                                                   \
    constexpr static int itemsCount() noexcept {                        \
      constexpr const char* itemSymbols[] {                             \
        RH__ENUM_STRINGIFY_LIST(__VA_ARGS__)                            \
      };                                                                \
      return sizeof(itemSymbols) / sizeof(itemSymbols[0]);              \
    }                                                                   \
    static std::string itemName(int index) noexcept {                   \
      if(index < 0 || index >= itemsCount()) return std::string();      \
      const char* symbol = itemSymbol(index);                           \
      const char* name = itemRenamed(symbol);                           \
      size_t nameLength = name == symbol ?                              \
                          itemNameLength(name) :                        \
                          itemRenamedLength(name);                      \
      return std::string(name, nameLength);                             \
    }                                                                   \
    static std::string itemName(ItemType item) noexcept {               \
      return itemName(itemIndex(item));                                 \
    }                                                                   \
    static void itemName(                                               \
      ItemType item, std::function<void(std::string)> callback)         \
    {                                                                   \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValue(i) == item) {                                      \
          callback(itemName(i));                                        \
        }                                                               \
      }                                                                 \
    }                                                                   \
    static int itemIndex(const char* name) noexcept {                   \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        const char* symbol = itemSymbol(i);                             \
        const char* rsymbol = itemRenamed(symbol);                      \
        if(rsymbol == symbol) {                                         \
          if(itemBeginsWithName(symbol, name)) return i;                \
        }                                                               \
        else {                                                          \
          if(itemRenamedBeginsWithName(rsymbol, name)) return i;        \
        }                                                               \
      }                                                                 \
      return -1;                                                        \
    }                                                                   \
    constexpr static int itemIndex(ItemType item) noexcept {            \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValue(i) == item) return i;                              \
      }                                                                 \
      return -1;                                                        \
    }                                                                   \
    static void                                                         \
    itemIndex(ItemType item, std::function<void(int)> callback)         \
    {                                                                   \
      for(int i = 0; i < itemsCount(); ++i) {                           \
        if(itemValue(i) == item) callback(i);                           \
      }                                                                 \
    }                                                                   \
    constexpr static ItemType itemValue(int ItemType ## _index) {       \
      if(ItemType ## _index < 0 ||                                      \
         ItemType ## _index >= itemsCount())                            \
      {                                                                 \
        itemThrowInvalidArgument("Invalid enum index");                 \
      }                                                                 \
      return itemValueNoRangeCheck(ItemType ## _index);                 \
    }                                                                   \
    static ItemType                                                     \
    itemValue(const char* ItemType ## _name) {                          \
      int ItemType ## _index = itemIndex(ItemType ## _name);            \
      if(ItemType ## _index < 0) {                                      \
        itemThrowInvalidArgument("Invalid enum name");                  \
      }                                                                 \
      return itemValueNoRangeCheck(ItemType ## _index);                 \
    }                                                                   \
    constexpr RH__EnumShell(ItemType item = itemValue(0)) noexcept      \
      : item_(item)                                                     \
    {}                                                                  \
    RH__EnumShell(const char* name)                                     \
      : item_(itemValue(name))                                          \
    {}                                                                  \
    RH__EnumShell(std::string name)                                     \
      : item_(itemValue(name.c_str()))                                  \
    {}                                                                  \
    constexpr RH__EnumShell&                                            \
    operator =(ItemType value) noexcept {                               \
      item_ = value; return *this;                                      \
    }                                                                   \
    RH__EnumShell& operator =(const char* name) {                       \
      item_ = itemValue(name); return *this;                            \
    }                                                                   \
    RH__EnumShell& operator =(std::string name) {                       \
      return this->operator =(name.c_str());                            \
    }                                                                   \
    bool operator ==(const char* name) {                                \
      return item_ == itemValue(name);                                  \
    }                                                                   \
    bool operator ==(std::string name) {                                \
      return this->operator ==(name.c_str());                           \
    }                                                                   \
    bool operator !=(const char* name) {                                \
      return !(item_ == itemValue(name));                               \
    }                                                                   \
    bool operator !=(std::string name) {                                \
      return this->operator !=(name.c_str());                           \
    }                                                                   \
    constexpr operator ItemType() const noexcept {return itemValue();}  \
    constexpr ItemType itemValue() const noexcept {return item_;}       \
    void itemName(const char* name) {                                   \
      item_ = itemValue(name);                                          \
    }                                                                   \
    void itemName(std::string name) {                                   \
      item_ = itemValue(name.c_str());                                  \
    }                                                                   \
    std::string itemName() const noexcept {                             \
      return RH__EnumShell::itemName(item_);                            \
    }                                                                   \
    void itemName(std::function<void(std::string)> callback) const {    \
      RH__EnumShell::itemName(item_, callback);                         \
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
    constexpr static char itemRenamedQuote() noexcept {                 \
      constexpr char quote = RH__ENUM_DEFAULT_QUOTE;                    \
      return                                                            \
        (sizeof(itemRenames) / sizeof(itemRenames[0])) < 2 ? quote :    \
        itemRenames[1] == ',' ? itemRenames[0] : quote;                 \
    }                                                                   \
    constexpr static const char*                                        \
    itemRenamesDiscardQuotePrefix() noexcept {                          \
      return                                                            \
        (sizeof(itemRenames) / sizeof(itemRenames[0])) < 2 ?            \
        itemRenames :                                                   \
        itemRenames[1] == ',' ? &itemRenames[2] : itemRenames;          \
    }                                                                   \
    constexpr static size_t                                             \
    itemRenamedLength(const char* iter) noexcept {                      \
      constexpr char quote = itemRenamedQuote();                        \
      char quoteStr[2] = {quote, '\n'};                                 \
      const char* end = nullptr;                                        \
      if(iter[-1] == quote) { quoteStr[0] = quote; end = quoteStr; }    \
      else { end = "," RH__ENUM_WHITE_SPACE; };                         \
      size_t i = 0;                                                     \
      do {                                                              \
        if(itemFindCharInString(iter[i], end) > -1) return i; ++i;      \
      }                                                                 \
      while(iter[i] != '\0' && i < RH__ENUM_ITEM_NAME_LENGTH_MAX);      \
      return i;                                                         \
    }                                                                   \
    constexpr static bool                                               \
    itemBeginsWithName(const char* item,                                \
                       const char* name,                                \
                       int nameLength =                                 \
                       RH__ENUM_ITEM_NAME_LENGTH_MAX) noexcept          \
    {                                                                   \
      constexpr const char* space = RH__ENUM_WHITE_SPACE;               \
      constexpr char quote = itemRenamedQuote();                        \
      int i = 0;                                                        \
      do {                                                              \
        if(item[i] != name[i]) return false; ++i;                       \
      }                                                                 \
      while(name[i] != '\0' && i < nameLength);                         \
      if(itemFindCharInString(item[i], space) > -1 ||                   \
         item[i] == '=' ||                                              \
         item[i] == quote ||                                            \
         item[i] == '\0') return true;                                  \
      else return false;                                                \
    }                                                                   \
    static bool                                                         \
    itemRenamedBeginsWithName(const char* item,                         \
                              const char* beg) noexcept                 \
    {                                                                   \
      int i = 0;                                                        \
      constexpr char quote = itemRenamedQuote();                        \
      if(item[-1] == quote) {                                           \
        do {                                                            \
          if(item[i] != beg[i]) return false; ++i;                      \
        }                                                               \
        while(beg[i] != '\0' && i < RH__ENUM_ITEM_NAME_LENGTH_MAX);     \
        if(item[i] == quote) return true;                               \
        else return false;                                              \
      }                                                                 \
      else {                                                            \
        constexpr const char* invalid = "," RH__ENUM_WHITE_SPACE;       \
        do {                                                            \
          if(itemFindCharInString(item[i], invalid) > -1) return false; \
          if(item[i] != beg[i]) return false; ++i;                      \
        }                                                               \
        while(beg[i] != '\0' && i < RH__ENUM_ITEM_NAME_LENGTH_MAX);     \
        const char* nonSpaceIter = itemSkipWhiteSpace(&item[i]);        \
        if(*nonSpaceIter == ',' || *nonSpaceIter == '\0') return true;  \
        else return false;                                              \
      }                                                                 \
    }                                                                   \
    static const char*                                                  \
    itemRenamed(const char* name) noexcept {                            \
      constexpr const char* renames = itemRenamesDiscardQuotePrefix();  \
      if(renames[0] == '\0') return name;                               \
      constexpr char quote = itemRenamedQuote();                        \
      const char* iter = itemSkipWhiteSpace(renames);                   \
      do {                                                              \
        if(itemBeginsWithName(iter, name, itemNameLength(name))) {      \
          const char* afterAssignIter = itemNextChar(iter, '=');        \
          const char* nonSpaceIter =                                    \
            itemSkipWhiteSpace(afterAssignIter);                        \
          if(*nonSpaceIter == quote) return nonSpaceIter + 1;           \
          else return nonSpaceIter;                                     \
        }                                                               \
        iter = itemNextChar(iter, ',');                                 \
      }                                                                 \
      while(*iter != '\0');                                             \
      return name;                                                      \
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
                 Item, UNTYPED, ~, "", __VA_ARGS__)

#define RH_ENUM_RSHELL(EnumSell, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, NOCLASS,                \
                 Item, UNTYPED, ~, itemRenames, __VA_ARGS__)


#define RH_ENUM_UTYPE_SHELL(EnumSell, UType, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,             \
                 Item, TYPED, UType, "", __VA_ARGS__)

#define RH_ENUM_UTYPE_RSHELL(EnumSell, UType, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                           \
                 Item, TYPED, UType, itemRenames, __VA_ARGS__)


#define RH_ENUM_CLASS_SHELL(EnumSell, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,      \
                 Item, UNTYPED, ~, "", __VA_ARGS__)

#define RH_ENUM_CLASS_RSHELL(EnumSell, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                    \
                 Item, UNTYPED, ~, itemRenames, __VA_ARGS__)


#define RH_ENUM_CLASS_UTYPE_SHELL(EnumSell, UType, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                   \
                 Item, TYPED, UType, "", __VA_ARGS__)

#define RH_ENUM_CLASS_UTYPE_RSHELL(EnumSell, UType, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                                 \
                 Item, TYPED, UType, itemRenames, __VA_ARGS__)


#define RH_ENUM_CLASS_ITEM_SHELL(EnumSell, Item, ...)  \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                  \
                 Item, UNTYPED, ~, "", __VA_ARGS__)

#define RH_ENUM_CLASS_ITEM_RSHELL(EnumSell, Item, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                               \
                 Item, UNTYPED, ~, itemRenames, __VA_ARGS__)


#define RH_ENUM_CLASS_ITEM_UTYPE_SHELL(EnumSell, Item, UType, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                              \
                 Item, TYPED, UType, "", __VA_ARGS__)

#define RH_ENUM_CLASS_ITEM_UTYPE_RSHELL(EnumSell, Item,          \
                                        UType, itemRenames, ...) \
  RH__ENUM_SHELL(EnumSell, WITHCLASS,                            \
                 Item, TYPED, UType, itemRenames, __VA_ARGS__)

#endif //__RH_ENUM_SHELL_hpp__

// Emacs, here are this file hints,
// Use nlinum mode instead of linum mode, as linum sometimes hangs
// over long C macro definitions parsing.
// Local Variables:
// eval: (progn (linum-mode -1) (nlinum-mode 1))
// End:
