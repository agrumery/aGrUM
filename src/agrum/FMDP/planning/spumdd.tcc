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
* @brief Template implementation of FMDP/planning/SPUMDD.h classes.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

// =========================================================================
#include <math.h>
#include <thread>
#include <vector>
#include <algorithm>
// =========================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionGraph.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/spumdd.h>
// =========================================================================

/// For shorter line
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
  SPUMDD<GUM_SCALAR>::SPUMDD ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon ) : __fmdp(fmdp) {

    GUM_CONSTRUCTOR ( SPUMDD );

    __threshold = epsilon;
    __vFunction = nullptr;
  }

// ===========================================================================
// Default destructor
// ===========================================================================
  template<typename GUM_SCALAR> INLINE
  SPUMDD<GUM_SCALAR>::~SPUMDD() {
    GUM_DESTRUCTOR ( SPUMDD );

    if(__vFunction)
        delete __vFunction;
  }


/* ************************************************************************************************** **/
/* **                                                                                                 **/
/* **                                   Planning Methods                                              **/
/* **                                                                                                 **/
/* ************************************************************************************************** **/

    // ===========================================================================
    // Makes a spudd planning on FMDP
    // ===========================================================================
    template<typename GUM_SCALAR>
    void SPUMDD<GUM_SCALAR>::initialize(  ) {

        __threshold *= ( 1 - __fmdp->discount() ) / ( 2 * __fmdp->discount() );

        for ( auto varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter ) {
          __elVarSeq << *varIter;
          std::cout << (*varIter)->name() << " ";
        }
        std::cout << std::endl;

        for ( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter )
            std::cout << *actionIter << " ";
        std::cout << std::endl;

        __vFunction = new MultiDimDecisionGraph< GUM_SCALAR >();
        __vFunction->copy(*(RECAST(__fmdp->reward())));

    }

  // ===========================================================================
  // Makes a spudd planning on FMDP
  // ===========================================================================
  template<typename GUM_SCALAR>
  MultiDimDecisionGraph<GUM_SCALAR>* SPUMDD<GUM_SCALAR>::makePlanning( Idx nbStep ) {

    // *****************************************************************************************
    // Initialisation
    // *****************************************************************************************



    // *****************************************************************************************
    // Initialisation of  Vnew and Vtemp
    MultiDimDecisionGraph< GUM_SCALAR >* Vnew = nullptr;
    MultiDimDecisionGraph< GUM_SCALAR >* Vtemp = nullptr;

    // *****************************************************************************************
    // Main loop
    // *****************************************************************************************
    Idx nbIte = 0;
    GUM_SCALAR gap = __threshold + 1;
    while ( gap > __threshold && nbIte < nbStep) {

      ++nbIte;

      // *****************************************************************************************
      // Loop reset
      Vnew = new MultiDimDecisionGraph< GUM_SCALAR >();
      Vnew->copyAndReassign ( *__vFunction, __fmdp->mapMainPrime() );

      std::cout << "Hello" << std::endl;
      // *****************************************************************************************
      // For each action
      std::vector<std::thread> workers;
      for ( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter  ) {
        evalQaction( this, Vnew, *actionIter );
//        NodeId threadActionId = *actionIter;
//        std::cout << "Launching Thread " << threadActionId << std::endl;
//        workers.push_back( std::thread([this, Vnew, threadActionId](){evalQaction( this, Vnew, threadActionId );}));
      }
      std::for_each(workers.begin(), workers.end(), [](std::thread &w){ w.join(); });


      // *****************************************************************************************
      // Next to evaluate main value function, we take maximise over all action value, ...
      delete Vnew;

      auto actionIter = __fmdp->beginActions();
      Vnew = __qFunctionTable[*actionIter];
      ++actionIter;

      while ( actionIter != __fmdp->endActions() ) {
        Vtemp = Vnew;
        Vnew = maximize2MultiDimDecisionGraphs ( Vnew, __qFunctionTable[ *actionIter ] );

        delete Vtemp;
        delete __qFunctionTable[ *actionIter ];
        ++actionIter;
      }
      __qFunctionTable.clear();


      // *******************************************************************************************
      // Next, we eval the new function value
      Vtemp = Vnew;
      Vnew = __addReward ( Vtemp );
      delete Vtemp;


      // *****************************************************************************************
      // Then we compare new value function and the old one
      MultiDimDecisionGraph< GUM_SCALAR >* deltaV = subtract2MultiDimDecisionGraphs ( Vnew, __vFunction );
      gap = 0;

      for ( auto valIter = deltaV->values().cbeginSafe(); valIter != deltaV->values().cendSafe(); ++valIter )
        if ( gap < fabs ( valIter.second() ) )
          gap = fabs ( valIter.second() );

      delete deltaV;

      std::cout << " ------------------- Fin itération n° " << nbIte << std::endl << " Gap : " << gap <<  " - " << __threshold << std::endl;

      // *****************************************************************************************
      // And eventually we update pointers for next loop
      delete __vFunction;
      __vFunction = Vnew;

    }

//    __evalPolicy ( Vold );
    return __vFunction;

//     delete Vold;
  }  



  // ===========================================================================
  // Evals the value function for current fmdp action
  // ===========================================================================
  template<typename GUM_SCALAR>
  void
