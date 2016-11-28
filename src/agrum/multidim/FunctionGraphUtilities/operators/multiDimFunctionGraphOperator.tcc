/****************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                                *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/
/**
* @file
* @brief Class used to compute the operation between two decision diagrams
*
* @author Jean-Christophe Magnan
*/

#include <agrum/multidim/FunctionGraphUtilities/internalNode.h>
#include <agrum/multidim/FunctionGraphUtilities/operators/multiDimFunctionGraphOperator.h>

namespace gum {

  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  MultiDimFunctionGraphOperator<GUM_SCALAR, FUNCTOR, TerminalNodePolicy>::
      MultiDimFunctionGraphOperator(
          const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* DG1,
          const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* DG2 )
      : __DG1( DG1 )
      , __DG2( DG2 )
      , __function()
      , __DG1InstantiationNeeded( DG1->realSize(), true, false )
      , __DG2InstantiationNeeded( DG2->realSize(), true, false ) {
    GUM_CONSTRUCTOR( MultiDimFunctionGraphOperator );
    __rd =
        MultiDimFunctionGraph<GUM_SCALAR,
                              TerminalNodePolicy>::getReducedAndOrderedInstance();
    __nbVar = 0;
    __default = nullptr;

    __nbCall = 0;
    __nbVar = 0;
    __sizeVarRetro = 1;
  }

  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  MultiDimFunctionGraphOperator<GUM_SCALAR, FUNCTOR, TerminalNodePolicy>::
      ~MultiDimFunctionGraphOperator() {

    GUM_DESTRUCTOR( MultiDimFunctionGraphOperator );


    for ( auto instIter = __DG1InstantiationNeeded.beginSafe();
          instIter != __DG1InstantiationNeeded.endSafe();
          ++instIter )
      SOA_DEALLOCATE( instIter.val(), sizeof( short int ) * __nbVar );

    for ( auto instIter = __DG2InstantiationNeeded.beginSafe();
          instIter != __DG2InstantiationNeeded.endSafe();
          ++instIter )
      SOA_DEALLOCATE( instIter.val(), sizeof( short int ) * __nbVar );

    if ( __nbVar != 0 ) SOA_DEALLOCATE( __default, sizeof( short int ) * __nbVar );
  }


  // This function is the main function. To be call every time an operation
  // between the two given Function Graphs is required
  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>*
  MultiDimFunctionGraphOperator<GUM_SCALAR,
                                FUNCTOR,
                                TerminalNodePolicy>::compute() {

    __establishVarOrder();
    __findRetrogradeVariables( __DG1, __DG1InstantiationNeeded );
    __findRetrogradeVariables( __DG2, __DG2InstantiationNeeded );

    Idx* varInst = nullptr;
    if ( __nbVar != 0 ) {
      varInst = static_cast<Idx*>( SOA_ALLOCATE( sizeof( Idx ) * __nbVar ) );
      for ( Idx i = 0; i < __nbVar; i++ )
        varInst[i] = (Idx)0;
    }

    O4DGContext conti( varInst, __nbVar );
    conti.setDG1Node( __DG1->root() );
    conti.setDG2Node( __DG2->root() );

    NodeId root = __compute( conti, (Idx)0 - 1 );
    __rd->manager()->setRootNode( root );

    if ( __nbVar != 0 ) SOA_DEALLOCATE( varInst, sizeof( Idx ) * __nbVar );

    return __rd;
  }

