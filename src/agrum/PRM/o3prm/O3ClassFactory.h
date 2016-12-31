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
#include <regex>
#include <string>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/o3prm/O3NameSolver.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>

#ifndef GUM_PRM_O3PRM_O3CLASS_FACTORY_H
#define GUM_PRM_O3PRM_O3CLASS_FACTORY_H

namespace gum {
  namespace prm {
    namespace o3prm {

      /**
       * @class O3ClassFactory
       * @headerfile O3ClassFactory.h <agrum/PRM/o3prm/O3ClassFactory.h>
       * @ingroup o3prm_group
       *
       * @brief Builds gum::prm::Class from gum::prm::o3prm::O3Class.
       *
       * @tparam GUM_SCALAR The scalar type used by the gum::prm:PRM.
       */
      template <typename GUM_SCALAR>
      class O3ClassFactory {

        public:
        O3ClassFactory( PRM<GUM_SCALAR>&          prm,
                        O3PRM&                    o3_prm,
                        O3NameSolver<GUM_SCALAR>& solver,
                        ErrorsContainer&          errors );
        O3ClassFactory( const O3ClassFactory<GUM_SCALAR>& src );
        O3ClassFactory( O3ClassFactory<GUM_SCALAR>&& src );
        ~O3ClassFactory();
        O3ClassFactory<GUM_SCALAR>&
        operator=( const O3ClassFactory<GUM_SCALAR>& src );
        O3ClassFactory<GUM_SCALAR>& operator=( O3ClassFactory<GUM_SCALAR>&& src );

        void buildClasses();

        void buildImplementations();

        void buildParameters();

        void buildReferenceSlots();

        void declareAggregates();

        void completeAggregates();

        void declareAttributes();

        void completeAttributes();


        private:
        PRM<GUM_SCALAR>*          __prm;
        O3PRM*                    __o3_prm;
        O3NameSolver<GUM_SCALAR>* __solver;
        ErrorsContainer*          __errors;

        HashTable<std::string, gum::NodeId> __nameMap;
        HashTable<std::string, O3Class*>    __classMap;
        HashTable<NodeId, O3Class*>         __nodeMap;
        DAG                   __dag;
        std::vector<O3Class*> __o3Classes;

        /// @name Checking classes
        /// @{
        void __setO3ClassCreationOrder();

        bool __checkAndAddNodesToDag();

        bool __checkAndAddArcsToDag();

        bool __checkO3Classes();
        /// @}

        /// @name Checking Implementations
        /// @{
        bool __checkImplementation( O3Class& c );

        bool
        __checkImplementation( O3Class& c,
                               O3Label& i,
                               HashTable<std::string, O3Attribute*>&     attrMap,
                               HashTable<std::string, O3Aggregate*>&     aggMap,
                               HashTable<std::string, O3ReferenceSlot*>& refMap );

        bool __checkImplementation( O3Label&                   o3_type,
                                    const PRMType<GUM_SCALAR>& type );

        bool
        __checkImplementation( O3Label&                                    o3_type,
                               const PRMClassElementContainer<GUM_SCALAR>& type );
        /// @}

        /// @name Checking and Adding Paramteters
        /// @{
        void __addParameters( PRMFactory<GUM_SCALAR>& factory, O3Class& c );

        /// @}

        /// @name Checking and adding Reference Slots
        /// @{
        void __addReferenceSlots( O3Class& c );


        bool __checkReferenceSlot( O3Class& c, O3ReferenceSlot& ref );
        /// @}

        /// @name Checking and adding Attributes
        /// @{
        void __declareAttribute( O3Class& c );

        bool __checkAttributeForDeclaration( O3Class& o3_c, O3Attribute& attr );

        void __completeAttribute( PRMFactory<GUM_SCALAR>& factory, O3Class& c );

        bool __checkAttributeForCompletion( const O3Class& o3_c,
                                            O3Attribute&   attr );

        bool __checkParent( const PRMClass<GUM_SCALAR>& c, const O3Label& prnt );

        bool __checkLocalParent( const PRMClass<GUM_SCALAR>& c,
                                 const O3Label&              prnt );

        bool __checkRemoteParent( const PRMClassElementContainer<GUM_SCALAR>& c,
                                  const O3Label& prnt );


        bool __checkRawCPT( const PRMClass<GUM_SCALAR>& c, O3RawCPT& attr );

        bool __checkRuleCPT( const PRMClass<GUM_SCALAR>& c, O3RuleCPT& attr );

        /// @}

        /// @name Utility methods
        /// @{
        const PRMClassElement<GUM_SCALAR>*
        __resolveSlotChain( const PRMClassElementContainer<GUM_SCALAR>& c,
                            const O3Label&                              chain );

        bool __checkSlotChainLink( const PRMClassElementContainer<GUM_SCALAR>& c,
                                   const O3Label&     chain,
                                   const std::string& s );
        /// @}

        /// @name Checking and adding Aggregates
        /// @{
        void __declareAggregates( O3Class& c );
        void __completeAggregates( PRMFactory<GUM_SCALAR>& factory, O3Class& c );

        bool __checkAggregateForDeclaration( O3Class& o3class, O3Aggregate& agg );
        bool __checkAggregateForCompletion( O3Class& o3class, O3Aggregate& agg );

        const PRMType<GUM_SCALAR>* __checkAggParents( O3Class&     o3class,
                                                      O3Aggregate& agg );

        bool __checkAggTypeLegality( O3Class& o3class, O3Aggregate& agg );


        bool __checkAggParameters( O3Class&                   o3class,
                                   O3Aggregate&               agg,
                                   const PRMType<GUM_SCALAR>* t );

        bool __checkParametersNumber( O3Aggregate& agg, Size n );

        bool __checkParameterValue( O3Aggregate&                         agg,
                                    const gum::prm::PRMType<GUM_SCALAR>& t );

        // @}
      };

    }  // o3prm
  }    // prm
}  // gum

// always include the implementation of the templates
#include <agrum/PRM/o3prm/O3ClassFactory_tpl.h>


extern template class gum::prm::o3prm::O3ClassFactory<float>;
extern template class gum::prm::o3prm::O3ClassFactory<double>;


#endif  // GUM_PRM_O3PRM_O3CLASS_FACTORY_H
