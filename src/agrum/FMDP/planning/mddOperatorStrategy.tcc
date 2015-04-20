/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
/**
* @file
* @brief Template implementation of FMDP/planning/MDDOperatorStrategy.h classes.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

// =========================================================================
#include <math.h>
#include <thread>
#include <vector>
#include <queue>
#include <algorithm>
#include <utility>
// =========================================================================
#include <agrum/core/functors.h>
// =========================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/FunctionGraphUtilities/operators/regress.h>
#include <agrum/multidim/FunctionGraphUtilities/operators/multiDimFunctionGraphOperator.h>
// =========================================================================
#include <agrum/FMDP/planning/mddOperatorStrategy.h>
// =========================================================================

/// For shorter line and hence more comprehensive code only
#define RECAST(x) reinterpret_cast<const MultiDimFunctionGraph<GUM_SCALAR>*>(x)

namespace gum {


  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                                Constructors / Destructors                                       **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Default constructor
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    MDDOperatorStrategy<GUM_SCALAR>::MDDOperatorStrategy (  ) {
      GUM_CONSTRUCTOR ( MDDOperatorStrategy );
    }

    // ===========================================================================
    // Default destructor
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    MDDOperatorStrategy<GUM_SCALAR>::~MDDOperatorStrategy() {
      GUM_DESTRUCTOR ( MDDOperatorStrategy );
    }



  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                             Graph Function Operations Methods                                   **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ==========================================================================
    /// Performs a multiplication/projection on given qAction
    /// @param qAction : the computed Q(s,a)
    /// @param pxip : the transition probabilities for the xip variable
    /// @param xip : the variable we eliminate on the projection
    /// @warning given qAction is deleted, return the new one
    // ==========================================================================
    template<typename GUM_SCALAR>
    MultiDimFunctionGraph<GUM_SCALAR>* MDDOperatorStrategy<GUM_SCALAR>::regress(const MultiDimFunctionGraph< GUM_SCALAR >* Vold,
                                                                    Idx actionId,
                                                                    const FMDP< GUM_SCALAR >* fmdp,
                                                                    const Set<const DiscreteVariable*>& elVarSeq ){
      MultiDimFunctionGraph< GUM_SCALAR >* qAction = MultiDimFunctionGraph<GUM_SCALAR>::getReducedAndOrderedInstance();
      qAction->copy( *Vold );

      const DiscreteVariable* xip = this->_lastVar(qAction);

      while( this->_shouldEleminateVar(xip, fmdp) ){
        const MultiDimFunctionGraph<GUM_SCALAR>* pxi = RECAST(fmdp->transition(actionId,fmdp->mapMainPrime().first(xip)));
        Regress<GUM_SCALAR, std::multiplies, std::plus> r( qAction, pxi, &elVarSeq, xip, (GUM_SCALAR)0 );
        MultiDimFunctionGraph<GUM_SCALAR>* temp = r.compute();
        delete qAction;
        qAction = temp;
        xip=this->_lastVar(qAction);
      }

      return qAction;

    }

    // ==========================================================================
    /// Maximizes between QAction and VFunction
    /// @param qAction : the computed Q(s,a)
    /// @param vFunction : the vFunction so far
    /// @warning given vFunction and qAction are deleted, returns the new one
    // ==========================================================================
    template<typename GUM_SCALAR>
    MultiDimFunctionGraph<GUM_SCALAR>* MDDOperatorStrategy<GUM_SCALAR>::maximize( const MultiDimFunctionGraph< GUM_SCALAR >* vFunction,
                                                                      const MultiDimFunctionGraph< GUM_SCALAR >* qAction){
      MultiDimFunctionGraph<GUM_SCALAR>* ret = maximize2MultiDimFunctionGraphs ( vFunction, qAction );
      delete vFunction;
      delete qAction;
      return ret;
    }


    // ==========================================================================
    /// ArgMaximizes between QAction and VFunction
    /// @param qAction : the computed Q(s,a)
    /// @param vFunction : the vFunction so far
    /// @warning given vFunction and qAction are deleted, returns the new one
    // ==========================================================================
    template<typename GUM_SCALAR>
    MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* MDDOperatorStrategy<GUM_SCALAR>::argmaximize(
                        const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* vFunction,
                        const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* qAction){

      MultiDimFunctionGraphOperator< ArgMaxSet<GUM_SCALAR, Idx>, ArgumentMaximisesAction, SetTerminalNodePolicy > argmaxope(
            vFunction, qAction );
      MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* ret = argmaxope.compute();
      delete vFunction;
      delete qAction;
      return ret;
    }

    // ==========================================================================
    /// Adds reward to given function( whether a qAction or vFunction)
    /// @param reward : R(s) or R(s,a)
    /// @param function : either V(s) or Q(s,a)
    /// @warning given function is deleted, returns the new one
    // ==========================================================================
    template<typename GUM_SCALAR>
    MultiDimFunctionGraph<GUM_SCALAR>* MDDOperatorStrategy<GUM_SCALAR>::add(const MultiDimFunctionGraph< GUM_SCALAR >* function,
                                                                const MultiDimFunctionGraph< GUM_SCALAR >* reward){
      MultiDimFunctionGraph<GUM_SCALAR>* ret = add2MultiDimFunctionGraphs ( function, reward );
      delete function;
      return ret;
    }

    // ==========================================================================
    /// Subtract current VFunction from old VFunction to see if threshold is
    /// reached or not
    /// @param old and new VFuntion
    /// @warning this time, nothing is deleted
    // ==========================================================================
    template<typename GUM_SCALAR>
    MultiDimFunctionGraph<GUM_SCALAR>* MDDOperatorStrategy<GUM_SCALAR>::subtract(const MultiDimFunctionGraph< GUM_SCALAR >* newVF,
                                                         const MultiDimFunctionGraph< GUM_SCALAR >* oldVF){
      return subtract2MultiDimFunctionGraphs ( newVF, oldVF );
    }


} // end of namespace gum
