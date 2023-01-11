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
 * @brief Utilities for manipulating strings.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */
#ifndef GUM_UTILS_STRING_H
#define GUM_UTILS_STRING_H

#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

#include <agrum/agrum.h>

#ifndef HAVE_MKSTEMP   // mainly windows
#  include <io.h>
#endif

namespace gum {
  /// @ingroup utilities_group
  /// @{

  /**
   * @brief Returns a path to a unique file name.
   * @return Returns a path to a unique file name.
   */
  std::string getUniqueFileName();

  /**
   * @brief Returns the lowercase version of str.
   * @return Returns the lowercase version of str.
   */
  std::string toLower(std::string str);

  /**
   * @brief Returns true if value ends with ending.
   * @return Returns true if value ends with ending.
   */
  bool endsWith(std::string const& value, std::string const& ending);

  /**
   * @brief Split str using the delimiter
   * @param orig searched string
   * @param delimiter string
   * @return Vector of splitted strings
   */
  std::vector< std::string > split(const std::string& orig, const std::string& delimiter);

  /**
   * @brief Replace val by new_val in s.
   * @param s A string.
   * @param val The value to replace in s.
   * @param new_val The new value to replace val in s.
   * @return A new string with val replaced by new_val.
   */
  std::string replace(const std::string& s, const std::string& val, const std::string& new_val);

  /**
   * @brief return true is a string contains an integer value
   * @param s A string
   * @return true if s exactly contains an int (s can start with "-" or "+")
   */
  bool isInteger(const std::string& val);

  /**
   * @brief return true is a string contains an integer value
   * @param s A string
   * @return true if s exactly contains an int (s can start with "-" or "+")
   */
  bool isIntegerWithResult(const std::string& val, int* res);

  /**
   * @brief return true is a string contains a numerical (double) value
   * @param s A string
   * @return true if s exactly contains a double
   */
  bool isNumerical(const std::string& val);

  /**
   * @brief return true is a string contains a numerical (double) value
   * @param s A string     *
   * @return true if s exactly contains a double
   */
  bool isNumericalWithResult(const std::string& val, double* res);


  /**
   * @brief trim from start (in place)
   * @param s A string
   */
  void ltrim(std::string& s);

  /**
   * @brief trim from end (in place)
   * @param s A string
   */
  void rtrim(std::string& s);

  /**
   * @brief trim from both ends (in place)
   * @param s A string
   */
  void trim(std::string& s);

  /**
   * @brief remove all newlines in a string
   * @param s A string
   * @return the new string with no newline
   */
  std::string remove_newline(const std::string& s);

  template < typename T >
  inline std::string compact_tostr(T value) {
    std::ostringstream s;
    s.precision(std::numeric_limits< T >::digits10);
    s << value;
    return s.str();
  }
  /// @}
} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/tools/core/utils_string_inl.h>
#endif   // GUM_NO_INLINE

#endif /* GUM_UTILS_STRING_H */
