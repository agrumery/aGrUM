/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Contains usefull methods for random stuff.
 *
 * @author Vincent RENAUDINEAU and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <algorithm>

// to ease IDE parser
#include <agrum/tools/core/utils_string.h>
namespace gum {

  INLINE
  std::string toLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
  }

  INLINE
  void ltrim(std::string& s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
  }

  INLINE
  void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); })
               .base(),
            s.end());
  }

  INLINE
  void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
  }

  INLINE
  std::string remove_newline(const std::string& s) {
    std::string res{s};
    std::erase(res, '\n');
    std::erase(res, '\r');
    return res;
  }

  INLINE
  bool isInteger(const std::string& val) { return isIntegerWithResult(val, nullptr); }

  INLINE
  bool isNumerical(const std::string& val) { return isNumericalWithResult(val, nullptr); }
} /* namespace gum */
