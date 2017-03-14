// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
// compile: g++ RH_FOR_EACH_generator.cxx -o RH_FOR_EACH_generator -std=c++11 -Wall

#include <iostream>
#include <fstream>
#include <string>

int forMax = 100;
int paramsCount = 0;
bool hasLastMacro = false;
bool outputToFile = true;
std::string prefix = "RH_";

std::string generateLastCall(bool lastMacro) {
  return lastMacro ? "l" : "a";
}

std::string generateFECode(bool lastMacro, int count) {
  std::string result = lastMacro ? "L" : std::string();
  if(paramsCount > 0) {
    result += std::string("P") + std::to_string(count);
  }
  return result;
}

std::string generatePostfix(bool lastMacro, int count) {
  std::string result = generateFECode(lastMacro, count);
  if(!result.empty()) result = "_" + result;
  return result;
}

std::string generateCommaLast(bool lastMacro) {
  return lastMacro ? ", l" : "";
}

std::string generateParams(int count, bool leadingComma, bool trailingComma) {
  std::string result;
  for(int i = 0; i < count; ++i) {
    if(i > 0) result += ", ";
    else if(leadingComma) result += ", ";
    result += "p" + std::to_string(i);
  }
  if(!result.empty() && trailingComma) result += ", ";
  return result;
}

// see http://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
std::string replaceAllCopy(std::string str,
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

std::string templateHead =
R"(// Hey Emacs, this is -*- coding: utf-8 mode: c++ -*-
#ifndef __{prefix}FOR_EACH{postfix}_h__
#define __{prefix}FOR_EACH{postfix}_h__

// This guard is required for MSVC compilers to do macro-expansion correctly.
// see http://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly
#define {prefix}_FE{feCode}_EXPAND(x) x

// Make FOREACH macro
#define {prefix}_FE{feCode}_1(a{commaLast}{commaParams}, x) {lastCall}({paramsComma}x))";

std::string templateFE_nString =
"#define {prefix}_FE{feCode}_{incN}(a{commaLast}{commaParams}, x, ...) {prefix}_FE{feCode}_EXPAND(a({paramsComma}x) {prefix}_FE{feCode}_{N}(a{commaLast}{commaParams}, __VA_ARGS__))";

std::string templateGetMacro =
"#define {prefix}_FE{feCode}_GET_MACRO({sequence}name,...) name";

std::string templateTail =
R"(
#define {prefix}FOR_EACH{postfix}(a{commaLast}{commaParams}, ...) \
{prefix}_FE{feCode}_EXPAND({prefix}_FE{feCode}_GET_MACRO(__VA_ARGS__,{sequence})(a{commaLast}{commaParams}, __VA_ARGS__))

#endif //__{prefix}FOR_EACH{postfix}_h__)";

int main(int argc, char *argv[]) {
  std::string postfix = generatePostfix(hasLastMacro, paramsCount);
  std::string feCode = generateFECode(hasLastMacro, paramsCount);
  std::string commaLast = generateCommaLast(hasLastMacro);
  std::string commaParams = generateParams(paramsCount, true, false);
  std::string paramsComma = generateParams(paramsCount, false, true);

  std::ostream* out = nullptr;
  std::fstream fileStream;
  if(outputToFile) {
    std::string fileName = prefix + "FOR_EACH" + postfix + ".h";
    fileStream.open(fileName, std::ios::out);
    out = &fileStream;
  }
  else out = &std::cout;

  std::string str = templateHead;
  str = replaceAllCopy(str, "{commaLast}", commaLast);
  str = replaceAllCopy(str, "{commaParams}", commaParams);
  str = replaceAllCopy(str, "{paramsComma}", paramsComma);
  str = replaceAllCopy(str, "{feCode}", feCode);
  str = replaceAllCopy(str, "{lastCall}", generateLastCall(hasLastMacro));
  str = replaceAllCopy(str, "{prefix}", prefix);
  str = replaceAllCopy(str, "{postfix}", postfix);
  *out << str << std::endl;

  str = "";
  for(int i=1; i<forMax; ++i) {
    str = templateFE_nString;
    std::string N = std::to_string(i);
    str = replaceAllCopy(str, "{N}", N);
    std::string incN = std::to_string(i+1);
    str = replaceAllCopy(str, "{incN}", incN);
    str = replaceAllCopy(str, "{commaLast}", commaLast);
    str = replaceAllCopy(str, "{commaParams}", commaParams);
    str = replaceAllCopy(str, "{paramsComma}", paramsComma);
    str = replaceAllCopy(str, "{feCode}", feCode);
    str = replaceAllCopy(str, "{prefix}", prefix);
    *out << str << std::endl;
  }
  *out << std::endl;

  str = "";
  for(int i=1; i<=forMax; ++i) {
    str += std::string("_") + std::to_string(i) + ",";
  }
  str = replaceAllCopy(templateGetMacro, "{sequence}", str);
  str = replaceAllCopy(str, "{feCode}", feCode);
  str = replaceAllCopy(str, "{prefix}", prefix);
  *out << str << std::endl;

  str = "";
  for(int i=forMax; i>0; --i) {
    str += std::string("{prefix}_FE") +
           feCode + "_" + std::to_string(i) + ",";
  }
  str = replaceAllCopy(templateTail, "{sequence}", str);
  str = replaceAllCopy(str, "{commaLast}", commaLast);
  str = replaceAllCopy(str, "{commaParams}", commaParams);
  str = replaceAllCopy(str, "{paramsComma}", paramsComma);
  str = replaceAllCopy(str, "{feCode}", feCode);
  str = replaceAllCopy(str, "{prefix}", prefix);
  str = replaceAllCopy(str, "{postfix}", postfix);
  *out << str << std::endl;

  return 0;
}

// Emacs, here are file hints,
// Local Variables:
// compile-command: "g++ RH_FOR_EACH_generator.cxx -o RH_FOR_EACH_generator -std=c++11 -Wall"
// End:
