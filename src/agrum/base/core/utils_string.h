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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */
#ifndef GUM_UTILS_STRING_H
#define GUM_UTILS_STRING_H

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <string_view>

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
  std::string toLower(std::string_view str);

  /**
   * true if needle in s
   *
   * @param s
   * @param needle   * @return
   */
  bool contains(std::string_view s, std::string_view needle);

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
  std::vector< std::string > split(std::string_view orig, std::string_view delimiter);

  /**
   * @brief Replace val by new_val in s.
   * @param s A string.
   * @param val The value to replace in s.
   * @param new_val The new value to replace val in s.
   * @return A new string with val replaced by new_val.
   */
  std::string replace(std::string_view s, std::string_view val, std::string_view new_val);

  /**
   * @brief return true is a string contains an integer value
   * @param s A string
   * @return true if s exactly contains an int (s can start with "-" or "+")
   */
  bool isInteger(std::string_view val);

  /**
   * @brief return true is a string contains an integer value
   * @param s A string
   * @return true if s exactly contains an int (s can start with "-" or "+")
   */
  bool isIntegerWithResult(std::string_view val, int* res);

  /**
   * @brief return true is a string contains a numerical (double) value
   * @param s A string
   * @return true if s exactly contains a double
   */
  bool isNumerical(std::string_view val);

  /**
   * @brief return true is a string contains a numerical (double) value
   * @param s A string     *
   * @return true if s exactly contains a double
   */
  bool isNumericalWithResult(std::string_view val, double* res);


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
   * @brief trim from both ends (copying)
   * @param s A string
   * @return the new string trimmed from start
   */
  std::string trim_copy(std::string_view s);

  /**
   * @brief remove all newlines in a string
   * @param s A string
   * @return the new string with no newline
   */
  std::string remove_newline(std::string_view s);

  template < typename T >
  inline std::string compact_tostr(T value) {
    std::ostringstream s;
    s.precision(7);
    s << value;
    return s.str();
  }

  /// @}
} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/base/core/utils_string_inl.h>
#endif   // GUM_NO_INLINE

#endif   /* GUM_UTILS_STRING_H */