//  MultiDimDecisionGraph<GUM_SCALAR>*
//  SPUMDD<GUM_SCALAR>::__evalActionValue ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold, Idx actionId ) {
  evalQaction( SPUMDD<GUM_SCALAR>* planer, const MultiDimDecisionGraph< GUM_SCALAR >* Vold, Idx actionId ){

//    std::cout << "Thread " << actionId << " has begun." << std::endl;

    // *****************************************************************************************
    // Initialisation
    MultiDimDecisionGraph< GUM_SCALAR >* Vaction = new MultiDimDecisionGraph< GUM_SCALAR >();
    Vaction->copy( *Vold );
    MultiDimDecisionGraph< GUM_SCALAR >* Vtemp = nullptr;
    Set< const DiscreteVariable* > varSet;

    // *****************************************************************************************
    // To evaluate action value function, we multiply old main value function by transition table
    // of each variable
//    std::cout << "Thread " << actionId << " initiates var elemination." << std::endl;
    for ( auto varIter = planer->beginVarElimination(); varIter != planer->endVarElemination(); --varIter ) {

//      std::cout << "Thread " << actionId << " - Var : " << (*varIter)->name() << std::endl;
      // ***************************************************************************************
      // Multiplication of Vaction by current variable's CPT
      Vtemp = Vaction;
      Vaction = multiply2MultiDimDecisionGraphs ( Vaction, RECAST( planer->fmdp()->transition( actionId, *varIter)));
      delete Vtemp;

      // ***************************************************************************************
      // Projection of Vaction on current var by summing on each of its modalities
      Vtemp = Vaction;
      varSet << planer->fmdp()->main2prime(*varIter);
      Vaction = projectSumMultiDimDecisionGraph ( Vaction, varSet );
      varSet >> planer->fmdp()->main2prime(*varIter);
      delete Vtemp;
    }

    planer->addQaction(actionId, Vaction);
//    return Vaction;

  }
  template<typename GUM_SCALAR>
  void
  SPUMDD<GUM_SCALAR>::addQaction(Idx actionId, MultiDimDecisionGraph< GUM_SCALAR >* qaction) {
      __qTableMutex.lock();
      __qFunctionTable.insert ( actionId, qaction );
      __qTableMutex.unlock();
  }



