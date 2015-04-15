#ifndef CAST_UNICODE_H
#define CAST_UNICODE_H

#include <locale>
#include <iostream>
#include <string>
#include <sstream>

inline std::wstring widen(const std::string &str) {
  std::wostringstream wstm;
  const std::ctype<wchar_t> &ctfacet =
      std::use_facet<std::ctype<wchar_t>>(wstm.getloc());

  for (size_t i = 0; i < str.size(); ++i)
    wstm << ctfacet.widen(str[i]);

  return wstm.str();
}

inline std::string narrow(const std::wstring &str) {
  std::ostringstream stm;
  const std::ctype<char> &ctfacet = std::use_facet<std::ctype<char>>(stm.getloc());

  for (std::size_t i = 0; i < str.size(); ++i)
    stm << ctfacet.narrow(str[i], 0);

  return stm.str();
}

#endif // CAST_UNICODE_H
