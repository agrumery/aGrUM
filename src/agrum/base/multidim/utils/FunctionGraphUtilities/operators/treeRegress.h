/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Class used to compute the operation between two decision diagrams
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_TREE_REGRESS_H
#define GUM_TREE_REGRESS_H

#include <functional>

#include <agrum/base/multidim/implementations/multiDimFunctionGraph.h>

namespace gum {

  /**
   * @class TreeRegress
   * @headerfile treeRegress.h <agrum/base/multidim/patterns/treeRegress.h>
   * @ingroup multidim_group
   *
   * @brief Class used to perform Decision Tree Regression in the FMDP Framework
   */
  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class PROJECTOPERATOR,
             template < typename > class TerminalNodePolicy = ExactTerminalNodePolicy >
  class TreeRegress {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /// Default constructor.
    TreeRegress(
        const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*                    qAction,
        const Bijection< const DiscreteVariable*,
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
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _xPloreVFunc_(NodeId currentNodeId);

    /// The function graphs used for the operation
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _vFunc_;
    const Bijection< const DiscreteVariable*,
                     const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* >
                                              _pxi_;
    HashTable< const DiscreteVariable*, Idx > _context_;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class TreeRegress< double, std::multiplies, std::plus >;
#endif

}   // namespace gum

#include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/treeRegress_tpl.h>

#endif   // GUM_REGRESS_H
