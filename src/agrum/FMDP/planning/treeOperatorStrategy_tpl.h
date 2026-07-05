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

#pragma once


/**
 * @file
 * @brief Template implementation of FMDP/planning/TreeOperatorStrategy.h
 * classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// =========================================================================
#include <algorithm>
#include <queue>
#include <utility>
#include <vector>
// =========================================================================
#include <agrum/base/multidim/instantiation.h>

#include <agrum/base/core/math/math_utils.h>
// =========================================================================
#include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/treeOperator.h>
#include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/treeRegress.h>
#include <agrum/FMDP/planning/treeOperatorStrategy.h>
// =========================================================================

/// For shorter line and hence more comprehensive code only
#define RECAST(x) reinterpret_cast< const MultiDimFunctionGraph< GUM_ELEMENT >* >(x)

namespace gum {


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                Constructors / Destructors **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Default constructor
  // ===========================================================================
  template < typename GUM_ELEMENT >
  INLINE TreeOperatorStrategy< GUM_ELEMENT >::TreeOperatorStrategy() {
    GUM_CONSTRUCTOR(TreeOperatorStrategy);
  }

  // ===========================================================================
  // Default destructor
  // ===========================================================================
  template < typename GUM_ELEMENT >
  INLINE TreeOperatorStrategy< GUM_ELEMENT >::~TreeOperatorStrategy() {
    GUM_DESTRUCTOR(TreeOperatorStrategy);
  }

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                             Graph Function Operations Methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/


  // ==========================================================================
  /// Performs a multiplication/projection on given qAction
  /// @param qAction : the computed Q(s,a)
  /// @param pxip : the transition probabilities for the xip variable
  /// @param xip : the variable we eliminate on the projection
  /// @warning given qAction is deleted, return the new one
  // ==========================================================================
  template < typename GUM_ELEMENT >
  MultiDimFunctionGraph< GUM_ELEMENT >*
      TreeOperatorStrategy< GUM_ELEMENT >::regress(const MultiDimFunctionGraph< GUM_ELEMENT >* Vold,
                                                   Idx                        actionId,
                                                   const FMDP< GUM_ELEMENT >* fmdp,
                                                   const gum::VariableSet&    elVarSeq) {
    // ******************************************************************************
    // Initialisation :
    // Creating a copy of last Vfunction to deduce from the new Qaction
    // And finding the first var to eleminate (the one at the end)
    Bijection< const DiscreteVariable*, const MultiDimFunctionGraph< GUM_ELEMENT >* > pxi;
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter
         = Vold->variablesSequence().beginSafe();
         varIter != Vold->variablesSequence().endSafe();
         ++varIter) {
      pxi.insert(*varIter,
                 RECAST(fmdp->transition(actionId, fmdp->mapMainPrime().first(*varIter))));
    }

    TreeRegress< GUM_ELEMENT, std::multiplies, std::plus > tr(
        Vold,
        pxi);   //, Vold->variablesSequence(), (GUM_ELEMENT) 0.0 );
    return tr.compute();
  }

  // ==========================================================================
  /// @warning given f1 and f2 are deleted, returns the new one
  // ==========================================================================
  template < typename GUM_ELEMENT >
  MultiDimFunctionGraph< GUM_ELEMENT >*
      TreeOperatorStrategy< GUM_ELEMENT >::maximize(const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
                                                    const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
                                                    Idx del) {
    TreeOperator< GUM_ELEMENT, Maximizes > opi(f1, f2);
    MultiDimFunctionGraph< GUM_ELEMENT >*  ret = opi.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  /// @warning given f1 and f2 are deleted, returns the new one
  // ==========================================================================
  template < typename GUM_ELEMENT >
  MultiDimFunctionGraph< GUM_ELEMENT >*
      TreeOperatorStrategy< GUM_ELEMENT >::minimize(const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
                                                    const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
                                                    Idx del) {
    TreeOperator< GUM_ELEMENT, Minimizes > opi(f1, f2);
    MultiDimFunctionGraph< GUM_ELEMENT >*  ret = opi.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  // ==========================================================================
  template < typename GUM_ELEMENT >
  MultiDimFunctionGraph< GUM_ELEMENT >*
      TreeOperatorStrategy< GUM_ELEMENT >::multiply(const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
                                                    const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
                                                    Idx del) {
    TreeOperator< GUM_ELEMENT, std::multiplies > opi(f1, f2);
    MultiDimFunctionGraph< GUM_ELEMENT >*        ret = opi.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  /// ArgMaximizes between f2 and f1
  /// @warning given f1 and f2 are deleted, returns the new one
  // ==========================================================================
  template < typename GUM_ELEMENT >
  MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >*
      TreeOperatorStrategy< GUM_ELEMENT >::argmaximize(
          const MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >* f1,
          const MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >* f2,
          Idx del) {
    TreeOperator< ArgMaxSet< GUM_ELEMENT, Idx >, ArgumentMaximisesAction, SetTerminalNodePolicy >
        argmaxope(f1, f2);
    MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >* ret
        = argmaxope.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  /// Adds reward to given function( whether a f2 or f1)
  /// @param reward : R(s) or R(s,a)
  /// @param function : either V(s) or Q(s,a)
  /// @warning given function is deleted, returns the new one
  // ==========================================================================
  template < typename GUM_ELEMENT >
  MultiDimFunctionGraph< GUM_ELEMENT >*
      TreeOperatorStrategy< GUM_ELEMENT >::add(const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
                                               const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
                                               Idx                                         del) {
    TreeOperator< GUM_ELEMENT, std::plus > opi(f1, f2);
    MultiDimFunctionGraph< GUM_ELEMENT >*  ret = opi.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  /// Subtract current f1 from old f1 to see if threshold is
  /// reached or not
  /// @param old and new VFuntion
  /// @warning this time, nothing is deleted
  // ==========================================================================
  template < typename GUM_ELEMENT >
  MultiDimFunctionGraph< GUM_ELEMENT >*
      TreeOperatorStrategy< GUM_ELEMENT >::subtract(const MultiDimFunctionGraph< GUM_ELEMENT >* f1,
                                                    const MultiDimFunctionGraph< GUM_ELEMENT >* f2,
                                                    Idx del) {
    TreeOperator< GUM_ELEMENT, std::minus > opi(f1, f2);
    MultiDimFunctionGraph< GUM_ELEMENT >*   ret = opi.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  template < typename GUM_ELEMENT >
  INLINE MultiDimFunctionGraph< GUM_ELEMENT, ExactTerminalNodePolicy >*
         TreeOperatorStrategy< GUM_ELEMENT >::getFunctionInstance() {
    return MultiDimFunctionGraph< GUM_ELEMENT >::getTreeInstance();
  }

  template < typename GUM_ELEMENT >
  MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >, SetTerminalNodePolicy >*
      TreeOperatorStrategy< GUM_ELEMENT >::getArgMaxFunctionInstance() {
    return MultiDimFunctionGraph< ArgMaxSet< GUM_ELEMENT, Idx >,
                                  SetTerminalNodePolicy >::getTreeInstance();
  }

  template < typename GUM_ELEMENT >
  INLINE MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >*
         TreeOperatorStrategy< GUM_ELEMENT >::getAggregatorInstance() {
    return MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >::getTreeInstance();
  }

}   // end of namespace gum
