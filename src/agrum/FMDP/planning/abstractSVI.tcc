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
* @brief Template implementation of FMDP/planning/AbstractSVI.h classes.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

// =========================================================================
#include <math.h>
#include <vector>
#include <queue>
//#include <algorithm>
//#include <utility>
// =========================================================================
#include <agrum/core/functors.h>
// =========================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionGraph.h>
#include <agrum/multidim/decisionGraphUtilities/multiDimDecisionGraphOperator.h>
// =========================================================================
#include <agrum/FMDP/planning/abstractSVI.h>
// =========================================================================

/// For shorter line and hence more comprehensive code purposes only
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
    AbstractSVI<GUM_SCALAR>::AbstractSVI ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon ) : _fmdp(fmdp) {

      GUM_CONSTRUCTOR ( AbstractSVI );

      __threshold = epsilon;
      _vFunction = nullptr;
      _optimalPolicy = nullptr;
    }

    // ===========================================================================
    // Default destructor
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    AbstractSVI<GUM_SCALAR>::~AbstractSVI() {

      GUM_DESTRUCTOR ( AbstractSVI );

      if(_vFunction){
        delete _vFunction;
      }

      if(_optimalPolicy)
        delete _optimalPolicy;
    }



  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                                Datastructure access methods                                     **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Initializes data structure needed for making the planning
    // ===========================================================================
    template<typename GUM_SCALAR>
    std::string AbstractSVI<GUM_SCALAR>::optimalPolicy2String(){

      // ************************************************************************
      // Discarding the case where no \pi* have been computed
      if(!__optimalPolicy || __optimalPolicy->root() == 0 )
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
      fifo.push(__optimalPolicy->root());
      visited << __optimalPolicy->root();


      // ************************************************************************
      // Main loop
      while(!fifo.empty()){

        // Node to visit
        NodeId currentNodeId = fifo.front();
        fifo.pop();

        // Checking if it is terminal
        if ( __optimalPolicy->isTerminalNode( currentNodeId ) ){

          // Get back the associated ActionSet
          ActionSet ase = __optimalPolicy->nodeValue(currentNodeId);

          // Creating a line for this node
          terminalStream << tab << currentNodeId << ";" << tab << currentNodeId  << " [label=\""<< currentNodeId << " - ";

          // Enumerating and adding to the line the associated optimal actions
          for( SequenceIteratorSafe<Idx> valIter = ase.beginSafe(); valIter != ase.endSafe(); ++valIter )
            terminalStream << _fmdp->actionName(*valIter) << " ";

          // Terminating line
          terminalStream << "\"];"<< std::endl;
        }

        // Either wise
        {
          // Geting back the associated internal node
          const InternalNode* currentNode = __optimalPolicy->node( currentNodeId );

          // Creating a line in internalnode stream for this node
          nonTerminalStream << tab << currentNodeId << ";" << tab << currentNodeId  << " [label=\""<< currentNodeId << " - "
                             << currentNode->nodeVar()->name() << "\"];" << std::endl;

          // Going through the sons and agregating them according the the sons Ids
          HashTable<NodeId, LinkedList<Idx>*> sonMap;
          for ( Idx sonIter = 0; sonIter < currentNode->nbSons(); ++sonIter ){
            if( !sonMap.exists( currentNode->son(sonIter) ) )
              sonMap.insert( currentNode->son(sonIter), new LinkedList<Idx>() );
            sonMap[currentNode->son(sonIter)]->addLink(sonIter);
          }

          // Adding to the arc stram
          for( auto sonIter = sonMap.beginSafe(); sonIter != sonMap.endSafe(); ++sonIter ){
            arcstream << tab <<  currentNodeId << " -> " << sonIter.key() << " [label=\" ";
            Link<Idx>* modaIter = sonIter.val()->list();
            while(modaIter){
              arcstream << currentNode->nodeVar()->label( modaIter->element() );
              if(modaIter->nextLink()) arcstream << ", ";
              modaIter = modaIter->nextLink();
            }
            arcstream << "\",color=\"#00ff00\"];" << std::endl;
            delete sonIter.val();
          }
        }
      }

      // Terminating
      output << terminalStream.str() << std::endl << nonTerminalStream.str() << std::endl <<  arcstream.str() << std::endl << "}" << std::endl;

      return output.str();
    }



  /* ************************************************************************************************** **/
  /* **                                                                                                 **/
  /* **                                     Planning Methods                                            **/
  /* **                                                                                                 **/
  /* ************************************************************************************************** **/

    // ===========================================================================
    // Initializes data structure needed for making the planning
    // ===========================================================================
    template<typename GUM_SCALAR>
    void AbstractSVI<GUM_SCALAR>::initialize(  ) {

      // Determination of the threshold value
      __threshold *= ( 1 - _fmdp->discount() ) / ( 2 * _fmdp->discount() );

        // Establishement of sequence of variable elemination
      for ( auto varIter = _fmdp->beginVariables(); varIter != _fmdp->endVariables(); ++varIter )
        __elVarSeq << _fmdp->main2prime(*varIter);

      // Initialisation of the value function
      _vFunction = new MultiDimDecisionGraph< GUM_SCALAR >();
      __optimalPolicy = new MultiDimDecisionGraph< ActionSet, SetTerminalNodePolicy >();
      __firstTime = true;
    }


    // ===========================================================================
    // Performs a value iteration
    // ===========================================================================
    template<typename GUM_SCALAR>
    void AbstractSVI<GUM_SCALAR>::makePlanning( Idx nbStep ) {

      if(__firstTime){
        _vFunction->copy(*(RECAST(_fmdp->reward())));
        __firstTime = false;
      }

      // *****************************************************************************************
      // Main loop
      // *****************************************************************************************
      Idx nbIte = 0;
      GUM_SCALAR gap = __threshold + 1;
      while ( gap > __threshold && nbIte < nbStep) {

        ++nbIte;

        MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = _valueIteration();

        // *****************************************************************************************
        // Then we compare new value function and the old one
        MultiDimDecisionGraph< GUM_SCALAR >* deltaV = _subtract( newVFunction, _vFunction );
        gap = 0;

        for ( deltaV->beginValues(); deltaV->hasValue(); deltaV->nextValue() )
          if ( gap < fabs ( deltaV->value() ) )
            gap = fabs ( deltaV->value() );
        delete deltaV;

        std::cout << " ------------------- Fin itération n° " << nbIte << std::endl << " Gap : " << gap <<  " - " << __threshold << std::endl;

        // *****************************************************************************************
        // And eventually we update pointers for next loop
        delete _vFunction;
        _vFunction = newVFunction;

      }

      // *****************************************************************************************
      // Policy matching value function research
      // *****************************************************************************************
      _evalPolicy ( );
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
    MultiDimDecisionGraph< GUM_SCALAR >* AbstractSVI<GUM_SCALAR>::_valueIteration() {

      // *****************************************************************************************
      // Loop reset
      MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = new MultiDimDecisionGraph< GUM_SCALAR >();
      newVFunction->copyAndReassign ( *_vFunction, _fmdp->mapMainPrime() );

      // *****************************************************************************************
      // For each action
      std::vector<MultiDimDecisionGraph<GUM_SCALAR>*> qActionsSet;
      for ( auto actionIter = _fmdp->beginActions(); actionIter != _fmdp->endActions(); ++actionIter  ) {
        MultiDimDecisionGraph<GUM_SCALAR>* qAction = _evalQaction( newVFunction, *actionIter );
        qActionsSet.push_back(qAction);
      }
      delete newVFunction;

      // *****************************************************************************************
      // Next to evaluate main value function, we take maximise over all action value, ...
      _maximiseQactions(qActionsSet);

      // *******************************************************************************************
      // Next, we eval the new function value
      newVFunction = _addReward ( newVFunction );

      return newVFunction;
    }


    // ===========================================================================
    // Evals the q function for current fmdp action
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph< GUM_SCALAR >*
    AbstractSVI<GUM_SCALAR>::_evalQaction( const MultiDimDecisionGraph< GUM_SCALAR >* Vold, Idx actionId ){

      // ******************************************************************************
      // Initialisation :
      // Creating a copy of last Vfunction to deduce from the new Qaction
      // And finding the first var to eleminate (the one at the end)
      MultiDimDecisionGraph< GUM_SCALAR >* qAction = new MultiDimDecisionGraph< GUM_SCALAR >();
      qAction->copy( *Vold );
      const DiscreteVariable* xip = this->_lastVar(qAction);

      while( this->_shouldEleminateVar(xip) ){
          qAction = _regress(qAction, RECAST(_fmdp->transition(actionId,_fmdp->mapMainPrime().first(xip))), xip);
          xip=this->_lastVar(qAction);
      }
      return qAction;
    }



    // ===========================================================================
    // Maximise the AAction to iobtain the vFunction
    // ===========================================================================
    template<typename GUM_SCALAR>
    void
    AbstractSVI<GUM_SCALAR>::_maximiseQactions( std::vector<MultiDimDecisionGraph<GUM_SCALAR>*>& qActionsSet) {

      MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = qActionsSet.back();
      qActionsSet.pop_back();

      while ( !qActionsSet.empty() ) {
        MultiDimDecisionGraph<double>* qAction = qActionsSet.back();
        qActionsSet.pop_back();
        newVFunction = _maximize ( newVFunction, qAction );
      }

      return newVFunction;
    }


    // ===========================================================================
    // Updates the value function by multiplying by discount and adding reward
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph<GUM_SCALAR>*
    AbstractSVI<GUM_SCALAR>::_addReward ( MultiDimDecisionGraph< GUM_SCALAR >* Vold ) {

      // *****************************************************************************************
      // ... we multiply the result by the discount factor, ...
      MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = new MultiDimDecisionGraph<GUM_SCALAR>();
      newVFunction->copyAndMultiplyByScalar ( *Vold, _fmdp->discount() );

      // *****************************************************************************************
      // ... and finally add reward
      newVFunction = _add(newVFunction, RECAST( _fmdp->reward() ));

      delete Vold;

      return newVFunction;
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
    AbstractSVI<GUM_SCALAR>::_evalPolicy (  ) {

      // *****************************************************************************************
      // Loop reset
      MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = new MultiDimDecisionGraph< GUM_SCALAR >();
      newVFunction->copyAndReassign ( *_vFunction, _fmdp->mapMainPrime() );

      std::vector<MultiDimDecisionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>*> argMaxQActionsSet;
      // *****************************************************************************************
      // For each action
      for ( auto actionIter = _fmdp->beginActions(); actionIter != _fmdp->endActions(); ++actionIter  ) {

        MultiDimDecisionGraph<GUM_SCALAR>* qAction = _evalQaction( newVFunction, *actionIter );

        qAction = _addReward ( qAction );

        argMaxQActionsSet.push_back( _makeArgMax(qAction, *actionIter) );
      }
      delete newVFunction;


      // *****************************************************************************************
      // Next to evaluate main value function, we take maximise over all action value, ...
      MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* argMaxVFunction = _argmaximiseQactions( argMaxQActionsSet );

      // *****************************************************************************************
      // Next to evaluate main value function, we take maximise over all action value, ...
      _extractOptimalPolicy ( argMaxVFunction ) ;
      delete argMaxVFunction;
    }


    // ===========================================================================
    // Creates a copy of given in parameter decision Graph and replaces leaves
    // of that Graph by a pair containing value of the leaf and action to which
    // is bind this Graph (given in parameter).
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >*
    AbstractSVI<GUM_SCALAR>::_makeArgMax ( const MultiDimDecisionGraph<GUM_SCALAR>* qAction, Idx actionId ) {

      MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* amcpy
          = new MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >();

      // Insertion des nouvelles variables
      for( SequenceIteratorSafe<const DiscreteVariable*> varIter = qAction->variablesSequence().beginSafe(); varIter != qAction->variablesSequence().endSafe(); ++varIter)
        amcpy->add(**varIter);

      HashTable<NodeId, NodeId> src2dest;
      amcpy->manager()->setRootNode( __recurArgMaxCopy( qAction->root(), actionId, qAction, amcpy, src2dest ) );

      delete qAction;
      return amcpy;
    }


    // ==========================================================================
    // Recursion part for the createArgMaxCopy
    // ==========================================================================
    template<typename GUM_SCALAR>
    NodeId
    AbstractSVI<GUM_SCALAR>::__recurArgMaxCopy( NodeId currentNodeId,
                                                Idx actionId,
                                                const MultiDimDecisionGraph<GUM_SCALAR>* src,
                                                const MultiDimDecisionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* argMaxCpy,
                                                HashTable<NodeId,NodeId>& visitedNodes){

      if( visitedNodes.exists(currentNodeId))
        return visitedNodes[currentNodeId];

      NodeId nody;
      if(src->isTerminalNode(currentNodeId)){
        ArgMaxSet<GUM_SCALAR, Idx> leaf( src->nodeValue(currentNodeId), actionId );
        nody = argMaxCpy->manager()->addTerminalNode(leaf);
      } else {
        const InternalNode* currentNode = src->node(currentNodeId);
        NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*currentNode->nodeVar()->domainSize()) );
        for( Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda )
          sonsMap[moda] = __recurArgMaxCopy(currentNode->son(moda), actionId, src, argMaxCpy,visitedNodes);
        nody = argMaxCpy->manager()->checkIsomorphism(currentNode->nodeVar(), sonsMap);
      }
      visitedNodes.insert(currentNodeId, nody);
      return nody;
    }


    // ===========================================================================
    // Creates a copy of given in parameter decision Graph and replaces leaves
    // of that Graph by a pair containing value of the leaf and action to which
    // is bind this Graph (given in parameter).
    // ===========================================================================
    template<typename GUM_SCALAR>
    void
    AbstractSVI<GUM_SCALAR>::_extractOptimalPolicy (
        const MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* argMaxOptimalValueFunction ) {

      _optimalPolicy->clear();

      // Insertion des nouvelles variables
      for( SequenceIteratorSafe<const DiscreteVariable*> varIter = argMaxOptimalValueFunction->variablesSequence().beginSafe(); varIter != argMaxOptimalValueFunction->variablesSequence().endSafe(); ++varIter)
        _optimalPolicy->add(**varIter);

      HashTable<NodeId, NodeId> src2dest;
      _optimalPolicy->manager()->setRootNode( __recurExtractOptPol( argMaxOptimalValueFunction->root(), argMaxOptimalValueFunction, src2dest ));

      delete argMaxOptimalValueFunction;
    }


    // ==========================================================================
    // Recursion part for the createArgMaxCopy
    // ==========================================================================
    template<typename GUM_SCALAR>
    void AbstractSVI<GUM_SCALAR>::__recurExtractOptPol(NodeId currentNodeId,
                                                       const MultiDimDecisionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* argMaxOptVFunc,
                                                       HashTable<NodeId,NodeId>& visitedNodes){
      if( visitedNodes.exists(currentNodeId))
        return visitedNodes[currentNodeId];

      NodeId nody;
      if(argMaxOptVFunc->isTerminalNode(currentNodeId)){
        ActionSet leaf;
        __transferActionIds( argMaxOptVFunc->nodeValue(currentNodeId), leaf );
        nody = _optimalPolicy->manager()->addTerminalNode(leaf);
      } else {
        const InternalNode* currentNode = argMaxOptVFunc->node(currentNodeId);
        NodeId* sonsMap = static_cast<NodeId*>( ALLOCATE(sizeof(NodeId)*currentNode->nodeVar()->domainSize()) );
        for( Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda )
          sonsMap[moda] = __recurExtractOptPol(currentNode->son(moda), argMaxOptVFunc, visitedNodes);
        nody = _optimalPolicy->manager()->checkIsomorphism( currentNode->nodeVar(), sonsMap);
      }
      visitedNodes.insert(currentNodeId, nody);
      return nody;
    }

    // ==========================================================================
    // Extract from an ArgMaxSet the associated ActionSet
    // ==========================================================================
    template<typename GUM_SCALAR>
    void AbstractSVI<GUM_SCALAR>::__transferActionIds( const ArgMaxSet<GUM_SCALAR, Idx>& src, ActionSet& dest){
      for( auto idi = src.beginSafe(); idi != src.endSafe(); ++idi )
        dest += *idi;
    }


} // end of namespace gum
