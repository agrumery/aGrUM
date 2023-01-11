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
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/multidim/instantiation.h>
// =========================================================================
#include <agrum/FMDP/planning/treeOperatorStrategy.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/treeOperator.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/treeRegress.h>
// =========================================================================

/// For shorter line and hence more comprehensive code only
#define RECAST(x) reinterpret_cast< const MultiDimFunctionGraph< GUM_SCALAR >* >(x)

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
  template < typename GUM_SCALAR >
  INLINE TreeOperatorStrategy< GUM_SCALAR >::TreeOperatorStrategy() {
    GUM_CONSTRUCTOR(TreeOperatorStrategy);
  }

  // ===========================================================================
  // Default destructor
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE TreeOperatorStrategy< GUM_SCALAR >::~TreeOperatorStrategy() {
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
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     TreeOperatorStrategy< GUM_SCALAR >::regress(const MultiDimFunctionGraph< GUM_SCALAR >* Vold,
                                                 Idx                                   actionId,
                                                 const FMDP< GUM_SCALAR >*             fmdp,
                                                 const Set< const DiscreteVariable* >& elVarSeq) {
    // ******************************************************************************
    // Initialisation :
    // Creating a copy of last Vfunction to deduce from the new Qaction
    // And finding the first var to eleminate (the one at the end)
    Bijection< const DiscreteVariable*, const MultiDimFunctionGraph< GUM_SCALAR >* > pxi;
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter
         = Vold->variablesSequence().beginSafe();
         varIter != Vold->variablesSequence().endSafe();
         ++varIter) {
      pxi.insert(*varIter,
                 RECAST(fmdp->transition(actionId, fmdp->mapMainPrime().first(*varIter))));
    }

    TreeRegress< GUM_SCALAR, std::multiplies, std::plus > tr(
       Vold,
       pxi);   //, Vold->variablesSequence(), (GUM_SCALAR) 0.0 );
    return tr.compute();
  }

  // ==========================================================================
  /// @warning given f1 and f2 are deleted, returns the new one
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     TreeOperatorStrategy< GUM_SCALAR >::maximize(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                                  const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                                  Idx                                        del) {
    TreeOperator< GUM_SCALAR, Maximizes > opi(f1, f2);
    MultiDimFunctionGraph< GUM_SCALAR >*  ret = opi.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  /// @warning given f1 and f2 are deleted, returns the new one
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     TreeOperatorStrategy< GUM_SCALAR >::minimize(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                                  const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                                  Idx                                        del) {
    TreeOperator< GUM_SCALAR, Minimizes > opi(f1, f2);
    MultiDimFunctionGraph< GUM_SCALAR >*  ret = opi.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     TreeOperatorStrategy< GUM_SCALAR >::multiply(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                                  const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                                  Idx                                        del) {
    TreeOperator< GUM_SCALAR, std::multiplies > opi(f1, f2);
    MultiDimFunctionGraph< GUM_SCALAR >*        ret = opi.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }


  // ==========================================================================
  /// ArgMaximizes between f2 and f1
  /// @warning given f1 and f2 are deleted, returns the new one
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
     TreeOperatorStrategy< GUM_SCALAR >::argmaximize(
        const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* f1,
        const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* f2,
        Idx                                                                                 del) {
    TreeOperator< ArgMaxSet< GUM_SCALAR, Idx >, ArgumentMaximisesAction, SetTerminalNodePolicy >
                                                                                  argmaxope(f1, f2);
    MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* ret
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
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     TreeOperatorStrategy< GUM_SCALAR >::add(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                             const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                             Idx                                        del) {
    TreeOperator< GUM_SCALAR, std::plus > opi(f1, f2);
    MultiDimFunctionGraph< GUM_SCALAR >*  ret = opi.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  /// Subtract current f1 from old f1 to see if threshold is
  /// reached or not
  /// @param old and new VFuntion
  /// @warning this time, nothing is deleted
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
     TreeOperatorStrategy< GUM_SCALAR >::subtract(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                                  const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                                  Idx                                        del) {
    TreeOperator< GUM_SCALAR, std::minus > opi(f1, f2);
    MultiDimFunctionGraph< GUM_SCALAR >*   ret = opi.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }


}   // end of namespace gum
