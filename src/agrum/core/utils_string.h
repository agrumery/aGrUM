/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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
#include <string>
#include <vector>

#include <agrum/config.h>

#ifndef HAVE_MKSTEMP  // mainly windows
#include <io.h>
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
  std::string toLower( std::string str );

  /**
   * @brief Returns true if value ends with ending.
   * @return Returns true if value ends with ending.
   */
  bool endsWith( std::string const& value, std::string const& ending );

  /**
   * @brief Split str using the regexp delimiter
   * @param orig searched string
   * @param delimiter  regexp for delimiters
   * @return Vector of splitted strings
   */
  std::vector<std::string> split( const std::string& orig,
                                  const std::string& delimiter );
  /// @}
} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/core/utils_string_inl.h>
#endif  // GUM_NO_INLINE

#endif /* GUM_UTILS_STRING_H */
