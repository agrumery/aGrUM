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
* @brief Template implementation of FMDP/planning/PSPUMDD.h classes.
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
#include <agrum/core/OMPThreads.h>
#include <agrum/core/functors.h>
// =========================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionGraph.h>
#include <agrum/multidim/decisionGraphUtilities/multiDimDecisionGraphOperator.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/pspumdd.h>
#include <agrum/FMDP/planning/regress.h>
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
    PSPUMDD<GUM_SCALAR>::PSPUMDD ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon ) : __fmdp(fmdp) {

    GUM_CONSTRUCTOR ( PSPUMDD );

      __threshold = epsilon;
      __vFunction = nullptr;
      __optimalPolicy = nullptr;
    }

    // ===========================================================================
    // Default destructor
    // ===========================================================================
    template<typename GUM_SCALAR> INLINE
    PSPUMDD<GUM_SCALAR>::~PSPUMDD() {
      GUM_DESTRUCTOR ( PSPUMDD );

      if(__vFunction){
        delete __vFunction;
      }

      if(__optimalPolicy)
        delete __optimalPolicy;
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
    void PSPUMDD<GUM_SCALAR>::initialize(  ) {

      // Determination of the threshold value
      __threshold *= ( 1 - __fmdp->discount() ) / ( 2 * __fmdp->discount() );

      // Establishement of sequence of variable elemination
      for ( auto varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter )
        __elVarSeq << __fmdp->main2prime(*varIter);

      // Initialisation of the value function
      __vFunction = new MultiDimDecisionGraph< GUM_SCALAR >();
      __optimalPolicy = new MultiDimDecisionGraph< ActionSet, SetTerminalNodePolicy >();
      __firstTime = true;
    }


    // ===========================================================================
    // Makes a spudd planning on FMDP
    // ===========================================================================
    template<typename GUM_SCALAR>
    void PSPUMDD<GUM_SCALAR>::makePlanning( Idx nbStep ) {

      if(__firstTime){
        __vFunction->copy(*(RECAST(__fmdp->reward())));
        __firstTime = false;
      }

      // *****************************************************************************************
      // Main loop
      // *****************************************************************************************
      Idx nbIte = 0;
      GUM_SCALAR gap = __threshold + 1;
      while ( gap > __threshold && nbIte < nbStep) {

        ++nbIte;

        MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = __valueIteration();

        // *****************************************************************************************
        // Then we compare new value function and the old one
        MultiDimDecisionGraph< GUM_SCALAR >* deltaV = subtract2MultiDimDecisionGraphs ( newVFunction, __vFunction );
        gap = 0;
        GUM_SCALAR lol = 0;

        for ( deltaV->beginValues(); deltaV->hasValue(); deltaV->nextValue() )
          if ( gap < fabs ( deltaV->value() ) ){
            gap = fabs ( deltaV->value() );
            lol = deltaV->value();
          }
        delete deltaV;

//        std::cout << newVFunction->toDot() << std::endl;

        std::cout << " ------------------- Fin itération n° " << nbIte << std::endl << " Gap : " << lol <<  " - " << __threshold << std::endl;

        // *****************************************************************************************
        // And eventually we update pointers for next loop
        delete __vFunction;
        __vFunction = newVFunction;

      }

      // *****************************************************************************************
      // Policy matching value function research
      // *****************************************************************************************
      __evalPolicy ( );
    }


    // ===========================================================================
    // Performs a single step of value iteration
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph< GUM_SCALAR >* PSPUMDD<GUM_SCALAR>::__valueIteration() {

      // *****************************************************************************************
      // Loop reset
      MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = new MultiDimDecisionGraph< GUM_SCALAR >();
      newVFunction->copyAndReassign ( *__vFunction, __fmdp->mapMainPrime() );

      // *****************************************************************************************
      // For each action
      //#pragma omp parallel for
      for ( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter  ) {
        NodeId threadActionId = *actionIter;
        MultiDimDecisionGraph<GUM_SCALAR>* qAction = evalQaction( this, newVFunction, threadActionId );
        #pragma omp critical(qaccess)
        {
          this->addQaction(qAction);
        }
      }
      delete newVFunction;


      // *****************************************************************************************
      // Next to evaluate main value function, we take maximise over all action value, ...
      newVFunction = __qFunctionSet.back();
      __qFunctionSet.pop_back();
      MultiDimDecisionGraph< GUM_SCALAR >* vTemp = nullptr;

      while ( !__qFunctionSet.empty() ) {
        vTemp = newVFunction;
        newVFunction = maximize2MultiDimDecisionGraphs ( newVFunction, __qFunctionSet.back() );

        delete vTemp;
        delete __qFunctionSet.back();
        __qFunctionSet.pop_back();
      }


      // *******************************************************************************************
      // Next, we eval the new function value
      vTemp = newVFunction;
      newVFunction = __addReward ( newVFunction );
      delete vTemp;

      return newVFunction;
    }


    // ===========================================================================
    // Evals the q function for current fmdp action
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph< GUM_SCALAR >*
    evalQaction( PSPUMDD<GUM_SCALAR>* planer, const MultiDimDecisionGraph< GUM_SCALAR >* Vold, Idx actionId ){

//      std::cout << "===========================================================================================" << std::endl;
//      std::cout << "Thread " << actionId << " - " << planer->fmdp()->actionName(actionId)  << std::endl;

      // ******************************************************************************
      // Initialisation :
      // Creating a copy of last Vfunction to deduce from the new Qaction
      // And finding the first var to eleminate (the one at the end)

      MultiDimDecisionGraph< GUM_SCALAR >* qAction = new MultiDimDecisionGraph< GUM_SCALAR >();
      qAction->copy( *Vold );
//      std::cout << qAction->toDot() << std::endl;
      MultiDimDecisionGraph< GUM_SCALAR >* vTemp = nullptr;
//      std::cout << "ping" << std::endl;
      const DiscreteVariable* v = nullptr;
      if( qAction->variablesSequence().size() > 0 )
        v = qAction->variablesSequence().atPos( qAction->variablesSequence().size() - 1 );
//      std::cout << "pong" << std::endl;

      while( planer->shouldEleminateVar(v) ){
//          std::cout << "ping : Eliminating var " << v->toString() << std::endl;
          vTemp = qAction;
//          std::cout << RECAST( planer->fmdp()->transition( actionId, planer->fmdp()->mapMainPrime().first(v) ) )->toDot() << std::endl;
//          std::cout << "pong" << std::endl;

          Regress<GUM_SCALAR, std::multiplies, std::plus> r(
                qAction, RECAST( planer->fmdp()->transition( actionId, planer->fmdp()->mapMainPrime().first(v) ) ),
                planer->elVarSeq(), v, (GUM_SCALAR)0);
//          std::cout << "ping" << std::endl;
          qAction = r.compute();
//          std::cout << "pong" << std::endl;

          delete vTemp;
          v=nullptr;
          if(qAction->variablesSequence().size() > 0)
            v = qAction->variablesSequence().atPos( qAction->variablesSequence().size() - 1 );
//          std::cout << "ping : next var " << v->toString() << std::endl;
      }
//      std::cout << qAction->toDot() << std::endl;
//      std::cout << "FIN " << actionId << " - " << planer->fmdp()->actionName(actionId)  << std::endl;
//      std::cout << "===========================================================================================" << std::endl;
      return qAction;
    }



    // ===========================================================================
    // Asynchronely insert a qAction into the set
    // ===========================================================================
    template<typename GUM_SCALAR>
    void
    PSPUMDD<GUM_SCALAR>::addQaction( MultiDimDecisionGraph< GUM_SCALAR >* qaction) {
        __qFunctionSet.push_back ( qaction );
    }


    // ===========================================================================
    // Updates the value function by multiplying by discount and adding reward
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph<GUM_SCALAR>*
    PSPUMDD<GUM_SCALAR>::__addReward ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold ) {

      // *****************************************************************************************
      // ... we multiply the result by the discount factor, ...
      MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = new MultiDimDecisionGraph<GUM_SCALAR>();
      newVFunction->copyAndMultiplyByScalar ( *Vold, __fmdp->discount() );

      // *****************************************************************************************
      // ... and finally add reward
      MultiDimDecisionGraph< GUM_SCALAR >* vTemp = newVFunction;
      newVFunction = add2MultiDimDecisionGraphs ( RECAST( __fmdp->reward() ), vTemp );

      delete vTemp;

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
    PSPUMDD<GUM_SCALAR>::__evalPolicy (  ) {

      // *****************************************************************************************
      // Loop reset
      MultiDimDecisionGraph< GUM_SCALAR >* newVFunction = new MultiDimDecisionGraph< GUM_SCALAR >();
      newVFunction->copyAndReassign ( *__vFunction, __fmdp->mapMainPrime() );

      // *****************************************************************************************
      // For each action
//      std::vector<std::future<Multi>> workers;
      for ( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter  ) {
        NodeId threadActionId = *actionIter;
//        std::cout << "Launching Thread " << threadActionId << std::endl;
//        workers.push_back( std::thread([this, newVFunction, threadActionId](){
        MultiDimDecisionGraph<GUM_SCALAR>* qAction = evalQaction( this, newVFunction, threadActionId );

        // *******************************************************************************************
        // Next, we eval the new function value
        MultiDimDecisionGraph< GUM_SCALAR >* vTemp = qAction;
        qAction = __addReward ( qAction );
        delete vTemp;

        this->addQaction( __createArgMaxCopy(qAction, threadActionId) );
        delete qAction;
//        }));
      }
      delete newVFunction;
//      std::for_each(workers.begin(), workers.end(), [](std::thread &w){ w.join(); });


      // *****************************************************************************************
      // Next to evaluate main value function, we take maximise over all action value, ...

      MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* argMaxVFunction = __argMaxQFunctionSet.back();
      __argMaxQFunctionSet.pop_back();
      MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* argMaxvTemp = nullptr;

      while ( !__argMaxQFunctionSet.empty() ) {
        argMaxvTemp = argMaxVFunction;
        MultiDimDecisionGraphOperator< ArgMaxSet<GUM_SCALAR, Idx>, ArgumentMaximisesAction, SetTerminalNodePolicy > argmaxope(
              argMaxVFunction, __argMaxQFunctionSet.back() );
        argMaxVFunction = argmaxope.compute();

        delete argMaxvTemp;
        delete __argMaxQFunctionSet.back();
        __argMaxQFunctionSet.pop_back();
      }

      __extractOptimalPolicy ( argMaxVFunction ) ;
      delete argMaxVFunction;
    }


    // ===========================================================================
    // Asynchronely insert a qAction into the set
    // ===========================================================================
    template<typename GUM_SCALAR>
    void
    PSPUMDD<GUM_SCALAR>::addQaction(MultiDimDecisionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy> *qaction ) {
        __argMaxQFunctionSet.push_back ( qaction );
    }


    // ===========================================================================
    // Creates a copy of given in parameter decision Graph and replaces leaves
    // of that Graph by a pair containing value of the leaf and action to which
    // is bind this Graph (given in parameter).
    // ===========================================================================
    template<typename GUM_SCALAR>
    MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >*
    PSPUMDD<GUM_SCALAR>::__createArgMaxCopy ( const MultiDimDecisionGraph<GUM_SCALAR>* qAction, Idx actionId ) {

      MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* amcpy
          = new MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >();

      // Insertion des nouvelles variables
      for( SequenceIteratorSafe<const DiscreteVariable*> varIter = qAction->variablesSequence().beginSafe(); varIter != qAction->variablesSequence().endSafe(); ++varIter)
        amcpy->add(**varIter);

      std::vector<NodeId> lifo;
      Bijection<NodeId, NodeId> src2dest;

      if(qAction->isTerminalNode(qAction->root())){
        ArgMaxSet<GUM_SCALAR, Idx> leaf( qAction->nodeValue(qAction->root()), actionId );
        amcpy->manager()->setRootNode(amcpy->manager()->addTerminalNode(leaf));
      } else {
        amcpy->manager()->setRootNode(amcpy->manager()->addNonTerminalNode( qAction->node(qAction->root())->nodeVar() ));
        src2dest.insert( qAction->root(), amcpy->root() );
        lifo.push_back(qAction->root());
      }

      // Parcours en profondeur du diagramme source
      while( !lifo.empty() ){
        NodeId currentSrcNodeId = lifo.back();
        lifo.pop_back();

        const InternalNode* currentSrcNode = qAction->node(currentSrcNodeId);

        for( Idx index = 0; index < currentSrcNode->nbSons(); ++index ){
          if( !src2dest.existsFirst(currentSrcNode->son(index)) ){
            NodeId srcSonNodeId = currentSrcNode->son(index), destSonNodeId = 0;
            if( qAction->isTerminalNode(srcSonNodeId) ){
              ArgMaxSet<GUM_SCALAR, Idx> leaf( qAction->nodeValue(srcSonNodeId), actionId );
              destSonNodeId = amcpy->manager()->addTerminalNode(leaf);
            } else {
              destSonNodeId = amcpy->manager()->addNonTerminalNode(qAction->node(srcSonNodeId)->nodeVar());
              lifo.push_back(srcSonNodeId);
            }
            src2dest.insert( srcSonNodeId, destSonNodeId );
          }
          amcpy->manager()->setSon( src2dest.second(currentSrcNodeId), index, src2dest.second(currentSrcNode->son(index)));
        }
      }

      return amcpy;
    }

    // ===========================================================================
    // Creates a copy of given in parameter decision Graph and replaces leaves
    // of that Graph by a pair containing value of the leaf and action to which
    // is bind this Graph (given in parameter).
    // ===========================================================================
    template<typename GUM_SCALAR>
    void
    PSPUMDD<GUM_SCALAR>::__extractOptimalPolicy (
        const MultiDimDecisionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* argMaxOptimalPolicy ) {

//      std::cout << argMaxOptimalPolicy->toDot() << std::endl;
      __optimalPolicy->clear();

      // Insertion des nouvelles variables
      for( SequenceIteratorSafe<const DiscreteVariable*> varIter = argMaxOptimalPolicy->variablesSequence().beginSafe(); varIter != argMaxOptimalPolicy->variablesSequence().endSafe(); ++varIter)
        __optimalPolicy->add(**varIter);

      std::vector<NodeId> lifo;
      HashTable<NodeId, NodeId> src2dest;

      if(argMaxOptimalPolicy->isTerminalNode(argMaxOptimalPolicy->root())){
        ActionSet leaf;
        __transferActionIds( argMaxOptimalPolicy->nodeValue( argMaxOptimalPolicy->root() ), leaf );
        __optimalPolicy->manager()->setRootNode(__optimalPolicy->manager()->addTerminalNode(leaf));
      } else {
        __optimalPolicy->manager()->setRootNode(__optimalPolicy->manager()->addNonTerminalNode( argMaxOptimalPolicy->node(argMaxOptimalPolicy->root())->nodeVar() ));
        src2dest.insert( argMaxOptimalPolicy->root(), __optimalPolicy->root() );
        lifo.push_back(argMaxOptimalPolicy->root());
      }

      // Parcours en profondeur du diagramme source
      while( !lifo.empty() ){
        NodeId currentSrcNodeId = lifo.back();
        lifo.pop_back();

        const InternalNode* currentSrcNode = argMaxOptimalPolicy->node(currentSrcNodeId);

        for( Idx index = 0; index < currentSrcNode->nbSons(); ++index ){
          if( !src2dest.exists(currentSrcNode->son(index)) ){
            NodeId srcSonNodeId = currentSrcNode->son(index), destSonNodeId = 0;
            if( argMaxOptimalPolicy->isTerminalNode(srcSonNodeId) ){
//                std::cout << "N : " << srcSonNodeId << std::endl;
              ActionSet leaf;
              __transferActionIds( argMaxOptimalPolicy->nodeValue( srcSonNodeId ), leaf );
              destSonNodeId = __optimalPolicy->manager()->addTerminalNode(leaf);
//              std::cout << "Done" << std::endl;
            } else {
              destSonNodeId = __optimalPolicy->manager()->addNonTerminalNode(argMaxOptimalPolicy->node(srcSonNodeId)->nodeVar());
              lifo.push_back(srcSonNodeId);
            }
            src2dest.insert( srcSonNodeId, destSonNodeId );
          }
          __optimalPolicy->manager()->setSon( src2dest[currentSrcNodeId], index, src2dest[currentSrcNode->son(index)] );
        }
      }
      __optimalPolicy->manager()->reduce();
      __optimalPolicy->manager()->clean();
//      std::cout << __optimalPolicy->toDot() << std::endl;
    }

    template<typename GUM_SCALAR>
    void PSPUMDD<GUM_SCALAR>::__transferActionIds( const ArgMaxSet<GUM_SCALAR, Idx>& src, ActionSet& dest){
      for( auto idi = src.beginSafe(); idi != src.endSafe(); ++idi )
        dest += *idi;
    }

    template<typename GUM_SCALAR>
    std::string PSPUMDD<GUM_SCALAR>::optimalPolicy2String(){
      if(!__optimalPolicy || __optimalPolicy->root() == 0 )
        return "NO OPTIMAL POLICY CALCULATED YET";

      std::stringstream output;
      std::stringstream terminalStream;
      std::stringstream nonTerminalStream;
      std::stringstream arcstream;
      output << std::endl << "digraph \" OPTIMAL POLICY \" {" << std::endl;

      terminalStream << "node [shape = box];" << std::endl;
      nonTerminalStream << "node [shape = ellipse];" << std::endl;
      std::string tab = "  ";

      Set<NodeId> visited;
      std::queue<NodeId> fifo;
      fifo.push(__optimalPolicy->root());
      visited << __optimalPolicy->root();

      while(!fifo.empty()){

        NodeId currentNodeId = fifo.front();
        fifo.pop();

        if ( __optimalPolicy->isTerminalNode( currentNodeId ) ){
          ActionSet ase = __optimalPolicy->nodeValue(currentNodeId);
          terminalStream << tab << currentNodeId << ";" << tab << currentNodeId  << " [label=\""<< currentNodeId << " - ";
          for( SequenceIteratorSafe<Idx> valIter = ase.beginSafe(); valIter != ase.endSafe(); ++valIter )
            terminalStream << __fmdp->actionName(*valIter) << " ";
          terminalStream << "\"];"<< std::endl;
        } else {
          const InternalNode* currentNode = __optimalPolicy->node(currentNodeId);
          nonTerminalStream << tab << currentNodeId << ";" << tab << currentNodeId  << " [label=\""<< currentNodeId << " - "
                            << currentNode->nodeVar()->name() << "\"];" << std::endl;

          for ( Idx sonIter = 0; sonIter < currentNode->nbSons(); ++sonIter ){
            arcstream << tab <<  currentNodeId << " -> " << currentNode->son(sonIter)
                      << " [label=\"" << currentNode->nodeVar()->label ( sonIter ) << "\",color=\"#0000ff\"];" << std::endl;
            if( !visited.exists(currentNode->son(sonIter)) ){
              fifo.push(currentNode->son(sonIter));
              visited << currentNode->son(sonIter);
            }
          }
        }
      }

      output << terminalStream.str() << std::endl << nonTerminalStream.str() << std::endl <<  arcstream.str() << std::endl << "}" << std::endl;

      return output.str();
    }


} // end of namespace gum
