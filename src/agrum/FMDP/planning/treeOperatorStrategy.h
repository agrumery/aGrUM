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
 * @brief Headers of the TreeOperatorStrategy planer class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_TREE_OPERATOR_STRATEGY_H
#define GUM_TREE_OPERATOR_STRATEGY_H
// =========================================================================
#include <agrum/FMDP/planning/IOperatorStrategy.h>

// =========================================================================

namespace gum {

  /**
   * @class TreeOperatorStrategy treeOperatorStrategy.h
   * <agrum/FMDP/planning/treeOperatorStrategy.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a TreeOperatorStrategy planning on given in parameter factored
   * markov decision process
   *
   */
  template < typename GUM_ELEMENT >
  class TreeOperatorStrategy: public IOperatorStrategy< GUM_ELEMENT > {
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{

    public:
    // ==========================================================================
    /// Default constructor
    // ==========================================================================
    TreeOperatorStrategy();

    // ==========================================================================
    /// Default destructor
    // ==========================================================================
    ~TreeOperatorStrategy() override;

    /// @}


    // ###################################################################
    /// @name
    // ###################################################################
    /// @{

    public:
    MultiDimFunctionGraph< GUM_ELEMENT, ExactTerminalNodePolicy >* getFunctionInstance() override;

    MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >*
        getArgMaxFunctionInstance() override;

    MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >* getAggregatorInstance() override;

    /// @}

    // ###################################################################
    /// @name Graph Function Operations Methods
    // ###################################################################
    /// @{

    protected:
    // ==========================================================================
    /// Computes Qaction for given actionid
    /// @return qAction : the computed Q(s,a)
    // ==========================================================================
    MultiDimFunctionGraph< GUM_ELEMENT >* regress(const MultiDimFunctionGraph< GUM_ELEMENT >* Vold,
                                                  Idx                        actionId,
                                                  const FMDP< GUM_ELEMENT >* fmdp,
                                                  const gum::VariableSet&    elVarSeq) override;

    // ==========================================================================
    /// @warning given vFunction and qAction are deleted, returns the new one
    // ==========================================================================
    MultiDimFunctionGraph< GUM_ELEMENT >* maximize(const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
                                                   const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
                                                   Idx del = 3) override;

    // ==========================================================================
    /// @warning Minimze the two given functions
    // ==========================================================================
    MultiDimFunctionGraph< GUM_ELEMENT >* minimize(const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
                                                   const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
                                                   Idx del = 3) override;

    // ==========================================================================
    /// @warning given f1 and f2 are deleted, returns the new one
    // ==========================================================================
    MultiDimFunctionGraph< GUM_ELEMENT >* multiply(const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
                                                   const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
                                                   Idx del = 3) override;

    // ==========================================================================
    /// @warning given vFunction and qAction are deleted, returns the new one
    // ==========================================================================
    MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >* argmaximize(
        const MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >* f1,
        const MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >* f2,
        Idx del = 3) override;

    // ==========================================================================
    /// @warning given function is deleted, returns the new one
    // ==========================================================================
    MultiDimFunctionGraph< GUM_ELEMENT >* add(const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
                                              const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
                                              Idx del = 1) override;

    // ==========================================================================
    /// @warning this time, nothing is deleted
    // ==========================================================================
    MultiDimFunctionGraph< GUM_ELEMENT >* subtract(const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
                                                   const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
                                                   Idx del = 0) override;


    /// @}
  };
} /* namespace gum */

#include <agrum/FMDP/planning/treeOperatorStrategy_tpl.h>

#endif   // GUM_TREE_OPERATOR_STRATEGY_H
