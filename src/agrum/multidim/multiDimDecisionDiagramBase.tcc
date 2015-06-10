
/********************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES*
*   {prenom.nom}_at_lip6.fr *
**
*   This program is free software; you can redistribute it and/or modify *
*   it under the terms of the GNU General Public License as published by*
*   the Free Software Foundation; either version 2 of the License, or *
*   (at your option) any later version.*
* *
*   This program is distributed in the hope that it will be useful,*
*   but WITHOUT ANY WARRANTY; without even the implied warranty of *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the *
*   GNU General Public License for more details.*
* *
*   You should have received a copy of the GNU General Public License*
*   along with this program; if not, write to the*
*   Free Software Foundation, Inc.,*
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*********************************************************************************/
/**
* @file
* @brief Template implementation of MultiDimDecisionDiagramBase.h classe.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

#include <sstream>
#include <math.h>

#include <agrum/core/priorityQueue.h>

#include <agrum/multidim/multiDimDecisionDiagramBase.h>

#include <agrum/multidim/patterns/DDUtility/DDModeMacro.h>

namespace gum {

  /* *****************************************************************************************************
   */
  /*                                                                                                                                                         */
  /*                                  CONSTRUCTOR, DESTRUCTOR & CO */
  /*                                                                                                                                                        */
  /* ****************************************************************************************************
   */

  // Default constructor

  template <typename GUM_SCALAR>
  INLINE MultiDimDecisionDiagramBase<GUM_SCALAR>::MultiDimDecisionDiagramBase()
      : MultiDimReadOnly<GUM_SCALAR>(), __name("MultiDimDecisionDiagram"),
        __isInstanciated(false), __instanciationModeOn(false) {
    GUM_CONSTRUCTOR(MultiDimDecisionDiagramBase);
  }

  // Copy constructor

  template <typename GUM_SCALAR>
  INLINE MultiDimDecisionDiagramBase<GUM_SCALAR>::MultiDimDecisionDiagramBase(
      const MultiDimDecisionDiagramBase<GUM_SCALAR> &source)
      : MultiDimReadOnly<GUM_SCALAR>(source), __name("MultiDimDecisionDiagram"),
        __isInstanciated(false), __instanciationModeOn(false) {
    GUM_CONSTRUCTOR(MultiDimDecisionDiagramBase);
    this->copy(source);
  }

  // destructor

  template <typename GUM_SCALAR>
  INLINE MultiDimDecisionDiagramBase<GUM_SCALAR>::~MultiDimDecisionDiagramBase() {

    GUM_DESTRUCTOR(MultiDimDecisionDiagramBase);

    for (auto iter_node = __graph.nodes().beginSafe();
         iter_node != __graph.nodes().endSafe(); ++iter_node) {
      if (*iter_node != 0)
        if (!__valueMap.existsFirst(*iter_node) && __arcMap[*iter_node] != nullptr)
          delete __arcMap[*iter_node];
    }

    for (HashTableIteratorSafe<const DiscreteVariable *, List<NodeId> *> iter =
             __var2NodeIdMap.beginSafe();
         iter != __var2NodeIdMap.endSafe(); ++iter)
      delete iter.val();

    for (HashTableIteratorSafe<const DiscreteVariable *, std::vector<Idx> *> iter =
             __varUsedModalitiesMap.beginSafe();
         iter != __varUsedModalitiesMap.endSafe(); ++iter)
      delete iter.val();
  }

  /* *****************************************************************************************************
   */
  /*                                                                                                                                                         */
  /*                                  MULTIDIM METHODS */
  /*                                                                                                                                                        */
  /* ****************************************************************************************************
   */

  /**
   * ***********************************************************************************
   * **/
  /**        Accessors/Modifiers **/
  /**
   * ***********************************************************************************
   * **/

  // Returns the real name of this multidim implementation

  template <typename GUM_SCALAR>
  INLINE const std::string &MultiDimDecisionDiagramBase<GUM_SCALAR>::name() const {

    return __name;
  }

  // Returns valued pointed by inst

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR
      MultiDimDecisionDiagramBase<GUM_SCALAR>::get(const Instantiation &inst) const {

    NodeId i = __root;

    while (!isTerminalNode(i)) {
      if ((*__arcMap[i])[inst.val(*__variableMap[i])] != 0)
        i = (*__arcMap[i])[inst.val(*__variableMap[i])];
      else
        i = __defaultArcMap[i];
    }

    return this->__valueMap.second(i);
  }

  // Returns the terminal node associated to value pointed by inst

  template <typename GUM_SCALAR>
  INLINE NodeId MultiDimDecisionDiagramBase<GUM_SCALAR>::getNode(
      const Instantiation &inst) const {

    NodeId i = __root;

    while (!isTerminalNode(i)) {
      if ((*__arcMap[i])[inst.val(*__variableMap[i])] != 0)
        i = (*__arcMap[i])[inst.val(*__variableMap[i])];
      else
        i = __defaultArcMap[i];
    }

    return i;
  }

  /**
   * ***********************************************************************************
   * **/
  /**      Implementation of MultiDimInterface **/
  /**
   * ***********************************************************************************
   * **/

  // Adds a new var to the variables of the multidimensional matrix.
  // throw OperationNotAllowed cause it's not authorize on read only multidim

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramBase<GUM_SCALAR>::add(const DiscreteVariable &v) {
    GUM_ERROR(OperationNotAllowed, " Can't add a variable to a read only multidim ");
  }

  // Removes a var from the variables of the multidimensional matrix.
  // @throw OperationNotAllowed cause it's not authorize on read only multidim

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramBase<GUM_SCALAR>::erase(const DiscreteVariable &v) {
    GUM_ERROR(OperationNotAllowed,
              " Can't remove a variable from a read only multidim ");
  }

  // Returns the real number of parameter used in this table.

  template <typename GUM_SCALAR>
  INLINE Size MultiDimDecisionDiagramBase<GUM_SCALAR>::realSize() const {
    return __graph.size() - 1;
  }

  // Indicates if diagram has no value inside at all

  template <typename GUM_SCALAR>
  INLINE bool MultiDimDecisionDiagramBase<GUM_SCALAR>::empty() const {
    return __valueMap.empty();
  }

  /**
   * ***********************************************************************************
   * **/
  /**    Slave management and extension due to slave management **/
  /**
   * ***********************************************************************************
   * **/

  // Listen to change in a given Instantiation

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::changeNotification(
      Instantiation &i, const DiscreteVariable *var, const Idx &oldval,
      const Idx &newval) {}

  // Listen to setFirst in a given Instantiation

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramBase<GUM_SCALAR>::setFirstNotification(Instantiation &i) {}

  // Listen to setLast in a given Instantiation

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramBase<GUM_SCALAR>::setLastNotification(Instantiation &i) {}

  // Listen to increment in a given Instantiation

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramBase<GUM_SCALAR>::setIncNotification(Instantiation &i) {}

  // Listen to decrement in a given Instantiation

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramBase<GUM_SCALAR>::setDecNotification(Instantiation &i) {}

  // Listen to an assignement of value in a given Instantiation

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramBase<GUM_SCALAR>::setChangeNotification(Instantiation &i) {}

  /**
   * ***********************************************************************************
   * **/
  /**        Copy Methods **/
  /**
   * ***********************************************************************************
   * **/

  // Makes a copy of given decision diagram
  // @throw OperationNotAllowed cause this is a read only

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::copy(
      const MultiDimDecisionDiagramBase<GUM_SCALAR> &source,
      bool fullInstanciation) {

    if (__isInstanciated)
      GUM_ERROR(OperationNotAllowed, "This is a read only");

    this->beginInstantiation();
    this->setVariableSequence(source.variablesSequence());
    this->setDiagramNodes(source.nodesMap());
    __valueMap = source.valuesMap();
    __variableMap.resize(source.nodesMap().size());
    __var2NodeIdMap.resize(source.variablesSequence().size());
    __varUsedModalitiesMap.resize(source.variablesSequence().size());
    __arcMap.resize(source.nodesMap().size());
    __defaultArcMap.resize(source.nodesMap().size());

    for (auto iter_node = __graph.nodes().beginSafe();
         iter_node != __graph.nodes().endSafe(); ++iter_node) {
      if (*iter_node != 0 && !source.isTerminalNode(*iter_node)) {

        __variableMap.insert(*iter_node, source.nodeVariable(*iter_node));

        if (!__var2NodeIdMap.exists(source.nodeVariable(*iter_node)))
          __var2NodeIdMap.insert(source.nodeVariable(*iter_node),
                                 new List<NodeId>(*(source.variableNodes(
                                     source.nodeVariable(*iter_node)))));

        if (!__varUsedModalitiesMap.exists(source.nodeVariable(*iter_node)))
          __varUsedModalitiesMap.insert(
              source.nodeVariable(*iter_node),
              new std::vector<Idx>(*(source.variableUsedModalities(
                  source.nodeVariable(*iter_node)))));

        __arcMap.insert(*iter_node,
                        new std::vector<NodeId>(*(source.nodeSons(*iter_node))));

        if (source.hasNodeDefaultSon(*iter_node))
          __defaultArcMap.insert(*iter_node, source.nodeDefaultSon(*iter_node));
      }
    }

    this->setRoot(source.root());

    if (fullInstanciation)
      this->endInstantiation();
  }

  // Removes all variables in this Container and copy content from src, variable
  // included
  // @throw OperationNotAllowed cause this is a read only and a decision diagram (
  // which
  // makes quiet complicate any copy operation as a matter of fact )

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::copy(
      const MultiDimContainer<GUM_SCALAR> &src) {

    GUM_ERROR(OperationNotAllowed, "This is a read only with a special structure");
  }

  // ===========================================================================
  // Performs a copy of given in parameter table plus a change of variable based upon
  // bijection given in parameter.
  // ===========================================================================
  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::copyAndReassign(
      const MultiDimDecisionDiagramBase<GUM_SCALAR> *source,
      const Bijection<const DiscreteVariable *, const DiscreteVariable *> &old2new) {

    this->beginInstantiation();

#ifndef O4DDWITHORDER
    Sequence<const DiscreteVariable *> primeVarSeq;

    for (SequenceIteratorSafe<const DiscreteVariable *> seqIter =
             source->variablesSequence().beginSafe();
         seqIter != source->variablesSequence().endSafe(); ++seqIter)
      primeVarSeq.insert(old2new.second(*seqIter));

    this->setVariableSequence(primeVarSeq);
#else
    this->setVariableSequence(source->variablesSequence());
#endif

    this->setDiagramNodes(source->nodesMap());

    __valueMap = source->valuesMap();

    __variableMap.resize(source->nodesMap().size());

    __var2NodeIdMap.resize(source->variablesSequence().size());

    __varUsedModalitiesMap.resize(source->variablesSequence().size());

    __arcMap.resize(source->nodesMap().size());

    __defaultArcMap.resize(source->nodesMap().size());

    for (auto nodeIter = source->nodesMap().beginSafe();
         nodeIter != source->nodesMap().endSafe(); ++nodeIter) {

      if (*nodeIter != 0 && !source->isTerminalNode(*nodeIter)) {

        __variableMap.insert(*nodeIter,
                             old2new.second(source->unsafeNodeVariable(*nodeIter)));

        if (!__var2NodeIdMap.exists(
                old2new.second(source->unsafeNodeVariable(*nodeIter))))
          __var2NodeIdMap.insert(
              old2new.second(source->unsafeNodeVariable(*nodeIter)),
              new List<NodeId>(
                  *(source->variableNodes(source->unsafeNodeVariable(*nodeIter)))));

        if (!__varUsedModalitiesMap.exists(
                old2new.second(source->unsafeNodeVariable(*nodeIter))))
          __varUsedModalitiesMap.insert(
              old2new.second(source->unsafeNodeVariable(*nodeIter)),
              new std::vector<Idx>(*(source->variableUsedModalities(
                  source->unsafeNodeVariable(*nodeIter)))));

        __arcMap.insert(*nodeIter, new std::vector<NodeId>(
                                       *(source->unsafeNodeSons(*nodeIter))));

        if (source->unsafeHasNodeDefaultSon(*nodeIter))
          __defaultArcMap.insert(*nodeIter, source->unsafeNodeDefaultSon(*nodeIter));
      }
    }

    this->setRoot(source->root());

    this->endInstantiation();
  }

  /**
   * ***********************************************************************************
   * **/
  /**        Various Methods **/
  /**
   * ***********************************************************************************
   * **/

  // Displays the internal representation of i.

  template <typename GUM_SCALAR>
  INLINE const std::string
  MultiDimDecisionDiagramBase<GUM_SCALAR>::toString() const {

    return MultiDimReadOnly<GUM_SCALAR>::toString();
  }

  // Displays the internal representation of i.

  template <typename GUM_SCALAR>
  INLINE const std::string
  MultiDimDecisionDiagramBase<GUM_SCALAR>::toString(const Instantiation *i) const {

    std::stringstream sBuff;
    sBuff << (*i) << " = " << get(*i);
    return sBuff.str();
  }

  // Displays the DecisionDiagramBase in the dot format

  template <typename GUM_SCALAR>
  std::string
  MultiDimDecisionDiagramBase<GUM_SCALAR>::toDot(const std::string graphName) const {

    std::stringstream output;
    std::stringstream terminalStream;
    std::stringstream nonTerminalStream;
    std::stringstream arcstream;
    std::stringstream defaultarcstream;
    output << std::endl << "digraph \"" << graphName << "\" {" << std::endl;

    terminalStream << "node [shape = box];" << std::endl;
    nonTerminalStream << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for (auto iter_node = __graph.nodes().beginSafe();
         iter_node != __graph.nodes().endSafe(); ++iter_node) {
      if (*iter_node != 0) {
        if (isTerminalNode(*iter_node)) {
          terminalStream << tab << *iter_node << ";" << tab << *iter_node
                         << " [label=\"" << *iter_node << "-"
                         << std::setprecision(15)
                         << this->__valueMap.second(*iter_node) << "\"]"
                         << ";" << std::endl;
        } else {
          nonTerminalStream << tab << *iter_node << ";" << tab << *iter_node
                            << " [label=\"" << *iter_node << "-"
                            << __variableMap[*iter_node]->name() << "\"]"
                            << ";" << std::endl;

          if (__arcMap[*iter_node] != nullptr) {
            for (std::vector<NodeId>::iterator sonIter =
                     __arcMap[*iter_node]->begin();
                 sonIter != __arcMap[*iter_node]->end(); ++sonIter)
              if (*sonIter != 0)
                arcstream << tab << *iter_node << " -> " << *sonIter << " [label=\""
                          << __variableMap[*iter_node]->label(std::distance(
                                 __arcMap[*iter_node]->begin(), sonIter))
                          << "\",color=\"#0000ff\"]"
                          << ";" << std::endl;
          }

          if (__defaultArcMap.exists(*iter_node))
            defaultarcstream << tab << *iter_node << " -> "
                             << __defaultArcMap[*iter_node] << " [color=\"#ff0000\"]"
                             << ";" << std::endl;
        }
      }
    }

    output << terminalStream.str() << std::endl << nonTerminalStream.str()
           << std::endl << arcstream.str() << std::endl << defaultarcstream.str()
           << "}" << std::endl;

    // std::stringstream usedModalities;
    // usedModalities << " Used Modalities Tables : " << std::endl;
    // for( HashTableConstIteratorSafe< const DiscreteVariable*, std::vector<bool>* >
    // varIter = __varUsedModalitiesMap.beginSafe(); varIter !=
    // __varUsedModalitiesMap.endSafe(); ++varIter ){
    // usedModalities << varIter.key()->name();
    // for( std::vector<bool>::iterator modalityIter = (*varIter)->begin();
    // modalityIter != (*varIter)->end(); ++modalityIter )
    //       usedModalities << " Modality : " << std::distance( (*varIter)->begin(),
    //       modalityIter ) << " - used : " << *modalityIter << "|";
    // usedModalities << std::endl;
    // }
    // output << usedModalities.str();

    return output.str();
  }

  // Returns the number of variable trully present in the diagram

  template <typename GUM_SCALAR>
  Size MultiDimDecisionDiagramBase<GUM_SCALAR>::diagramVarSize() const {

    Sequence<const DiscreteVariable *> varTopo = this->variablesSequence();

    for (SequenceIteratorSafe<const DiscreteVariable *> ite1 = varTopo.beginSafe();
         ite1 != varTopo.endSafe();) {
      bool isin = false;
      HashTableConstIteratorSafe<NodeId, const DiscreteVariable *> ite2 =
          __variableMap.beginSafe();

      while (ite2 != __variableMap.endSafe()) {
        if (**ite1 == *(ite2.val())) {
          isin = true;
          break;
        }

        ++ite2;
      }

      if (isin)
        ++ite1;
      else
        varTopo.erase(*ite1);
    }

    return varTopo.size();
  }

  /**
   * ***********************************************************************************
   * **/
  /**        Diagram Handlers **/
  /**
   * ***********************************************************************************
   * **/

  // Returns associated variable of given node
  // @throw InvalidNode if Node is terminal

  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable *
  MultiDimDecisionDiagramBase<GUM_SCALAR>::nodeVariable(NodeId n) const {

    if (isTerminalNode(n)) {
      GUM_ERROR(InvalidNode, " Node " << n << " is a terminal node. ");
    }

    return unsafeNodeVariable(n);
  }

  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable *
  MultiDimDecisionDiagramBase<GUM_SCALAR>::unsafeNodeVariable(NodeId n) const {

    return __variableMap[n];
  }

  // Returns associated nodes of the variable pointed by the given node

  template <typename GUM_SCALAR>
  INLINE const List<NodeId> *MultiDimDecisionDiagramBase<GUM_SCALAR>::variableNodes(
      const DiscreteVariable *v) const {

    if (!__var2NodeIdMap.exists(v))
      return nullptr;

    return __var2NodeIdMap[v];
  }

  // Returns a boolean vector that indicates for all modalities that can take given
  // in parameter variable if an arc is associated to it in diagram

  template <typename GUM_SCALAR>
  INLINE const std::vector<Idx> *
  MultiDimDecisionDiagramBase<GUM_SCALAR>::variableUsedModalities(
      const DiscreteVariable *v) const {

    if (!__varUsedModalitiesMap.exists(v))
      return nullptr;

    return __varUsedModalitiesMap[v];
  }

  // Returns value associated to given node
  // @throw InvalidNode if node isn't terminal

  template <typename GUM_SCALAR>
  INLINE const GUM_SCALAR &
  MultiDimDecisionDiagramBase<GUM_SCALAR>::nodeValue(NodeId n) const {

    if (!isTerminalNode(n)) {
      GUM_ERROR(InvalidNode, " Node " << n << " is a non terminal node. ");
    }

    return unsafeNodeValue(n);
  }

  template <typename GUM_SCALAR>
  INLINE const GUM_SCALAR &
  MultiDimDecisionDiagramBase<GUM_SCALAR>::unsafeNodeValue(NodeId n) const {

    return this->__valueMap.second(n);
  }

  // Returns node's sons map
  // @throw InvalidNode if node is terminal

  template <typename GUM_SCALAR>
  INLINE const std::vector<NodeId> *
  MultiDimDecisionDiagramBase<GUM_SCALAR>::nodeSons(NodeId n) const {

    if (isTerminalNode(n)) {
      GUM_ERROR(InvalidNode, " Node " << n << " is a terminal node. ");
    }

    return unsafeNodeSons(n);
  }

  template <typename GUM_SCALAR>
  INLINE const std::vector<NodeId> *
  MultiDimDecisionDiagramBase<GUM_SCALAR>::unsafeNodeSons(NodeId n) const {

    if (!__arcMap.exists(n))
      return nullptr;

    return __arcMap[n];
  }

  // Returns true if node has a default son

  template <typename GUM_SCALAR>
  INLINE bool
  MultiDimDecisionDiagramBase<GUM_SCALAR>::hasNodeDefaultSon(NodeId n) const {

    if (isTerminalNode(n)) {
      GUM_ERROR(InvalidNode, " Node " << n << " is a terminal node. ");
    }

    return unsafeHasNodeDefaultSon(n);
  }

  template <typename GUM_SCALAR>
  INLINE bool
  MultiDimDecisionDiagramBase<GUM_SCALAR>::unsafeHasNodeDefaultSon(NodeId n) const {

    return __defaultArcMap.exists(n);
  }

  // Returns node's default son
  // @throw InvalidNode if node is terminal
  // @throw NotFound if node doesn't have a default son

  template <typename GUM_SCALAR>
  INLINE NodeId
      MultiDimDecisionDiagramBase<GUM_SCALAR>::nodeDefaultSon(NodeId n) const {

    if (isTerminalNode(n)) {
      GUM_ERROR(InvalidNode, " Node " << n << " is a terminal node. ");
    }

    if (!__defaultArcMap.exists(n)) {
      GUM_ERROR(NotFound, " Node " << n << " doesn't have a default son.");
    }

    return unsafeNodeDefaultSon(n);
  }

  template <typename GUM_SCALAR>
  INLINE NodeId
      MultiDimDecisionDiagramBase<GUM_SCALAR>::unsafeNodeDefaultSon(NodeId n) const {

    return __defaultArcMap[n];
  }

  // Returns true if node is a terminal node

  template <typename GUM_SCALAR>
  INLINE bool
  MultiDimDecisionDiagramBase<GUM_SCALAR>::isTerminalNode(NodeId id) const {

    return (__valueMap.existsFirst(id));
  }

  // Returns true if variable has at least one node associated in diagram

  template <typename GUM_SCALAR>
  INLINE bool MultiDimDecisionDiagramBase<GUM_SCALAR>::isInDiagramVariable(
      const DiscreteVariable *v) const {

    return (__var2NodeIdMap.exists(v));
  }

  // Returns a hashtable containing for each node a list of variable
  // This method looks, for each path in the diagram, if a var does not precede
  // others in
  // the given in parameter order.

  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramBase<GUM_SCALAR>::findRetrogradeVariables(
      const Sequence<const DiscreteVariable *> *varsSeq,
      HashTable<NodeId, Set<const DiscreteVariable *> *> *retrogradeVariablesTable)
      const {

    if (this->__variableMap.empty())
      return;

    HashTable<NodeId, Set<const DiscreteVariable *> *> *preceedingVariablesTable =
        new HashTable<NodeId, Set<const DiscreteVariable *> *>();
    __makePreceedingVariablesLists(preceedingVariablesTable);

    List<NodeId> *visitedNodes = new List<NodeId>();
    __findRetorgradeVariables(varsSeq, this->__root, retrogradeVariablesTable,
                              preceedingVariablesTable, visitedNodes);
    delete visitedNodes;

    for (HashTableIteratorSafe<NodeId, Set<const DiscreteVariable *> *> iterH =
             retrogradeVariablesTable->beginSafe();
         iterH != retrogradeVariablesTable->endSafe(); ++iterH) {
      Set<const DiscreteVariable *> finalSet = *(iterH.val());

      for (SetIteratorSafe<const DiscreteVariable *> iterS = finalSet.beginSafe();
           iterS != finalSet.endSafe(); ++iterS)
        if (varsSeq->pos(*iterS) >= varsSeq->pos(this->nodeVariable(iterH.key())))
          (iterH.val())->erase(*iterS);
    }

    //~ std::stringstream varString;
    //~ varString << std::endl << " Sequence variable : ";
    //~ for( SequenceIteratorSafe< const DiscreteVariable* > iter = varsSeq->begin();
    // iter != varsSeq->end(); ++iter )
    //~ varString << (*iter)->name() << " - ";
    //~ varString << std::endl;
    //~ GUM_TRACE( varString.str() );
    //~
    //~ GUM_TRACE( std::endl << this->toDot() );
    //~
    //~ std::stringstream preceedingVarLog;
    //~ preceedingVarLog << std::endl << " Preceeding variable Table : ";
    //~ for( HashTableConstIteratorSafe< NodeId, Set< const DiscreteVariable* >* >
    // iterH = preceedingVariablesTable->begin(); iterH !=
    // preceedingVariablesTable->end(); ++iterH ){
    //~ preceedingVarLog << std::endl << "Noeud : " << iterH.key() << " - Variable :
    //" << this->nodeVariable( iterH.key() )->toString() << " - Preceeding Variable :
    //";
    //~ for( SetIteratorSafe< const DiscreteVariable* > iterS =
    //(*iterH)->beginSafe(); iterS != (*iterH)->endSafe(); ++iterS )
    //~ preceedingVarLog << (*iterS)->name() << " - ";
    //~ preceedingVarLog << std::endl;
    //~ }
    //~ GUM_TRACE( preceedingVarLog.str() );
    //~
    //~ std::stringstream retorgradeVarLog;
    //~ retorgradeVarLog << std::endl << " Retrograde variable Table : ";
    //~ for( HashTableConstIteratorSafe< NodeId, Set< const DiscreteVariable* >* >
    // iterH = retrogradeVariablesTable->begin(); iterH !=
    // retrogradeVariablesTable->end(); ++iterH ){
    //~ retorgradeVarLog << std::endl << "Noeud : " << iterH.key() << " - Variable :
    //" << this->nodeVariable( iterH.key() )->toString() << " - Preceeding Variable :
    //";
    //~ for( SetIterator< const DiscreteVariable* > iterS = (*iterH)->begin(); iterS
    //!= (*iterH)->end(); ++iterS )
    //~ retorgradeVarLog << (*iterS)->name() << " - ";
    //~ retorgradeVarLog << std::endl;
    //~ }
    //~ GUM_TRACE( retorgradeVarLog.str() );

    for (HashTableIteratorSafe<NodeId, Set<const DiscreteVariable *> *> iterH =
             preceedingVariablesTable->beginSafe();
         iterH != preceedingVariablesTable->endSafe(); ++iterH)
      delete iterH.val();

    delete preceedingVariablesTable;
  }

  //  Instantiates this diagram by multiplying leaf of given diagram by factor

  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramBase<GUM_SCALAR>::multiplyByScalar(
      const MultiDimDecisionDiagramBase<GUM_SCALAR> *m, GUM_SCALAR factor) {

    if (__isInstanciated)
      GUM_ERROR(OperationNotAllowed,
                "Decision diagram has already been instanciated");

    this->copy(*m, false);

    Bijection<NodeId, GUM_SCALAR> newValueMap;

    for (BijectionIteratorSafe<NodeId, GUM_SCALAR> valueIter =
             __valueMap.beginSafe();
         valueIter != __valueMap.endSafe(); ++valueIter) {
      GUM_SCALAR tempVal = valueIter.second() * factor;

      if (newValueMap.existsSecond(tempVal)) {
        NodeId nody = newValueMap.first(tempVal);

        for (auto iter_node = __graph.nodes().beginSafe();
             iter_node != __graph.nodes().endSafe(); ++iter_node)
          if (*iter_node != 0 && !isTerminalNode(*iter_node)) {
            if (!__arcMap.exists(*iter_node))
              std::cout << "OW! OW! : " << *iter_node << std::endl;

            for (std::vector<NodeId>::iterator sonsIter =
                     __arcMap[*iter_node]->begin();
                 sonsIter != __arcMap[*iter_node]->end(); ++sonsIter)
              if (*sonsIter == valueIter.first())
                *sonsIter = nody;

            if (__defaultArcMap.exists(*iter_node) &&
                __defaultArcMap[*iter_node] == valueIter.first()) {
              __defaultArcMap.erase(*iter_node);
              __defaultArcMap.insert(*iter_node, nody);
            }
          }

        __graph.eraseNode(valueIter.first());
      } else
        newValueMap.insert(valueIter.first(), tempVal);
    }

    __valueMap = newValueMap;
  }

  /**
   * ***********************************************************************************
   * **/
  /**      Structure instantiation **/
  /**
   * ***********************************************************************************
   * **/

  // Puts the multiDim in instantiation mode
  // @throw OperationNotAllowed if diagram has already been instanciated

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::beginInstantiation() {

    if (__isInstanciated) {
      GUM_ERROR(OperationNotAllowed, "Cannot operates modification a "
                                     "multidimdecisiondiagram once it has been "
                                     "created");
    }

    __instanciationModeOn = true;
  }

  // Puts the multiDim in instantiation mode

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::endInstantiation() {

    __instanciationModeOn = false;
    __isInstanciated = true;
  }

  // Sets once and for all variable sequence.
  // @throw OperationNotAllowed if diagram not in instanciation mode

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::setVariableSequence(
      const Sequence<const DiscreteVariable *> &varList) {

    if (!__instanciationModeOn) {
      GUM_ERROR(OperationNotAllowed,
                "Must first be in instanciation mode to do such thing");
    }

    for (Sequence<const DiscreteVariable *>::iterator_safe iter =
             varList.beginSafe();
         iter != varList.endSafe(); ++iter)
      MultiDimImplementation<GUM_SCALAR>::add(**iter);
  }

  // Sets once and for all nodes of the diagram.
  // @throw OperationNotAllowed if diagram not in instanciation mode

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::setDiagramNodes(
      const NodeGraphPart &model) {

    if (!__instanciationModeOn) {
      GUM_ERROR(OperationNotAllowed,
                "Must first be in instanciation mode to do such thing");
    }

    __graph = model;
  }

  // Binds once and for all nodes to variables.
  // @throw OperationNotAllowed if diagram not in instanciation mode

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::setVariableMap(
      const NodeProperty<const DiscreteVariable *> &varMap) {

    if (!__instanciationModeOn) {
      GUM_ERROR(OperationNotAllowed,
                "Must first be in instanciation mode to do such thing");
    }

    __variableMap = varMap;
  }

  // Sets the map linking variable to all nodes bond to it
  // @throw OperationNotAllowed if diagram not in instanciation mode

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::setVar2NodeMap(
      const HashTable<const DiscreteVariable *, List<NodeId> *> var2NodeMap) {

    if (!__instanciationModeOn) {
      GUM_ERROR(OperationNotAllowed,
                "Must first be in instanciation mode to do such thing");
    }

    for (HashTableConstIteratorSafe<const DiscreteVariable *, List<NodeId> *>
             varIter = var2NodeMap.beginSafe();
         varIter != var2NodeMap.endSafe(); ++varIter)
      __var2NodeIdMap.insert(varIter.key(), new List<NodeId>(*(varIter.val())));
  }

  //  Sets the map linking variable to used modality in graph
  // @throw OperationNotAllowed if diagram has already been instanciated or if not in
  // instanciation mode

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramBase<GUM_SCALAR>::setVarUsedModalitiesMap(const HashTable<
      const DiscreteVariable *, std::vector<Idx> *> varUsedModalitiesMap) {

    if (!__instanciationModeOn) {
      GUM_ERROR(OperationNotAllowed,
                "Must first be in instanciation mode to do such thing");
    }

    for (HashTableConstIteratorSafe<const DiscreteVariable *, std::vector<Idx> *>
             varIter = varUsedModalitiesMap.beginSafe();
         varIter != varUsedModalitiesMap.endSafe(); ++varIter)
      __varUsedModalitiesMap.insert(varIter.key(),
                                    new std::vector<Idx>(*(varIter.val())));
  }

  // Binds once and for all terminal nodes to value.
  // @throw OperationNotAllowed if diagram not in instanciation mode

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::setValueMap(
      const Bijection<NodeId, GUM_SCALAR> &valueMap) {

    if (!__instanciationModeOn) {
      GUM_ERROR(OperationNotAllowed,
                "Must first be in instanciation mode to do such thing");
    }

    __valueMap = valueMap;
  }

  // Links once and for all nodes of the graph.
  // @throw OperationNotAllowed if diagram not in instanciation mode

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::setDiagramArcs(
      const NodeProperty<std::vector<NodeId> *> &arcMap,
      const NodeProperty<NodeId> &defaultArcMap) {

    if (!__instanciationModeOn) {
      GUM_ERROR(OperationNotAllowed,
                "Must first be in instanciation mode to do such thing");
    }

    __defaultArcMap = defaultArcMap;

    for (HashTableConstIteratorSafe<NodeId, std::vector<NodeId> *> arcIter =
             arcMap.beginSafe();
         arcIter != arcMap.endSafe(); ++arcIter)
      __arcMap.insert(arcIter.key(), new std::vector<NodeId>(*(arcIter.val())));
  }

  // Sets once and for all root node.
  // @throw OperationNotAllowed if diagram not in instanciation mode

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecisionDiagramBase<GUM_SCALAR>::setRoot(const NodeId &root) {

    if (!__instanciationModeOn) {
      GUM_ERROR(OperationNotAllowed,
                "Must first be in instanciation mode to do such thing");
    }

    __root = root;
  }

  /**
   * ***********************************************************************************
   * **/
  /**        Protected Methods **/
  /**
   * ***********************************************************************************
   * **/

  // Returns data addressed by inst

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR &
  MultiDimDecisionDiagramBase<GUM_SCALAR>::_get(const Instantiation &inst) const {

    GUM_ERROR(OperationNotAllowed,
              "a MultiDimDecisionDiagram is a read only MultiDim");
  }

  // Supposed to replace var x by y. But not authorized in a
  // MultiDimDecisionDiagramBase
  // @throw OperationNotAllowed without condition.

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecisionDiagramBase<GUM_SCALAR>::_swap(const DiscreteVariable *x,
                                                 const DiscreteVariable *y) {

    GUM_ERROR(OperationNotAllowed, "Can't be done in a MultiDimDecisionDiagram");
  }

  // And the recursive parts of this function
  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramBase<GUM_SCALAR>::__makePreceedingVariablesLists(
      HashTable<NodeId, Set<const DiscreteVariable *> *> *preceedingVariablesTable)
      const {

    PriorityQueue<NodeId> fifo;
    fifo.insert(this->__root,
                this->variablesSequence().pos(__variableMap[this->__root]));
    preceedingVariablesTable->insert(this->__root,
                                     new Set<const DiscreteVariable *>());

    while (!fifo.empty()) {

      NodeId currentNode = fifo.pop();

      if (!this->isTerminalNode(currentNode)) {

        for (std::vector<NodeId>::iterator arcIter = __arcMap[currentNode]->begin();
             arcIter != __arcMap[currentNode]->end(); arcIter++) {

          if (*arcIter != 0 && !this->isTerminalNode(*arcIter)) {

            if (preceedingVariablesTable->exists(*arcIter)) {

              Set<const DiscreteVariable *> *setTemp =
                  (*preceedingVariablesTable)[*arcIter];
              (*preceedingVariablesTable)[*arcIter] =
                  new Set<const DiscreteVariable *>(
                      *((*preceedingVariablesTable)[currentNode]) + *setTemp);
              delete setTemp;

            } else
              preceedingVariablesTable->insert(
                  *arcIter, new Set<const DiscreteVariable *>(
                                *((*preceedingVariablesTable)[currentNode])));

            (*preceedingVariablesTable)[*arcIter]->insert(
                __variableMap[currentNode]);

            if (!fifo.contains(*arcIter))
              fifo.insert(*arcIter,
                          this->variablesSequence().pos(__variableMap[*arcIter]));
          }
        }

        if (__defaultArcMap.exists(currentNode) &&
            !this->isTerminalNode(__defaultArcMap[currentNode])) {

          NodeId defaultSon = __defaultArcMap[currentNode];

          if (preceedingVariablesTable->exists(defaultSon)) {

            Set<const DiscreteVariable *> *setTemp =
                (*preceedingVariablesTable)[defaultSon];
            (*preceedingVariablesTable)[defaultSon] =
                new Set<const DiscreteVariable *>(
                    *((*preceedingVariablesTable)[currentNode]) + *setTemp);
            delete setTemp;

          } else
            preceedingVariablesTable->insert(
                defaultSon, new Set<const DiscreteVariable *>(
                                *((*preceedingVariablesTable)[currentNode])));

          (*preceedingVariablesTable)[defaultSon]->insert(
              __variableMap[currentNode]);

          if (!fifo.contains(defaultSon))
            fifo.insert(defaultSon,
                        this->variablesSequence().pos(__variableMap[defaultSon]));
        }
      }
    }
  }

  // And the recursive parts of this function
  template <typename GUM_SCALAR>
  void MultiDimDecisionDiagramBase<GUM_SCALAR>::__findRetorgradeVariables(
      const Sequence<const DiscreteVariable *> *varsSeq, const NodeId currentNode,
      HashTable<NodeId, Set<const DiscreteVariable *> *> *retrogradeVarTable,
      HashTable<NodeId, Set<const DiscreteVariable *> *> *preceedingVariablesTable,
      List<NodeId> *visitedNodes) const {

    if (!retrogradeVarTable->exists(currentNode))
      retrogradeVarTable->insert(currentNode, new Set<const DiscreteVariable *>());

    Set<const DiscreteVariable *> *currentVarSet =
        (*retrogradeVarTable)[currentNode];

    for (std::vector<NodeId>::iterator sonsIter = __arcMap[currentNode]->begin();
         sonsIter != __arcMap[currentNode]->end(); sonsIter++) {

      if (*sonsIter != 0 && !this->isTerminalNode(*sonsIter)) {

        if (!visitedNodes->exists(*sonsIter))
          this->__findRetorgradeVariables(varsSeq, *sonsIter, retrogradeVarTable,
                                          preceedingVariablesTable, visitedNodes);

        Set<const DiscreteVariable *> *setTemp = currentVarSet;
        currentVarSet = new Set<const DiscreteVariable *>(
            *((*retrogradeVarTable)[*sonsIter]) + *setTemp);
        delete setTemp;
      }
    }

    if (__defaultArcMap.exists(currentNode) &&
        !this->isTerminalNode(__defaultArcMap[currentNode])) {

      NodeId defaultSon = this->nodeDefaultSon(currentNode);

      if (!visitedNodes->exists(defaultSon))
        this->__findRetorgradeVariables(varsSeq, defaultSon, retrogradeVarTable,
                                        preceedingVariablesTable, visitedNodes);

      Set<const DiscreteVariable *> *setTemp = currentVarSet;
      currentVarSet = new Set<const DiscreteVariable *>(
          *((*retrogradeVarTable)[defaultSon]) + *setTemp);
      delete setTemp;
    }

    retrogradeVarTable->erase(currentNode);
    retrogradeVarTable->insert(currentNode, currentVarSet);

    for (SetIteratorSafe<const DiscreteVariable *> nodeParentsVarIter =
             (*preceedingVariablesTable)[currentNode]->beginSafe();
         nodeParentsVarIter != (*preceedingVariablesTable)[currentNode]->endSafe();
         ++nodeParentsVarIter)
      if (varsSeq->pos(*nodeParentsVarIter) >
          varsSeq->pos(this->nodeVariable(currentNode))) {
        currentVarSet->insert(this->nodeVariable(currentNode));
        break;
      }

    visitedNodes->insert(currentNode);
  }
}