// ===========================================================================
// Updates the value function by multiplying by discount and adding reward
// ===========================================================================
  template<typename GUM_SCALAR>
  MultiDimDecisionGraph<GUM_SCALAR>*
  SPUMDD<GUM_SCALAR>::__addReward ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold ) {

    // *****************************************************************************************
    // ... we multiply the result by the discount factor, ...
    MultiDimDecisionGraph< GUM_SCALAR >* Vnew = new MultiDimDecisionGraph<GUM_SCALAR>();
    Vnew->copyAndMultiplyByScalar ( *Vold, __fmdp->discount() );

    // *****************************************************************************************
    // ... and finally add reward
    MultiDimDecisionGraph< GUM_SCALAR >* Vtemp = Vnew;
    Vnew = add2MultiDimDecisionGraphs ( RECAST( __fmdp->reward() ), Vtemp );

    delete Vtemp;

    return Vnew;

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
    SPUMDD<GUM_SCALAR>::__evalPolicy ( const MultiDimDecisionGraph< GUM_SCALAR >* V ) {

      // *****************************************************************************************
      // We have to do one last step to get best policy
      // *****************************************************************************************
      MultiDimDecisionGraph< std::pair< double, long > >* Vamnew = __argMaxValueFunction ( V );

//      MultiDimDecisionGraphFactory< Idx >* factory = new MultiDimDecisionGraphFactory<Idx>();
//      factory->setVariablesSequence ( Vamnew->variablesSequence() );
//      HashTable< NodeId, NodeId > explorationTable;
//      __makeOptimalPolicyDecisionGraph ( Vamnew, Vamnew->root(), factory, explorationTable );
//      delete Vamnew;

//      MultiDimDecisionGraph< Idx >* optimalPolicy = factory->getMultiDimDecisionGraph ( false, 0, true );

//      __displayOptimalPolicy ( optimalPolicy );

//      delete optimalPolicy;
//      delete factory;

    }

// ===========================================================================
// Performs one last step of the algorithm to obtain the arg max equivalent
// of the so far computed value function
// ===========================================================================
  template<typename GUM_SCALAR>
  MultiDimDecisionGraph< std::pair< double, long > >*
  SPUMDD< GUM_SCALAR >::__argMaxValueFunction ( const MultiDimDecisionGraph< GUM_SCALAR >* V ) {

    // *****************************************************************************************
    // Loop reset
    MultiDimDecisionGraph< GUM_SCALAR >* Vnew = new MultiDimDecisionGraph<GUM_SCALAR>();
    Vnew->copyAndReassign ( V, __fmdp->mapMainPrime() );
    MultiDimDecisionGraph< GUM_SCALAR >* Vtemp;

    // *****************************************************************************************
    // For each action
    for ( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter ) {

      MultiDimDecisionGraph< GUM_SCALAR >* Vaction = __evalActionValue ( Vnew, elVarSeq );

      Vtemp = reinterpret_cast<MultiDimDecisionGraph<GUM_SCALAR>*> ( Vaction->newFactory() );
      Vtemp->multiplyByScalar ( Vaction, __fmdp->discount() );
      delete Vaction;
      Vaction = Vtemp;

      Vtemp = Vaction;
      Vaction = add2MultiDimDecisionGraphs ( reinterpret_cast<const MultiDimDecisionGraph<GUM_SCALAR>*> ( __fmdp->reward() ), Vaction );
      delete Vtemp;

      VactionCollector.insert ( *actionIter, Vaction );
      __fmdp->nextAction();
    }

    delete Vnew;
    MultiDimDecisionGraph< std::pair< double, long > >* Vamnew = nullptr;

//     for ( BijectionIteratorSafe< Idx, MultiDimDecisionGraph< GUM_SCALAR >* > VActionsIter = VactionCollector.begin(); VActionsIter != VactionCollector.end(); ++VActionsIter ) {
    for ( Idx acta = 1; acta < VactionCollector.size() + 1; acta++ ) {
//         MultiDimDecisionGraph< std::pair< double, long > >* Vamaction = __createArgMaxCopy( VActionsIter.second(), VActionsIter.first() );
      MultiDimDecisionGraph< std::pair< double, long > >* Vamaction = __createArgMaxCopy ( VactionCollector.second ( acta ), acta );
      delete VactionCollector.second ( acta );
      MultiDimDecisionGraph< std::pair< double, long > >* Vamtemp = Vamnew;
      Vamnew = __argMaxOn2MultiDimDecisionGraphs ( Vamnew, Vamaction );
      delete Vamtemp;
      delete Vamaction;
    }

    return Vamnew;
      return nullptr;

  }

// ===========================================================================
// Creates a copy of given in parameter decision Graph and replaces leaves
// of that Graph by a pair containing value of the leaf and action to which
// is bind this Graph (given in parameter).
// ===========================================================================
  template<typename GUM_SCALAR>
  MultiDimDecisionGraph< std::pair< double, long > >*
  SPUMDD<GUM_SCALAR>::__createArgMaxCopy ( const MultiDimDecisionGraph<GUM_SCALAR>* Vaction, Idx actionId ) {

    MultiDimDecisionGraph< std::pair< double, long > >* amcpy = new MultiDimDecisionGraph< std::pair< double, long > >();

    // Insertion des nouvelles variables
    for( SequenceIteratorSafe<const DiscreteVariable*> varIter = Vaction->variablesSequence().beginSafe(); varIter != Vaction->variablesSequence().endSafe(); ++varIter)
      amcpy->add(**varIter);

    std::vector<NodeId> lifo;
    Bijection<NodeId, NodeId> src2dest;

    if(Vaction->isTerminalNode(Vaction->root()))
      amcpy->manager()->setRootNode(amcpy->manager()->addTerminalNode(Vaction->nodeValue(Vaction->root())));
    else {
      amcpy->manager()->setRootNode(amcpy->manager()->addNonTerminalNode( Vaction->node(Vaction->root())->nodeVar() ));
      src2dest.insert( Vaction->root(), amcpy->root() );
      lifo.push_back(Vaction->root());
    }

    // Parcours en profondeur du diagramme source
    while( !lifo.empty() ){
      NodeId currentSrcNodeId = lifo.back();
      lifo.pop_back();

      const MultiDimDecisionGraph< GUM_SCALAR >::InternalNode* currentSrcNode = Vaction->node(currentSrcNodeId);

      for( Idx index = 0; index < currentSrcNode->nbSons(); ++index ){
        if( !src2dest.existsFirst(currentSrcNode->son(index)) ){
          NodeId srcSonNodeId = currentSrcNode->son(index), destSonNodeId = 0;
          if( Vaction->isTerminalNode(srcSonNodeId) ){
            destSonNodeId = amcpy->manager()->addTerminalNode(Vaction->nodeValue(srcSonNodeId));
          } else {
            destSonNodeId = amcpy->manager()->addNonTerminalNode(Vaction->node(srcSonNodeId)->nodeVar());
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
// Once final V is computed upon arg max on last Vactions, this function
// creates a Graph where all leaves tied to the same action are merged
// together.
// Since this function is a recursive one to ease the merge of all identic
// nodes to converge toward a cannonical policy, a factory and the current
// node are needed to build resulting Graph. Also we need an exploration
// table to avoid exploration of already visited sub-graph part.
// ===========================================================================
//  template<typename GUM_SCALAR>
//  NodeId
//  SPUMDD<GUM_SCALAR>::__makeOptimalPolicyDecisionGraph ( const MultiDimDecisionGraph< std::pair< double, long > >* V,
//      const NodeId& currentNode,
//      MultiDimDecisionGraphFactoryBase<Idx>* factory,
//      HashTable< NodeId, NodeId >& explorationTable ) {

//    std::vector<NodeId> sonsMap ( V->unsafeNodeSons ( currentNode )->size(), 0 );

//    for ( std::vector<NodeId>::const_iterator sonIter = V->unsafeNodeSons ( currentNode )->begin(); sonIter != V->unsafeNodeSons ( currentNode )->end(); ++ sonIter ) {
//      NodeId sonId = 0;

//      if ( *sonIter != 0 ) {
//        if ( explorationTable.exists ( *sonIter ) )
//          sonId = explorationTable[*sonIter];
//        else {
//          if ( V->isTerminalNode ( *sonIter ) )
//            sonId = factory->addTerminalNode ( V->unsafeNodeValue ( *sonIter ).second );
//          else
//            sonId = __makeOptimalPolicyDecisionGraph ( V, *sonIter, factory, explorationTable );

//          explorationTable.insert ( *sonIter, sonId );
//        }
//      }

//      sonsMap[ std::distance ( V->unsafeNodeSons ( currentNode )->begin(), sonIter ) ] = sonId;
//    }

//    NodeId defaultSonId = 0;

//    if ( V->unsafeHasNodeDefaultSon ( currentNode ) ) {
//      NodeId defaultSon = V->unsafeNodeDefaultSon ( currentNode );

//      if ( explorationTable.exists ( defaultSon ) )
//        defaultSonId = explorationTable[ defaultSon ];
//      else {
//        if ( V->isTerminalNode ( defaultSon ) )
//          defaultSonId = factory->addTerminalNode ( V->unsafeNodeValue ( defaultSon ).second );
//        else
//          defaultSonId = __makeOptimalPolicyDecisionGraph ( V, defaultSon, factory, explorationTable );

//        explorationTable.insert ( defaultSon, defaultSonId );
//      }
//    } std::pair<Idx, Idx> res;

//      if ( dD1 == nullptr || dD2 == nullptr ) {
//        res.first = 0;
//        res.second = 0;
//        return res;
//      }

      // *****************************************************************************
      // First we determine the new var sequence

//      Sequence< const DiscreteVariable* > dD1VarSeq = dD1->variablesSequence();
//      Sequence< const DiscreteVariable* > dD2VarSeq = dD2->variablesSequence();
//      Sequence< const DiscreteVariable* > fusVarSeq;

//      SequenceIteratorSafe< const DiscreteVariable* > iterS1 = dD1VarSeq.beginSafe();
//      SequenceIteratorSafe< const DiscreteVariable* > iterS2 = dD2VarSeq.beginSafe();

//      while ( iterS1 != dD1VarSeq.endSafe() || iterS2 != dD2VarSeq.endSafe() ) {
//        if ( iterS1 == dD1VarSeq.endSafe() ) {
//          for ( ; iterS2 != dD2VarSeq.endSafe(); ++iterS2 )
//            if ( !fusVarSeq.exists ( *iterS2 ) )
//              fusVarSeq.insert ( *iterS2 );
//        } else if ( iterS2 == dD2VarSeq.endSafe() ) {
//          for ( ; iterS1 != dD1VarSeq.endSafe(); ++iterS1 )
//            if ( !fusVarSeq.exists ( *iterS1 ) )
//              fusVarSeq.insert ( *iterS1 );
//        } else {
//          if ( *iterS1 == *iterS2 ) {
//            if ( !fusVarSeq.exists ( *iterS1 ) )
//              fusVarSeq.insert ( *iterS1 );

//            ++iterS1;
//            ++iterS2;
//            continue;
//          }

//          if ( dD1VarSeq.pos ( *iterS1 ) <= dD2VarSeq.pos ( *iterS2 ) || dD1VarSeq.exists ( *iterS2 ) ) {
//            if ( !fusVarSeq.exists ( *iterS1 ) )
//              fusVarSeq.insert ( *iterS1 );

//            ++iterS1;
//            continue;
//          } else {
//            if ( !fusVarSeq.exists ( *iterS2 ) )
//              fusVarSeq.insert ( *iterS2 );

//            ++iterS2;
//            continue;
//          }
//        }
//      }

//      // ******************************************************************************
//      // Then we search in second Graph for possible preneeded variable
//      Idx sizeRetro = 1;
//      Idx nbRetroVar = 0;

//      for ( iterS2 = dD2VarSeq.beginSafe(); iterS2 != dD2VarSeq.endSafe(); ++iterS2 )
//        for ( iterS1 = iterS2; iterS1 != dD2VarSeq.rendSafe(); --iterS1 )
//          if ( fusVarSeq.pos ( *iterS1 ) > fusVarSeq.pos ( *iterS2 ) ) {
//            nbRetroVar++;
//            sizeRetro *= ( *iterS2 )->domainSize();
//          }

//      res.first = nbRetroVar;
//      res.second = sizeRetro;

//      return res;



//    if ( defaultSonId != 0 ) {
//      Idx nbDefault = 0;

//      for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap ) {
//        if ( *iterArcMap == 0 )
//          ++nbDefault;

//        if ( *iterArcMap == defaultSonId ) {
//          ++nbDefault;
//          sonsMap[ std::distance ( sonsMap.begin(), iterArcMap ) ] = 0;
//        }
//      }

//      if ( nbDefault == 1 )
//        for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap )
//          if ( *iterArcMap == 0 ) {
//            sonsMap[ std::distance ( sonsMap.begin(), iterArcMap ) ] = defaultSonId;
//            defaultSonId = 0;
//            break;
//          }
//    }

//    return factory->unsafeAddNonTerminalNodeWithArcs ( V->unsafeNodeVariable ( currentNode ), sonsMap, defaultSonId );
//      return nullptr;

//  }


} // end of namespace gum



// ===========================================================================
// Computed arg max of two vactions given in parameter
// ===========================================================================

//#define GUM_MULTI_DIM_OPERATOR(x,y) x.first>y.first?x:y
//#define GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION DecisionGraphRecur4argMax

//#define GUM_DECISION_Graph_OPERATOR_FUNCTION_DEFINITION
//#ifdef O4DDWITHORDER
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_FUNCTION DecisionGraphGoDown4argMax
//#ifdef O4DDDEBUG
//#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctionsDebug.h>
//#else
//#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctions.h>
//#endif
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_FUNCTION
//#else
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_LEADER_FUNCTION DecisionGraphGoDownOnLeader4argMax
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionGraphGoDownOnFollower4argMax
//#ifdef O4DDDEBUG
//#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctionsDebug.h>
//#else
//#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctions.h>
//#endif
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_LEADER_FUNCTION
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_FOLLOWER_FUNCTION
//#endif
//#undef GUM_DECISION_Graph_OPERATOR_FUNCTION_DEFINITION

//template <typename GUM_SCALAR>
//gum::MultiDimDecisionGraph< std::pair< double, long > >*
//gum::SPUMDD<GUM_SCALAR>::__argMaxOn2MultiDimDecisionGraphs ( const gum::MultiDimDecisionGraph< std::pair< double, long > >* Vaction1,
//    const gum::MultiDimDecisionGraph< std::pair< double, long > >* Vaction2 ) {

//  gum::MultiDimDecisionGraph< std::pair< double, long > >* ret = nullptr;

//  if ( ( Vaction1 == nullptr || Vaction1->empty() ) && ( Vaction2 == nullptr || Vaction2->empty() ) )
//    return ret;

//  if ( Vaction1 == nullptr || Vaction1->empty() ) {
//    ret = reinterpret_cast< gum::MultiDimDecisionGraph< std::pair< double, long > >* > ( Vaction2->newFactory() );
//    ret->copy ( *Vaction2 );
//    return ret;
//  }

//  if ( Vaction2 == nullptr || Vaction2->empty() ) {
//    ret = reinterpret_cast< gum::MultiDimDecisionGraph< std::pair< double, long > >* > ( Vaction1->newFactory() );
//    ret->copy ( *Vaction1 );
//    return ret;
//  }

//  gum::NonOrderedOperatorData< std::pair< double, long > > opData;
//  opData.initialize ( Vaction1, Vaction2 );

//#ifdef O4DDWITHORDER
//  gum::GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION ( opData, "" );
//#else
//  gum::GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION ( opData, nullptr, "" );
//#endif
//  std::pair< double, long > defaultPair ( 0, 0 );
//  ret = opData.factory->getMultiDimDecisionGraph ( false, defaultPair );

//  return ret;
//}

//#undef GUM_MULTI_DIM_OPERATOR
//#undef GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION

//// ===========================================================================
//// Computed the difference of two policy to find out if it's the same or not.
//// ===========================================================================

//#define GUM_MULTI_DIM_OPERATOR(x,y) T( x.first - y.first, x.second - y.second)
//#define GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION DecisionGraphRecur4difPol

//#define GUM_DECISION_Graph_OPERATOR_FUNCTION_DEFINITION
//#ifdef O4DDWITHORDER
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_FUNCTION DecisionGraphGoDown4difPol
//#ifdef O4DDDEBUG
//#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctionsDebug.h>
//#else
//#include <agrum/multidim/patterns/DDUtility/o4DDWithOrderRecursionFunctions.h>
//#endif
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_FUNCTION
//#else
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_LEADER_FUNCTION DecisionGraphGoDownOnLeader4difPol
//#define GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionGraphGoDownOnFollower4difPol
//#ifdef O4DDDEBUG
//#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctionsDebug.h>
//#else
//#include <agrum/multidim/patterns/DDUtility/o4DDWithoutOrderRecursionFunctions.h>
//#endif
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_LEADER_FUNCTION
//#undef GUM_MULTI_DIM_DECISION_Graph_GO_DOWN_ON_FOLLOWER_FUNCTION
//#endif
//#undef GUM_DECISION_Graph_OPERATOR_FUNCTION_DEFINITION

//template <typename GUM_SCALAR>
//gum::MultiDimDecisionGraph< std::pair< double, long > >*
//gum::SPUMDD<GUM_SCALAR>::__differenceOnPolicy ( const gum::MultiDimDecisionGraph< std::pair< double, long > >* Vaction1,
//    const gum::MultiDimDecisionGraph< std::pair< double, long > >* Vaction2 ) {

//  gum::MultiDimDecisionGraph< std::pair< double, long > >* ret = nullptr;

//  if ( ( Vaction1 == nullptr || Vaction1->empty() ) && ( Vaction2 == nullptr || Vaction2->empty() ) )
//    return ret;

//  if ( Vaction1 == nullptr || Vaction1->empty() ) {
//    ret = reinterpret_cast< gum::MultiDimDecisionGraph< std::pair< double, long > >* > ( Vaction2->newFactory() );
//    ret->copy ( *Vaction2 );
//    return ret;
//  }

//  if ( Vaction2 == nullptr || Vaction2->empty() ) {
//    ret = reinterpret_cast< gum::MultiDimDecisionGraph< std::pair< double, long > >* > ( Vaction1->newFactory() );
//    ret->copy ( *Vaction1 );
//    return ret;
//  }

//  gum::NonOrderedOperatorData< std::pair< double, long > > opData;
//  opData.initialize ( Vaction1, Vaction2 );

//#ifdef O4DDWITHORDER
//  gum::GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION ( opData, "" );
//#else
//  gum::GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION ( opData, nullptr, "" );
//#endif
//  //     //std::cout << "Fin opération" << std::endl;
//  std::pair< double, long > defaultPair ( 0, 0 );
//  ret = opData.factory->getMultiDimDecisionGraph ( false, defaultPair );

//  return ret;
//}

//#undef GUM_MULTI_DIM_OPERATOR
//#undef GUM_MULTI_DIM_DECISION_Graph_RECUR_FUNCTION
