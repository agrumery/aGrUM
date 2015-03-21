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
* @brief Template implementation of FMDP/planning/RMaxMDD.h classes.
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
#include <agrum/FMDP/planning/RMaxMDD.h>
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
    RMaxMDD<GUM_SCALAR>::RMaxMDD ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, SPIMDDI* spim, double minExplo, GUM_SCALAR epsilon ) : SPUMDD<GUM_SCALAR>(fmdp, epsilon),
                                                                                                                                            __spim(spim),
                                                                                                                                            __minExplo(minExplo){

      GUM_CONSTRUCTOR ( RMaxMDD );
    }

    // ===========================================================================
    // Default destructor
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    RMaxMDD<GUM_SCALAR>::~RMaxMDD() {
      GUM_DESTRUCTOR ( RMaxMDD );
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
    void RMaxMDD<GUM_SCALAR>::initialize(  ) {
      SPUMDD<GUM_SCALAR>::initialize();

      for( SequenceIteratorSafe<Idx> actionIter = this->_fmdp->beginActions(); actionIter != this->_fmdp->endActions(); ++actionIter){
        __rmaxMap.insert(*actionIter, new  MultiDimDecisionGraph<GUM_SCALAR>());
        __dispatchMap.insert(*actionIter, new  MultiDimDecisionGraph<GUM_SCALAR>());
      }
    }


    // ===========================================================================
    // Performs a single step of value iteration
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph< GUM_SCALAR >* RMaxMDD<GUM_SCALAR>::_valueIteration() {

      for( SequenceIteratorSafe<Idx> actionIter = this->_fmdp->beginActions(); actionIter != this->_fmdp->endActions(); ++actionIter ){
        __rmaxMap[*actionIter]->clear();
        __dispatchMap[*actionIter]->clear();

        MultiDimDecisionGraph<GUM_SCALAR>* dTemp =  new  MultiDimDecisionGraph<GUM_SCALAR>();
        dTemp->manager()->setRootNode(dTemp->manager()->addTerminalNode(std::numeric_limits<GUM_SCALAR>::max()));
        for( SequenceIteratorSafe<const DiscreteVariable*> varIter = this->_fmdp->beginVariables(); varIter != this->_fmdp->endVariables(); ++varIter ){
          MultiDimDecisionGraph<GUM_SCALAR>* temp = dTemp;
          MultiDimDecisionGraph<GUM_SCALAR>* counter = __spim->extractCount(*actionIter, *varIter);
          dTemp = minimize2MultiDimDecisionGraphs( dTemp, counter );
          delete temp;
          delete counter;
        }

        __rmaxMap[*actionIter]->clear();
        __dispatchMap[*actionIter]->clear();
        for( SequenceIteratorSafe<const DiscreteVariable*> varIter = dTemp->variablesSequence().beginSafe();
                varIter != dTemp->variablesSequence().endSafe(); ++varIter ){
          __rmaxMap[*actionIter]->add(**varIter);
          __dispatchMap[*actionIter]->add(**varIter);
        }
        __rmaxMap[*actionIter]->manager()->setRootNode(
              __createRMax(dTemp->root(), dTemp, __rmaxMap[*actionIter], std::numeric_limits<double>::min(), __spim->rMax()/(1-this->_fmdp->discount())) );
        __dispatchMap[*actionIter]->manager()->setRootNode( __createRMax(dTemp->root(), dTemp, __dispatchMap[*actionIter], 1.0, 0.0) );

        delete dTemp;
      }


      return SPUMDD<GUM_SCALAR>::_valueIteration();
    }


    template<typename GUM_SCALAR>
    NodeId RMaxMDD<GUM_SCALAR>::__createRMax(NodeId currentNodeId,
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

    // ===========================================================================
    // Evals the q function for current fmdp action
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph< GUM_SCALAR >*
    RMaxMDD<GUM_SCALAR>::_evalQaction( const MultiDimDecisionGraph< GUM_SCALAR >* Vold, Idx actionId ){
      MultiDimDecisionGraph<GUM_SCALAR>* qAction = SPUMDD<GUM_SCALAR>::_evalQaction(Vold, actionId);
      MultiDimDecisionGraph<GUM_SCALAR>* temp = qAction;
      qAction = this->_addReward(qAction);
      delete temp;
      temp = qAction;
      qAction = multiply2MultiDimDecisionGraphs( qAction, __dispatchMap[actionId] );
      delete temp;
      temp = qAction;
      qAction = maximize2MultiDimDecisionGraphs( qAction, __rmaxMap[actionId] );
      delete temp;
      return qAction;
    }

} // end of namespace gum