  // This function computes an efficient order for the final decision diagrams.
  // Its main criterion to do so is the number of re-exploration to be done.
  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  void MultiDimFunctionGraphOperator<GUM_SCALAR,
                                     FUNCTOR,
                                     TerminalNodePolicy>::__establishVarOrder() {

    SequenceIteratorSafe<const DiscreteVariable*> fite =
        __DG1->variablesSequence().beginSafe();
    SequenceIteratorSafe<const DiscreteVariable*> site =
        __DG2->variablesSequence().beginSafe();

    while ( fite != __DG1->variablesSequence().endSafe() &&
            site != __DG2->variablesSequence().endSafe() ) {

      // Test : if var from first order is already in final order
      // we move onto the next one
      if ( __rd->variablesSequence().exists( *fite ) ) {
        ++fite;
        continue;
      }

      // Test : if var from second order is already in final order
      // we move onto the next one
      if ( __rd->variablesSequence().exists( *site ) ) {
        ++site;
        continue;
      }

      // Test : is current var of the first order present in the second order.
      // if not we add it to final order
      if ( !__DG2->variablesSequence().exists( *fite ) ) {
        __rd->add( **fite );
        ++fite;
        continue;
      }

      // Test : is current var of the second order present in the first order.
      // if not we add it to final order
      if ( !__DG1->variablesSequence().exists( *site ) ) {
        __rd->add( **site );
        ++site;
        continue;
      }

      // Test : is current var of the second order present in the first order.
      // if not we add it to final order
      if ( *fite == *site ) {
        __rd->add( **fite );
        ++fite;
        ++site;
        continue;
      }

      // Test : the current tested situation is when two retrograde variables
      // are detected.
      // Chosen solution here is to find compute domainSize in between
      // and chose the one with the smallest
      __nbVarRetro++;
      if ( __distance( __DG1, *fite, *site ) <
           __distance( __DG2, *site, *fite ) ) {
        __rd->add( **fite );
        __sizeVarRetro *= ( *fite )->domainSize();
        ++fite;
        continue;
      } else {
        __rd->add( **site );
        __sizeVarRetro *= ( *site )->domainSize();
        ++site;
        continue;
      }
    }

    // Whenever an iterator has finished its sequence,
    // the other may still be in the middle of its one.
    // Hence, this part ensures that any variables remaining
    // will be added to the final sequence if needed.
    if ( fite == __DG1->variablesSequence().endSafe() ) {
      for ( ; site != __DG2->variablesSequence().endSafe(); ++site )
        if ( !__rd->variablesSequence().exists( *site ) ) __rd->add( **site );
    } else {
      for ( ; fite != __DG1->variablesSequence().endSafe(); ++fite )
        if ( !__rd->variablesSequence().exists( *fite ) ) __rd->add( **fite );
    }


    // Various initialization needed now that we have a bigger picture
    __nbVar = __rd->variablesSequence().size();

    if ( __nbVar != 0 ) {
      __default =
          static_cast<short int*>( SOA_ALLOCATE( sizeof( short int ) * __nbVar ) );
      for ( Idx i = 0; i < __nbVar; i++ )
        __default[i] = (short int)0;
    }
  }

  // This function computes the number of re-exploration needed whenever to
  // retrograde variables collides
  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  INLINE Idx
         MultiDimFunctionGraphOperator<GUM_SCALAR, FUNCTOR, TerminalNodePolicy>::
      __distance( const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* d,
                  const DiscreteVariable* from,
                  const DiscreteVariable* to ) {

    Idx posi = d->variablesSequence().pos( from );
    Idx dist = 1;

    while ( d->variablesSequence().atPos( posi ) != to ) {
      dist *= ( *( d->variablesSequence().atPos( posi ) ) ).domainSize();
      posi++;
    }

    return dist;
  }


