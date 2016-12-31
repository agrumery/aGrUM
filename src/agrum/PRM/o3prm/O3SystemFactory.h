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
 * @brief Headers for the O3SystemFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <memory>
#include <string>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/o3prm/O3NameSolver.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>

#ifndef GUM_PRM_O3PRM_O3SYSTEM_FACTORY_H
#define GUM_PRM_O3PRM_O3SYSTEM_FACTORY_H

namespace gum {
  namespace prm {
    namespace o3prm {

      /**
       * @class O3SystemFactory
       * @headerfile O3SystemFactory.h <agrum/PRM/o3prm/O3SystemFactory.h>
       * @ingroup o3prm_group
       *
       * @brief Builds gum::prm::PRMSystem from gum::prm::o3prm::O3System.
       *
       * @tparam GUM_SCALAR The scalar type used by the gum::prm::PRM.
       */
      template <typename GUM_SCALAR>
      class O3SystemFactory {

        public:
        O3SystemFactory( PRM<GUM_SCALAR>&          prm,
                         O3PRM&                    o3_prm,
                         O3NameSolver<GUM_SCALAR>& solver,
                         ErrorsContainer&          errors );
        O3SystemFactory( const O3SystemFactory<GUM_SCALAR>& src );
        O3SystemFactory( O3SystemFactory<GUM_SCALAR>&& src );
        ~O3SystemFactory();
        O3SystemFactory<GUM_SCALAR>&
        operator=( const O3SystemFactory<GUM_SCALAR>& src );
        O3SystemFactory<GUM_SCALAR>&
        operator=( O3SystemFactory<GUM_SCALAR>&& src );

        void build();

        private:
        PRM<GUM_SCALAR>*          __prm;
        O3PRM*                    __o3_prm;
        O3NameSolver<GUM_SCALAR>* __solver;
        ErrorsContainer*          __errors;

        HashTable<std::string, O3Instance*> __nameMap;

        void __addInstances( PRMFactory<GUM_SCALAR>& factory, O3System& sys );

        void __addAssignments( PRMFactory<GUM_SCALAR>& factory, O3System& sys );

        void __addIncrements( PRMFactory<GUM_SCALAR>& factory, O3System& sys );

        bool __checkSystem( O3System& sys );

        bool __checkIncrements( O3System& sys );

        bool __checkParameters( const PRMClass<GUM_SCALAR>& type,
                                const O3Instance&           inst );

        bool __checkInstance( O3System& sys );

        bool __checkAssignments( O3System& sys );
      };

    }  // o3prm
  }    // prm
}  // gum

// always include the implementation of the templates
#include <agrum/PRM/o3prm/O3SystemFactory_tpl.h>


extern template class gum::prm::o3prm::O3SystemFactory<float>;
extern template class gum::prm::o3prm::O3SystemFactory<double>;


#endif  // GUM_PRM_O3PRM_O3SYSTEM_FACTORY_H
