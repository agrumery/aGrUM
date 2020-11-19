/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe GONZALES(@AMU)
 *
 */
#include <agrum/tools/core/sequence.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraphManager.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/link.h>

namespace gum {

  // Default constructor
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::
     MultiDimFunctionGraphManager(
        MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* mddg) {
    GUM_CONSTRUCTOR(MultiDimFunctionGraphManager);
    functionGraph__ = mddg;
  }

  // Destructor
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::
     ~MultiDimFunctionGraphManager() {
    GUM_DESTRUCTOR(MultiDimFunctionGraphManager);
  }

  /// Sets root node of decision diagram
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE void
     MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::setRootNode(
        const NodeId& root) {
    functionGraph__->root__ = root;
  }

  // Inserts a new non terminal node in graph.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::
     addInternalNode(const DiscreteVariable* var, NodeId nid) {
    InternalNode* newNodeStruct = new InternalNode(var);

    functionGraph__->internalNodeMap__.insert(nid, newNodeStruct);

    functionGraph__->var2NodeIdMap__[var]->addLink(nid);

    return nid;
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::
     addInternalNode(const DiscreteVariable* var) {
    InternalNode* newNodeStruct = new InternalNode(var);
    NodeId        nid = functionGraph__->model__.addNode();
    functionGraph__->internalNodeMap__.insert(nid, newNodeStruct);
    functionGraph__->var2NodeIdMap__[var]->addLink(nid);

    return nid;
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::
     addInternalNode_(const DiscreteVariable* var, NodeId* sons) {
    InternalNode* newNodeStruct = new InternalNode(var, sons);
    NodeId        nid = functionGraph__->model__.addNode();
    functionGraph__->internalNodeMap__.insert(nid, newNodeStruct);
    functionGraph__->var2NodeIdMap__[var]->addLink(nid);
    for (Idx i = 0; i < newNodeStruct->nbSons(); i++)
      if (!functionGraph__->isTerminalNode(sons[i]))
        functionGraph__->internalNodeMap__[sons[i]]->addParent(nid, i);

    return nid;
  }

  // Adds a value to the MultiDimFunctionGraph.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::
     addTerminalNode(const GUM_SCALAR& value) {
    if (functionGraph__->existsTerminalNodeWithValue(value))
      return functionGraph__->terminalNodeId(value);

    NodeId node = functionGraph__->model__.addNode();
    functionGraph__->addTerminalNode(node, value);
    return node;
  }

  // Erases a node from the diagram.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::eraseNode(
     NodeId eraseId, NodeId replacingId, bool updateParents) {
    if (!functionGraph__->model__.exists(eraseId))
      GUM_ERROR(NotFound, "Node : " << eraseId << " doesn't exists in the graph")

    if (functionGraph__->isTerminalNode(eraseId)) {
      for (auto iterVar = functionGraph__->variablesSequence().begin();
           iterVar != functionGraph__->variablesSequence().end();
           ++iterVar) {
        Link< NodeId >* nodeIter =
           functionGraph__->var2NodeIdMap__[*iterVar]->list();
        while (nodeIter != nullptr) {
          for (Idx modality = 0; modality < (*iterVar)->domainSize(); ++modality)
            if (functionGraph__->node(nodeIter->element())->son(modality)
                == eraseId)
              setSon(nodeIter->element(), modality, replacingId);

          nodeIter = nodeIter->nextLink();
        }
      }
      functionGraph__->eraseTerminalNode(eraseId);

    } else {
      InternalNode* eraseNode = functionGraph__->internalNodeMap__[eraseId];

      if (updateParents) {
        Link< Parent >* picle = eraseNode->parents();
        while (picle != nullptr) {
          setSon(
             picle->element().parentId, picle->element().modality, replacingId);
          picle = picle->nextLink();
        }
      }

      functionGraph__
         ->var2NodeIdMap__[functionGraph__->internalNodeMap__[eraseId]->nodeVar()]
         ->searchAndRemoveLink(eraseId);

      delete functionGraph__->internalNodeMap__[eraseId];
      functionGraph__->internalNodeMap__.erase(eraseId);
    }

    functionGraph__->model__.eraseNode(eraseId);

    if (functionGraph__->root__ == eraseId) functionGraph__->root__ = replacingId;
  }

  /// Sets nodes son for given modality to designated son node
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE void
     MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::setSon(
        const NodeId& node, const Idx& modality, const NodeId& sonNode) {
    // Ensuring that both nodes exists in the graph
    if (!functionGraph__->model__.exists(node))
      GUM_ERROR(NotFound, "Node : " << node << " doesn't exists in the graph")
    if (!functionGraph__->model__.exists(sonNode))
      GUM_ERROR(NotFound, "Node : " << sonNode << " doesn't exists in the graph")

    // Check if starting node is not terminal
    if (functionGraph__->isTerminalNode(node))
      GUM_ERROR(InvalidNode,
                "You cannot insert an arc from terminal node : " << node)

    // Check if associated modality is lower than node bound variable domain
    // size
    if (functionGraph__->isInternalNode(node)
        && modality
              > functionGraph__->internalNodeMap__[node]->nodeVar()->domainSize()
                   - 1)
      GUM_ERROR(
         InvalidArgument,
         "Modality "
            << modality << "is higher than domain size "
            << functionGraph__->internalNodeMap__[node]->nodeVar()->domainSize()
            << "minus 1 of variable "
            << functionGraph__->internalNodeMap__[node]->nodeVar()->name())

    // Check if variable order is respected
    if (functionGraph__->isInternalNode(sonNode)
        && functionGraph__->variablesSequence().pos(
              functionGraph__->internalNodeMap__[node]->nodeVar())
              >= functionGraph__->variablesSequence().pos(
                 functionGraph__->internalNodeMap__[sonNode]->nodeVar()))
      GUM_ERROR(
         OperationNotAllowed,
         "Variable " << functionGraph__->internalNodeMap__[node]->nodeVar()
                     << " is after variable "
                     << functionGraph__->internalNodeMap__[sonNode]->nodeVar()
                     << "in Function Graph order.")

    functionGraph__->internalNodeMap__[node]->setSon(modality, sonNode);
    if (sonNode && !functionGraph__->isTerminalNode(sonNode))
      functionGraph__->internalNodeMap__[sonNode]->addParent(node, modality);
  }

  // Changes var position in variable sequence
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void MultiDimFunctionGraphManager< GUM_SCALAR,
                                     TerminalNodePolicy >::minimizeSize() {
    // Ordering variables by number of nodes asssociated to them
    Sequence< const DiscreteVariable* >       siftingSeq;
    HashTable< const DiscreteVariable*, Idx > varLvlSize;
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter =
            functionGraph__->variablesSequence().beginSafe();
         varIter != functionGraph__->variablesSequence().endSafe();
         ++varIter) {
      const Link< NodeId >* curElem =
         functionGraph__->var2NodeIdMap__[*varIter]->list();
      Idx nbElem = 0;
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
    for (SequenceIteratorSafe< const DiscreteVariable* > sifIter =
            siftingSeq.beginSafe();
         sifIter != siftingSeq.endSafe();
         ++sifIter) {
      // Initialization
      Idx currentPos = functionGraph__->variablesSequence().pos(*sifIter);
      Idx bestSize = functionGraph__->realSize();
      Idx bestPos = currentPos;


      // Sifting towards upper places
      while (currentPos > 0) {
        moveTo(*sifIter, currentPos - 1);
        currentPos = functionGraph__->variablesSequence().pos(*sifIter);
        if (functionGraph__->realSize() < bestSize) {
          bestPos = currentPos;
          bestSize = functionGraph__->realSize();
        }
      }

      // Sifting towards lower places
      while (currentPos < functionGraph__->variablesSequence().size() - 1) {
        moveTo(*sifIter, currentPos + 1);
        currentPos = functionGraph__->variablesSequence().pos(*sifIter);
        if (functionGraph__->realSize() < bestSize) {
          bestPos = currentPos;
          bestSize = functionGraph__->realSize();
        }
      }

      moveTo(*sifIter, bestPos);
    }
  }

  // Changes var position in variable sequence
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::moveTo(
     const DiscreteVariable* movedVar, Idx desiredPos) {
    // First we determine the position of both variable
    // We also determine which one precede the other
    if (functionGraph__->variablesSequence().pos(movedVar) > desiredPos)
      for (Idx currentPos = functionGraph__->variablesSequence().pos(movedVar);
           currentPos != desiredPos;
           currentPos--) {
        const DiscreteVariable* preVar =
           functionGraph__->variablesSequence().atPos(currentPos - 1);
        if (functionGraph__->var2NodeIdMap__[preVar]->list()
            && functionGraph__->var2NodeIdMap__[movedVar]->list())
          adjacentSwap__(preVar, movedVar);
        functionGraph__->invert_(currentPos - 1, currentPos);
      }
    else
      for (Idx currentPos = functionGraph__->variablesSequence().pos(movedVar);
           currentPos != desiredPos;
           currentPos++) {
        const DiscreteVariable* suiVar =
           functionGraph__->variablesSequence().atPos(currentPos + 1);
        if (functionGraph__->var2NodeIdMap__[suiVar]->list()
            && functionGraph__->var2NodeIdMap__[movedVar]->list())
          adjacentSwap__(movedVar, suiVar);
        functionGraph__->invert_(currentPos, currentPos + 1);
      }
  }

  // Swap two adjacent variable.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::
     adjacentSwap__(const DiscreteVariable* x, const DiscreteVariable* y) {
    LinkedList< NodeId >* oldxNodes = functionGraph__->var2NodeIdMap__[x];
    functionGraph__->var2NodeIdMap__[x] = new LinkedList< NodeId >();
    LinkedList< NodeId >* oldyNodes = functionGraph__->var2NodeIdMap__[y];
    functionGraph__->var2NodeIdMap__[y] = new LinkedList< NodeId >();


    InternalNode* currentOldXNode = nullptr;
    NodeId*       currentNewXNodeSons = nullptr;
    Idx           indx = 0;

    NodeId* currentNewYNodeSons = nullptr;
    NodeId  currentNewYNodeId = 0;
    Idx     indy = 0;

    while (oldxNodes->list()) {
      // Recuperating a node associated to variables x
      currentOldXNode =
         functionGraph__->internalNodeMap__[oldxNodes->list()->element()];

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
          if (!functionGraph__->isTerminalNode(currentOldXNode->son(indx))
              && functionGraph__->node(currentOldXNode->son(indx))->nodeVar() == y)
            currentNewXNodeSons[indx] =
               functionGraph__->node(currentOldXNode->son(indx))->son(indy);
        }

        // Inserting the new node bound to x
        currentNewYNodeSons[indy] = nodeRedundancyCheck_(x, currentNewXNodeSons);
      }

      // Replacing old node x by new node y
      currentNewYNodeId = currentNewYNodeSons[0];
      if (isRedundant__(y, currentNewYNodeSons)) {
        migrateNode_(oldxNodes->list()->element(), currentNewYNodeId);
        SOA_DEALLOCATE(currentNewYNodeSons, y->domainSize() * sizeof(NodeId));
      } else {
        currentNewYNodeId = checkIsomorphism__(y, currentNewYNodeSons);
        if (currentNewYNodeId != 0) {
          migrateNode_(oldxNodes->list()->element(), currentNewYNodeId);
          SOA_DEALLOCATE(currentNewYNodeSons, y->domainSize() * sizeof(NodeId));
        } else {
          // Updating the sons (they must not consider old x as their parent)
          for (Idx i = 0; i < currentOldXNode->nodeVar()->domainSize(); ++i) {
            if (functionGraph__->internalNodeMap__.exists(
                   currentOldXNode->son(i))) {
              functionGraph__->internalNodeMap__[currentOldXNode->son(i)]
                 ->removeParent(oldxNodes->list()->element(), i);
            }
          }
          // Reaffecting old node x internal attributes to correct new one
          currentOldXNode->setNode(y, currentNewYNodeSons);
          // Updating new sons (they must consider the node as a parent)
          for (Idx i = 0; i < currentOldXNode->nodeVar()->domainSize(); ++i) {
            if (functionGraph__->internalNodeMap__.exists(
                   currentNewYNodeSons[i])) {
              functionGraph__->internalNodeMap__[currentNewYNodeSons[i]]
                 ->addParent(oldxNodes->list()->element(), i);
            }
          }

          functionGraph__->var2NodeIdMap__[y]->addLink(
             oldxNodes->list()->element());
        }
      }

      oldxNodes->searchAndRemoveLink(oldxNodes->list()->element());
    }
    delete oldxNodes;

