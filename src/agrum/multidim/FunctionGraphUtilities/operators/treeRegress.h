/****************************************************************************
 *  Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES     *
 *  {prenom.nom}_at_lip6.fr                                                 *
 *                                                                          *
 *  This program is free software; you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation; either version 2 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  This program is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program; if not, write to the                           *
 *  Free Software Foundation, Inc.,                                         *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                *
 ****************************************************************************/
/**
 * @file
 * @brief Class used to compute the operation between two decision diagrams
 *
 * @author Jean-Christophe Magnan
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_TREE_REGRESS_H
#define GUM_TREE_REGRESS_H

#include <functional>

#include <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>
#include <agrum/multidim/multiDimFunctionGraph.h>

namespace gum {

  /**
   * @class TreeRegress treeRegress.h <agrum/multidim/patterns/treeRegress.h>
   * @ingroup multidim_group
   *
   * @brief Class used to perform Decision Tree Regression in the FMDP Framework
   */
  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class PROJECTOPERATOR,
            template <typename> class TerminalNodePolicy = ExactTerminalNodePolicy>
  class TreeRegress {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /// Default constructor.
    TreeRegress(
        const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* qAction,
        const Bijection<
            const DiscreteVariable*,
            const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>*> pxi );

    /// Default destructor.
    ~TreeRegress();

    /// @}
    // ============================================================================
    /// @name Main Method
    // ============================================================================
    /// @{

    /// Computes and builds the Function Graph that is the result of the
    /// operation
    MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* compute();

    /// @}

    private:
    /// The whatever 1
    MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>*
    __xPloreVFunc( NodeId currentNodeId );

    /// The function graphs used for the operation
    const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* __vFunc;
    const Bijection<const DiscreteVariable*,
                    const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>*>
        __pxi;
    HashTable<const DiscreteVariable*, Idx> __context;
  };

  extern template class TreeRegress<double, std::multiplies, std::plus>;

}  // namespace gum

#include <agrum/multidim/FunctionGraphUtilities/operators/treeRegress_tpl.h>

#endif  // GUM_REGRESS_H
