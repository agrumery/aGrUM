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
* @brief Template implementation of AdaptiveRMaxPlaner classes.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

// =========================================================================
#include <cmath>
#include <queue>
#include <vector>
//#include <algorithm>
//#include <utility>
// =========================================================================
#include <agrum/core/functors.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimFunctionGraph.h>
#include <agrum/multidim/potential.h>
// =========================================================================
#include <agrum/FMDP/planning/adaptiveRMaxPlaner.h>
// =========================================================================

/// For shorter line and hence more comprehensive code purposes only
#define RECASTED( x ) reinterpret_cast<const MultiDimFunctionGraph<double>*>( x )

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
  AdaptiveRMaxPlaner::AdaptiveRMaxPlaner( IOperatorStrategy<double>* opi,
                                          double                   discountFactor,
                                          double                   epsilon,
                                          const ILearningStrategy* learner,
                                          bool                     verbose )
      : StructuredPlaner( opi, discountFactor, epsilon, verbose )
      , IDecisionStrategy()
      , __fmdpLearner( learner )
      , __initialized( false ) {

    GUM_CONSTRUCTOR( AdaptiveRMaxPlaner );
  }

  // ===========================================================================
  // Default destructor
  // ===========================================================================
  AdaptiveRMaxPlaner::~AdaptiveRMaxPlaner() {

    GUM_DESTRUCTOR( AdaptiveRMaxPlaner );

    for ( HashTableIteratorSafe<Idx, StatesCounter*> scIter =
              __counterTable.beginSafe();
          scIter != __counterTable.endSafe();
          ++scIter )
      delete scIter.val();
  }

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                     Planning Methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ==========================================================================
  // Initializes data structure needed for making the planning
  // ==========================================================================
  void AdaptiveRMaxPlaner::initialize( const FMDP<double>* fmdp ) {
    if ( !__initialized ) {
      StructuredPlaner::initialize( fmdp );
      IDecisionStrategy::initialize( fmdp );
      for ( auto actionIter = fmdp->beginActions();
            actionIter != fmdp->endActions();
            ++actionIter ) {
        __counterTable.insert( *actionIter, new StatesCounter() );
        __initializedTable.insert( *actionIter, false );
      }
      __initialized = true;
    }
  }

  // ===========================================================================
  // Performs a value iteration
  // ===========================================================================
  void AdaptiveRMaxPlaner::makePlanning( Idx nbStep ) {

    __makeRMaxFunctionGraphs();

    StructuredPlaner::makePlanning( nbStep );

    __clearTables();
  }

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                   Value Iteration Methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Performs a single step of value iteration
  // ===========================================================================
  void AdaptiveRMaxPlaner::_initVFunction() {
    _vFunction->manager()->setRootNode(
        _vFunction->manager()->addTerminalNode( 0.0 ) );
    for ( auto actionIter = _fmdp->beginActions();
          actionIter != _fmdp->endActions();
          ++actionIter )
      _vFunction = this->_operator->add(
          _vFunction, RECASTED( this->_fmdp->reward( *actionIter ) ), 1 );
  }

  // ===========================================================================
  // Performs a single step of value iteration
  // ===========================================================================
  MultiDimFunctionGraph<double>* AdaptiveRMaxPlaner::_valueIteration() {

    // *****************************************************************************************
    // Loop reset
    MultiDimFunctionGraph<double>* newVFunction = _operator->getFunctionInstance();
    newVFunction->copyAndReassign( *_vFunction, _fmdp->mapMainPrime() );

    // *****************************************************************************************
    // For each action
    std::vector<MultiDimFunctionGraph<double>*> qActionsSet;
    for ( auto actionIter = _fmdp->beginActions();
          actionIter != _fmdp->endActions();
          ++actionIter ) {
      MultiDimFunctionGraph<double>* qAction =
          _evalQaction( newVFunction, *actionIter );

      // *******************************************************************************************
      // Next, we add the reward
      qAction = _addReward( qAction, *actionIter );

      qAction = this->_operator->maximize(
          __actionsRMaxTable[*actionIter],
          this->_operator->multiply( qAction, __actionsBoolTable[*actionIter], 1 ),
          2 );

      qActionsSet.push_back( qAction );
    }
    delete newVFunction;

    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    newVFunction = _maximiseQactions( qActionsSet );

    return newVFunction;
  }

  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                   Optimal Policy Evaluation Methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Evals the policy corresponding to the given value function
  // ===========================================================================
  void AdaptiveRMaxPlaner::_evalPolicy() {

    // *****************************************************************************************
    // Loop reset
    MultiDimFunctionGraph<double>* newVFunction = _operator->getFunctionInstance();
    newVFunction->copyAndReassign( *_vFunction, _fmdp->mapMainPrime() );

    std::vector<
        MultiDimFunctionGraph<ArgMaxSet<double, Idx>, SetTerminalNodePolicy>*>
        argMaxQActionsSet;
    // *****************************************************************************************
    // For each action
    for ( auto actionIter = _fmdp->beginActions();
          actionIter != _fmdp->endActions();
          ++actionIter ) {

      MultiDimFunctionGraph<double>* qAction =
          this->_evalQaction( newVFunction, *actionIter );

      qAction = this->_addReward( qAction, *actionIter );

      qAction = this->_operator->maximize(
          __actionsRMaxTable[*actionIter],
          this->_operator->multiply( qAction, __actionsBoolTable[*actionIter], 1 ),
          2 );

      argMaxQActionsSet.push_back( _makeArgMax( qAction, *actionIter ) );
    }
    delete newVFunction;

    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    MultiDimFunctionGraph<ArgMaxSet<double, Idx>, SetTerminalNodePolicy>*
        argMaxVFunction = _argmaximiseQactions( argMaxQActionsSet );

    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    _extractOptimalPolicy( argMaxVFunction );
  }

  // ===========================================================================
  //
  // ===========================================================================
  void AdaptiveRMaxPlaner::__makeRMaxFunctionGraphs() {

    __rThreshold =
        __fmdpLearner->modaMax() * 5 > 30 ? __fmdpLearner->modaMax() * 5 : 30;
    __rmax = __fmdpLearner->rMax() / ( 1.0 - this->_discountFactor );

    for ( auto actionIter = this->fmdp()->beginActions();
          actionIter != this->fmdp()->endActions();
          ++actionIter ) {

      std::vector<MultiDimFunctionGraph<double>*> rmaxs;
      std::vector<MultiDimFunctionGraph<double>*> boolQs;

      for ( auto varIter = this->fmdp()->beginVariables();
            varIter != this->fmdp()->endVariables();
            ++varIter ) {

        const IVisitableGraphLearner* visited = __counterTable[*actionIter];

        MultiDimFunctionGraph<double>* varRMax =
            this->_operator->getFunctionInstance();
        MultiDimFunctionGraph<double>* varBoolQ =
            this->_operator->getFunctionInstance();

        visited->insertSetOfVars( varRMax );
        visited->insertSetOfVars( varBoolQ );

        std::pair<NodeId, NodeId> rooty =
            __visitLearner( visited, visited->root(), varRMax, varBoolQ );
        varRMax->manager()->setRootNode( rooty.first );
        varRMax->manager()->reduce();
        varRMax->manager()->clean();
        varBoolQ->manager()->setRootNode( rooty.second );
        varBoolQ->manager()->reduce();
        varBoolQ->manager()->clean();

        rmaxs.push_back( varRMax );
        boolQs.push_back( varBoolQ );

        //          std::cout << RECASTED(this->_fmdp->transition(*actionIter,
        //          *varIter))->toDot() << std::endl;
        //          for( auto varIter2 =
        //          RECASTED(this->_fmdp->transition(*actionIter,
        //          *varIter))->variablesSequence().beginSafe(); varIter2 !=
        //          RECASTED(this->_fmdp->transition(*actionIter,
        //          *varIter))->variablesSequence().endSafe(); ++varIter2 )
        //              std::cout << (*varIter2)->name() << " | ";
        //          std::cout << std::endl;

        //          std::cout << varRMax->toDot() << std::endl;
        //          for( auto varIter =
        //          varRMax->variablesSequence().beginSafe(); varIter !=
        //          varRMax->variablesSequence().endSafe(); ++varIter )
        //              std::cout << (*varIter)->name() << " | ";
        //          std::cout << std::endl;

        //          std::cout << varBoolQ->toDot() << std::endl;
        //          for( auto varIter =
        //          varBoolQ->variablesSequence().beginSafe(); varIter !=
        //          varBoolQ->variablesSequence().endSafe(); ++varIter )
        //              std::cout << (*varIter)->name() << " | ";
        //          std::cout << std::endl;
      }

      //        std::cout << "Maximising" << std::endl;
      __actionsRMaxTable.insert( *actionIter, this->_maximiseQactions( rmaxs ) );
      __actionsBoolTable.insert( *actionIter, this->_minimiseFunctions( boolQs ) );
    }
  }

  // ===========================================================================
  //
  // ===========================================================================
  std::pair<NodeId, NodeId>
  AdaptiveRMaxPlaner::__visitLearner( const IVisitableGraphLearner*  visited,
                                      NodeId                         currentNodeId,
                                      MultiDimFunctionGraph<double>* rmax,
                                      MultiDimFunctionGraph<double>* boolQ ) {

    std::pair<NodeId, NodeId> rep;
    if ( visited->isTerminal( currentNodeId ) ) {
      rep.first = rmax->manager()->addTerminalNode(
          visited->nodeNbObservation( currentNodeId ) < __rThreshold ? __rmax
                                                                     : 0.0 );
      rep.second = boolQ->manager()->addTerminalNode(
          visited->nodeNbObservation( currentNodeId ) < __rThreshold ? 0.0 : 1.0 );
      return rep;
    }

    NodeId* rmaxsons = static_cast<NodeId*>( SOA_ALLOCATE(
        sizeof( NodeId ) * visited->nodeVar( currentNodeId )->domainSize() ) );
    NodeId* bqsons = static_cast<NodeId*>( SOA_ALLOCATE(
        sizeof( NodeId ) * visited->nodeVar( currentNodeId )->domainSize() ) );

    for ( Idx moda = 0; moda < visited->nodeVar( currentNodeId )->domainSize();
          ++moda ) {
      std::pair<NodeId, NodeId> sonp = __visitLearner(
          visited, visited->nodeSon( currentNodeId, moda ), rmax, boolQ );
      rmaxsons[moda] = sonp.first;
      bqsons[moda] = sonp.second;
    }

    rep.first = rmax->manager()->addInternalNode(
        visited->nodeVar( currentNodeId ), rmaxsons );
    rep.second = boolQ->manager()->addInternalNode(
        visited->nodeVar( currentNodeId ), bqsons );
    return rep;
  }

  // ===========================================================================
  //
  // ===========================================================================
  void AdaptiveRMaxPlaner::__clearTables() {

    for ( auto actionIter = this->fmdp()->beginActions();
          actionIter != this->fmdp()->endActions();
          ++actionIter ) {
      delete __actionsBoolTable[*actionIter];
      delete __actionsRMaxTable[*actionIter];
    }
    __actionsRMaxTable.clear();
    __actionsBoolTable.clear();
  }

}  // end of namespace gum
