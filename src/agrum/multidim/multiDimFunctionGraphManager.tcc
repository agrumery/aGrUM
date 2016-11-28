/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Template methods of gum::MultiDimFunctionGraphManager.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 * @author Jean-Christophe Magnan
 *
 */
#include <agrum/core/sequence.h>
#include <agrum/multidim/FunctionGraphUtilities/link.h>
#include <agrum/multidim/multiDimFunctionGraphManager.h>

namespace gum {

  // Default constructor
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::
      MultiDimFunctionGraphManager(
          MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* mddg ) {
    GUM_CONSTRUCTOR( MultiDimFunctionGraphManager );
    __functionGraph = mddg;
  }

  // Destructor
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::
      ~MultiDimFunctionGraphManager() {
    GUM_DESTRUCTOR( MultiDimFunctionGraphManager );
  }

  /// Sets root node of decision diagram
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::setRootNode(
      const NodeId& root ) {
    __functionGraph->__root = root;
  }

  // Inserts a new non terminal node in graph.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE NodeId
         MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::addInternalNode(
      const DiscreteVariable* var, NodeId nid ) {

    InternalNode* newNodeStruct = new InternalNode( var );

    __functionGraph->__internalNodeMap.insert( nid, newNodeStruct );

    __functionGraph->__var2NodeIdMap[var]->addLink( nid );

    return nid;
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE NodeId
         MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::addInternalNode(
      const DiscreteVariable* var ) {
    InternalNode* newNodeStruct = new InternalNode( var );
    NodeId        nid = __functionGraph->__model.addNode();
    __functionGraph->__internalNodeMap.insert( nid, newNodeStruct );
    __functionGraph->__var2NodeIdMap[var]->addLink( nid );

    return nid;
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE NodeId
         MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::_addInternalNode(
      const DiscreteVariable* var, NodeId* sons ) {
    InternalNode* newNodeStruct = new InternalNode( var, sons );
    NodeId        nid = __functionGraph->__model.addNode();
    __functionGraph->__internalNodeMap.insert( nid, newNodeStruct );
    __functionGraph->__var2NodeIdMap[var]->addLink( nid );
    for ( Idx i = 0; i < newNodeStruct->nbSons(); i++ )
      if ( !__functionGraph->isTerminalNode( sons[i] ) )
        __functionGraph->__internalNodeMap[sons[i]]->addParent( nid, i );

    return nid;
  }

  // Adds a value to the MultiDimFunctionGraph.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE NodeId
         MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::addTerminalNode(
      const GUM_SCALAR& value ) {

    if ( __functionGraph->existsTerminalNodeWithValue( value ) )
      return __functionGraph->terminalNodeId( value );

    NodeId node = __functionGraph->__model.addNode();
    __functionGraph->addTerminalNode( node, value );
    return node;
  }

  // Erases a node from the diagram.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  void MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::eraseNode(
      NodeId eraseId, NodeId replacingId, bool updateParents ) {

    if ( !__functionGraph->__model.exists( eraseId ) )
      GUM_ERROR( NotFound, "Node : " << eraseId << " doesn't exists in the graph" )

    if ( __functionGraph->isTerminalNode( eraseId ) ) {

      for ( auto iterVar = __functionGraph->variablesSequence().begin();
            iterVar != __functionGraph->variablesSequence().end();
            ++iterVar ) {

        Link<NodeId>* nodeIter =
            __functionGraph->__var2NodeIdMap[*iterVar]->list();
        while ( nodeIter != nullptr ) {

          for ( Idx modality = 0; modality < ( *iterVar )->domainSize();
                ++modality )
            if ( __functionGraph->node( nodeIter->element() )->son( modality ) ==
                 eraseId )
              setSon( nodeIter->element(), modality, replacingId );

          nodeIter = nodeIter->nextLink();
        }
      }
      __functionGraph->eraseTerminalNode( eraseId );

    } else {

      InternalNode* eraseNode = __functionGraph->__internalNodeMap[eraseId];

      if ( updateParents ) {
        Link<Parent>* picle = eraseNode->parents();
        while ( picle != nullptr ) {
          setSon(
              picle->element().parentId, picle->element().modality, replacingId );
          picle = picle->nextLink();
        }
      }

      __functionGraph
          ->__var2NodeIdMap[__functionGraph->__internalNodeMap[eraseId]->nodeVar()]
          ->searchAndRemoveLink( eraseId );

      delete __functionGraph->__internalNodeMap[eraseId];
      __functionGraph->__internalNodeMap.erase( eraseId );
    }

    __functionGraph->__model.eraseNode( eraseId );

    if ( __functionGraph->__root == eraseId )
      __functionGraph->__root = replacingId;
  }

  /// Sets nodes son for given modality to designated son node
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::setSon(
      const NodeId& node, const Idx& modality, const NodeId& sonNode ) {

    // Ensuring that both nodes exists in the graph
    if ( !__functionGraph->__model.exists( node ) )
      GUM_ERROR( NotFound, "Node : " << node << " doesn't exists in the graph" )
    if ( !__functionGraph->__model.exists( sonNode ) )
      GUM_ERROR( NotFound, "Node : " << sonNode << " doesn't exists in the graph" )

    // Check if starting node is not terminal
    if ( __functionGraph->isTerminalNode( node ) )
      GUM_ERROR( InvalidNode,
                 "You cannot insert an arc from terminal node : " << node )

    // Check if associated modality is lower than node bound variable domain
    // size
    if ( __functionGraph->isInternalNode( node ) &&
         modality >
             __functionGraph->__internalNodeMap[node]->nodeVar()->domainSize() -
                 1 )
      GUM_ERROR(
          InvalidArgument,
          "Modality "
              << modality
              << "is higher than domain size "
              << __functionGraph->__internalNodeMap[node]->nodeVar()->domainSize()
              << "minus 1 of variable "
              << __functionGraph->__internalNodeMap[node]->nodeVar()->name() )

    // Check if variable order is respected
    if ( __functionGraph->isInternalNode( sonNode ) &&
         __functionGraph->variablesSequence().pos(
             __functionGraph->__internalNodeMap[node]->nodeVar() ) >=
             __functionGraph->variablesSequence().pos(
                 __functionGraph->__internalNodeMap[sonNode]->nodeVar() ) )
      GUM_ERROR( OperationNotAllowed,
                 "Variable "
                     << __functionGraph->__internalNodeMap[node]->nodeVar()
                     << " is after variable "
                     << __functionGraph->__internalNodeMap[sonNode]->nodeVar()
                     << "in Function Graph order." )

    __functionGraph->__internalNodeMap[node]->setSon( modality, sonNode );
    if ( sonNode && !__functionGraph->isTerminalNode( sonNode ) )
      __functionGraph->__internalNodeMap[sonNode]->addParent( node, modality );
  }

  // Changes var position in variable sequence
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  void
  MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::minimizeSize() {

    // Ordering variables by number of nodes asssociated to them
    Sequence<const DiscreteVariable*> siftingSeq;
    HashTable<const DiscreteVariable*, Idx> varLvlSize;
    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              __functionGraph->variablesSequence().beginSafe();
          varIter != __functionGraph->variablesSequence().endSafe();
          ++varIter ) {
      const Link<NodeId>* curElem =
          __functionGraph->__var2NodeIdMap[*varIter]->list();
      Idx nbElem = 0;
      for ( ; curElem != nullptr; nbElem++, curElem = curElem->nextLink() )
        ;
      varLvlSize.insert( *varIter, nbElem );
      siftingSeq.insert( *varIter );
      Idx pos = siftingSeq.pos( *varIter );
      while ( pos > 0 && varLvlSize[siftingSeq.atPos( pos - 1 )] > nbElem ) {
        siftingSeq.swap( pos - 1, pos );
        pos--;
      }
    }

    // Sifting var par var
    for ( SequenceIteratorSafe<const DiscreteVariable*> sifIter =
              siftingSeq.beginSafe();
          sifIter != siftingSeq.endSafe();
          ++sifIter ) {

      // Initialization
      Idx currentPos = __functionGraph->variablesSequence().pos( *sifIter );
      Idx bestSize = __functionGraph->realSize();
      Idx bestPos = currentPos;


      // Sifting towards upper places
      while ( currentPos > 0 ) {
        moveTo( *sifIter, currentPos - 1 );
        currentPos = __functionGraph->variablesSequence().pos( *sifIter );
        if ( __functionGraph->realSize() < bestSize ) {
          bestPos = currentPos;
          bestSize = __functionGraph->realSize();
        }
      }

      // Sifting towards lower places
      while ( currentPos < __functionGraph->variablesSequence().size() - 1 ) {
        moveTo( *sifIter, currentPos + 1 );
        currentPos = __functionGraph->variablesSequence().pos( *sifIter );
        if ( __functionGraph->realSize() < bestSize ) {
          bestPos = currentPos;
          bestSize = __functionGraph->realSize();
        }
      }

      moveTo( *sifIter, bestPos );
    }
  }

