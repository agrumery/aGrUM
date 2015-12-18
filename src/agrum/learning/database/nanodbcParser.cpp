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

#include <agrum/learning/database/nanodbcParser.h>

#include <stdexcept>
#include <agrum/core/exceptions.h>

#ifdef GUM_NO_INLINE
#include <agrum/learning/database/nanodbcParser.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    NanodbcParser::NanodbcParser( nanodbc::result& result )
        : __result( result )
        , __data( result.columns() ) {
      GUM_CONSTRUCTOR( NanodbcParser );
    }

    NanodbcParser::~NanodbcParser() { GUM_DESTRUCTOR( NanodbcParser ); }

    bool NanodbcParser::next() {
      try {
        if ( __result.next() ) {
          for ( auto i = 0; i < __result.columns(); ++i ) {
            __data[i] = __result.get<std::string>( i );
          }
          return true;
        }
      } catch ( std::runtime_error& e ) {
        GUM_ERROR( DatabaseError, std::string( e.what() ) );
      }
      __data.clear();
      return false;
    }

  }  // namespace learning

}  // namespace gum