  // This function computes for every nodes if any retrograde variable is
  // present below
  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraphOperator<GUM_SCALAR, FUNCTOR, TerminalNodePolicy>::
      __findRetrogradeVariables(
          const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* dg,
          HashTable<NodeId, short int*>& dgInstNeed ) {

    HashTable<NodeId, short int*> nodesVarDescendant;
    Size tableSize = Size( __nbVar * sizeof( short int ) );

    for ( auto varIter = dg->variablesSequence().rbeginSafe();
          varIter != dg->variablesSequence().rendSafe();
          --varIter ) {

      Idx                 varPos = __rd->variablesSequence().pos( *varIter );
      const Link<NodeId>* nodeIter = dg->varNodeListe( *varIter )->list();
      while ( nodeIter != nullptr ) {

        short int* instantiationNeeded =
            static_cast<short int*>( SOA_ALLOCATE( tableSize ) );
        dgInstNeed.insert( nodeIter->element(), instantiationNeeded );

        short int* varDescendant =
            static_cast<short int*>( SOA_ALLOCATE( tableSize ) );
        nodesVarDescendant.insert( nodeIter->element(), varDescendant );
        for ( Idx j = 0; j < __nbVar; j++ ) {
          instantiationNeeded[j] = (short int)0;
          varDescendant[j] = (short int)0;
        }

        varDescendant[varPos] = (short int)1;
        for ( Idx modality = 0;
              modality < dg->node( nodeIter->element() )->nbSons();
              ++modality ) {
          if ( !dg->isTerminalNode(
                   dg->node( nodeIter->element() )->son( modality ) ) ) {
            short int* sonVarDescendant =
                nodesVarDescendant[dg->node( nodeIter->element() )
                                       ->son( modality )];
            for ( Idx varIdx = 0; varIdx < __nbVar; varIdx++ ) {
              varDescendant[varIdx] += sonVarDescendant[varIdx];
              if ( varDescendant[varIdx] && varIdx < varPos )
                instantiationNeeded[varIdx] = (short int)1;
            }
          }
        }
        nodeIter = nodeIter->nextLink();
      }
    }

    for ( auto varIter = dg->variablesSequence().beginSafe();
          varIter != dg->variablesSequence().endSafe();
          ++varIter ) {

      const Link<NodeId>* nodeIter = dg->varNodeListe( *varIter )->list();
      while ( nodeIter != nullptr ) {

        for ( Idx modality = 0;
              modality < dg->node( nodeIter->element() )->nbSons();
              ++modality ) {
          NodeId sonId = dg->node( nodeIter->element() )->son( modality );
          if ( !dg->isTerminalNode( sonId ) ) {
            for ( Idx varIdx = 0; varIdx < __nbVar; ++varIdx ) {
              if ( dgInstNeed[nodeIter->element()][varIdx] &&
                   nodesVarDescendant[sonId][varIdx] ) {
                dgInstNeed[sonId][varIdx] = (short int)1;
              }
            }
          }
        }
        nodeIter = nodeIter->nextLink();
      }
    }

    for ( HashTableIterator<NodeId, short int*> it = nodesVarDescendant.begin();
          it != nodesVarDescendant.end();
          ++it ) {
      SOA_DEALLOCATE( it.val(), tableSize );
    }
    nodesVarDescendant.clear();
  }


  /// Main recursion function, called every time we move on a node to determine
  /// what we have to do