    while (oldyNodes->list()) {
      NodeId curId = oldyNodes->list()->element();
      if (functionGraph__->internalNodeMap__[curId]->parents() == nullptr) {
        for (Idx i = 0;
             i
             < functionGraph__->internalNodeMap__[curId]->nodeVar()->domainSize();
             ++i)
          if (functionGraph__->internalNodeMap__.exists(
                 functionGraph__->internalNodeMap__[curId]->son(i)))
            functionGraph__
               ->internalNodeMap__[functionGraph__->internalNodeMap__[curId]->son(
                  i)]
               ->removeParent(curId, i);
        delete functionGraph__->internalNodeMap__[curId];
        functionGraph__->internalNodeMap__.erase(curId);
        functionGraph__->model__.eraseNode(curId);
      } else {
        functionGraph__->var2NodeIdMap__[y]->addLink(curId);
      }
      oldyNodes->searchAndRemoveLink(curId);
    }
    delete oldyNodes;
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE void
     MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::migrateNode_(
        const NodeId& origin, const NodeId& destination) {
    InternalNode* org = functionGraph__->internalNodeMap__[origin];
    // Upating parents after the change
    Link< Parent >* picle = org->parents();
    while (picle != nullptr) {
      setSon(picle->element().parentId, picle->element().modality, destination);
      picle = picle->nextLink();
    }

    // Updating sons after the change
    for (Idx i = 0; i < org->nbSons(); ++i)
      if (functionGraph__->internalNodeMap__.exists(org->son(i)))
        functionGraph__->internalNodeMap__[org->son(i)]->removeParent(origin, i);

    delete org;
    functionGraph__->internalNodeMap__.erase(origin);
    functionGraph__->model__.eraseNode(origin);

    if (functionGraph__->root() == origin) this->setRootNode(destination);
  }

