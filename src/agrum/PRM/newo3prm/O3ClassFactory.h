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
 * @brief Headers for the O3ClassFactory class.
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

#ifndef GUM_PRM_O3PRM_O3CLASS_FACTORY_H
#define GUM_PRM_O3PRM_O3CLASS_FACTORY_H

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      class O3ClassFactory {

        public:
        O3ClassFactory();
        O3ClassFactory( const O3ClassFactory<GUM_SCALAR>& src );
        O3ClassFactory( O3ClassFactory<GUM_SCALAR>&& src );
        ~O3ClassFactory();
        O3ClassFactory<GUM_SCALAR>&
        operator=( const O3ClassFactory<GUM_SCALAR>& src );
        O3ClassFactory<GUM_SCALAR>&
        operator=( O3ClassFactory<GUM_SCALAR>&& src );

        void buildClasses( PRM<GUM_SCALAR>& prm,
                           O3PRM& o3_prm,
                           ErrorsContainer& errors );

        void buildImplementations( PRM<GUM_SCALAR>& prm,
                                   O3PRM& o3_prm,
                                   ErrorsContainer& errors );

        void buildParameters( PRM<GUM_SCALAR>& prm,
                              O3PRM& o3_prm,
                              ErrorsContainer& errors );

        void buildReferenceSlots( PRM<GUM_SCALAR>& prm,
                                  O3PRM& o3_prm,
                                  ErrorsContainer& errors );

        void declareAttributes( PRM<GUM_SCALAR>& prm,
                                O3PRM& o3_prm,
                                ErrorsContainer& errors );

        void completeAttributes( PRM<GUM_SCALAR>& prm,
                                 O3PRM& o3_prm,
                                 ErrorsContainer& errors );

        void buildAggregates( PRM<GUM_SCALAR>& prm,
                              O3PRM& o3_prm,
                              ErrorsContainer& errors );

        private:
        HashTable<std::string, std::string> __typeName;
        HashTable<std::string, std::string> __refName;
        HashTable<std::string, std::string> __interfaceName;
        HashTable<std::string, const O3Class*> __superMap;
        HashTable<std::string, gum::NodeId> __nameMap;
        HashTable<std::string, O3Class*> __classMap;
        HashTable<NodeId, O3Class*> __nodeMap;
        DAG __dag;
        std::vector<O3Class*> __o3Classes;
        bool __build;

        void __initialize();

        void __setO3ClassCreationOrder();

        bool __checkAndAddNodesToDag( const PRM<GUM_SCALAR>& prm,
                                      const O3PRM& o3_prm,
                                      ErrorsContainer& errors );

        bool __checkAndAddArcsToDag( const O3PRM& prm, ErrorsContainer& errors );

        bool __checkO3Classes( const PRM<GUM_SCALAR>& prm,
                               O3PRM& tmp_prm,
                               ErrorsContainer& errors );

        bool __checkLocalParent( const Class<GUM_SCALAR>& c,
                                 const O3Label& prnt,
                                 ErrorsContainer& errors );

        bool __checkRemoteParent( const ClassElementContainer<GUM_SCALAR>& c,
                                  const O3Label& prnt,
                                  ErrorsContainer& errors );

        bool __checkParent( const Class<GUM_SCALAR>& c,
                            const O3Label& prnt,
                            ErrorsContainer& errors );

        bool __checkSlotChainLink( const ClassElementContainer<GUM_SCALAR>& c,
                                   const O3Label& chain,
                                   const std::string& s,
                                   ErrorsContainer& errors );

        bool __checkRawCPT( const PRM<GUM_SCALAR>& prm,
                            const Class<GUM_SCALAR>& c,
                            O3RawCPT& attr,
                            ErrorsContainer& errors );

        bool __checkRuleCPT( const Class<GUM_SCALAR>& c,
                             O3RuleCPT& attr,
                             ErrorsContainer& errors );

        bool __checkAttributeForDeclaration( const PRM<GUM_SCALAR>& prm,
                                             const O3Class& o3_c,
                                             O3Attribute& attr,
                                             ErrorsContainer& errors );

        bool __checkAttributeForCompletion( const PRM<GUM_SCALAR>& prm,
                                            const O3Class& o3_c,
                                            O3Attribute& attr,
                                            ErrorsContainer& errors );

        bool __checkReferenceSlot( const PRM<GUM_SCALAR>& prm,
                                   const O3Class& c,
                                   const O3ReferenceSlot& ref,
                                   ErrorsContainer& errors );

        bool __checkParametersNumber( const O3Aggregate& agg,
                                      size_t n,
                                      ErrorsContainer& errors );

        bool __checkParameterValue( const O3Aggregate& agg,
                                    const gum::prm::Type<GUM_SCALAR>& t,
                                    ErrorsContainer& errors );

        const Type<GUM_SCALAR>* __checkAggParents( const PRM<GUM_SCALAR>& prm,
                                                   const O3Class& o3class,
                                                   const O3Aggregate& agg,
                                                   ErrorsContainer& errors );

        bool __checkAggTypeLegality( const PRM<GUM_SCALAR>& prm,
                                     const O3Class& o3class,
                                     const O3Aggregate& agg,
                                     ErrorsContainer& errors );

        bool __checkAggParameters( const PRM<GUM_SCALAR>& prm,
                                   const O3Class& o3class,
                                   const O3Aggregate& agg,
                                   const Type<GUM_SCALAR>* t,
                                   ErrorsContainer& errors );

        bool __checkAggregate( const PRM<GUM_SCALAR>& prm,
                               const O3Class& o3class,
                               const O3Aggregate& agg,
                               ErrorsContainer& errors );

        bool __checkImplementation( const PRM<GUM_SCALAR>& prm,
                                    const O3PRM& o3_prm,
                                    ErrorsContainer& errors );

        bool __checkImplementation( const PRM<GUM_SCALAR>& prm,
                                    const O3Class& c,
                                    const Attribute<GUM_SCALAR>& attr );

        const ClassElement<GUM_SCALAR>*
        __resolveSlotChain( const ClassElementContainer<GUM_SCALAR>& c,
                            const O3Label& chain,
                            ErrorsContainer& errors );

        void __addParameters( PRMFactory<GUM_SCALAR>& factory,
                              const O3Class& c,
                              ErrorsContainer& errors );

        void __addReferenceSlots( PRMFactory<GUM_SCALAR>& factory,
                                  const O3Class& c,
                                  ErrorsContainer& errors );

        void __declareAttribute( PRMFactory<GUM_SCALAR>& factory,
                                 O3Class& c,
                                 ErrorsContainer& errors );

        void __completeAttribute( PRMFactory<GUM_SCALAR>& factory,
                                  O3Class& c,
                                  ErrorsContainer& errors );

        void __addAggregates( PRMFactory<GUM_SCALAR>& factory,
                              const O3Class& c,
                              ErrorsContainer& errors );

        bool __resolveInterface( const PRM<GUM_SCALAR>& prm,
                                 const O3Label& name,
                                 ErrorsContainer& errors );

        bool __resolveClass( const O3Label& super, ErrorsContainer& errors );

        bool __resolveReferenceName( const PRM<GUM_SCALAR>& prm,
                                     const O3Label& name,
                                     ErrorsContainer& errors );

        bool __resolveTypeName( const PRM<GUM_SCALAR>& prm,
                                const O3Label& name,
                                ErrorsContainer& errors );
      };

    }  // o3prm
  }    // prm
}  // gum

#include <agrum/PRM/newo3prm/O3ClassFactory.tcc>

extern template class gum::prm::o3prm::O3ClassFactory<float>;
extern template class gum::prm::o3prm::O3ClassFactory<double>;

#endif  // GUM_PRM_O3PRM_O3CLASS_FACTORY_H

