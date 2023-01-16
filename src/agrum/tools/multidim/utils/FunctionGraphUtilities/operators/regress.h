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
 * @brief Class used to compute the operation between two decision diagrams
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#ifndef GUM_REGRESS_H
#define GUM_REGRESS_H

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/o4DGContext.h>

namespace gum {

  /**
   * @class Regress
   * @headerfile regress.h <agrum/tools/multidim/patterns/regress.h>
   * @brief Class used to perform Function Graph Operations in the FMDP
   * Framework
   * @ingroup multidim_group
   */
  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename > class TerminalNodePolicy = ExactTerminalNodePolicy >
  class Regress {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /// Default constructor.
    Regress(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* vfunction,
            const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* probDist,
            const Set< const DiscreteVariable* >*                          primedVars,
            const DiscreteVariable*                                        targetVar,
            const GUM_SCALAR                                               neutral);

    /// Default destructor.
    ~Regress();

    /// @}

    // ============================================================================
    /// @name Main Method
    // ============================================================================
    /// @{

    /// Computes and builds the Function Graph that is the result of the
    /// operation
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* compute();

    /// @}

    private:
    /// Computes an order for the final Decision graph that will minimize the
    /// number
    /// of re exploration
    void _establishVarOrder_();

    /// Establish for each node in both function graph if it has retrograde
    /// variables
    /// beneath it
    void
       _findRetrogradeVariables_(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dg,
                                 HashTable< NodeId, short int* >& dgInstNeed);

    /// The main recursion function
    NodeId _compute_(O4DGContext& currentSituation, Idx lastInstVarPos);

    /// One of the two function graphs used for the operation
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _DG1_;

    /// The other one
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _DG2_;

    /// The resulting function graph
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _rd_;

    /// The set of variables we want to keep at the end
    const Set< const DiscreteVariable* >* _primedVars_;

    /// The variable we work on to eleminate
    const DiscreteVariable* _targetVar_;

    /// The function to be performed on the leaves
    const GUM_SCALAR _neutral_;

    /// The total number of variable implied in the operation
    Idx _nbVar_;

    /// The functions to be performed on the leaves
    const COMBINEOPERATOR< GUM_SCALAR > _combine_;
    const PROJECTOPERATOR< GUM_SCALAR > _project_;

    /// The hashtable used to know if two pair of nodes have already been
    /// visited
    HashTable< double, NodeId > _explorationTable_;

    /// Table uses to know if a given node of given function graph has
    /// retrograde variables
    HashTable< NodeId, short int* > _DG1InstantiationNeeded_;
    HashTable< NodeId, short int* > _DG2InstantiationNeeded_;

    /// Just a computationnal trick
    short int* _default_;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class Regress< double, std::multiplies, std::plus >;
#endif

}   // namespace gum

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/regress_tpl.h>

#endif   // GUM_REGRESS_H
