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

#include <agrum/core/set.h>
#include <agrum/core/hashTable.h>
#include <agrum/PRM/PRM.h>
#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/newo3prm/o3prm.h>
#include <agrum/PRM/newo3prm/utils.h>

#ifndef GUM_PRM_O3PRM_O3SYSTEM_FACTORY_H
#define GUM_PRM_O3PRM_O3SYSTEM_FACTORY_H

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      class O3SystemFactory {

        public:
        O3SystemFactory();
        O3SystemFactory( const O3SystemFactory<GUM_SCALAR>& src );
        O3SystemFactory( O3SystemFactory<GUM_SCALAR>&& src );
        ~O3SystemFactory();
        O3SystemFactory<GUM_SCALAR>&
        operator=( const O3SystemFactory<GUM_SCALAR>& src );
        O3SystemFactory<GUM_SCALAR>&
        operator=( O3SystemFactory<GUM_SCALAR>&& src );

        void build( PRM<GUM_SCALAR>& prm, O3PRM& o3_prm, ErrorsContainer& errors );

        private:
        HashTable<std::string, O3Instance*> __nameMap;

        bool
        __checkSystem( PRM<GUM_SCALAR>& prm, O3System& sys, ErrorsContainer& errors );

        bool __checkIncrements( PRM<GUM_SCALAR>& prm,
                                O3System& sys,
                                ErrorsContainer& errors );

        bool __checkParameters( const Class<GUM_SCALAR>& type,
                                const O3Instance& inst,
                                ErrorsContainer& errors );
        
        bool __checkInstance( PRM<GUM_SCALAR>& prm,
                              O3System& sys,
                              ErrorsContainer& errors );

        bool __checkAssignments( PRM<GUM_SCALAR>& prm,
                                 O3System& sys,
                                 ErrorsContainer& errors );
      };

    }  // o3prm
  }    // prm
}  // gum

#include <agrum/PRM/newo3prm/O3SystemFactory.tcc>

extern template class gum::prm::o3prm::O3SystemFactory<float>;
extern template class gum::prm::o3prm::O3SystemFactory<double>;

#endif  // GUM_PRM_O3PRM_O3SYSTEM_FACTORY_H