  // Changes var position in variable sequence
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  void MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::moveTo(
      const DiscreteVariable* movedVar, Idx desiredPos ) {

    // First we determine the position of both variable
    // We also determine which one precede the other
    if ( __functionGraph->variablesSequence().pos( movedVar ) > desiredPos )
      for ( Idx currentPos = __functionGraph->variablesSequence().pos( movedVar );
            currentPos != desiredPos;
            currentPos-- ) {
        const DiscreteVariable* preVar =
            __functionGraph->variablesSequence().atPos( currentPos - 1 );
        if ( __functionGraph->__var2NodeIdMap[preVar]->list() &&
             __functionGraph->__var2NodeIdMap[movedVar]->list() )
          __adjacentSwap( preVar, movedVar );
        __functionGraph->_invert( currentPos - 1, currentPos );
      }
    else
      for ( Idx currentPos = __functionGraph->variablesSequence().pos( movedVar );
            currentPos != desiredPos;
            currentPos++ ) {
        const DiscreteVariable* suiVar =
            __functionGraph->variablesSequence().atPos( currentPos + 1 );
        if ( __functionGraph->__var2NodeIdMap[suiVar]->list() &&
             __functionGraph->__var2NodeIdMap[movedVar]->list() )
          __adjacentSwap( movedVar, suiVar );
        __functionGraph->_invert( currentPos, currentPos + 1 );
      }
  }

