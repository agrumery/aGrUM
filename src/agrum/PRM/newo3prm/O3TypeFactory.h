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
 * @brief Headers for the O3TypeFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <memory>
#include <string>

#include <agrum/core/set.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/errorsContainer.h>
#include <agrum/PRM/PRM.h>
#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/newo3prm/o3prm.h>
#include <agrum/PRM/newo3prm/utils.h>

#ifndef GUM_PRM_O3PRM_O3TYPE_FACTORY_H
#define GUM_PRM_O3PRM_O3TYPE_FACTORY_H

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      class O3TypeFactory {

        public:
        O3TypeFactory();
        O3TypeFactory( const O3TypeFactory<GUM_SCALAR>& src );
        O3TypeFactory( O3TypeFactory<GUM_SCALAR>&& src );
        ~O3TypeFactory();
        O3TypeFactory& operator=( const O3TypeFactory<GUM_SCALAR>& src );
        O3TypeFactory& operator=( O3TypeFactory<GUM_SCALAR>&& src );

        void build( PRM<GUM_SCALAR>& prm,
                    const O3PRM& my_o3prm,
                    ErrorsContainer& errors );

        private:
        HashTable<std::string, gum::NodeId> __nameMap;
        HashTable<std::string, const O3Type*> __typeMap;
        HashTable<NodeId, const O3Type*> __nodeMap;
        DAG __dag;
        std::vector<const O3Type*> __o3Types;
        std::vector<const O3IntType*> __o3IntTypes;

        void __initialize();
        void __setO3TypeCreationOrder();
        bool __isPrimitiveType( const O3Type& type );
        bool __addTypes2Dag( PRM<GUM_SCALAR>& prm,
                             const O3PRM& tmp_prm,
                             ErrorsContainer& errors );
        bool __addArcs2Dag( const O3PRM& prm, ErrorsContainer& errors );
        bool __checkO3Types( PRM<GUM_SCALAR>& prm,
                             const O3PRM& tmp_prm,
                             ErrorsContainer& errors );
        bool __checkO3IntTypes( const O3PRM& prm, ErrorsContainer& errors );
      };

    }  // o3prm
  }    // prm
}  // gum

#include <agrum/PRM/newo3prm/O3TypeFactory.tcc>

extern template class gum::prm::o3prm::O3TypeFactory<float>;
extern template class gum::prm::o3prm::O3TypeFactory<double>;

#endif  // GUM_PRM_O3PRM_O3TYPE_FACTORY_H

