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
/** @file
 * @brief utilities for manipulating strings
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 */
#ifndef GUM_UTILS_STRING_H
#define GUM_UTILS_STRING_H

#include <vector>
#include <string>

#include <agrum/config.h>


namespace gum {


  /// splits a CSV-like string into a vector of fields
  /** This function splits a given string representing a single line of a CSV
   * file into a set of strings, each one corresponding to a field of the line
   * (fields are separated by character field_separator). In addition, some fields
   * may be enclosed within delimiters (usually ") and characters may be
   * escaped using the escape_char character.
   * @param str_to_split the string in CVS format to split
   * @param field_separator the character to use to separate fileds in the CSV
   * @param field_delimiter the character to use to encapsulate, for instance,
   * strings within a field of the CVS
   * @param escape_char the character to use to escape the next character in, say,
   * a string field of the CSV
   * @throws IOError is thrown if, in a field, a delimiter is opened
   * but not closed. */

  std::vector<std::string> SplitCSVLine( const std::string& str_to_split,
                                         char field_separator = ';',
                                         char field_delimiter = '"',
                                         char escape_char = '\\' );


} /* namespace gum */


#endif /* GUM_UTILS_STRING_H */
