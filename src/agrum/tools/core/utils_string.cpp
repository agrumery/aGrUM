/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Utilities for manipulating strings.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */
#include <iterator>
#include <regex>

#include <agrum/tools/core/utils_string.h>

namespace gum {

  std::string getUniqueFileName() {
#ifdef HAVE_MKSTEMP
    char tmpFileName_[] = "fileXXXXXX";
    int  fd             = mkstemp(tmpFileName_);
    close(fd);
#else   // mainly Windows
    char tmpFileName_[] = "fileXXXXXX";
    _mktemp_s(tmpFileName_, strlen(tmpFileName_));
#endif

    return std::string(tmpFileName_);
  }

  bool endsWith(std::string const& value, std::string const& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
  }

  std::vector< std::string > split(const std::string& str,
                                   const std::string& delim) {
    std::vector< std::string > tokens;
    size_t                     prev = 0, pos = 0;
    do {
      pos = str.find(delim, prev);
      if (pos == std::string::npos) pos = str.length();
      std::string token = str.substr(prev, pos - prev);
      if (!token.empty()) tokens.push_back(token);
      prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
  }

  /** not usable for gcc 4.8
std::vector<std::string> split( const std::string& orig,
                                const std::string& delimiter ) {

  std::regex rgx( delimiter );

  std::sregex_token_iterator first{begin( orig ), end( orig ), rgx, -1}, last;

  return {first, last};
} */

  std::string replace(const std::string& s,
                      const std::string& val,
                      const std::string& new_val) {
    auto retVal = s;
    auto pos    = retVal.find(val);
    while (pos != std::string::npos) {
      std::stringstream sBuff;
      sBuff << s.substr(0, pos) << new_val
            << s.substr(pos + val.size(), std::string::npos);
      retVal = sBuff.str();
      pos    = retVal.find(val);
    }
    return retVal;
  }

} /* namespace gum */

#ifdef GUM_NO_INLINE
#  include <agrum/tools/core/utils_string_inl.h>
#endif   // GUM_NO_INLINE
