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
* @brief Template implementation of FMDP/planning/StructuredPlaner.h classes.
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
// =========================================================================
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimFunctionGraph.h>
#include <agrum/multidim/potential.h>
// =========================================================================
#include <agrum/FMDP/planning/structuredPlaner.h>
// =========================================================================

/// For shorter line and hence more comprehensive code purposes only
#define RECAST( x ) reinterpret_cast<const MultiDimFunctionGraph<GUM_SCALAR>*>( x )

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
  template <typename GUM_SCALAR>
  INLINE StructuredPlaner<GUM_SCALAR>::StructuredPlaner(
      IOperatorStrategy<GUM_SCALAR>* opi,
      GUM_SCALAR                     discountFactor,
      GUM_SCALAR                     epsilon,
      bool                           verbose )
      : _discountFactor( discountFactor )
      , _operator( opi )
      , _verbose( verbose ) {

    GUM_CONSTRUCTOR( StructuredPlaner );

    __threshold = epsilon;
    _vFunction = nullptr;
    _optimalPolicy = nullptr;
  }

  // ===========================================================================
  // Default destructor
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE StructuredPlaner<GUM_SCALAR>::~StructuredPlaner() {

    GUM_DESTRUCTOR( StructuredPlaner );

    if ( _vFunction ) {
      delete _vFunction;
    }

    if ( _optimalPolicy ) delete _optimalPolicy;

    delete _operator;
  }


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                Datastructure access methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Initializes data structure needed for making the planning
  // ===========================================================================
  template <typename GUM_SCALAR>
  std::string StructuredPlaner<GUM_SCALAR>::optimalPolicy2String() {

    // ************************************************************************
    // Discarding the case where no \pi* have been computed
    if ( !_optimalPolicy || _optimalPolicy->root() == 0 )
      return "NO OPTIMAL POLICY CALCULATED YET";

    // ************************************************************************
    // Initialisation

    // Declaration of the needed string stream
    std::stringstream output;
    std::stringstream terminalStream;
    std::stringstream nonTerminalStream;
    std::stringstream arcstream;

    // First line for the toDot
    output << std::endl << "digraph \" OPTIMAL POLICY \" {" << std::endl;

    // Form line for the internal node stream en the terminal node stream
    terminalStream << "node [shape = box];" << std::endl;
    nonTerminalStream << "node [shape = ellipse];" << std::endl;

    // For somme clarity in the final string
    std::string tab = "\t";

    // To know if we already checked a node or not
    Set<NodeId> visited;

    // FIFO of nodes to visit
    std::queue<NodeId> fifo;

    // Loading the FIFO
    fifo.push( _optimalPolicy->root() );
    visited << _optimalPolicy->root();


    // ************************************************************************
    // Main loop
    while ( !fifo.empty() ) {

      // Node to visit
      NodeId currentNodeId = fifo.front();
      fifo.pop();

      // Checking if it is terminal
      if ( _optimalPolicy->isTerminalNode( currentNodeId ) ) {

        // Get back the associated ActionSet
        ActionSet ase = _optimalPolicy->nodeValue( currentNodeId );

        // Creating a line for this node
        terminalStream << tab << currentNodeId << ";" << tab << currentNodeId
                       << " [label=\"" << currentNodeId << " - ";

        // Enumerating and adding to the line the associated optimal actions
        for ( SequenceIteratorSafe<Idx> valIter = ase.beginSafe();
              valIter != ase.endSafe();
              ++valIter )
          terminalStream << _fmdp->actionName( *valIter ) << " ";

        // Terminating line
        terminalStream << "\"];" << std::endl;
        continue;
      }

      // Either wise
      {
        // Geting back the associated internal node
        const InternalNode* currentNode = _optimalPolicy->node( currentNodeId );

        // Creating a line in internalnode stream for this node
        nonTerminalStream << tab << currentNodeId << ";" << tab << currentNodeId
                          << " [label=\"" << currentNodeId << " - "
                          << currentNode->nodeVar()->name() << "\"];" << std::endl;

        // Going through the sons and agregating them according the the sons Ids
        HashTable<NodeId, LinkedList<Idx>*> sonMap;
        for ( Idx sonIter = 0; sonIter < currentNode->nbSons(); ++sonIter ) {
          if ( !visited.exists( currentNode->son( sonIter ) ) ) {
            fifo.push( currentNode->son( sonIter ) );
            visited << currentNode->son( sonIter );
          }
          if ( !sonMap.exists( currentNode->son( sonIter ) ) )
            sonMap.insert( currentNode->son( sonIter ), new LinkedList<Idx>() );
          sonMap[currentNode->son( sonIter )]->addLink( sonIter );
        }

        // Adding to the arc stram
        for ( auto sonIter = sonMap.beginSafe(); sonIter != sonMap.endSafe();
              ++sonIter ) {
          arcstream << tab << currentNodeId << " -> " << sonIter.key()
                    << " [label=\" ";
          Link<Idx>* modaIter = sonIter.val()->list();
          while ( modaIter ) {
            arcstream << currentNode->nodeVar()->label( modaIter->element() );
            if ( modaIter->nextLink() ) arcstream << ", ";
            modaIter = modaIter->nextLink();
          }
          arcstream << "\",color=\"#00ff00\"];" << std::endl;
          delete sonIter.val();
        }
      }
    }

    // Terminating
    output << terminalStream.str() << std::endl
           << nonTerminalStream.str() << std::endl
           << arcstream.str() << std::endl
           << "}" << std::endl;

    return output.str();
  }


  /* **************************************************************************************************
   * **/
  /* ** **/
  /* **                                     Planning Methods **/
  /* ** **/
  /* **************************************************************************************************
   * **/

  // ===========================================================================
  // Initializes data structure needed for making the planning
  // ===========================================================================
  template <typename GUM_SCALAR>
  void StructuredPlaner<GUM_SCALAR>::initialize( const FMDP<GUM_SCALAR>* fmdp ) {

    _fmdp = fmdp;

    // Determination of the threshold value
    __threshold *= ( 1 - _discountFactor ) / ( 2 * _discountFactor );

    // Establishement of sequence of variable elemination
    for ( auto varIter = _fmdp->beginVariables(); varIter != _fmdp->endVariables();
          ++varIter )
      _elVarSeq << _fmdp->main2prime( *varIter );

    // Initialisation of the value function
    _vFunction = _operator->getFunctionInstance();
    _optimalPolicy = _operator->getAggregatorInstance();
    __firstTime = true;
  }


  // ===========================================================================
  // Performs a value iteration
  // ===========================================================================
  template <typename GUM_SCALAR>
  void StructuredPlaner<GUM_SCALAR>::makePlanning( Idx nbStep ) {
    if ( __firstTime ) {
      this->_initVFunction();
      __firstTime = false;
    }

    // *****************************************************************************************
    // Main loop
    // *****************************************************************************************
    Idx        nbIte = 0;
    GUM_SCALAR gap = __threshold + 1;
    while ( ( gap > __threshold ) && ( nbIte < nbStep ) ) {

      ++nbIte;

      MultiDimFunctionGraph<GUM_SCALAR>* newVFunction = this->_valueIteration();

      // *****************************************************************************************
      // Then we compare new value function and the old one
      MultiDimFunctionGraph<GUM_SCALAR>* deltaV =
          _operator->subtract( newVFunction, _vFunction );
      gap = 0;

      for ( deltaV->beginValues(); deltaV->hasValue(); deltaV->nextValue() )
        if ( gap < fabs( deltaV->value() ) ) gap = fabs( deltaV->value() );
      delete deltaV;

      if ( _verbose )
        std::cout << " ------------------- Fin itération n° " << nbIte << std::endl
                  << " Gap : " << gap << " - " << __threshold << std::endl;

      // *****************************************************************************************
      // And eventually we update pointers for next loop
      delete _vFunction;
      _vFunction = newVFunction;
    }

    // *****************************************************************************************
    // Policy matching value function research
    // *****************************************************************************************
    this->_evalPolicy();
  }


  // ===========================================================================
  // Performs a single step of value iteration
  // ===========================================================================
  template <typename GUM_SCALAR>
  void StructuredPlaner<GUM_SCALAR>::_initVFunction() {
    _vFunction->copy( *( RECAST( _fmdp->reward() ) ) );
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
  template <typename GUM_SCALAR>
  MultiDimFunctionGraph<GUM_SCALAR>*
  StructuredPlaner<GUM_SCALAR>::_valueIteration() {

    // *****************************************************************************************
    // Loop reset
    MultiDimFunctionGraph<GUM_SCALAR>* newVFunction =
        _operator->getFunctionInstance();
    newVFunction->copyAndReassign( *_vFunction, _fmdp->mapMainPrime() );

    // *****************************************************************************************
    // For each action
    std::vector<MultiDimFunctionGraph<GUM_SCALAR>*> qActionsSet;
    for ( auto actionIter = _fmdp->beginActions();
          actionIter != _fmdp->endActions();
          ++actionIter ) {
      MultiDimFunctionGraph<GUM_SCALAR>* qAction =
          this->_evalQaction( newVFunction, *actionIter );
      qActionsSet.push_back( qAction );
    }
    delete newVFunction;

    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    newVFunction = this->_maximiseQactions( qActionsSet );

    // *******************************************************************************************
    // Next, we eval the new function value
    newVFunction = this->_addReward( newVFunction );

    return newVFunction;
  }


  // ===========================================================================
  // Evals the q function for current fmdp action
  // ===========================================================================
  template <typename GUM_SCALAR>
  MultiDimFunctionGraph<GUM_SCALAR>* StructuredPlaner<GUM_SCALAR>::_evalQaction(
      const MultiDimFunctionGraph<GUM_SCALAR>* Vold, Idx actionId ) {

    // ******************************************************************************
    // Initialisation :
    // Creating a copy of last Vfunction to deduce from the new Qaction
    // And finding the first var to eleminate (the one at the end)

    return _operator->regress( Vold, actionId, this->_fmdp, this->_elVarSeq );
  }


  // ===========================================================================
  // Maximise the AAction to iobtain the vFunction
  // ===========================================================================
  template <typename GUM_SCALAR>
  MultiDimFunctionGraph<GUM_SCALAR>*
  StructuredPlaner<GUM_SCALAR>::_maximiseQactions(
      std::vector<MultiDimFunctionGraph<GUM_SCALAR>*>& qActionsSet ) {

    MultiDimFunctionGraph<GUM_SCALAR>* newVFunction = qActionsSet.back();
    qActionsSet.pop_back();

    while ( !qActionsSet.empty() ) {
      MultiDimFunctionGraph<GUM_SCALAR>* qAction = qActionsSet.back();
      qActionsSet.pop_back();
      newVFunction = _operator->maximize( newVFunction, qAction );
    }

    return newVFunction;
  }


  // ===========================================================================
  // Maximise the AAction to iobtain the vFunction
  // ===========================================================================
  template <typename GUM_SCALAR>
  MultiDimFunctionGraph<GUM_SCALAR>*
  StructuredPlaner<GUM_SCALAR>::_minimiseFunctions(
      std::vector<MultiDimFunctionGraph<GUM_SCALAR>*>& qActionsSet ) {

    MultiDimFunctionGraph<GUM_SCALAR>* newVFunction = qActionsSet.back();
    qActionsSet.pop_back();

    while ( !qActionsSet.empty() ) {
      MultiDimFunctionGraph<GUM_SCALAR>* qAction = qActionsSet.back();
      qActionsSet.pop_back();
      newVFunction = _operator->minimize( newVFunction, qAction );
    }

    return newVFunction;
  }


  // ===========================================================================
  // Updates the value function by multiplying by discount and adding reward
  // ===========================================================================
  template <typename GUM_SCALAR>
  MultiDimFunctionGraph<GUM_SCALAR>* StructuredPlaner<GUM_SCALAR>::_addReward(
      MultiDimFunctionGraph<GUM_SCALAR>* Vold, Idx actionId ) {

    // *****************************************************************************************
    // ... we multiply the result by the discount factor, ...
    MultiDimFunctionGraph<GUM_SCALAR>* newVFunction =
        _operator->getFunctionInstance();
    newVFunction->copyAndMultiplyByScalar( *Vold, this->_discountFactor );
    delete Vold;

    // *****************************************************************************************
    // ... and finally add reward
    newVFunction =
        _operator->add( newVFunction, RECAST( _fmdp->reward( actionId ) ) );

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
  template <typename GUM_SCALAR>
  void StructuredPlaner<GUM_SCALAR>::_evalPolicy() {

    // *****************************************************************************************
    // Loop reset
    MultiDimFunctionGraph<GUM_SCALAR>* newVFunction =
        _operator->getFunctionInstance();
    newVFunction->copyAndReassign( *_vFunction, _fmdp->mapMainPrime() );

    std::vector<
        MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*>
        argMaxQActionsSet;
    // *****************************************************************************************
    // For each action
    for ( auto actionIter = _fmdp->beginActions();
          actionIter != _fmdp->endActions();
          ++actionIter ) {

      MultiDimFunctionGraph<GUM_SCALAR>* qAction =
          this->_evalQaction( newVFunction, *actionIter );

      qAction = this->_addReward( qAction );

      argMaxQActionsSet.push_back( _makeArgMax( qAction, *actionIter ) );
    }
    delete newVFunction;


    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*
        argMaxVFunction = _argmaximiseQactions( argMaxQActionsSet );

    // *****************************************************************************************
    // Next to evaluate main value function, we take maximise over all action
    // value, ...
    _extractOptimalPolicy( argMaxVFunction );
  }


  // ===========================================================================
  // Creates a copy of given in parameter decision Graph and replaces leaves
  // of that Graph by a pair containing value of the leaf and action to which
  // is bind this Graph (given in parameter).
  // ===========================================================================
  template <typename GUM_SCALAR>
  MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*
  StructuredPlaner<GUM_SCALAR>::_makeArgMax(
      const MultiDimFunctionGraph<GUM_SCALAR>* qAction, Idx actionId ) {

    MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*
        amcpy = _operator->getArgMaxFunctionInstance();

    // Insertion des nouvelles variables
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              qAction->variablesSequence().beginSafe();
          varIter != qAction->variablesSequence().endSafe();
          ++varIter )
      amcpy->add( **varIter );

    HashTable<NodeId, NodeId> src2dest;
    amcpy->manager()->setRootNode(
        __recurArgMaxCopy( qAction->root(), actionId, qAction, amcpy, src2dest ) );

    delete qAction;
    return amcpy;
  }


  // ==========================================================================
  // Recursion part for the createArgMaxCopy
  // ==========================================================================
  template <typename GUM_SCALAR>
  NodeId StructuredPlaner<GUM_SCALAR>::__recurArgMaxCopy(
      NodeId                                   currentNodeId,
      Idx                                      actionId,
      const MultiDimFunctionGraph<GUM_SCALAR>* src,
      MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*
          argMaxCpy,
      HashTable<NodeId, NodeId>& visitedNodes ) {

    if ( visitedNodes.exists( currentNodeId ) ) return visitedNodes[currentNodeId];

    NodeId nody;
    if ( src->isTerminalNode( currentNodeId ) ) {
      ArgMaxSet<GUM_SCALAR, Idx> leaf( src->nodeValue( currentNodeId ), actionId );
      nody = argMaxCpy->manager()->addTerminalNode( leaf );
    } else {
      const InternalNode* currentNode = src->node( currentNodeId );
      NodeId*             sonsMap = static_cast<NodeId*>( SOA_ALLOCATE(
          sizeof( NodeId ) * currentNode->nodeVar()->domainSize() ) );
      for ( Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda )
        sonsMap[moda] = __recurArgMaxCopy(
            currentNode->son( moda ), actionId, src, argMaxCpy, visitedNodes );
      nody =
          argMaxCpy->manager()->addInternalNode( currentNode->nodeVar(), sonsMap );
    }
    visitedNodes.insert( currentNodeId, nody );
    return nody;
  }


  // ===========================================================================
  // Performs argmax_a Q(s,a)
  // ===========================================================================
  template <typename GUM_SCALAR>
  MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*
  StructuredPlaner<GUM_SCALAR>::_argmaximiseQactions(
      std::vector<MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>,
                                        SetTerminalNodePolicy>*>& qActionsSet ) {

    MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*
        newVFunction = qActionsSet.back();
    qActionsSet.pop_back();

    while ( !qActionsSet.empty() ) {
      MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*
          qAction = qActionsSet.back();
      qActionsSet.pop_back();
      newVFunction = _operator->argmaximize( newVFunction, qAction );
    }

    return newVFunction;
  }

  // ===========================================================================
  // Creates a copy of given in parameter decision Graph and replaces leaves
  // of that Graph by a pair containing value of the leaf and action to which
  // is bind this Graph (given in parameter).
  // ===========================================================================
  template <typename GUM_SCALAR>
  void StructuredPlaner<GUM_SCALAR>::_extractOptimalPolicy(
      const MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>,
                                  SetTerminalNodePolicy>*
          argMaxOptimalValueFunction ) {

    _optimalPolicy->clear();

    // Insertion des nouvelles variables
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              argMaxOptimalValueFunction->variablesSequence().beginSafe();
          varIter != argMaxOptimalValueFunction->variablesSequence().endSafe();
          ++varIter )
      _optimalPolicy->add( **varIter );

    HashTable<NodeId, NodeId> src2dest;
    _optimalPolicy->manager()->setRootNode(
        __recurExtractOptPol( argMaxOptimalValueFunction->root(),
                              argMaxOptimalValueFunction,
                              src2dest ) );

    delete argMaxOptimalValueFunction;
  }


  // ==========================================================================
  // Recursion part for the createArgMaxCopy
  // ==========================================================================
  template <typename GUM_SCALAR>
  NodeId StructuredPlaner<GUM_SCALAR>::__recurExtractOptPol(
      NodeId currentNodeId,
      const MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>,
                                  SetTerminalNodePolicy>* argMaxOptVFunc,
      HashTable<NodeId, NodeId>& visitedNodes ) {
    if ( visitedNodes.exists( currentNodeId ) ) return visitedNodes[currentNodeId];

    NodeId nody;
    if ( argMaxOptVFunc->isTerminalNode( currentNodeId ) ) {
      ActionSet leaf;
      __transferActionIds( argMaxOptVFunc->nodeValue( currentNodeId ), leaf );
      nody = _optimalPolicy->manager()->addTerminalNode( leaf );
    } else {
      const InternalNode* currentNode = argMaxOptVFunc->node( currentNodeId );
      NodeId*             sonsMap = static_cast<NodeId*>( SOA_ALLOCATE(
          sizeof( NodeId ) * currentNode->nodeVar()->domainSize() ) );
      for ( Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda )
        sonsMap[moda] = __recurExtractOptPol(
            currentNode->son( moda ), argMaxOptVFunc, visitedNodes );
      nody = _optimalPolicy->manager()->addInternalNode( currentNode->nodeVar(),
                                                         sonsMap );
    }
    visitedNodes.insert( currentNodeId, nody );
    return nody;
  }

  // ==========================================================================
  // Extract from an ArgMaxSet the associated ActionSet
  // ==========================================================================
  template <typename GUM_SCALAR>
  void StructuredPlaner<GUM_SCALAR>::__transferActionIds(
      const ArgMaxSet<GUM_SCALAR, Idx>& src, ActionSet& dest ) {
    for ( auto idi = src.beginSafe(); idi != src.endSafe(); ++idi )
      dest += *idi;
  }


}  // end of namespace gum
