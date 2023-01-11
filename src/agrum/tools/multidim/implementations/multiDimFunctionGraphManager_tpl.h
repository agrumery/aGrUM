/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Template methods of gum::MultiDimFunctionGraphManager.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */
#include <agrum/tools/core/sequence.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraphManager.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/link.h>

namespace gum {

  // Default constructor
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE
     MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::MultiDimFunctionGraphManager(
        MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* mddg) {
    GUM_CONSTRUCTOR(MultiDimFunctionGraphManager);
    _functionGraph_ = mddg;
  }

  // Destructor
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE MultiDimFunctionGraphManager< GUM_SCALAR,
                                       TerminalNodePolicy >::~MultiDimFunctionGraphManager() {
    GUM_DESTRUCTOR(MultiDimFunctionGraphManager);
  }

  /// Sets root node of decision diagram
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::setRootNode(
     const NodeId& root) {
    _functionGraph_->_root_ = root;
  }

  // Inserts a new non terminal node in graph.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::addInternalNode(
     const DiscreteVariable* var,
     NodeId                  nid) {
    InternalNode* newNodeStruct = new InternalNode(var);

    _functionGraph_->_internalNodeMap_.insert(nid, newNodeStruct);

    _functionGraph_->_var2NodeIdMap_[var]->addLink(nid);

    return nid;
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::addInternalNode(
     const DiscreteVariable* var) {
    InternalNode* newNodeStruct = new InternalNode(var);
    NodeId        nid           = _functionGraph_->_model_.addNode();
    _functionGraph_->_internalNodeMap_.insert(nid, newNodeStruct);
    _functionGraph_->_var2NodeIdMap_[var]->addLink(nid);

    return nid;
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::addInternalNode_(
     const DiscreteVariable* var,
     NodeId*                 sons) {
    InternalNode* newNodeStruct = new InternalNode(var, sons);
    NodeId        nid           = _functionGraph_->_model_.addNode();
    _functionGraph_->_internalNodeMap_.insert(nid, newNodeStruct);
    _functionGraph_->_var2NodeIdMap_[var]->addLink(nid);
    for (Idx i = 0; i < newNodeStruct->nbSons(); i++)
      if (!_functionGraph_->isTerminalNode(sons[i]))
        _functionGraph_->_internalNodeMap_[sons[i]]->addParent(nid, i);

    return nid;
  }

  // Adds a value to the MultiDimFunctionGraph.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::addTerminalNode(
     const GUM_SCALAR& value) {
    if (_functionGraph_->existsTerminalNodeWithValue(value))
      return _functionGraph_->terminalNodeId(value);

    NodeId node = _functionGraph_->_model_.addNode();
    _functionGraph_->addTerminalNode(node, value);
    return node;
  }

  // Erases a node from the diagram.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void
     MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::eraseNode(NodeId eraseId,
                                                                               NodeId replacingId,
                                                                               bool updateParents) {
    if (!_functionGraph_->_model_.exists(eraseId))
      GUM_ERROR(NotFound, "Node : " << eraseId << " doesn't exists in the graph")

    if (_functionGraph_->isTerminalNode(eraseId)) {
      for (auto iterVar = _functionGraph_->variablesSequence().begin();
           iterVar != _functionGraph_->variablesSequence().end();
           ++iterVar) {
        Link< NodeId >* nodeIter = _functionGraph_->_var2NodeIdMap_[*iterVar]->list();
        while (nodeIter != nullptr) {
          for (Idx modality = 0; modality < (*iterVar)->domainSize(); ++modality)
            if (_functionGraph_->node(nodeIter->element())->son(modality) == eraseId)
              setSon(nodeIter->element(), modality, replacingId);

          nodeIter = nodeIter->nextLink();
        }
      }
      _functionGraph_->eraseTerminalNode(eraseId);

    } else {
      InternalNode* eraseNode = _functionGraph_->_internalNodeMap_[eraseId];

      if (updateParents) {
        Link< Parent >* picle = eraseNode->parents();
        while (picle != nullptr) {
          setSon(picle->element().parentId, picle->element().modality, replacingId);
          picle = picle->nextLink();
        }
      }

      _functionGraph_->_var2NodeIdMap_[_functionGraph_->_internalNodeMap_[eraseId]->nodeVar()]
         ->searchAndRemoveLink(eraseId);

      delete _functionGraph_->_internalNodeMap_[eraseId];
      _functionGraph_->_internalNodeMap_.erase(eraseId);
    }

    _functionGraph_->_model_.eraseNode(eraseId);

    if (_functionGraph_->_root_ == eraseId) _functionGraph_->_root_ = replacingId;
  }

  /// Sets nodes son for given modality to designated son node
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE void
     MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::setSon(const NodeId& node,
                                                                            const Idx&    modality,
                                                                            const NodeId& sonNode) {
    // Ensuring that both nodes exists in the graph
    if (!_functionGraph_->_model_.exists(node))
      GUM_ERROR(NotFound, "Node : " << node << " doesn't exists in the graph")
    if (!_functionGraph_->_model_.exists(sonNode))
      GUM_ERROR(NotFound, "Node : " << sonNode << " doesn't exists in the graph")

    // Check if starting node is not terminal
    if (_functionGraph_->isTerminalNode(node))
      GUM_ERROR(InvalidNode, "You cannot insert an arc from terminal node : " << node)

    // Check if associated modality is lower than node bound variable domain
    // size
    if (_functionGraph_->isInternalNode(node)
        && modality > _functionGraph_->_internalNodeMap_[node]->nodeVar()->domainSize() - 1)
      GUM_ERROR(InvalidArgument,
                "Modality " << modality << "is higher than domain size "
                            << _functionGraph_->_internalNodeMap_[node]->nodeVar()->domainSize()
                            << "minus 1 of variable "
                            << _functionGraph_->_internalNodeMap_[node]->nodeVar()->name())

    // Check if variable order is respected
    if (_functionGraph_->isInternalNode(sonNode)
        && _functionGraph_->variablesSequence().pos(
              _functionGraph_->_internalNodeMap_[node]->nodeVar())
              >= _functionGraph_->variablesSequence().pos(
                 _functionGraph_->_internalNodeMap_[sonNode]->nodeVar()))
      GUM_ERROR(OperationNotAllowed,
                "Variable " << _functionGraph_->_internalNodeMap_[node]->nodeVar()
                            << " is after variable "
                            << _functionGraph_->_internalNodeMap_[sonNode]->nodeVar()
                            << "in Function Graph order.")

    _functionGraph_->_internalNodeMap_[node]->setSon(modality, sonNode);
    if (sonNode && !_functionGraph_->isTerminalNode(sonNode))
      _functionGraph_->_internalNodeMap_[sonNode]->addParent(node, modality);
  }

  // Changes var position in variable sequence
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::minimizeSize() {
    // Ordering variables by number of nodes asssociated to them
    Sequence< const DiscreteVariable* >       siftingSeq;
    HashTable< const DiscreteVariable*, Idx > varLvlSize;
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter
         = _functionGraph_->variablesSequence().beginSafe();
         varIter != _functionGraph_->variablesSequence().endSafe();
         ++varIter) {
      const Link< NodeId >* curElem = _functionGraph_->_var2NodeIdMap_[*varIter]->list();
      Idx                   nbElem  = 0;
      for (; curElem != nullptr; nbElem++, curElem = curElem->nextLink())
        ;
      varLvlSize.insert(*varIter, nbElem);
      siftingSeq.insert(*varIter);
      Idx pos = siftingSeq.pos(*varIter);
      while (pos > 0 && varLvlSize[siftingSeq.atPos(pos - 1)] > nbElem) {
        siftingSeq.swap(pos - 1, pos);
        pos--;
      }
    }

    // Sifting var par var
    for (SequenceIteratorSafe< const DiscreteVariable* > sifIter = siftingSeq.beginSafe();
         sifIter != siftingSeq.endSafe();
         ++sifIter) {
      // Initialization
      Idx currentPos = _functionGraph_->variablesSequence().pos(*sifIter);
      Idx bestSize   = _functionGraph_->realSize();
      Idx bestPos    = currentPos;


      // Sifting towards upper places
      while (currentPos > 0) {
        moveTo(*sifIter, currentPos - 1);
        currentPos = _functionGraph_->variablesSequence().pos(*sifIter);
        if (_functionGraph_->realSize() < bestSize) {
          bestPos  = currentPos;
          bestSize = _functionGraph_->realSize();
        }
      }

      // Sifting towards lower places
      while (currentPos < _functionGraph_->variablesSequence().size() - 1) {
        moveTo(*sifIter, currentPos + 1);
        currentPos = _functionGraph_->variablesSequence().pos(*sifIter);
        if (_functionGraph_->realSize() < bestSize) {
          bestPos  = currentPos;
          bestSize = _functionGraph_->realSize();
        }
      }

      moveTo(*sifIter, bestPos);
    }
  }

  // Changes var position in variable sequence
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::moveTo(
     const DiscreteVariable* movedVar,
     Idx                     desiredPos) {
    // First we determine the position of both variable
    // We also determine which one precede the other
    if (_functionGraph_->variablesSequence().pos(movedVar) > desiredPos)
      for (Idx currentPos = _functionGraph_->variablesSequence().pos(movedVar);
           currentPos != desiredPos;
           currentPos--) {
        const DiscreteVariable* preVar = _functionGraph_->variablesSequence().atPos(currentPos - 1);
        if (_functionGraph_->_var2NodeIdMap_[preVar]->list()
            && _functionGraph_->_var2NodeIdMap_[movedVar]->list())
          _adjacentSwap_(preVar, movedVar);
        _functionGraph_->invert_(currentPos - 1, currentPos);
      }
    else
      for (Idx currentPos = _functionGraph_->variablesSequence().pos(movedVar);
           currentPos != desiredPos;
           currentPos++) {
        const DiscreteVariable* suiVar = _functionGraph_->variablesSequence().atPos(currentPos + 1);
        if (_functionGraph_->_var2NodeIdMap_[suiVar]->list()
            && _functionGraph_->_var2NodeIdMap_[movedVar]->list())
          _adjacentSwap_(movedVar, suiVar);
        _functionGraph_->invert_(currentPos, currentPos + 1);
      }
  }

  // Swap two adjacent variable.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::_adjacentSwap_(
     const DiscreteVariable* x,
     const DiscreteVariable* y) {
    LinkedList< NodeId >* oldxNodes     = _functionGraph_->_var2NodeIdMap_[x];
    _functionGraph_->_var2NodeIdMap_[x] = new LinkedList< NodeId >();
    LinkedList< NodeId >* oldyNodes     = _functionGraph_->_var2NodeIdMap_[y];
    _functionGraph_->_var2NodeIdMap_[y] = new LinkedList< NodeId >();


    InternalNode* currentOldXNode     = nullptr;
    NodeId*       currentNewXNodeSons = nullptr;
    Idx           indx                = 0;

    NodeId* currentNewYNodeSons = nullptr;
    NodeId  currentNewYNodeId   = 0;
    Idx     indy                = 0;

    while (oldxNodes->list()) {
      // Recuperating a node associated to variables x
      currentOldXNode = _functionGraph_->_internalNodeMap_[oldxNodes->list()->element()];

      // Creating a new node associated to variable y
      currentNewYNodeSons = InternalNode::allocateNodeSons(y);

      // Now the graph needs to be remap by inserting nodes bound to x
      // for each values assumed by y
      for (indy = 0; indy < y->domainSize(); ++indy) {
        // Creating a new node bound to x that will be the son of the node
        // tied to y for the current value assumed by y
        currentNewXNodeSons = InternalNode::allocateNodeSons(x);

        // Iterating on the different values taht x can assumed to do the remap
        for (indx = 0; indx < x->domainSize(); ++indx) {
          currentNewXNodeSons[indx] = currentOldXNode->son(indx);
          if (!_functionGraph_->isTerminalNode(currentOldXNode->son(indx))
              && _functionGraph_->node(currentOldXNode->son(indx))->nodeVar() == y)
            currentNewXNodeSons[indx]
               = _functionGraph_->node(currentOldXNode->son(indx))->son(indy);
        }

        // Inserting the new node bound to x
        currentNewYNodeSons[indy] = nodeRedundancyCheck_(x, currentNewXNodeSons);
      }

      // Replacing old node x by new node y
      currentNewYNodeId = currentNewYNodeSons[0];
      if (_isRedundant_(y, currentNewYNodeSons)) {
        migrateNode_(oldxNodes->list()->element(), currentNewYNodeId);
        SOA_DEALLOCATE(currentNewYNodeSons, y->domainSize() * sizeof(NodeId));
      } else {
        currentNewYNodeId = _checkIsomorphism_(y, currentNewYNodeSons);
        if (currentNewYNodeId != 0) {
          migrateNode_(oldxNodes->list()->element(), currentNewYNodeId);
          SOA_DEALLOCATE(currentNewYNodeSons, y->domainSize() * sizeof(NodeId));
        } else {
          // Updating the sons (they must not consider old x as their parent)
          for (Idx i = 0; i < currentOldXNode->nodeVar()->domainSize(); ++i) {
            if (_functionGraph_->_internalNodeMap_.exists(currentOldXNode->son(i))) {
              _functionGraph_->_internalNodeMap_[currentOldXNode->son(i)]->removeParent(
                 oldxNodes->list()->element(),
                 i);
            }
          }
          // Reaffecting old node x internal attributes to correct new one
          currentOldXNode->setNode(y, currentNewYNodeSons);
          // Updating new sons (they must consider the node as a parent)
          for (Idx i = 0; i < currentOldXNode->nodeVar()->domainSize(); ++i) {
            if (_functionGraph_->_internalNodeMap_.exists(currentNewYNodeSons[i])) {
              _functionGraph_->_internalNodeMap_[currentNewYNodeSons[i]]->addParent(
                 oldxNodes->list()->element(),
                 i);
            }
          }

          _functionGraph_->_var2NodeIdMap_[y]->addLink(oldxNodes->list()->element());
        }
      }

      oldxNodes->searchAndRemoveLink(oldxNodes->list()->element());
    }
    delete oldxNodes;

    while (oldyNodes->list()) {
      NodeId curId = oldyNodes->list()->element();
      if (_functionGraph_->_internalNodeMap_[curId]->parents() == nullptr) {
        for (Idx i = 0; i < _functionGraph_->_internalNodeMap_[curId]->nodeVar()->domainSize(); ++i)
          if (_functionGraph_->_internalNodeMap_.exists(
                 _functionGraph_->_internalNodeMap_[curId]->son(i)))
            _functionGraph_->_internalNodeMap_[_functionGraph_->_internalNodeMap_[curId]->son(i)]
               ->removeParent(curId, i);
        delete _functionGraph_->_internalNodeMap_[curId];
        _functionGraph_->_internalNodeMap_.erase(curId);
        _functionGraph_->_model_.eraseNode(curId);
      } else {
        _functionGraph_->_var2NodeIdMap_[y]->addLink(curId);
      }
      oldyNodes->searchAndRemoveLink(curId);
    }
    delete oldyNodes;
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::migrateNode_(
     const NodeId& origin,
     const NodeId& destination) {
    InternalNode* org = _functionGraph_->_internalNodeMap_[origin];
    // Upating parents after the change
    Link< Parent >* picle = org->parents();
    while (picle != nullptr) {
      setSon(picle->element().parentId, picle->element().modality, destination);
      picle = picle->nextLink();
    }

    // Updating sons after the change
    for (Idx i = 0; i < org->nbSons(); ++i)
      if (_functionGraph_->_internalNodeMap_.exists(org->son(i)))
        _functionGraph_->_internalNodeMap_[org->son(i)]->removeParent(origin, i);

    delete org;
    _functionGraph_->_internalNodeMap_.erase(origin);
    _functionGraph_->_model_.eraseNode(origin);

    if (_functionGraph_->root() == origin) this->setRootNode(destination);
  }

  // Checks if a similar node does not already exists in the graph or
  // if it has the same child for every variable value.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId
     MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::nodeRedundancyCheck_(
        const DiscreteVariable* var,
        NodeId*                 sonsIds) {
    NodeId newNode = sonsIds[0];

    if (_isRedundant_(var, sonsIds)) {
      SOA_DEALLOCATE(sonsIds, sizeof(NodeId) * var->domainSize());
    } else {
      newNode = _checkIsomorphism_(var, sonsIds);
      if (newNode == 0) {
        newNode = addInternalNode_(var, sonsIds);
      } else {
        SOA_DEALLOCATE(sonsIds, sizeof(NodeId) * var->domainSize());
      }
    }

    return newNode;
  }

  // Checks if a similar node does not already exists in the graph.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::_checkIsomorphism_(
     const DiscreteVariable* var,
     NodeId*                 sons) {
    const InternalNode* nody = nullptr;
    Idx                 i    = 0;

    // Check abscence of identical node
    Link< NodeId >* currentElem = _functionGraph_->_var2NodeIdMap_[var]->list();
    while (currentElem != nullptr) {
      nody = _functionGraph_->_internalNodeMap_[currentElem->element()];

      // Check on the other sons
      i = 0;
      while (i < var->domainSize() && sons[i] == nody->son(i))
        ++i;
      if (i == var->domainSize()) return currentElem->element();

      currentElem = currentElem->nextLink();
    }
    return 0;
  }

  // Checks if node has the same child for every variable value
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE bool MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::_isRedundant_(
     const DiscreteVariable* var,
     NodeId*                 sons) {
    for (Idx m = 1; m < var->domainSize(); m++)
      if (sons[m] != sons[0]) return false;
    return true;
  }

  // Ensures that every isomorphic subgraphs are merged together.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::reduce_() {
    Link< NodeId >* currentNodeId = nullptr;
    Link< NodeId >* nextNodeId    = nullptr;
    InternalNode*   currentNode   = nullptr;
    bool            theSame       = true;
    Idx             currentInd;

    for (SequenceIterator< const DiscreteVariable* > varIter
         = _functionGraph_->variablesSequence().rbegin();
         varIter != _functionGraph_->variablesSequence().rend();
         --varIter) {
      currentNodeId = _functionGraph_->_var2NodeIdMap_[*varIter]->list();

      while (currentNodeId != nullptr) {
        nextNodeId  = currentNodeId->nextLink();
        currentNode = _functionGraph_->_internalNodeMap_[currentNodeId->element()];

        // First isomorphism to handle is the one where all node children are
        // the same
        theSame = true;
        for (currentInd = 1; currentInd < (*varIter)->domainSize(); currentInd++) {
          if (currentNode->son(currentInd) != currentNode->son(0)) {
            theSame = false;
            break;
          }
        }

        if (theSame == true) {
          migrateNode_(currentNodeId->element(), currentNode->son(0));
          _functionGraph_->_var2NodeIdMap_[*varIter]->searchAndRemoveLink(currentNodeId->element());
          currentNodeId = nextNodeId;
          continue;
        }

        // Second isomorphism to handle is the one where two nodes have same
        // variable and same children
        if (nextNodeId) {
          Link< NodeId >* anotherNodeId = currentNodeId->nextLink();
          InternalNode*   anotherNode   = nullptr;
          Idx             modality      = 0;
          while (anotherNodeId->nextLink() != nullptr) {
            nextNodeId  = anotherNodeId->nextLink();
            anotherNode = _functionGraph_->_internalNodeMap_[anotherNodeId->element()];

            // Check on the other sons
            for (modality = 0; modality < (*varIter)->domainSize(); ++modality) {
              if (anotherNode->son(modality) != currentNode->son(modality)) break;
              if (modality == (*varIter)->domainSize() - 1) {
                migrateNode_(anotherNodeId->element(), currentNodeId->element());
                _functionGraph_->_var2NodeIdMap_[*varIter]->searchAndRemoveLink(
                   anotherNodeId->element());
              }
            }

            anotherNodeId = nextNodeId;
          }
        }
        currentNodeId = currentNodeId->nextLink();
      }
    }
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::clean() {
    Sequence< const DiscreteVariable* > oldSequence(_functionGraph_->variablesSequence());
    for (SequenceIterator< const DiscreteVariable* > varIter = oldSequence.begin();
         varIter != oldSequence.end();
         ++varIter)
      if (!_functionGraph_->varNodeListe(*varIter)->list()) _functionGraph_->erase(**varIter);
  }

  // ==========================================================================
  // MultiDimFunctionGraphTreeManager
  // ==========================================================================

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  MultiDimFunctionGraphTreeManager< GUM_SCALAR, TerminalNodePolicy >::
     MultiDimFunctionGraphTreeManager(
        MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* master) :
      MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >(master) {
    GUM_CONSTRUCTOR(MultiDimFunctionGraphTreeManager);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  MultiDimFunctionGraphTreeManager< GUM_SCALAR,
                                    TerminalNodePolicy >::~MultiDimFunctionGraphTreeManager() {
    GUM_DESTRUCTOR(MultiDimFunctionGraphTreeManager);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  NodeId MultiDimFunctionGraphTreeManager< GUM_SCALAR, TerminalNodePolicy >::addInternalNode(
     const DiscreteVariable* var,
     NodeId*                 sons) {
    return this->addInternalNode_(var, sons);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void MultiDimFunctionGraphTreeManager< GUM_SCALAR, TerminalNodePolicy >::reduce() {}

  // ===========================================================================
  // MultiDimFunctionGraphROManager
  // ===========================================================================

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  MultiDimFunctionGraphROManager< GUM_SCALAR, TerminalNodePolicy >::MultiDimFunctionGraphROManager(
     MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* master) :
      MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >(master) {
    GUM_CONSTRUCTOR(MultiDimFunctionGraphROManager);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  MultiDimFunctionGraphROManager< GUM_SCALAR,
                                  TerminalNodePolicy >::~MultiDimFunctionGraphROManager() {
    GUM_DESTRUCTOR(MultiDimFunctionGraphROManager);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  NodeId MultiDimFunctionGraphROManager< GUM_SCALAR, TerminalNodePolicy >::addInternalNode(
     const DiscreteVariable* var,
     NodeId*                 sons) {
    return this->nodeRedundancyCheck_(var, sons);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void MultiDimFunctionGraphROManager< GUM_SCALAR, TerminalNodePolicy >::reduce() {
    this->reduce_();
  }

}   // namespace gum