  // Swap two adjacent variable.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  void
  MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::__adjacentSwap(
      const DiscreteVariable* x, const DiscreteVariable* y ) {

    LinkedList<NodeId>* oldxNodes = __functionGraph->__var2NodeIdMap[x];
    __functionGraph->__var2NodeIdMap[x] = new LinkedList<NodeId>();
    LinkedList<NodeId>* oldyNodes = __functionGraph->__var2NodeIdMap[y];
    __functionGraph->__var2NodeIdMap[y] = new LinkedList<NodeId>();


    InternalNode* currentOldXNode = nullptr;
    NodeId*       currentNewXNodeSons = nullptr;
    Idx           indx = 0;

    NodeId* currentNewYNodeSons = nullptr;
    NodeId  currentNewYNodeId = 0;
    Idx     indy = 0;

    while ( oldxNodes->list() ) {

      // Recuperating a node associated to variables x
      currentOldXNode =
          __functionGraph->__internalNodeMap[oldxNodes->list()->element()];

      // Creating a new node associated to variable y
      currentNewYNodeSons = InternalNode::allocateNodeSons( y );

      // Now the graph needs to be remap by inserting nodes bound to x
      // for each values assumed by y
      for ( indy = 0; indy < y->domainSize(); ++indy ) {

        // Creating a new node bound to x that will be the son of the node
        // tied to y for the current value assumed by y
        currentNewXNodeSons = InternalNode::allocateNodeSons( x );

        // Iterating on the different values taht x can assumed to do the remap
        for ( indx = 0; indx < x->domainSize(); ++indx ) {
          currentNewXNodeSons[indx] = currentOldXNode->son( indx );
          if ( !__functionGraph->isTerminalNode( currentOldXNode->son( indx ) ) &&
               __functionGraph->node( currentOldXNode->son( indx ) )->nodeVar() ==
                   y )
            currentNewXNodeSons[indx] =
                __functionGraph->node( currentOldXNode->son( indx ) )->son( indy );
        }

        // Inserting the new node bound to x
        currentNewYNodeSons[indy] = _nodeRedundancyCheck( x, currentNewXNodeSons );
      }

      // Replacing old node x by new node y
      currentNewYNodeId = currentNewYNodeSons[0];
      if ( __isRedundant( y, currentNewYNodeSons ) ) {
        _migrateNode( oldxNodes->list()->element(), currentNewYNodeId );
        SOA_DEALLOCATE( currentNewYNodeSons, y->domainSize() * sizeof( NodeId ) );
      } else {
        currentNewYNodeId = __checkIsomorphism( y, currentNewYNodeSons );
        if ( currentNewYNodeId != 0 ) {
          _migrateNode( oldxNodes->list()->element(), currentNewYNodeId );
          SOA_DEALLOCATE( currentNewYNodeSons,
                          y->domainSize() * sizeof( NodeId ) );
        } else {
          // Updating the sons (they must not consider old x as their parent)
          for ( Idx i = 0; i < currentOldXNode->nodeVar()->domainSize(); ++i ) {
            if ( __functionGraph->__internalNodeMap.exists(
                     currentOldXNode->son( i ) ) ) {
              __functionGraph->__internalNodeMap[currentOldXNode->son( i )]
                  ->removeParent( oldxNodes->list()->element(), i );
            }
          }
          // Reaffecting old node x internal attributes to correct new one
          currentOldXNode->setNode( y, currentNewYNodeSons );
          // Updating new sons (they must consider the node as a parent)
          for ( Idx i = 0; i < currentOldXNode->nodeVar()->domainSize(); ++i ) {
            if ( __functionGraph->__internalNodeMap.exists(
                     currentNewYNodeSons[i] ) ) {
              __functionGraph->__internalNodeMap[currentNewYNodeSons[i]]
                  ->addParent( oldxNodes->list()->element(), i );
            }
          }

          __functionGraph->__var2NodeIdMap[y]->addLink(
              oldxNodes->list()->element() );
        }
      }

      oldxNodes->searchAndRemoveLink( oldxNodes->list()->element() );
    }
    delete oldxNodes;

    while ( oldyNodes->list() ) {
      NodeId curId = oldyNodes->list()->element();
      if ( __functionGraph->__internalNodeMap[curId]->parents() == nullptr ) {

        for ( Idx i = 0;
              i <
              __functionGraph->__internalNodeMap[curId]->nodeVar()->domainSize();
              ++i )
          if ( __functionGraph->__internalNodeMap.exists(
                   __functionGraph->__internalNodeMap[curId]->son( i ) ) )
            __functionGraph
                ->__internalNodeMap[__functionGraph->__internalNodeMap[curId]->son(
                    i )]
                ->removeParent( curId, i );
        delete __functionGraph->__internalNodeMap[curId];
        __functionGraph->__internalNodeMap.erase( curId );
        __functionGraph->__model.eraseNode( curId );
      } else {
        __functionGraph->__var2NodeIdMap[y]->addLink( curId );
      }
      oldyNodes->searchAndRemoveLink( curId );
    }
    delete oldyNodes;
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::_migrateNode(
      const NodeId& origin, const NodeId& destination ) {

    InternalNode* org = __functionGraph->__internalNodeMap[origin];
    // Upating parents after the change
    Link<Parent>* picle = org->parents();
    while ( picle != nullptr ) {
      setSon( picle->element().parentId, picle->element().modality, destination );
      picle = picle->nextLink();
    }

    // Updating sons after the change
    for ( Idx i = 0; i < org->nbSons(); ++i )
      if ( __functionGraph->__internalNodeMap.exists( org->son( i ) ) )
        __functionGraph->__internalNodeMap[org->son( i )]->removeParent( origin,
                                                                         i );

    delete org;
    __functionGraph->__internalNodeMap.erase( origin );
    __functionGraph->__model.eraseNode( origin );

    if ( __functionGraph->root() == origin ) this->setRootNode( destination );
  }

  // Checks if a similar node does not already exists in the graph or
  // if it has the same child for every variable value.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE NodeId MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::
      _nodeRedundancyCheck( const DiscreteVariable* var, NodeId* sonsIds ) {

    NodeId newNode = sonsIds[0];

    if ( __isRedundant( var, sonsIds ) ) {
      SOA_DEALLOCATE( sonsIds, sizeof( NodeId ) * var->domainSize() );
    } else {
      newNode = __checkIsomorphism( var, sonsIds );
      if ( newNode == 0 ) {
        newNode = _addInternalNode( var, sonsIds );
      } else {
        SOA_DEALLOCATE( sonsIds, sizeof( NodeId ) * var->domainSize() );
      }
    }

    return newNode;
  }

  // Checks if a similar node does not already exists in the graph.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE NodeId
         MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::__checkIsomorphism(
      const DiscreteVariable* var, NodeId* sons ) {

    const InternalNode* nody = nullptr;
    Idx                 i = 0;

    // Check abscence of identical node
    Link<NodeId>* currentElem = __functionGraph->__var2NodeIdMap[var]->list();
    while ( currentElem != nullptr ) {

      nody = __functionGraph->__internalNodeMap[currentElem->element()];

      // Check on the other sons
      i = 0;
      while ( i < var->domainSize() && sons[i] == nody->son( i ) )
        ++i;
      if ( i == var->domainSize() ) return currentElem->element();

      currentElem = currentElem->nextLink();
    }
    return 0;
  }

  // Checks if node has the same child for every variable value
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE bool
  MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::__isRedundant(
      const DiscreteVariable* var, NodeId* sons ) {
    for ( Idx m = 1; m < var->domainSize(); m++ )
      if ( sons[m] != sons[0] ) return false;
    return true;
  }

  // Ensures that every isomorphic subgraphs are merged together.
  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::_reduce() {
    Link<NodeId>* currentNodeId = nullptr;
    Link<NodeId>* nextNodeId = nullptr;
    InternalNode* currentNode = nullptr;
    bool          theSame = true;
    Idx           currentInd;

    for ( SequenceIterator<const DiscreteVariable*> varIter =
              __functionGraph->variablesSequence().rbegin();
          varIter != __functionGraph->variablesSequence().rend();
          --varIter ) {

      currentNodeId = __functionGraph->__var2NodeIdMap[*varIter]->list();

      while ( currentNodeId != nullptr ) {

        nextNodeId = currentNodeId->nextLink();
        currentNode = __functionGraph->__internalNodeMap[currentNodeId->element()];

        // First isomorphism to handle is the one where all node children are
        // the same
        theSame = true;
        for ( currentInd = 1; currentInd < ( *varIter )->domainSize();
              currentInd++ ) {
          if ( currentNode->son( currentInd ) != currentNode->son( 0 ) ) {
            theSame = false;
            break;
          }
        }

        if ( theSame == true ) {
          _migrateNode( currentNodeId->element(), currentNode->son( 0 ) );
          __functionGraph->__var2NodeIdMap[*varIter]->searchAndRemoveLink(
              currentNodeId->element() );
          currentNodeId = nextNodeId;
          continue;
        }

        // Second isomorphism to handle is the one where two nodes have same
        // variable and same children
        if ( nextNodeId ) {
          Link<NodeId>* anotherNodeId = currentNodeId->nextLink();
          InternalNode* anotherNode = nullptr;
          Idx           modality = 0;
          while ( anotherNodeId->nextLink() != nullptr ) {

            nextNodeId = anotherNodeId->nextLink();
            anotherNode =
                __functionGraph->__internalNodeMap[anotherNodeId->element()];

            // Check on the other sons
            for ( modality = 0; modality < ( *varIter )->domainSize();
                  ++modality ) {
              if ( anotherNode->son( modality ) != currentNode->son( modality ) )
                break;
              if ( modality == ( *varIter )->domainSize() - 1 ) {
                _migrateNode( anotherNodeId->element(), currentNodeId->element() );
                __functionGraph->__var2NodeIdMap[*varIter]->searchAndRemoveLink(
                    anotherNodeId->element() );
              }
            }

            anotherNodeId = nextNodeId;
          }
        }
        currentNodeId = currentNodeId->nextLink();
      }
    }
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  INLINE void
  MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>::clean() {
    Sequence<const DiscreteVariable*> oldSequence(
        __functionGraph->variablesSequence() );
    for ( SequenceIterator<const DiscreteVariable*> varIter = oldSequence.begin();
          varIter != oldSequence.end();
          ++varIter )
      if ( !__functionGraph->varNodeListe( *varIter )->list() )
        __functionGraph->erase( **varIter );
  }

  // ==========================================================================
  // MultiDimFunctionGraphTreeManager
  // ==========================================================================

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  MultiDimFunctionGraphTreeManager<GUM_SCALAR, TerminalNodePolicy>::
      MultiDimFunctionGraphTreeManager(
          MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* master )
      : MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>( master ) {
    GUM_CONSTRUCTOR( MultiDimFunctionGraphTreeManager );
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  MultiDimFunctionGraphTreeManager<GUM_SCALAR, TerminalNodePolicy>::
      ~MultiDimFunctionGraphTreeManager() {
    GUM_DESTRUCTOR( MultiDimFunctionGraphTreeManager );
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  NodeId MultiDimFunctionGraphTreeManager<GUM_SCALAR, TerminalNodePolicy>::
      addInternalNode( const DiscreteVariable* var, NodeId* sons ) {
    return this->_addInternalNode( var, sons );
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  void MultiDimFunctionGraphTreeManager<GUM_SCALAR, TerminalNodePolicy>::reduce() {
  }

  // ===========================================================================
  // MultiDimFunctionGraphROManager
  // ===========================================================================

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  MultiDimFunctionGraphROManager<GUM_SCALAR, TerminalNodePolicy>::
      MultiDimFunctionGraphROManager(
          MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* master )
      : MultiDimFunctionGraphManager<GUM_SCALAR, TerminalNodePolicy>( master ) {
    GUM_CONSTRUCTOR( MultiDimFunctionGraphROManager );
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  MultiDimFunctionGraphROManager<GUM_SCALAR, TerminalNodePolicy>::
      ~MultiDimFunctionGraphROManager() {
    GUM_DESTRUCTOR( MultiDimFunctionGraphROManager );
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  NodeId
  MultiDimFunctionGraphROManager<GUM_SCALAR, TerminalNodePolicy>::addInternalNode(
      const DiscreteVariable* var, NodeId* sons ) {
    return this->_nodeRedundancyCheck( var, sons );
  }

  template <typename GUM_SCALAR, template <class> class TerminalNodePolicy>
  void MultiDimFunctionGraphROManager<GUM_SCALAR, TerminalNodePolicy>::reduce() {
    this->_reduce();
  }

}  // namespace gum