  // Checks if a similar node does not already exists in the graph or
  // if it has the same child for every variable value.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::
     nodeRedundancyCheck_(const DiscreteVariable* var, NodeId* sonsIds) {
    NodeId newNode = sonsIds[0];

    if (isRedundant__(var, sonsIds)) {
      SOA_DEALLOCATE(sonsIds, sizeof(NodeId) * var->domainSize());
    } else {
      newNode = checkIsomorphism__(var, sonsIds);
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
  INLINE NodeId MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::
     checkIsomorphism__(const DiscreteVariable* var, NodeId* sons) {
    const InternalNode* nody = nullptr;
    Idx                 i = 0;

    // Check abscence of identical node
    Link< NodeId >* currentElem = functionGraph__->var2NodeIdMap__[var]->list();
    while (currentElem != nullptr) {
      nody = functionGraph__->internalNodeMap__[currentElem->element()];

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
  INLINE bool
     MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::isRedundant__(
        const DiscreteVariable* var, NodeId* sons) {
    for (Idx m = 1; m < var->domainSize(); m++)
      if (sons[m] != sons[0]) return false;
    return true;
  }

  // Ensures that every isomorphic subgraphs are merged together.
  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  INLINE void
     MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::reduce_() {
    Link< NodeId >* currentNodeId = nullptr;
    Link< NodeId >* nextNodeId = nullptr;
    InternalNode*   currentNode = nullptr;
    bool            theSame = true;
    Idx             currentInd;

    for (SequenceIterator< const DiscreteVariable* > varIter =
            functionGraph__->variablesSequence().rbegin();
         varIter != functionGraph__->variablesSequence().rend();
         --varIter) {
      currentNodeId = functionGraph__->var2NodeIdMap__[*varIter]->list();

      while (currentNodeId != nullptr) {
        nextNodeId = currentNodeId->nextLink();
        currentNode = functionGraph__->internalNodeMap__[currentNodeId->element()];

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
          functionGraph__->var2NodeIdMap__[*varIter]->searchAndRemoveLink(
             currentNodeId->element());
          currentNodeId = nextNodeId;
          continue;
        }

        // Second isomorphism to handle is the one where two nodes have same
        // variable and same children
        if (nextNodeId) {
          Link< NodeId >* anotherNodeId = currentNodeId->nextLink();
          InternalNode*   anotherNode = nullptr;
          Idx             modality = 0;
          while (anotherNodeId->nextLink() != nullptr) {
            nextNodeId = anotherNodeId->nextLink();
            anotherNode =
               functionGraph__->internalNodeMap__[anotherNodeId->element()];

            // Check on the other sons
            for (modality = 0; modality < (*varIter)->domainSize(); ++modality) {
              if (anotherNode->son(modality) != currentNode->son(modality)) break;
              if (modality == (*varIter)->domainSize() - 1) {
                migrateNode_(anotherNodeId->element(), currentNodeId->element());
                functionGraph__->var2NodeIdMap__[*varIter]->searchAndRemoveLink(
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
  INLINE void
     MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >::clean() {
    Sequence< const DiscreteVariable* > oldSequence(
       functionGraph__->variablesSequence());
    for (SequenceIterator< const DiscreteVariable* > varIter = oldSequence.begin();
         varIter != oldSequence.end();
         ++varIter)
      if (!functionGraph__->varNodeListe(*varIter)->list())
        functionGraph__->erase(**varIter);
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
  MultiDimFunctionGraphTreeManager< GUM_SCALAR, TerminalNodePolicy >::
     ~MultiDimFunctionGraphTreeManager() {
    GUM_DESTRUCTOR(MultiDimFunctionGraphTreeManager);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  NodeId MultiDimFunctionGraphTreeManager< GUM_SCALAR, TerminalNodePolicy >::
     addInternalNode(const DiscreteVariable* var, NodeId* sons) {
    return this->addInternalNode_(var, sons);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void
     MultiDimFunctionGraphTreeManager< GUM_SCALAR, TerminalNodePolicy >::reduce() {
  }

  // ===========================================================================
  // MultiDimFunctionGraphROManager
  // ===========================================================================

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  MultiDimFunctionGraphROManager< GUM_SCALAR, TerminalNodePolicy >::
     MultiDimFunctionGraphROManager(
        MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* master) :
      MultiDimFunctionGraphManager< GUM_SCALAR, TerminalNodePolicy >(master) {
    GUM_CONSTRUCTOR(MultiDimFunctionGraphROManager);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  MultiDimFunctionGraphROManager< GUM_SCALAR, TerminalNodePolicy >::
     ~MultiDimFunctionGraphROManager() {
    GUM_DESTRUCTOR(MultiDimFunctionGraphROManager);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  NodeId MultiDimFunctionGraphROManager< GUM_SCALAR, TerminalNodePolicy >::
     addInternalNode(const DiscreteVariable* var, NodeId* sons) {
    return this->nodeRedundancyCheck_(var, sons);
  }

  template < typename GUM_SCALAR, template < class > class TerminalNodePolicy >
  void MultiDimFunctionGraphROManager< GUM_SCALAR, TerminalNodePolicy >::reduce() {
    this->reduce_();
  }

}   // namespace gum
