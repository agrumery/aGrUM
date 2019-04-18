
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 */
#ifndef GUM_UTILS_STRING_H
#define GUM_UTILS_STRING_H

#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

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
  std::vector< std::string > split(const std::string& orig,
                                   const std::string& delimiter);

  /**
   * @brief Replace val by new_val in s.
   * @param s A string.
   * @param val The value to replace in s.
   * @param new_val The new value to replace val in s.
   * @return A new string with val replaced by new_val.
   */
  std::string replace(const std::string& s,
                      const std::string& val,
                      const std::string& new_val);

  /// @}
} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/core/utils_string_inl.h>
#endif   // GUM_NO_INLINE

#endif /* GUM_UTILS_STRING_H */
