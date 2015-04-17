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
* @brief Template implementation of FMDP/planning/SVI.h classes.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

// =========================================================================
#include <math.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <utility>
// =========================================================================
#include <agrum/multidim/instantiation.h>
// =========================================================================
#include <agrum/FMDP/planning/svi.h>
#include <agrum/FMDP/planning/operators/treeRegress.h>
#include <agrum/FMDP/planning/operators/treeOperator.h>
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
    SVI<GUM_SCALAR>::SVI ( FMDP<GUM_SCALAR>* fmdp,
                                 GUM_SCALAR epsilon ) : AbstractSVI<GUM_SCALAR>(fmdp, epsilon) {
      GUM_CONSTRUCTOR ( SVI );
    }

    // ===========================================================================
    // Default destructor
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    SVI<GUM_SCALAR>::~SVI() {
      GUM_DESTRUCTOR ( SVI );
    }



  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                             Graph Function Operations Methods                                   **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Evals the q function for current fmdp action
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimFunctionGraph< GUM_SCALAR >*
    SVI<GUM_SCALAR>::_evalQaction( const MultiDimFunctionGraph< GUM_SCALAR >* Vold, Idx actionId ){

      // ******************************************************************************
      // Initialisation :
      // Creating a copy of last Vfunction to deduce from the new Qaction
      // And finding the first var to eleminate (the one at the end)
      Bijection<const DiscreteVariable*, const MultiDimFunctionGraph<GUM_SCALAR> *> pxi;
      for( SequenceIteratorSafe<const DiscreteVariable*> varIter = Vold->variablesSequence().beginSafe();
           varIter != Vold->variablesSequence().endSafe(); ++varIter ){
        pxi.insert( *varIter, RECAST(this->_fmdp->transition( actionId, this->_fmdp->mapMainPrime().first(*varIter )) ) );
      }

      TreeRegress<GUM_SCALAR, std::multiplies, std::plus> tr( Vold, pxi );//, Vold->variablesSequence(), (GUM_SCALAR) 0.0 );
      return tr.compute();
    }


    // ==========================================================================
    /// Performs a multiplication/projection on given qAction
    /// @param qAction : the computed Q(s,a)
    /// @param pxip : the transition probabilities for the xip variable
    /// @param xip : the variable we eliminate on the projection
    /// @warning given qAction is deleted, return the new one
    // ==========================================================================
    template<typename GUM_SCALAR>
    MultiDimFunctionGraph<GUM_SCALAR>* SVI<GUM_SCALAR>::_regress( const MultiDimFunctionGraph< GUM_SCALAR >* qAction,
                                                                  const MultiDimFunctionGraph< GUM_SCALAR >* pxi,
                                                                  const DiscreteVariable* xip){
      return nullptr;
    }

    // ==========================================================================
    /// Maximizes between QAction and VFunction
    /// @param qAction : the computed Q(s,a)
    /// @param vFunction : the vFunction so far
    /// @warning given vFunction and qAction are deleted, returns the new one
    // ==========================================================================
    template<typename GUM_SCALAR>
    MultiDimFunctionGraph<GUM_SCALAR>* SVI<GUM_SCALAR>::_maximize(const MultiDimFunctionGraph< GUM_SCALAR >* vFunction,
                                                                  const MultiDimFunctionGraph< GUM_SCALAR >* qAction){

      TreeOperator< GUM_SCALAR, Maximizes > argmaxope( vFunction, qAction );
      MultiDimFunctionGraph<GUM_SCALAR>* ret = argmaxope.compute();
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
    MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* SVI<GUM_SCALAR>::_argmaximize(
                        const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* vFunction,
                        const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* qAction){

      TreeOperator< ArgMaxSet<GUM_SCALAR, Idx>, ArgumentMaximisesAction, SetTerminalNodePolicy > argmaxope(
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
    MultiDimFunctionGraph<GUM_SCALAR>* SVI<GUM_SCALAR>::_add( const MultiDimFunctionGraph< GUM_SCALAR >* function,
                                                              const MultiDimFunctionGraph< GUM_SCALAR >* reward){

      TreeOperator< GUM_SCALAR, std::plus > argmaxope( function, reward );
      MultiDimFunctionGraph<GUM_SCALAR>* ret = argmaxope.compute();
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
    MultiDimFunctionGraph<GUM_SCALAR>* SVI<GUM_SCALAR>::_subtract(const MultiDimFunctionGraph< GUM_SCALAR >* newVF,
                                                                  const MultiDimFunctionGraph< GUM_SCALAR >* oldVF){

      TreeOperator< GUM_SCALAR, std::minus > argmaxope( newVF, oldVF );
      return argmaxope.compute();
    }


} // end of namespace gum
