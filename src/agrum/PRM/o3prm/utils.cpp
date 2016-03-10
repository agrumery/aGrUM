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
 * @brief Implementation for utilities functions for the O3PRM language.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/utils.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      std::vector<NodeId> topological_order( const gum::DAG& src ) {
        auto dag = src;
        auto roots = std::vector<NodeId>();
        auto order = std::vector<NodeId>();
        for ( const auto node : dag.nodes() ) {
          if ( dag.parents( node ).empty() ) {
            roots.push_back( node );
          }
        }
        while ( roots.size() ) {
          order.push_back( roots.back() );
          roots.pop_back();
          while ( dag.children( order.back() ).size() ) {
            auto child = *( dag.children( order.back() ).begin() );
            dag.eraseArc( Arc( order.back(), child ) );
            if ( dag.parents( child ).empty() ) {
              roots.push_back( child );
            }
          }
        }
        return std::move( order );
      }

    }
  }
}
