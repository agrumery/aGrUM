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
* @brief Template implementation of FMDP/planning/RMaxMDDPlaner.h classes.
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
#include <agrum/core/functors.h>
// =========================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionGraph.h>
#include <agrum/multidim/decisionGraphUtilities/multiDimDecisionGraphOperator.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/rmaxmddplaner.h>
#include <agrum/FMDP/planning/operators/regress.h>
// =========================================================================

/// For shorter line and hence more comprehensive code only
#define RECAST(x) reinterpret_cast<const MultiDimDecisionGraph<GUM_SCALAR>*>(x)

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
    RMaxMDDPlaner<GUM_SCALAR>::RMaxMDDPlaner ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, SPIMDDI* spim, double minExplo, GUM_SCALAR epsilon ) : SPUMDD<GUM_SCALAR>(fmdp, epsilon),
                                                                                                                                            __spim(spim),
                                                                                                                                            __minExplo(minExplo){

      GUM_CONSTRUCTOR ( RMaxMDDPlaner );
    }

    // ===========================================================================
    // Default destructor
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    RMaxMDDPlaner<GUM_SCALAR>::~RMaxMDDPlaner() {
      GUM_DESTRUCTOR ( RMaxMDDPlaner );
      for( SequenceIteratorSafe<Idx> actionIter = this->_fmdp->beginActions(); actionIter != this->_fmdp->endActions(); ++actionIter){
        delete __rmaxMap[*actionIter];
        delete __dispatchMap[*actionIter];
      }
    }



  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                                   Planning Methods                                              **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Initializes data structure needed for making the planning
    // ===========================================================================
    template<typename GUM_SCALAR>
    void RMaxMDDPlaner<GUM_SCALAR>::initialize(  ) {
      SPUMDD<GUM_SCALAR>::initialize();

      for( SequenceIteratorSafe<Idx> actionIter = this->_fmdp->beginActions(); actionIter != this->_fmdp->endActions(); ++actionIter){
        __rmaxMap.insert(*actionIter, new  MultiDimDecisionGraph<GUM_SCALAR>());
        __dispatchMap.insert(*actionIter, new  MultiDimDecisionGraph<GUM_SCALAR>());
      }
    }



  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                                   Value Iteration Methods                                       **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Performs a single step of value iteration
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph< GUM_SCALAR >* RMaxMDDPlaner<GUM_SCALAR>::_valueIteration() {

      for( SequenceIteratorSafe<Idx> actionIter = this->_fmdp->beginActions(); actionIter != this->_fmdp->endActions(); ++actionIter ){
        __rmaxMap[*actionIter]->clear();
        __dispatchMap[*actionIter]->clear();

        MultiDimDecisionGraph<GUM_SCALAR>* dsa =  new  MultiDimDecisionGraph<GUM_SCALAR>();
        dsa->manager()->setRootNode(dsa->manager()->addTerminalNode(std::numeric_limits<GUM_SCALAR>::max()));
        for( SequenceIteratorSafe<const DiscreteVariable*> varIter = this->_fmdp->beginVariables(); varIter != this->_fmdp->endVariables(); ++varIter ){
          dsa = _minimizes( __spim->extractCount(*actionIter, *varIter), dsa );
        }

        __rmaxMap[*actionIter]->clear();
        __dispatchMap[*actionIter]->clear();
        for( SequenceIteratorSafe<const DiscreteVariable*> varIter = dsa->variablesSequence().beginSafe();
                varIter != dsa->variablesSequence().endSafe(); ++varIter ){
          __rmaxMap[*actionIter]->add(**varIter);
          __dispatchMap[*actionIter]->add(**varIter);
        }
        __rmaxMap[*actionIter]->manager()->setRootNode(
              __createRMax(dsa->root(), dsa, __rmaxMap[*actionIter], std::numeric_limits<double>::min(), __spim->rMax()/(1-this->_fmdp->discount())) );
        __dispatchMap[*actionIter]->manager()->setRootNode( __createRMax(dsa->root(), dsa, __dispatchMap[*actionIter], 1.0, 0.0) );

        delete dsa;
      }


      // *****************************************************************************************
      // Loop reset
      MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = new MultiDimDecisionGraph< GUM_SCALAR >();
      newVFunction->copyAndReassign ( *this->_vFunction, this->_fmdp->mapMainPrime() );

      // *****************************************************************************************
      // For each action
      std::vector<MultiDimDecisionGraph<GUM_SCALAR>*> qActionsSet;
      for ( auto actionIter = this->_fmdp->beginActions(); actionIter != this->_fmdp->endActions(); ++actionIter  ) {
        MultiDimDecisionGraph<GUM_SCALAR>* qAction = _evalQaction( newVFunction, *actionIter );
        qActionsSet.push_back(qAction);
      }
      delete newVFunction;

      // *****************************************************************************************
      // Next to evaluate main value function, we take maximise over all action value, ...
      newVFunction = this->_maximiseQactions(qActionsSet);

      return newVFunction;
    }


    // ===========================================================================
    // Evals the q function for current fmdp action
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph< GUM_SCALAR >*
    RMaxMDDPlaner<GUM_SCALAR>::_evalQaction( const MultiDimDecisionGraph< GUM_SCALAR >* Vold, Idx actionId ){
      MultiDimDecisionGraph<GUM_SCALAR>* qAction = SPUMDD<GUM_SCALAR>::_evalQaction(Vold, actionId);
      qAction = this->_addReward(qAction);
      qAction = _multiply( qAction, __dispatchMap[actionId] );
      qAction = _rmaximize( qAction, __rmaxMap[actionId] );
      return qAction;
    }



  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                                   Optimal Policy Evaluation Methods                             **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Evals the policy corresponding to the given value function
    // ===========================================================================
    template<typename GUM_SCALAR>
    void
    RMaxMDDPlaner<GUM_SCALAR>::_evalPolicy (  ) {

      // *****************************************************************************************
      // Loop reset
      MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = new MultiDimDecisionGraph< GUM_SCALAR >();
      newVFunction->copyAndReassign ( *this->_vFunction, this->_fmdp->mapMainPrime() );

      std::vector<MultiDimDecisionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*> argMaxQActionsSet;
      // *****************************************************************************************
      // For each action
      for ( auto actionIter = this->_fmdp->beginActions(); actionIter != this->_fmdp->endActions(); ++actionIter  ) {

        MultiDimDecisionGraph<GUM_SCALAR>* qAction = _evalQaction( newVFunction, *actionIter );

        argMaxQActionsSet.push_back( this->_makeArgMax(qAction, *actionIter) );
      }
      delete newVFunction;


      // *****************************************************************************************
      // Next to evaluate main value function, we take maximise over all action value, ...
      MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >*
          argMaxVFunction = this->_argmaximiseQactions( argMaxQActionsSet );

      // *****************************************************************************************
      // Next to evaluate main value function, we take maximise over all action value, ...
      this->_extractOptimalPolicy ( argMaxVFunction ) ;
    }



  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                              Graph Function Operations Methods                                  **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ==========================================================================
    // Minimizes between d(X_i | s,a) and d(s|a)
    // @param dxisa : d(X_i | s,a)
    // @param dsa : d(s|a)
    // @warning given d(s|a) and  d(X_i | s,a) are deleted, returns the new one
    // ==========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph<GUM_SCALAR>* RMaxMDDPlaner<GUM_SCALAR>::_minimizes(const MultiDimDecisionGraph< GUM_SCALAR >* dxisa,
                                                                             const MultiDimDecisionGraph< GUM_SCALAR >* dsa){
      MultiDimDecisionGraph<GUM_SCALAR>* ret = minimize2MultiDimDecisionGraphs ( dxisa, dsa );
      delete dxisa;
      delete dsa;
      return ret;
    }


    // ==========================================================================
    // Maximizes between QAction and VFunction
    // @param qAction : the computed Q(s,a)
    // @param vFunction : the vFunction so far
    // @warning given vFunction and qAction are deleted, returns the new one
    // ==========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph<GUM_SCALAR>* RMaxMDDPlaner<GUM_SCALAR>::_rmaximize( const MultiDimDecisionGraph< GUM_SCALAR >* qAction,
                                                                              const MultiDimDecisionGraph< GUM_SCALAR >* rMax){
      MultiDimDecisionGraph<GUM_SCALAR>* ret = maximize2MultiDimDecisionGraphs ( qAction, rMax );
      delete qAction;
      return ret;
    }

    // ==========================================================================
    // Maximizes between QAction and its dispatch
    // @param qAction : the computed Q(s,a)
    // @param dispatch : for the maximization xwith rmax after
    // @warning given  qAction is deleted, returns the new one
    // ==========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph<GUM_SCALAR>* RMaxMDDPlaner<GUM_SCALAR>::_multiply(const MultiDimDecisionGraph< GUM_SCALAR >* qAction,
                                                                            const MultiDimDecisionGraph< GUM_SCALAR >* dispatch){
      MultiDimDecisionGraph<GUM_SCALAR>* ret = multiply2MultiDimDecisionGraphs( qAction, dispatch );
      delete qAction;
      return ret;
    }



  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                                        RMax Methods                                             **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Evals the q function for current fmdp action
    // ===========================================================================
    template<typename GUM_SCALAR>
    NodeId RMaxMDDPlaner<GUM_SCALAR>::__createRMax(NodeId currentNodeId,
                                              MultiDimDecisionGraph<GUM_SCALAR>* dTemp,
                                              MultiDimDecisionGraph<GUM_SCALAR>* sortie,
                                              double valSi,
                                              double valSinon) {

      if( dTemp->isTerminalNode(currentNodeId) ){
        if( dTemp->nodeValue(currentNodeId) >= __minExplo )
          return sortie->manager()->addTerminalNode( valSi );
        else
          return sortie->manager()->addTerminalNode( valSinon );
      }

      const InternalNode* currentNode = dTemp->node(currentNodeId);
      NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*currentNode->nodeVar()->domainSize()) );
      for( Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda )
        sonsMap[moda] = __createRMax(currentNode->son(moda), dTemp, sortie, valSi, valSinon);

      return sortie->manager()->addNonTerminalNode(currentNode->nodeVar(), sonsMap);
    }

} // end of namespace gum
