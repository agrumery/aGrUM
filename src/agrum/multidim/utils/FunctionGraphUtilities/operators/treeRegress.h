
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_TREE_REGRESS_H
#define GUM_TREE_REGRESS_H

#include <functional>

#include <agrum/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/multidim/utils/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>

namespace gum {

  /**
   * @class TreeRegress
   * @headerfile treeRegress.h <agrum/multidim/patterns/treeRegress.h>
   * @ingroup multidim_group
   *
   * @brief Class used to perform Decision Tree Regression in the FMDP Framework
   */
  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename > class TerminalNodePolicy =
                ExactTerminalNodePolicy >
  class TreeRegress {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /// Default constructor.
    TreeRegress(
       const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* qAction,
       const Bijection<
          const DiscreteVariable*,
          const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* > pxi);

    /// Default destructor.
    ~TreeRegress();

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
    /// The whatever 1
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*
       __xPloreVFunc(NodeId currentNodeId);

    /// The function graphs used for the operation
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* __vFunc;
    const Bijection<
       const DiscreteVariable*,
       const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* >
                                              __pxi;
    HashTable< const DiscreteVariable*, Idx > __context;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class TreeRegress< double, std::multiplies, std::plus >;
#endif

}   // namespace gum

#include <agrum/multidim/utils/FunctionGraphUtilities/operators/treeRegress_tpl.h>

#endif   // GUM_REGRESS_H
