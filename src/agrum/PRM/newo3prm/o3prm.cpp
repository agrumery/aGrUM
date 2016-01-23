/**************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Implementation for parsing function for the O3PRM language.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/newo3prm/o3prm.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      O3Type::O3Type( std::string name, std::vector<std::string>& labels )
          : __name( name )
          , __labels( labels ) {
        GUM_CONSTRUCTOR( O3Type );
      }

      O3Type::O3Type( const O3Type& src )
          : __name( src.__name )
          , __labels( src.__labels ) {
        GUM_CONS_CPY( O3Type );
      }

      O3Type::O3Type( O3Type&& src )
          : __name( std::move( src.__name ) )
          , __labels( std::move( src.__labels ) ) {
        GUM_CONS_MOV( O3Type );
      }

      O3Type::~O3Type() { GUM_DESTRUCTOR( O3Type ); }

      const std::string& O3Type::name() const { return __name; }
      const std::vector<std::string>& O3Type::labels() const { return __labels; }

      O3PRM::O3PRM() { GUM_CONSTRUCTOR( O3PRM ); }

      O3PRM::O3PRM( const O3PRM& src )
          : __types( src.__types ) {
        GUM_CONS_CPY( O3PRM );
      }

      O3PRM::O3PRM( O3PRM&& src )
          : __types( std::move( src.__types ) ) {
        GUM_CONS_MOV( O3PRM );
      }

      O3PRM::~O3PRM() { GUM_DESTRUCTOR( O3PRM ); }

      std::vector<O3Type>& O3PRM::types() { return __types; }
      const std::vector<O3Type>& O3PRM::types() const { return __types; }

    }  // o3prm
  }    // prm
}  // gum
