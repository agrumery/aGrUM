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

#include <agrum/core/utilsString.h>


namespace gum {


  // ==============================================================================
  // splits a CSV-like string into a vector of fields. Missing value are
  // labelized by "?"
  // ==============================================================================
  std::vector<std::string> SplitCSVLine( const std::string& str_to_split,
                                         char field_separator,
                                         char field_delimiter,
                                         char escape_char ) {
    std::vector<std::string> result;
    bool delimited = false;
    bool new_string_to_add = false;
    bool last_is_delimiter = false;
    std::string string_to_add;

    string_to_add.reserve( str_to_split.size() );

    // parse the string
    for ( unsigned int i = 0; i < str_to_split.size(); ++i ) {
      last_is_delimiter=false;
      
      // check wether we are reading an escape character
      if ( str_to_split[i] == escape_char ) {
        if ( i+1 < str_to_split.size() ) {
          string_to_add.append( 1, str_to_split[++i] );
          new_string_to_add = true;
        }

        continue;
      }

      // check wether we are reading a delimiter character
      if ( str_to_split[i] == field_delimiter ) {
        delimited = delimited ? false : true;
        continue;
      }

      // check if we do have a separator
      if (( str_to_split[i] == field_separator ) && !delimited ) {
        last_is_delimiter=true;

        if ( new_string_to_add ) {
          result.push_back( string_to_add );
          string_to_add.clear();
          new_string_to_add = false;
        }
        else {
          result.push_back( "?" );
        }

        continue;
      }

      string_to_add.append( 1, str_to_split[i] );
      new_string_to_add = true;
    }

    // if we opened a delimitered field but this field was not closed
    // by a delimiter, raise an axception
    if ( delimited )
      GUM_ERROR( IOError, "delimited field opened but not closed" );

    // check if there remains a string to push into the result vector
    if ( new_string_to_add )
      result.push_back( string_to_add );
    if (last_is_delimiter)
      result.push_back( "?");

    return result;
  }
  
} /* namespace gum */

