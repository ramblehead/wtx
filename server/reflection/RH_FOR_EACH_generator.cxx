// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
// compile: g++ RH_FOR_EACH_generator.cxx -o RH_FOR_EACH_generator -std=c++11 -Wall

#include <iostream>
#include <string>

// see http://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
std::string replace_all_copy(std::string str,
                             const std::string& from,
                             const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        // Handles case where 'to' is a substring of 'from'
        start_pos += to.length();
    }
    return str;
}

const int forMax = 100;
std::string macroPrefix = "RH_";

std::string templateHead =
R"(// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __{prefix}FOR_EACH_h__
#define __{prefix}FOR_EACH_h__

// This guard is required for MSVC compilers to do macro-expansion correctly.
// see http://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly
#define {prefix}_FE_EXPAND(x) x

// Make a FOREACH macro
#define {prefix}_FE_1(a, x) a(x))";

std::string templateFE_nString =
"#define {prefix}_FE_{incN}(a, x, ...) {prefix}_FE_EXPAND(a(x) {prefix}_FE_{N}(a, __VA_ARGS__))";

std::string templateGetMacro =
"#define {prefix}_FE_GET_MACRO({sequence}name,...) name";

std::string templateTail =
R"(
#define {prefix}FOR_EACH(a,...) \
{prefix}_FE_EXPAND({prefix}_FE_GET_MACRO(__VA_ARGS__,{sequence})(a,__VA_ARGS__))

#endif //__{prefix}FOR_EACH_h__)";

int main(int argc, char *argv[]) {
  std::string str = templateHead;
  str = replace_all_copy(str, "{prefix}", macroPrefix);
  std::cout << str << std::endl;

  str = "";
  for(int i=1; i<forMax; ++i) {
    str = templateFE_nString;
    std::string N = std::to_string(i);
    str = replace_all_copy(str, "{N}", N);
    std::string incN = std::to_string(i+1);
    str = replace_all_copy(str, "{incN}", incN);
    str = replace_all_copy(str, "{prefix}", macroPrefix);
    std::cout << str << std::endl;
  }
  std::cout << std::endl;

  str = "";
  for(int i=1; i<=forMax; ++i) {
    str += std::string("_") + std::to_string(i) + ",";
  }
  str = replace_all_copy(templateGetMacro, "{sequence}", str);
  str = replace_all_copy(str, "{prefix}", macroPrefix);
  std::cout << str << std::endl;

  str = "";
  for(int i=forMax; i>0; --i) {
    str += std::string("{prefix}_FE_") + std::to_string(i) + ",";
  }
  str = replace_all_copy(templateTail, "{sequence}", str);
  str = replace_all_copy(str, "{prefix}", macroPrefix);
  std::cout << str << std::endl;

  return 0;
}

// Emacs, here are file hints,
// Local Variables:
// compile-command: "g++ RH_FOR_EACH_generator.cxx -o RH_FOR_EACH_generator -std=c++11 -Wall"
// End:
