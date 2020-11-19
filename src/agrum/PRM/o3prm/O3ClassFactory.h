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
 * @brief Headers for the O3ClassFactory class.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 * @author Lionel TORTI
 */

#include <algorithm>
#include <memory>
#include <string>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/o3prm/O3NameSolver.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/core/set.h>

#ifndef GUM_PRM_O3PRM_O3CLASS_FACTORY_H
#  define GUM_PRM_O3PRM_O3CLASS_FACTORY_H

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
      template < typename GUM_SCALAR >
      class O3ClassFactory {
        public:
        O3ClassFactory(PRM< GUM_SCALAR >&          prm,
                       O3PRM&                      o3_prm,
                       O3NameSolver< GUM_SCALAR >& solver,
                       ErrorsContainer&            errors);
        O3ClassFactory(const O3ClassFactory< GUM_SCALAR >& src);
        O3ClassFactory(O3ClassFactory< GUM_SCALAR >&& src);
        ~O3ClassFactory();
        O3ClassFactory< GUM_SCALAR >&
           operator=(const O3ClassFactory< GUM_SCALAR >& src);
        O3ClassFactory< GUM_SCALAR >&
           operator=(O3ClassFactory< GUM_SCALAR >&& src);

        void buildClasses();

        void buildImplementations();

        void buildParameters();

        void buildReferenceSlots();

        void declareAggregates();

        void completeAggregates();

        void declareAttributes();

        void completeAttributes();


        private:
        PRM< GUM_SCALAR >*          prm__;
        O3PRM*                      o3_prm__;
        O3NameSolver< GUM_SCALAR >* solver__;
        ErrorsContainer*            errors__;

        HashTable< std::string, gum::NodeId > nameMap__;
        HashTable< std::string, O3Class* >    classMap__;
        HashTable< NodeId, O3Class* >         nodeMap__;
        DAG                                   dag__;
        std::vector< O3Class* >               o3Classes__;

        /// @name Checking classes
        /// @{
        void setO3ClassCreationOrder__();

        bool checkAndAddNodesToDag__();

        bool checkAndAddArcsToDag__();

        bool checkO3Classes__();
        /// @}

        /// @name Checking Implementations
        /// @{
        bool checkImplementation__(O3Class& c);

        bool checkImplementation__(
           O3Class&                                    c,
           O3Label&                                    i,
           HashTable< std::string, O3Attribute* >&     attrMap,
           HashTable< std::string, O3Aggregate* >&     aggMap,
           HashTable< std::string, O3ReferenceSlot* >& refMap);

        bool checkImplementation__(O3Label& o3_type, const PRMType& type);

        bool checkImplementation__(
           O3Label& o3_type, const PRMClassElementContainer< GUM_SCALAR >& type);
        /// @}

        /// @name Checking and Adding Paramteters
        /// @{
        void addParameters__(PRMFactory< GUM_SCALAR >& factory, O3Class& c);

        /// @}

        /// @name Checking and adding Reference Slots
        /// @{
        void addReferenceSlots__(O3Class& c);


        bool checkReferenceSlot__(O3Class& c, O3ReferenceSlot& ref);
        /// @}

        /// @name Checking and adding Attributes
        /// @{
        void declareAttribute__(O3Class& c);

        bool checkAttributeForDeclaration__(O3Class& o3_c, O3Attribute& attr);

        void completeAttribute__(PRMFactory< GUM_SCALAR >& factory, O3Class& c);

        bool checkAttributeForCompletion__(const O3Class& o3_c, O3Attribute& attr);

        bool checkParent__(const PRMClass< GUM_SCALAR >& c, const O3Label& prnt);

        bool checkLocalParent__(const PRMClass< GUM_SCALAR >& c,
                                const O3Label&                prnt);

        bool checkRemoteParent__(const PRMClassElementContainer< GUM_SCALAR >& c,
                                 const O3Label& prnt);

        bool checkRawCPT__(const PRMClass< GUM_SCALAR >& c, O3RawCPT& attr);

        bool checkRuleCPT__(const PRMClass< GUM_SCALAR >& c, O3RuleCPT& attr);

        bool checkLabelsNumber__(const O3RuleCPT&         attr,
                                 const O3RuleCPT::O3Rule& rule);

        bool checkLabelsValues__(const PRMClass< GUM_SCALAR >& c,
                                 const O3RuleCPT&              attr,
                                 const O3RuleCPT::O3Rule&      rule);

        void addParamsToForms__(
           const HashTable< std::string, const PRMParameter< GUM_SCALAR >* >&
                              scope,
           O3RuleCPT::O3Rule& rule);

        bool checkRuleCPTSumsTo1__(const PRMClass< GUM_SCALAR >& c,
                                   const O3RuleCPT&              attr,
                                   const O3RuleCPT::O3Rule&      rule);

        /// @}

        /// @name Utility methods
        /// @{
        const PRMClassElement< GUM_SCALAR >*
           resolveSlotChain__(const PRMClassElementContainer< GUM_SCALAR >& c,
                              const O3Label&                                chain);

        bool checkSlotChainLink__(const PRMClassElementContainer< GUM_SCALAR >& c,
                                  const O3Label&     chain,
                                  const std::string& s);
        /// @}

        /// @name Checking and adding Aggregates
        /// @{
        void declareAggregates__(O3Class& c);
        void completeAggregates__(PRMFactory< GUM_SCALAR >& factory, O3Class& c);

        bool checkAggregateForDeclaration__(O3Class& o3class, O3Aggregate& agg);
        bool checkAggregateForCompletion__(O3Class& o3class, O3Aggregate& agg);

        const PRMType* checkAggParents__(O3Class& o3class, O3Aggregate& agg);

        bool checkAggTypeLegality__(O3Class& o3class, O3Aggregate& agg);


        bool checkAggParameters__(O3Class&       o3class,
                                  O3Aggregate&   agg,
                                  const PRMType* t);

        bool checkParametersNumber__(O3Aggregate& agg, Size n);

        bool checkParameterValue__(O3Aggregate& agg, const gum::prm::PRMType& t);

        // @}
      };

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum

// always include the implementation of the templates
#  include <agrum/PRM/o3prm/O3ClassFactory_tpl.h>


#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::prm::o3prm::O3ClassFactory< double >;
#  endif


#endif   // GUM_PRM_O3PRM_O3CLASS_FACTORY_H
