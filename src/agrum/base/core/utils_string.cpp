/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Utilities for manipulating strings.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <chrono>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <charconv>

#include <agrum/base/core/utils_random.h>
#include <agrum/base/core/utils_string.h>

namespace gum {

  // get a unique file name using std::filesystem

  std::string getUniqueFileName() {
    // Get the temporary directory path
    std::filesystem::path temp_dir = std::filesystem::temp_directory_path();

    // Generate a unique file name using a timestamp and a random number
    auto now = std::chrono::system_clock::now();
    auto timestamp
        = std::chrono::duration_cast< std::chrono::milliseconds >(now.time_since_epoch()).count();

    auto dis = gum::randomValue(1000);

    std::string filename
        = "tempfile_" + std::to_string(timestamp) + "_" + std::to_string(dis) + ".tmp";

    // Combine the directory path and the file name
    std::filesystem::path temp_file_path = temp_dir / filename;

    return temp_file_path.string();
  }

  bool endsWith(std::string const& value, std::string const& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
  }

  std::vector< std::string > split(std::string_view str, std::string_view delim) {
    std::vector< std::string > tokens;
    size_t                     prev = 0, pos = 0;
    const auto                 lenstr   = str.length();
    const auto                 lendelim = delim.length();
    do {
      pos = str.find(delim, prev);
      if (pos == std::string::npos) pos = str.length();
      std::string token(str.substr(prev, pos - prev));
      if (!token.empty()) tokens.push_back(token);
      prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
  }

  std::string replace(const std::string_view& s,
                      const std::string_view& val,
                      const std::string_view& new_val) {
    const auto lens   = s.length();
    const auto lenval = val.length();
    if (lens < lenval || lenval == 0) return std::string{s};

    std::string res;
    res.reserve(lens);

  std::sregex_token_iterator first{begin( orig ), end( orig ), rgx, -1}, last;

  return {first, last};
} */

  std::string replace(std::string_view s, std::string_view val, std::string_view new_val) {
    std::string retVal(s);
    auto        pos = retVal.find(val);
    while (pos != std::string::npos) {
      std::stringstream sBuff;
      sBuff << s.substr(0, pos) << new_val << s.substr(pos + val.size(), std::string::npos);
      retVal = sBuff.str();
      pos    = retVal.find(val);
    }
    res += s.substr(start_pos);
    return res;
  }

  bool isIntegerWithResult(std::string_view val, int* res) {
    if (val.empty()) return false;
    std::size_t pos = 0;
    if ((val[0] == '+') || (val[0] == '-')) { pos = 1; }

    if (val.find_first_not_of("0123456789", pos) != std::string_view::npos) return false;

    if (res != nullptr) {
      std::string s(val);
      const char* p = (val[0] == '+') ? 1 + s.c_str() : s.c_str();
      *res          = std::stoi(p);
    }
    return has_digit;
  }

  bool isNumerical(const std::string_view& val) {
    if (val.empty()) return false;
    if (val == "inf" || val == "+inf" || val == "-inf") return true;

    int  dot_count  = 0;
    int  first_char = 0;
    bool has_digit  = false;
    for (unsigned char c: val) {
      if (first_char == 0) {
        first_char = 1;
        if (c == '+' || c == '-') { continue; }
      }
      if (c == '.') {
        dot_count++;
        if (dot_count > 1) return false;
        continue;
      }
      if (c=='e' || c=='E') {
        if (!has_digit) return false; // need at least one digit before e
        dot_count=1; // no right to dot anymore
        first_char=0; // allow + or - after e
        has_digit=false; // need at least one digit after e
        continue;
      }
      if (!std::isdigit(c)) return false;
      else has_digit = true;
    }
    return has_digit;
  }

  bool isIntegerWithResult(const std::string_view& val, int* res) {
    if (!isInteger(val)) return false;
    size_t pos = 0;
    if (val[0] == '+') pos = 1;
    std::from_chars(val.data() + pos, val.data() + val.size(), *res);
    return true;
  }

  bool isNumericalWithResult(std::string_view val, double* res) {
    std::string s(val);
    char*       endptr = nullptr;
    const char* str    = s.c_str();
    double      d      = std::strtod(str, &endptr);

    if (*endptr != '\0' || endptr == str) return false;
    if (res != nullptr) *res = d;

    return true;
  }


} /* namespace gum */

#ifdef GUM_NO_INLINE
#  include <agrum/base/core/utils_string_inl.h>
#endif   // GUM_NO_INLINE
