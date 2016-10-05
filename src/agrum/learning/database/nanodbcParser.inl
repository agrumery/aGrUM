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
 * @brief Class for parsing SQL results using Nanodbc.
 *
 * @author Lionel TORTI, Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifdef _ODBC

#include <agrum/learning/database/nanodbcParser.h>

namespace gum {

  namespace learning {

    INLINE
    const Size NanodbcParser::noLine() const {
      try {
        return __result.position();
      } catch ( std::runtime_error& e ) {
        return 0;
      }
    }

    INLINE
    const std::vector<std::string>& NanodbcParser::current() const {
      if ( __data.size() ) {
        return __data;
      }

      GUM_ERROR( NullElement, "No parsed data" );
    }

  }  // namespace learning

}  // namespace gum

#endif  // _ODBC
