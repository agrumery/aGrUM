/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Template implementation of FMDP/planning/MDDOperatorStrategy.h classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// =========================================================================
#include <algorithm>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
// =========================================================================
#include <agrum/tools/core/functors.h>

#include <agrum/tools/core/math/math_utils.h>
// =========================================================================
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/multiDimFunctionGraphOperator.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/regress.h>
// =========================================================================
#include <agrum/FMDP/planning/mddOperatorStrategy.h>
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
  INLINE MDDOperatorStrategy< GUM_SCALAR >::MDDOperatorStrategy() {
    GUM_CONSTRUCTOR(MDDOperatorStrategy);
  }

  // ===========================================================================
  // Default destructor
  // ===========================================================================
  template < typename GUM_SCALAR >
  INLINE MDDOperatorStrategy< GUM_SCALAR >::~MDDOperatorStrategy() {
    GUM_DESTRUCTOR(MDDOperatorStrategy);
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
      MDDOperatorStrategy< GUM_SCALAR >::regress(const MultiDimFunctionGraph< GUM_SCALAR >* Vold,
                                                 Idx                       actionId,
                                                 const FMDP< GUM_SCALAR >* fmdp,
                                                 const gum::VariableSet&   elVarSeq) {
    MultiDimFunctionGraph< GUM_SCALAR >* qAction
        = MultiDimFunctionGraph< GUM_SCALAR >::getReducedAndOrderedInstance();
    qAction->copy(*Vold);

    const DiscreteVariable* xip = this->lastVar_(qAction);

    while (this->shouldEleminateVar_(xip, fmdp)) {
      const MultiDimFunctionGraph< GUM_SCALAR >* pxi
          = RECAST(fmdp->transition(actionId, fmdp->mapMainPrime().first(xip)));
      Regress< GUM_SCALAR, std::multiplies, std::plus > r(qAction,
                                                          pxi,
                                                          &elVarSeq,
                                                          xip,
                                                          (GUM_SCALAR)0);
      MultiDimFunctionGraph< GUM_SCALAR >*              temp = r.compute();
      delete qAction;
      qAction = temp;
      xip     = this->lastVar_(qAction);
    }

    return qAction;
  }

  // ==========================================================================
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
      MDDOperatorStrategy< GUM_SCALAR >::maximize(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                                  const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                                  Idx                                        del) {
    MultiDimFunctionGraph< GUM_SCALAR >* ret = maximize2MultiDimFunctionGraphs(f1, f2);
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
      MDDOperatorStrategy< GUM_SCALAR >::minimize(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                                  const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                                  Idx                                        del) {
    MultiDimFunctionGraph< GUM_SCALAR >* ret = minimize2MultiDimFunctionGraphs(f1, f2);
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
      MDDOperatorStrategy< GUM_SCALAR >::multiply(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                                  const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                                  Idx                                        del) {
    MultiDimFunctionGraph< GUM_SCALAR >* ret = multiply2MultiDimFunctionGraphs(f1, f2);
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
      MDDOperatorStrategy< GUM_SCALAR >::argmaximize(
          const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* f1,
          const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* f2,
          Idx                                                                                 del) {
    MultiDimFunctionGraphOperator< ArgMaxSet< GUM_SCALAR, Idx >,
                                   ArgumentMaximisesAction,
                                   SetTerminalNodePolicy >
                                                                                  argmaxope(f1, f2);
    MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* ret
        = argmaxope.compute();
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  /// Adds reward to given function( whether a qAction or vFunction)
  /// @param reward : R(s) or R(s,a)
  /// @param function : either V(s) or Q(s,a)
  /// @warning given function is deleted, returns the new one
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
      MDDOperatorStrategy< GUM_SCALAR >::add(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                             const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                             Idx                                        del) {
    MultiDimFunctionGraph< GUM_SCALAR >* ret = add2MultiDimFunctionGraphs(f1, f2);
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }

  // ==========================================================================
  /// Subtract current VFunction from old VFunction to see if threshold is
  /// reached or not
  /// @param old and new VFuntion
  /// @warning this time, nothing is deleted
  // ==========================================================================
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
      MDDOperatorStrategy< GUM_SCALAR >::subtract(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                                  const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                                  Idx                                        del) {
    MultiDimFunctionGraph< GUM_SCALAR >* ret = subtract2MultiDimFunctionGraphs(f1, f2);
    this->deleteFunctionGraph_(f1, f2, del);
    return ret;
  }


}   // end of namespace gum
