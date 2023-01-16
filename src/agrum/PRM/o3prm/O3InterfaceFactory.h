/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Headers for the O3InterfaceFactory class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <memory>
#include <string>

#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/o3prm/O3NameSolver.h>

#ifndef GUM_PRM_O3PRM_O3INTERFACE_FACTORY_H
#  define GUM_PRM_O3PRM_O3INTERFACE_FACTORY_H

namespace gum {
  namespace prm {
    namespace o3prm {

      /**
       * @class O3InterfaceFactory
       * @headerfile O3InterfaceFactory.h <agrum/PRM/o3prm/O3InterfaceFactory.h>
       * @ingroup o3prm_group
       *
       * @brief Bulds gum::prm:PRMInterface from gum::prm::o3prm::O3Interface.
       *
       * @tparam GUM_SCALAR The scalar type used by the gum::prm::PRM.
       */
      template < typename GUM_SCALAR >
      class O3InterfaceFactory {
        public:
        O3InterfaceFactory(PRM< GUM_SCALAR >&          prm,
                           O3PRM&                      o3_prm,
                           O3NameSolver< GUM_SCALAR >& solver,
                           ErrorsContainer&            errors);
        O3InterfaceFactory(const O3InterfaceFactory< GUM_SCALAR >& src);
        O3InterfaceFactory(O3InterfaceFactory< GUM_SCALAR >&& src);
        ~O3InterfaceFactory();
        O3InterfaceFactory< GUM_SCALAR >& operator=(const O3InterfaceFactory< GUM_SCALAR >& src);
        O3InterfaceFactory< GUM_SCALAR >& operator=(O3InterfaceFactory< GUM_SCALAR >&& src);

        void buildInterfaces();

        void buildElements();

        private:
        PRM< GUM_SCALAR >*          _prm_;
        O3PRM*                      _o3_prm_;
        O3NameSolver< GUM_SCALAR >* _solver_;
        ErrorsContainer*            _errors_;

        HashTable< std::string, std::string >  _eltName_;
        HashTable< std::string, gum::NodeId >  _nameMap_;
        HashTable< std::string, O3Interface* > _interfaceMap_;
        HashTable< NodeId, O3Interface* >      _nodeMap_;
        DAG                                    _dag_;
        std::vector< O3Interface* >            _o3Interface_;

        bool _addInterface2Dag_();

        bool _addArcs2Dag_();

        void _setO3InterfaceCreationOrder_();

        bool _checkO3Interfaces_();

        bool _checkInterfaceElement_(O3Interface& i, O3InterfaceElement& elt);

        bool _checkOverloadLegality_(O3Interface& i, O3InterfaceElement& elt);

        bool _checkAttributeOverloadLegality_(O3Interface& i, O3InterfaceElement& elt);

        bool _checkReferenceOverloadLegality_(O3Interface& i, O3InterfaceElement& elt);

        bool _checkCyclicReference_(O3Interface& i, O3InterfaceElement& elt);
      };

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum

// always include the implementation of the templates
#  include <agrum/PRM/o3prm/O3InterfaceFactory_tpl.h>


#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::prm::o3prm::O3InterfaceFactory< double >;
#  endif


#endif   // GUM_PRM_O3PRM_O3INTERFACE_FACTORY_H
