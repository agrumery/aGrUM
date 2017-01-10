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
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <regex>
#include <iterator>

#include <agrum/core/utils_string.h>

namespace gum {

  std::string getUniqueFileName() {
#ifdef HAVE_MKSTEMP
    char _tmpFileName[] = "fileXXXXXX";
    int  fd = mkstemp( _tmpFileName );
    close( fd );
#else  // mainly Windows
    char _tmpFileName[] = "fileXXXXXX";
    _mktemp_s( _tmpFileName, strlen( _tmpFileName ) );
#endif

    return std::string( _tmpFileName );
  }

  bool endsWith( std::string const& value, std::string const& ending ) {
    if ( ending.size() > value.size() ) return false;
    return std::equal( ending.rbegin(), ending.rend(), value.rbegin() );
  }

  std::vector<std::string> split( const std::string& orig,
                                  const std::string& delimiter ) {

    std::regex rgx( delimiter );

    std::sregex_token_iterator first{begin( orig ), end( orig ), rgx, -1}, last;

    return {first, last};
  }
} /* namespace gum */

#ifdef GUM_NO_INLINE
#include <agrum/core/utils_string_inl.h>
#endif  // GUM_NO_INLINE
