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
 * @brief Headers for utilities functions for the O3PRM language.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#ifndef GUM_PRM_O3PRM_O3PRM_H
#define GUM_PRM_O3PRM_O3PRM_H

#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/core/sequence.h>
#include <agrum/graphs/DAG.h>
#include <agrum/core/errorsContainer.h>
#include <agrum/PRM/PRM.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      std::vector<NodeId> topological_order( const gum::DAG& src );

      std::string clean( const std::string& text );

      std::string print( const ParseError& err );

      template <typename GUM_SCALAR>
      bool name_used( const PRM<GUM_SCALAR>& prm, const std::string& name );

      std::string read_stream( std::istream& input );

      bool ends_with( std::string const& value, std::string const& ending );
    }
  }
}

#include <agrum/PRM/newo3prm/utils.tcc>

#endif  // GUM_PRM_O3PRM_O3PRM_H
