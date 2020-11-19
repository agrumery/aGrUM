/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Headers for the O3TypeFactory class.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 * @author Lionel TORTI
 */

#include <memory>
#include <string>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/o3prm/O3NameSolver.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/errors.h>
#include <agrum/tools/core/errorsContainer.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/core/set.h>

#ifndef GUM_PRM_O3PRM_O3TYPE_FACTORY_H
#  define GUM_PRM_O3PRM_O3TYPE_FACTORY_H

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
      template < typename GUM_SCALAR >
      class O3TypeFactory {
        public:
        O3TypeFactory(PRM< GUM_SCALAR >&          prm,
                      O3PRM&                      o3_prm,
                      O3NameSolver< GUM_SCALAR >& solver,
                      ErrorsContainer&            errors);
        O3TypeFactory(const O3TypeFactory< GUM_SCALAR >& src);
        O3TypeFactory(O3TypeFactory< GUM_SCALAR >&& src);
        ~O3TypeFactory();
        O3TypeFactory& operator=(const O3TypeFactory< GUM_SCALAR >& src);
        O3TypeFactory& operator=(O3TypeFactory< GUM_SCALAR >&& src);

        void build();

        private:
        PRM< GUM_SCALAR >*          prm__;
        O3PRM*                      o3_prm__;
        O3NameSolver< GUM_SCALAR >* solver__;
        ErrorsContainer*            errors__;

        bool build__;

        HashTable< std::string, O3Type* >     superMap__;
        HashTable< std::string, gum::NodeId > nameMap__;
        HashTable< std::string, O3Type* >     typeMap__;
        HashTable< NodeId, O3Type* >          nodeMap__;
        DAG                                   dag__;
        std::vector< O3Type* >                o3Types__;
        std::vector< O3IntType* >             o3IntTypes__;
        std::vector< O3RealType* >            o3RealTypes__;

        void buildTypes__();
        void buildIntTypes__();
        void buildRealTypes__();

        bool isPrimitiveType__(O3Type& type);

        bool checkO3Types__();
        void checkDepreactedO3Types__();
        bool addTypes2Dag__();
        bool addArcs2Dag__();
        bool checkLabels__(O3Type& type);

        void setO3TypeCreationOrder__();

        bool checkO3IntTypes__();
        bool checkO3RealTypes__();
      };

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum

// always include the implementation of the templates
#  include <agrum/PRM/o3prm/O3TypeFactory_tpl.h>


#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::prm::o3prm::O3TypeFactory< double >;
#  endif


#endif   // GUM_PRM_O3PRM_O3TYPE_FACTORY_H
