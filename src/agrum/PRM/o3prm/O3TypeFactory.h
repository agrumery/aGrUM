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

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/o3prm/O3NameSolver.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/errors.h>
#include <agrum/core/errorsContainer.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>

#ifndef GUM_PRM_O3PRM_O3TYPE_FACTORY_H
#define GUM_PRM_O3PRM_O3TYPE_FACTORY_H

namespace gum {
  namespace prm {
    namespace o3prm {

      /**
       * @class O3TypeFactory
       * @headerfile O3TypeFactory.h <agrum/PRM/o3prm/O3TypeFactory.h>
       * @ingroup o3prm_group
       *
       * @brief Builds gum::prm::PRMType from gum::prm::o3prm::O3Type,
       * gum::prm::o3prm::O3IntType and gum::prm::o3prm::O3RealType.
       *
       * @tparam GUM_SCALAR The scalar type used by the gum::prm::PRM.
       */
      template <typename GUM_SCALAR>
      class O3TypeFactory {

        public:
        O3TypeFactory( PRM<GUM_SCALAR>&          prm,
                       O3PRM&                    o3_prm,
                       O3NameSolver<GUM_SCALAR>& solver,
                       ErrorsContainer&          errors );
        O3TypeFactory( const O3TypeFactory<GUM_SCALAR>& src );
        O3TypeFactory( O3TypeFactory<GUM_SCALAR>&& src );
        ~O3TypeFactory();
        O3TypeFactory& operator=( const O3TypeFactory<GUM_SCALAR>& src );
        O3TypeFactory& operator=( O3TypeFactory<GUM_SCALAR>&& src );

        void build();

        private:
        PRM<GUM_SCALAR>*          __prm;
        O3PRM*                    __o3_prm;
        O3NameSolver<GUM_SCALAR>* __solver;
        ErrorsContainer*          __errors;

        bool __build;

        HashTable<std::string, O3Type*>     __superMap;
        HashTable<std::string, gum::NodeId> __nameMap;
        HashTable<std::string, O3Type*>     __typeMap;
        HashTable<NodeId, O3Type*>          __nodeMap;
        DAG                      __dag;
        std::vector<O3Type*>     __o3Types;
        std::vector<O3IntType*>  __o3IntTypes;
        std::vector<O3RealType*> __o3RealTypes;

        void __buildTypes();
        void __buildIntTypes();
        void __buildRealTypes();

        bool __isPrimitiveType( O3Type& type );

        bool __checkO3Types();
        bool __addTypes2Dag();
        bool __addArcs2Dag();
        bool __checkLabels( O3Type& type );

        void __setO3TypeCreationOrder();

        bool __checkO3IntTypes();
        bool __checkO3RealTypes();
      };

    }  // o3prm
  }    // prm
}  // gum

// always include the implementation of the templates
#include <agrum/PRM/o3prm/O3TypeFactory_tpl.h>


extern template class gum::prm::o3prm::O3TypeFactory<float>;
extern template class gum::prm::o3prm::O3TypeFactory<double>;


#endif  // GUM_PRM_O3PRM_O3TYPE_FACTORY_H
