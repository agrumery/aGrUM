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
 * @brief Headers for the O3NameSolver class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <algorithm>
#include <string>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/errors.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/utils_string.h>

#ifndef GUM_PRM_O3PRM_O3NAMESOLVER_H
#define GUM_PRM_O3PRM_O3NAMESOLVER_H

namespace gum {
  namespace prm {
    namespace o3prm {

      /**
       * @class O3NameSolver
       * @headerfile O3NameSolver.h <agrum/PRM/o3prm/O3NameSolver.h>
       * @ingroup o3prm_group
       *
       * @brief Resolves names for the different O3PRM factories.
       *
       * @tparam GUM_SCALAR The scalar type used by the gum::prm::PRM.
       */
      template <typename GUM_SCALAR>
      class O3NameSolver {

        public:
        O3NameSolver( PRM<GUM_SCALAR>& prm,
                      O3PRM&           o3_prm,
                      ErrorsContainer& errors );

        O3NameSolver( const O3NameSolver<GUM_SCALAR>& src );

        O3NameSolver( O3NameSolver<GUM_SCALAR>&& src );

        ~O3NameSolver();

        O3NameSolver<GUM_SCALAR>& operator=( const O3NameSolver<GUM_SCALAR>& src );

        O3NameSolver<GUM_SCALAR>& operator=( O3NameSolver<GUM_SCALAR>&& src );

        bool resolveClassElement( O3Label& name );

        bool resolveType( O3Label& name );

        bool resolveInterface( O3Label& name );

        bool resolveClass( O3Label& name );

        bool resolveSlotType( O3Label& name );

        private:
        PRM<GUM_SCALAR>* __prm;
        O3PRM*           __o3_prm;
        ErrorsContainer* __errors;

        using NameMap = HashTable<std::string, std::string>;

        NameMap __typeName;
        NameMap __eltName;
        NameMap __refName;
        NameMap __interfaceName;
        NameMap __className;
      };

    }  // o3prm
  }    // prm
}  // gum


extern template class gum::prm::o3prm::O3NameSolver<float>;
extern template class gum::prm::o3prm::O3NameSolver<double>;


// always include the implementation of the templates
#include <agrum/PRM/o3prm/O3NameSolver_tpl.h>

#endif  // GUM_PRM_O3PRM_O3NAMESOLVER_H
