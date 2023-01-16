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
 * @brief Headers for the O3ClassFactory class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <algorithm>
#include <memory>
#include <string>

#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/o3prm/O3NameSolver.h>

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
        O3ClassFactory< GUM_SCALAR >& operator=(const O3ClassFactory< GUM_SCALAR >& src);
        O3ClassFactory< GUM_SCALAR >& operator=(O3ClassFactory< GUM_SCALAR >&& src);

        void buildClasses();

        void buildImplementations();

        void buildParameters();

        void buildReferenceSlots();

        void declareAggregates();

        void completeAggregates();

        void declareAttributes();

        void completeAttributes();


        private:
        PRM< GUM_SCALAR >*          _prm_;
        O3PRM*                      _o3_prm_;
        O3NameSolver< GUM_SCALAR >* _solver_;
        ErrorsContainer*            _errors_;

        HashTable< std::string, gum::NodeId > _nameMap_;
        HashTable< std::string, O3Class* >    _classMap_;
        HashTable< NodeId, O3Class* >         _nodeMap_;
        DAG                                   _dag_;
        std::vector< O3Class* >               _o3Classes_;

        /// @name Checking classes
        /// @{
        void _setO3ClassCreationOrder_();

        bool _checkAndAddNodesToDag_();

        bool _checkAndAddArcsToDag_();

        bool _checkO3Classes_();
        /// @}

        /// @name Checking Implementations
        /// @{
        bool _checkImplementation_(O3Class& c);

        bool _checkImplementation_(O3Class&                                    c,
                                   O3Label&                                    i,
                                   HashTable< std::string, O3Attribute* >&     attrMap,
                                   HashTable< std::string, O3Aggregate* >&     aggMap,
                                   HashTable< std::string, O3ReferenceSlot* >& refMap);

        bool _checkImplementation_(O3Label& o3_type, const PRMType& type);

        bool _checkImplementation_(O3Label&                                      o3_type,
                                   const PRMClassElementContainer< GUM_SCALAR >& type);
        /// @}

        /// @name Checking and Adding Paramteters
        /// @{
        void _addParameters_(PRMFactory< GUM_SCALAR >& factory, O3Class& c);

        /// @}

        /// @name Checking and adding Reference Slots
        /// @{
        void _addReferenceSlots_(O3Class& c);


        bool _checkReferenceSlot_(O3Class& c, O3ReferenceSlot& ref);
        /// @}

        /// @name Checking and adding Attributes
        /// @{
        void _declareAttribute_(O3Class& c);

        bool _checkAttributeForDeclaration_(O3Class& o3_c, O3Attribute& attr);

        void _completeAttribute_(PRMFactory< GUM_SCALAR >& factory, O3Class& c);

        bool _checkAttributeForCompletion_(const O3Class& o3_c, O3Attribute& attr);

        bool _checkParent_(const PRMClass< GUM_SCALAR >& c, const O3Label& prnt);

        bool _checkLocalParent_(const PRMClass< GUM_SCALAR >& c, const O3Label& prnt);

        bool _checkRemoteParent_(const PRMClassElementContainer< GUM_SCALAR >& c,
                                 const O3Label&                                prnt);

        bool _checkRawCPT_(const PRMClass< GUM_SCALAR >& c, O3RawCPT& attr);

        bool _checkRuleCPT_(const PRMClass< GUM_SCALAR >& c, O3RuleCPT& attr);

        bool _checkLabelsNumber_(const O3RuleCPT& attr, const O3RuleCPT::O3Rule& rule);

        bool _checkLabelsValues_(const PRMClass< GUM_SCALAR >& c,
                                 const O3RuleCPT&              attr,
                                 const O3RuleCPT::O3Rule&      rule);

        void _addParamsToForms_(
           const HashTable< std::string, const PRMParameter< GUM_SCALAR >* >& scope,
           O3RuleCPT::O3Rule&                                                 rule);

        bool _checkRuleCPTSumsTo1_(const PRMClass< GUM_SCALAR >& c,
                                   const O3RuleCPT&              attr,
                                   const O3RuleCPT::O3Rule&      rule);

        /// @}

        /// @name Utility methods
        /// @{
        const PRMClassElement< GUM_SCALAR >*
           _resolveSlotChain_(const PRMClassElementContainer< GUM_SCALAR >& c,
                              const O3Label&                                chain);

        bool _checkSlotChainLink_(const PRMClassElementContainer< GUM_SCALAR >& c,
                                  const O3Label&                                chain,
                                  const std::string&                            s);
        /// @}

        /// @name Checking and adding Aggregates
        /// @{
        void _declareAggregates_(O3Class& c);
        void _completeAggregates_(PRMFactory< GUM_SCALAR >& factory, O3Class& c);

        bool _checkAggregateForDeclaration_(O3Class& o3class, O3Aggregate& agg);
        bool _checkAggregateForCompletion_(O3Class& o3class, O3Aggregate& agg);

        const PRMType* _checkAggParents_(O3Class& o3class, O3Aggregate& agg);

        bool _checkAggTypeLegality_(O3Class& o3class, O3Aggregate& agg);


        bool _checkAggParameters_(O3Class& o3class, O3Aggregate& agg, const PRMType* t);

        bool _checkParametersNumber_(O3Aggregate& agg, Size n);

        bool _checkParameterValue_(O3Aggregate& agg, const gum::prm::PRMType& t);

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