  // A key is used for prunning uneccesary operations since once a node has been
  // visited in a given context, there's no use to revisit him,
  // the result will be the same node, so we just have to do an association
  // context - node.
  // The context consists in :
  //              _ Leader node we are visiting.
  //              _ Follower node we are visiting.
  //              _ For all retrograde variables, if it has been instanciated
  //              before, current modality instanciated, meaning :
  //                      _ 0 means the variable hasn't be instanciated yet,
  //                      _ From 1 to domainSize + 1 means that current modality
  //                      index of variable is value - 1,
  //                      _ domainSize + 2 means variable is on default mode.
  // A key - node association is made each time we create a node in resulting
  // diagram.
  // Since GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION is a corner step in
  // algorithm ( meaning each time we explore a node we go trought
  // this function ), check only have to be at the beginning of that function.
  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  NodeId MultiDimFunctionGraphOperator<GUM_SCALAR, FUNCTOR, TerminalNodePolicy>::
      __compute( O4DGContext& currentSituation, Idx lastInstVarPos ) {

    __nbCall += 1;

    NodeId newNode = 0;


    // If both current nodes are terminal,
    // we only have to compute the resulting value
    if ( __DG1->isTerminalNode( currentSituation.DG1Node() ) &&
         __DG2->isTerminalNode( currentSituation.DG2Node() ) ) {

      // We have to compute new valueand we insert a new node in diagram with
      // this value, ...
      return __rd->manager()->addTerminalNode(
          __function( __DG1->terminalNodeValue( currentSituation.DG1Node() ),
                      __DG2->terminalNodeValue( currentSituation.DG2Node() ) ) );
    }

    // If not,
    // we'll have to do some exploration

    // First we ensure that we hadn't already visit this pair of node under hte
    // same circumstances

    short int* dg1NeededVar =
        __DG1InstantiationNeeded.exists( currentSituation.DG1Node() )
            ? __DG1InstantiationNeeded[currentSituation.DG1Node()]
            : __default;
    Idx dg1CurrentVarPos =
        __DG1->isTerminalNode( currentSituation.DG1Node() )
            ? __nbVar
            : __rd->variablesSequence().pos(
                  __DG1->node( currentSituation.DG1Node() )->nodeVar() );
    short int* dg2NeededVar =
        __DG2InstantiationNeeded.exists( currentSituation.DG2Node() )
            ? __DG2InstantiationNeeded[currentSituation.DG2Node()]
            : __default;
    Idx dg2CurrentVarPos =
        __DG2->isTerminalNode( currentSituation.DG2Node() )
            ? __nbVar
            : __rd->variablesSequence().pos(
                  __DG2->node( currentSituation.DG2Node() )->nodeVar() );

    short int* instNeeded =
        static_cast<short int*>( SOA_ALLOCATE( sizeof( short int ) * __nbVar ) );
    for ( Idx i = 0; i < __nbVar; i++ )
      instNeeded[i] = dg1NeededVar[i] + dg2NeededVar[i];

    double curSitKey = currentSituation.key( instNeeded );

    if ( __explorationTable.exists( curSitKey ) ) {
      SOA_DEALLOCATE( instNeeded, sizeof( short int ) * __nbVar );
      return __explorationTable[curSitKey];
    }

    // ====================================================

    NodeId origDG1 = currentSituation.DG1Node(),
           origDG2 = currentSituation.DG2Node();

    const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* leaddg = nullptr;
    NodeId leadNodeId = 0;
    Idx    leadVarPos = __rd->variablesSequence().size();
    typedef void ( O4DGContext::*SetNodeFunction )( const NodeId& );
    SetNodeFunction              leadFunction = nullptr;

    bool sameVar = false;

    if ( !__DG1->isTerminalNode( currentSituation.DG1Node() ) ) {

      if ( currentSituation.varModality( dg1CurrentVarPos ) != 0 ) {
        currentSituation.setDG1Node(
            __DG1->node( currentSituation.DG1Node() )
                ->son( currentSituation.varModality( dg1CurrentVarPos ) - 1 ) );

        newNode = __compute( currentSituation, lastInstVarPos );
        __explorationTable.insert( curSitKey, newNode );
        currentSituation.setDG1Node( origDG1 );
        currentSituation.setDG2Node( origDG2 );

        SOA_DEALLOCATE( instNeeded, sizeof( short int ) * __nbVar );

        return newNode;
      }

      leaddg = __DG1;
      leadNodeId = currentSituation.DG1Node();
      leadVarPos = dg1CurrentVarPos;
      leadFunction = &O4DGContext::setDG1Node;
    }

    if ( !__DG2->isTerminalNode( currentSituation.DG2Node() ) ) {

      if ( currentSituation.varModality( dg2CurrentVarPos ) != 0 ) {
        currentSituation.setDG2Node(
            __DG2->node( currentSituation.DG2Node() )
                ->son( currentSituation.varModality( dg2CurrentVarPos ) - 1 ) );

        newNode = __compute( currentSituation, lastInstVarPos );
        __explorationTable.insert( curSitKey, newNode );
        currentSituation.setDG1Node( origDG1 );
        currentSituation.setDG2Node( origDG2 );

        SOA_DEALLOCATE( instNeeded, sizeof( short int ) * __nbVar );

        return newNode;
      }

      if ( leadVarPos == dg2CurrentVarPos ) {
        sameVar = true;
      }

      if ( leadVarPos > dg2CurrentVarPos ) {
        leaddg = __DG2;
        leadNodeId = currentSituation.DG2Node();
        leadVarPos = dg2CurrentVarPos;
        leadFunction = &O4DGContext::setDG2Node;
      }
    }

    // ====================================================

    // Before exploring nodes, we have to ensure that every anticipated
    // exploration is done
    for ( Idx varPos = lastInstVarPos + 1; varPos < leadVarPos; ++varPos ) {

      if ( instNeeded[varPos] ) {

        const DiscreteVariable* curVar = __rd->variablesSequence().atPos( varPos );
        NodeId*                 sonsIds = static_cast<NodeId*>(
            SOA_ALLOCATE( sizeof( NodeId ) * curVar->domainSize() ) );

        for ( Idx modality = 0; modality < curVar->domainSize(); modality++ ) {

          currentSituation.chgVarModality( varPos, modality + 1 );

          sonsIds[modality] = __compute( currentSituation, varPos );
        }

        newNode = __rd->manager()->addInternalNode( curVar, sonsIds );

        __explorationTable.insert( curSitKey, newNode );
        currentSituation.chgVarModality( varPos, 0 );
        currentSituation.setDG1Node( origDG1 );
        currentSituation.setDG2Node( origDG2 );

        SOA_DEALLOCATE( instNeeded, sizeof( short int ) * __nbVar );

        return newNode;
      }
    }

    // ====================================================

    // If only one of the current node is terminal,
    // we have to pursue deeper on the other diagram
    if ( sameVar ) {

      // If so - meaning it's the same variable - we have to go
      // down on both
      const InternalNode* dg1Node = __DG1->node( origDG1 );
      const InternalNode* dg2Node = __DG2->node( origDG2 );

      const DiscreteVariable* curVar = dg1Node->nodeVar();
      Idx                     varPos = __rd->variablesSequence().pos( curVar );

      NodeId* sonsIds = static_cast<NodeId*>(
          SOA_ALLOCATE( sizeof( NodeId ) * curVar->domainSize() ) );

      for ( Idx modality = 0; modality < curVar->domainSize(); modality++ ) {

        currentSituation.chgVarModality( varPos, modality + 1 );
        currentSituation.setDG1Node( dg1Node->son( modality ) );
        currentSituation.setDG2Node( dg2Node->son( modality ) );

        sonsIds[modality] = __compute( currentSituation, varPos );
      }

      newNode = __rd->manager()->addInternalNode( curVar, sonsIds );

      __explorationTable.insert( curSitKey, newNode );
      currentSituation.chgVarModality( varPos, 0 );
      currentSituation.setDG1Node( origDG1 );
      currentSituation.setDG2Node( origDG2 );

      SOA_DEALLOCATE( instNeeded, sizeof( short int ) * __nbVar );

      return newNode;
    }
    // ====================================================
    else {


      const InternalNode* leaddgNode = leaddg->node( leadNodeId );

      const DiscreteVariable* curVar = leaddgNode->nodeVar();
      NodeId*                 sonsIds = static_cast<NodeId*>(
          SOA_ALLOCATE( sizeof( NodeId ) * curVar->domainSize() ) );

      for ( Idx modality = 0; modality < curVar->domainSize(); modality++ ) {
        currentSituation.chgVarModality( leadVarPos, modality + 1 );
        ( currentSituation.*leadFunction )( leaddgNode->son( modality ) );

        sonsIds[modality] = __compute( currentSituation, leadVarPos );
      }

      newNode = __rd->manager()->addInternalNode( curVar, sonsIds );

      __explorationTable.insert( curSitKey, newNode );
      currentSituation.chgVarModality( leadVarPos, 0 );
      currentSituation.setDG1Node( origDG1 );
      currentSituation.setDG2Node( origDG2 );

      SOA_DEALLOCATE( instNeeded, sizeof( short int ) * __nbVar );

      return newNode;
    }
  }

  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  INLINE Idx MultiDimFunctionGraphOperator<GUM_SCALAR,
                                           FUNCTOR,
                                           TerminalNodePolicy>::nbCall() {
    return __nbCall;
  }

  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  INLINE Idx MultiDimFunctionGraphOperator<GUM_SCALAR,
                                           FUNCTOR,
                                           TerminalNodePolicy>::nbVarRetro() {
    return __nbVarRetro;
  }

  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  INLINE Idx
         MultiDimFunctionGraphOperator<GUM_SCALAR,
                                FUNCTOR,
                                TerminalNodePolicy>::sizeVarRetroDomain() {
    return __sizeVarRetro;
  }

}  // namespace gum
