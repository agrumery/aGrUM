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
* @brief Template implementation of InfluenceDiagram/InfluenceDiagram.h classes.
*
* @author Jean-Christophe Magnan & Pierre_Henri WUILLEMIN
*/

#include <agrum/ID/influenceDiagram.h>
#include <cstdio>
#include <iostream>

namespace gum {

  // ===========================================================================
  // Constructors / Destructors
  // ===========================================================================

  /*
  * Default constructor.
  */
  template <typename GUM_SCALAR>
  INLINE InfluenceDiagram<GUM_SCALAR>::InfluenceDiagram()
      : DAGmodel() {
    GUM_CONSTRUCTOR(InfluenceDiagram);
  }

  /*
  * Destructor.
  */
  template <typename GUM_SCALAR> InfluenceDiagram<GUM_SCALAR>::~InfluenceDiagram() {
    GUM_DESTRUCTOR(InfluenceDiagram);
    _removeTables();
  }

  /*
  * Copy Constructor
  */
  template <typename GUM_SCALAR>
  InfluenceDiagram<GUM_SCALAR>::InfluenceDiagram(
      const InfluenceDiagram<GUM_SCALAR> &source)
      : DAGmodel(source), __variableMap(source.__variableMap) {
    GUM_CONS_CPY(InfluenceDiagram);
    _copyTables(source);
  }

  /*
  * Copy Operator
  */
  template <typename GUM_SCALAR>
  InfluenceDiagram<GUM_SCALAR> &InfluenceDiagram<GUM_SCALAR>::
  operator=(const InfluenceDiagram<GUM_SCALAR> &source) {
    if (this != &source) {
      DAGmodel::operator=(source);
      // Removing previous potentials
      _removeTables();
      __potentialMap.clear();
      __utilityMap.clear();

      __variableMap = source.__variableMap;

      // Copying tables
      _copyTables(source);
    }

    return *this;
  }

  /*
  * Removing ancient table
  */
  template <typename GUM_SCALAR> void InfluenceDiagram<GUM_SCALAR>::_removeTables() {
    for (const auto node : _dag.nodes()) {
      if (isChanceNode(node))
        delete &cpt(node);
      else if (isUtilityNode(node))
        delete &utility(node);
    }
  }

  /*
  * Copying tables from another influence diagram
  */
  template <typename GUM_SCALAR>
  void InfluenceDiagram<GUM_SCALAR>::_copyTables(
      const InfluenceDiagram<GUM_SCALAR> &IDsource) {
    // Copying potentials
    Potential<GUM_SCALAR> *potentialCpy = nullptr;

    for (const auto &pot : IDsource.__potentialMap) {
      // Instanciation of the node's CPT
      potentialCpy = new Potential<GUM_SCALAR>;
      (*potentialCpy) << variable(pot.first);

      // Addition of the parents
      for (const auto par : _dag.parents(pot.first))
        (*potentialCpy) << variable(par);

      // Filling up of the table
      Instantiation srcInst(*pot.second);
      Instantiation cpyInst(*potentialCpy);

      for (cpyInst.setFirst(); !cpyInst.end(); cpyInst.inc()) {
        for (Idx i = 0; i < cpyInst.nbrDim(); i++) {
          NodeId id = nodeId(cpyInst.variable(i));
          srcInst.chgVal(IDsource.variable(id), cpyInst.val(i));
        }

        potentialCpy->set(cpyInst, (*pot.second)[srcInst]);
      }

      // Adding cpt to cpt map
      __potentialMap.set(pot.first, potentialCpy);
    }

    // Copying Utilities
    UtilityTable<GUM_SCALAR> *utilityCpy;

    for (const auto &uti : IDsource.__utilityMap) {
      // Instanciation of the node's CPT
      utilityCpy = new UtilityTable<GUM_SCALAR>;
      (*utilityCpy) << variable(uti.first);

      // Addition of the parents
      for (const auto par : _dag.parents(uti.first))
        (*utilityCpy) << variable(par);

      // Filling up of the table
      Instantiation srcInst(*uti.second);

      Instantiation cpyInst(*utilityCpy);

      for (cpyInst.setFirst(); !cpyInst.end(); cpyInst.inc()) {
        for (Idx i = 0; i < cpyInst.nbrDim(); i++) {
          NodeId id = nodeId(cpyInst.variable(i));
          srcInst.chgVal(IDsource.variable(id), cpyInst.val(i));
        }

        utilityCpy->set(cpyInst, (*uti.second)[srcInst]);
      }

      // Adding cpt to cpt map
      __utilityMap.set(uti.first, utilityCpy);
    }
  }

  template <typename GUM_SCALAR>
  std::string InfluenceDiagram<GUM_SCALAR>::toDot(void) const {
    std::stringstream output;
    std::stringstream decisionNode;
    std::stringstream utilityNode;
    std::stringstream chanceNode;
    std::stringstream arcstream;
    output << "digraph \"";

    try {
      output << this->property("name") << "\" {" << std::endl;
    } catch (NotFound &) {
      output << "no_name\" {" << std::endl;
    }

    output << "  node [bgcolor=\"#AAAAAA\", style=filled];" << std::endl;

    decisionNode << "node [shape = box];" << std::endl;

    utilityNode << "node [shape = diamond];" << std::endl;
    chanceNode << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for (const auto node : _dag.nodes()) {
      if (isChanceNode(node))
        chanceNode << tab << "\"" << variable(node).name() << "\""
                   << ";";
      else if (isUtilityNode(node))
        utilityNode << tab << "\"" << variable(node).name() << "\""
                    << ";";
      else
        decisionNode << tab << "\"" << variable(node).name() << "\""
                     << ";";

      if (_dag.children(node).size() > 0)
        for (const auto chi : _dag.children(node))
          arcstream << tab << "\"" << variable(node).name() << "\""
                    << " -> "
                    << "\"" << variable(chi).name() << "\""
                    << ";" << std::endl;
    }

    output << decisionNode.str() << std::endl << utilityNode.str() << std::endl
           << chanceNode.str() << std::endl << arcstream.str() << std::endl << "}"
           << std::endl;

    return output.str();
  }

  template <typename GUM_SCALAR>
  std::string InfluenceDiagram<GUM_SCALAR>::toString(void) const {
    std::stringstream output;

    output << "Influence Diagram{" << std::endl;
    output << "  chance: " << chanceNodeSize() << "," << std::endl;
    output << "  utility: " << utilityNodeSize() << "," << std::endl;
    output << "  decision: " << decisionNodeSize() << "," << std::endl;
    output << "  arcs: " << dag().sizeArcs() << "," << std::endl;

    double dSize = log10DomainSize();

    if (dSize > 6)
      output << "  domainSize: 10^" << dSize;
    else
      output << "  domainSize: " << std::round(std::pow(10.0, dSize));

    output << std::endl << "}";

    return output.str();
  }

  // ===========================================================================
  // Variable manipulation methods.
  // ===========================================================================

  /*
  * Returns the CPT of a chance variable.
  */
  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR> &
  InfluenceDiagram<GUM_SCALAR>::cpt(NodeId varId) const {
    return *(__potentialMap[varId]);
  }

  /*
  * Returns the utility table of a utility node.
  */
  template <typename GUM_SCALAR>
  INLINE const UtilityTable<GUM_SCALAR> &
  InfluenceDiagram<GUM_SCALAR>::utility(NodeId varId) const {
    return *(__utilityMap[varId]);
  }

  /*
  * Return true if node is a utility one
  */
  template <typename GUM_SCALAR>
  INLINE bool InfluenceDiagram<GUM_SCALAR>::isUtilityNode(NodeId varId) const {
    return __utilityMap.exists(varId);
  }

  /*
  * Return true if node is a utility one
  */
  template <typename GUM_SCALAR>
  INLINE bool InfluenceDiagram<GUM_SCALAR>::isDecisionNode(NodeId varId) const {
    bool ret = true;

    if (isUtilityNode(varId) || isChanceNode(varId))
      ret = false;

    return ret;
  }

  /*
  * Return true if node is a chance one
  */
  template <typename GUM_SCALAR>
  INLINE bool InfluenceDiagram<GUM_SCALAR>::isChanceNode(NodeId varId) const {
    return __potentialMap.exists(varId);
  }

  /*
  * Returns the number of utility nodes
  */
  template <typename GUM_SCALAR>
  INLINE Size InfluenceDiagram<GUM_SCALAR>::utilityNodeSize() const {
    return __utilityMap.size();
  }

  /*
  * Returns the number of chance nodes
  */
  template <typename GUM_SCALAR>
  INLINE Size InfluenceDiagram<GUM_SCALAR>::chanceNodeSize() const {
    return __potentialMap.size();
  }

  /*
  * Returns the number of decision nodes
  */
  template <typename GUM_SCALAR>
  INLINE Size InfluenceDiagram<GUM_SCALAR>::decisionNodeSize() const {
    return (size() - __utilityMap.size() - __potentialMap.size());
  }

  /*
  * Returns a constant reference to the VariableNodeMap of this Influence Diagram
  */
  template <typename GUM_SCALAR>
  INLINE const VariableNodeMap &
  InfluenceDiagram<GUM_SCALAR>::variableNodeMap() const {
    return __variableMap;
  }

  /*
  * Returns a constant reference over a variable given it's node id.
  */
  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable &
  InfluenceDiagram<GUM_SCALAR>::variable(NodeId id) const {
    return __variableMap[id];
  }

  /*
  * Return id node from discrete var pointer.
  */
  template <typename GUM_SCALAR>
  INLINE NodeId
      InfluenceDiagram<GUM_SCALAR>::nodeId(const DiscreteVariable &var) const {
    return __variableMap.get(var);
  }

  // Getter by name
  template <typename GUM_SCALAR>
  INLINE NodeId
      InfluenceDiagram<GUM_SCALAR>::idFromName(const std::string &name) const {
    return __variableMap.idFromName(name);
  }

  // Getter by name
  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable &
  InfluenceDiagram<GUM_SCALAR>::variableFromName(const std::string &name) const {
    return __variableMap.variableFromName(name);
  }

  /*
  * Add a chance variable, it's associate node and it's CPT. The id of the new
  * variable is automatically generated.
  */
  template <typename GUM_SCALAR>
  NodeId InfluenceDiagram<GUM_SCALAR>::add(const DiscreteVariable &var,
                                           NodeId varId) {
    return addChanceNode(var, varId);
  }

  /*
  * Add a utility variable, it's associate node and it's UT. The id of the new
  * variable is automatically generated.
  * @Throws : Gum::InvalidArgument if var has more than one state
  */
  template <typename GUM_SCALAR>
  NodeId InfluenceDiagram<GUM_SCALAR>::addUtilityNode(const DiscreteVariable &var,
                                                      NodeId varId) {
    MultiDimArray<GUM_SCALAR> *newMultiDim = new MultiDimArray<GUM_SCALAR>();
    NodeId res = 0;

    try {
      res = addUtilityNode(var, newMultiDim, varId);
    } catch (Exception &e) {
      delete newMultiDim;
      throw;
    }

    return res;
  }

  /*
  * Add a decision variable. The id of the new
  * variable is automatically generated.
  */
  template <typename GUM_SCALAR>
  NodeId InfluenceDiagram<GUM_SCALAR>::addDecisionNode(const DiscreteVariable &var,
                                                       NodeId varId) {
    return _addNode(var, varId);
  }

  /*
  * Add a chance variable, it's associate node and it's CPT. The id of the new
  * variable is automatically generated.
  */
  template <typename GUM_SCALAR>
  NodeId InfluenceDiagram<GUM_SCALAR>::addChanceNode(const DiscreteVariable &var,
                                                     NodeId varId) {
    MultiDimArray<GUM_SCALAR> *newMultiDim = new MultiDimArray<GUM_SCALAR>();
    NodeId res = 0;

    try {
      res = addChanceNode(var, newMultiDim, varId);
    } catch (Exception &e) {
      delete newMultiDim;
      throw;
    }

    return res;
  }

  /*
  * Add a chance variable, it's associate node and it's CPT. The id of the new
  * variable is automatically generated.
  */
  template <typename GUM_SCALAR>
  NodeId InfluenceDiagram<GUM_SCALAR>::addChanceNode(
      const DiscreteVariable &var, MultiDimImplementation<GUM_SCALAR> *aContent,
      NodeId DesiredId) {
    NodeId proposedId = _addNode(var, DesiredId);

    Potential<GUM_SCALAR> *varcpt = new Potential<GUM_SCALAR>(aContent);
    (*varcpt) << variable(proposedId);
    __potentialMap.insert(proposedId, varcpt);

    return proposedId;
  }

  /*
  * Add a utility variable, it's associate node and it's UT. The id of the new
  * variable is automatically generated.
  * @Throws : Gum::InvalidArgument if var has more than one state
  */
  template <typename GUM_SCALAR>
  NodeId InfluenceDiagram<GUM_SCALAR>::addUtilityNode(
      const DiscreteVariable &var, MultiDimImplementation<GUM_SCALAR> *aContent,
      NodeId DesiredId) {
    if (var.domainSize() != 1) {
      GUM_ERROR(InvalidArgument, "Utility var have no state ( which implicates a "
                                 "single label for data output reasons ).");
    }

    NodeId proposedId = _addNode(var, DesiredId);

    UtilityTable<GUM_SCALAR> *varut = new UtilityTable<GUM_SCALAR>(aContent);

    (*varut) << variable(proposedId);

    __utilityMap.insert(proposedId, varut);

    return proposedId;
  }

  /*
  * Add a node
  */
  template <typename GUM_SCALAR>
  NodeId InfluenceDiagram<GUM_SCALAR>::_addNode(const DiscreteVariable &variableType,
                                                NodeId DesiredId) {
    // None thread safe code!
    NodeId proposedId;

    if (DesiredId == 0)
      proposedId = _dag.nextNodeId();
    else
      proposedId = DesiredId;

    __variableMap.insert(proposedId, variableType);

    _dag.addNode(proposedId);

    // end critical section
    return proposedId;
  }

  /*
  * Erase a Variable from the network and remove the variable from
  * all childs of id.
  * If no variable matches the id, then nothing is done.
  */
  template <typename GUM_SCALAR>
  void InfluenceDiagram<GUM_SCALAR>::erase(NodeId varId) {
    if (__variableMap.exists(varId)) {
      // Reduce the variable child's CPT or Utility Table if necessary
      for (const auto chi : _dag.children(varId))
        if (isChanceNode(chi))
          __potentialMap[chi]->erase(variable(varId));
        else if (isUtilityNode(chi))
          __utilityMap[chi]->erase(variable(varId));

      if (isChanceNode(varId)) {
        delete __potentialMap[varId];
        __potentialMap.erase(varId);
      } else if (isUtilityNode(varId)) {
        delete __utilityMap[varId];
        __utilityMap.erase(varId);
      }

      __variableMap.erase(varId);
      _dag.eraseNode(varId);
    }
  }

  /*
  * Erase a Variable from the network and remove the variable from
  * all childs of var.
  * If no variable matches, then nothing is done.
  */
  template <typename GUM_SCALAR>
  INLINE void InfluenceDiagram<GUM_SCALAR>::erase(const DiscreteVariable &var) {
    erase(__variableMap.get(var));
  }

  /* we allow the user to change the name of a variable
  */
  template <typename GUM_SCALAR>
  INLINE void
  InfluenceDiagram<GUM_SCALAR>::changeVariableName(NodeId id,
                                                   const std::string &new_name) {
    __variableMap.changeName(id, new_name);
  }

  // ===========================================================================
  // @name Arc manipulation methods.
  // ===========================================================================
  /*
  * Add an arc in the ID, and update diagram's chance nodes cpt if necessary.
  */
  template <typename GUM_SCALAR>
  INLINE void InfluenceDiagram<GUM_SCALAR>::insertArc(NodeId tail, NodeId head) {
    addArc(tail, head);
  }

  /*
  * Add an arc in the ID, and update diagram's chance nodes cpt if necessary.
  */
  template <typename GUM_SCALAR>
  INLINE void InfluenceDiagram<GUM_SCALAR>::addArc(NodeId tail, NodeId head) {
    if (isUtilityNode(tail)) {
      GUM_ERROR(InvalidArc, "Tail cannot be a utility node");
    }

    _dag.addArc(tail, head);

    if (isChanceNode(head))
      // Add parent in the child's CPT
      (*(__potentialMap[head])) << variable(tail);
    else if (isUtilityNode(head))
      // Add parent in the child's UT
      (*(__utilityMap[head])) << variable(tail);
  }

  /*
  * Removes an arc in the ID, and update diagram chance nodes cpt if necessary.
  *
  * If (tail, head) doesn't exist, the nothing happens.
  */
  template <typename GUM_SCALAR>
  INLINE void InfluenceDiagram<GUM_SCALAR>::eraseArc(const Arc &arc) {
    if (_dag.existsArc(arc)) {
      NodeId head = arc.head(), tail = arc.tail();
      _dag.eraseArc(arc);

      if (isChanceNode(head))
        // Removes parent in the child's CPT
        (*(__potentialMap[head])) >> variable(tail);
      else if (isUtilityNode(head))
        // Removes parent in the child's UT
        (*(__utilityMap[head])) >> variable(tail);
    }
  }

  /*
  * Removes an arc in the ID, and update diagram chance nodes cpt if necessary.
  *
  * If (tail, head) doesn't exist, the nothing happens.
  */
  template <typename GUM_SCALAR>
  INLINE void InfluenceDiagram<GUM_SCALAR>::eraseArc(NodeId tail, NodeId head) {
    eraseArc(Arc(tail, head));
  }

  // ===========================================================================
  // Graphical methods
  // ===========================================================================

  /*
  * The node's id are coherent with the variables and nodes of the topology.
  */
  template <typename GUM_SCALAR>
  void InfluenceDiagram<GUM_SCALAR>::_moralGraph(UndiGraph &graph) const {
    for (const auto node : _dag.nodes())
      if (!isUtilityNode(node))
        graph.addNode(node);

    for (const auto node : _dag.nodes()) {
      if (!isDecisionNode(node))
        for (const auto par : _dag.parents(node)) {
          if (isChanceNode(node))
            graph.addEdge(node, par);

          for (const auto par2 : _dag.parents(node))
            if (par != par2)
              graph.addEdge(par, par2);
        }
    }
  }

  /*
  * True if a directed path exist with all decison nodes
  */
  template <typename GUM_SCALAR>
  bool InfluenceDiagram<GUM_SCALAR>::decisionOrderExists() const {
    const Sequence<NodeId> &order = topologicalOrder(true);

    // Finding first decision node
    Sequence<NodeId>::const_iterator orderIter = order.begin();

    while ((orderIter != order.end()) && (!isDecisionNode(*orderIter)))
      ++orderIter;

    if (orderIter == order.end())
      return true;

    NodeId parentDecision = (*orderIter);
    ++orderIter;

    // Checking path between decisions nodes
    while (orderIter != order.end()) {
      if (isDecisionNode(*orderIter)) {
        if (!existsPathBetween(parentDecision, *orderIter))
          return false;

        parentDecision = *orderIter;
      }

      ++orderIter;
    }

    return true;
  }

  /*
  * Returns true if a path exists between source and destination
  */
  template <typename GUM_SCALAR>
  bool InfluenceDiagram<GUM_SCALAR>::existsPathBetween(NodeId src,
                                                       NodeId dest) const {
    List<NodeId> nodeFIFO;
    // mark[node] contains 0 if not visited
    // mark[node] = predecessor if visited
    NodeProperty<int> mark = _dag.nodesProperty((int)-1);
    NodeId current;

    mark[src] = (int)src;
    nodeFIFO.pushBack(src);

    while (!nodeFIFO.empty()) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      for (const auto new_one : _dag.children(current)) {
        if (mark[new_one] != -1)
          continue; // if this node is already marked, continue

        mark[new_one] = (int)current;

        if (new_one == dest)
          break; // if we reach *orderIter, stop.

        nodeFIFO.pushBack(new_one);
      }
    }

    if (mark[dest] == -1)
      return false;

    return true;
  }

  /*
  * Returns the decision graph
  */
  template <typename GUM_SCALAR>
  gum::DAG *InfluenceDiagram<GUM_SCALAR>::getDecisionGraph() const {
    gum::DAG *temporalGraph = new gum::DAG();

    for (const auto node : _dag.nodes()) {
      if (isDecisionNode(node)) {
        if (!temporalGraph->existsNode(node))
          temporalGraph->addNode(node);

        for (const auto chi : _getChildrenDecision(node)) {
          if (!temporalGraph->existsNode(chi))
            temporalGraph->addNode(chi);

          temporalGraph->addArc(node, chi);
        }
      }
    }

    return temporalGraph;
  }

  /*
  * Returns the list of children decision for a given nodeId
  */
  template <typename GUM_SCALAR>
  Sequence<NodeId>
  InfluenceDiagram<GUM_SCALAR>::_getChildrenDecision(NodeId parentDecision) const {
    Sequence<NodeId> childrenSeq;

    List<NodeId> nodeFIFO;
    NodeId current;

    // mark[node] contains false if not visited
    // mark[node] contains true if visited
    NodeProperty<bool> mark = _dag.nodesProperty(false);

    mark[parentDecision] = true;

    nodeFIFO.pushBack(parentDecision);

    while (!nodeFIFO.empty()) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      for (const auto new_one : _dag.children(current)) {
        if (mark[new_one])
          continue; // if this node is already marked, continue

        mark[new_one] = true;

        if (!isDecisionNode(new_one))
          nodeFIFO.pushBack(new_one);
        else
          childrenSeq.insert(new_one);
      }
    }

    return childrenSeq;
  }

  /*
  * Returns the sequence of decision nodes
  * @throw NotFound if such a sequence does not exist
  */
  template <typename GUM_SCALAR>
  std::vector<NodeId> *InfluenceDiagram<GUM_SCALAR>::getDecisionOrder() const {
    if (!decisionOrderExists()) {
      GUM_ERROR(NotFound, "No decision path exists");
    }

    std::vector<NodeId> *decisionSequence = new std::vector<NodeId>();

    for (const auto elt : topologicalOrder(false))
      if (isDecisionNode(elt))
        decisionSequence->push_back(elt);

    return decisionSequence;
  }

  /*
  * Returns partial temporal ordering
  * @throw NotFound if such a sequence does not exist
  */
  template <typename GUM_SCALAR>
  const List<NodeSet> &
  InfluenceDiagram<GUM_SCALAR>::getPartialTemporalOrder(bool clear) const {
    if (clear) {
      __temporalOrder.clear();

      std::vector<NodeId> *decisionOrder = getDecisionOrder();
      NodeSet nodeList = _dag.asNodeSet();

      for (unsigned int i = 0; i < decisionOrder->size(); i++) {
        NodeSet partialOrderedSet;

        for (const auto par : _dag.parents(decisionOrder->at(i))) {
          if (nodeList.contains(par) && isChanceNode(par)) {
            partialOrderedSet.insert(par);
            nodeList.erase(par);
          }
        }

        if (!partialOrderedSet.empty())
          __temporalOrder.pushFront(partialOrderedSet);

        NodeSet decisionSet;

        decisionSet.insert(decisionOrder->at(i));

        __temporalOrder.pushFront(decisionSet);
      }

      NodeSet lastSet; //= new gum::NodeSet();

      for (const auto node : nodeList)
        if (isChanceNode(node))
          lastSet.insert(node);

      if (!lastSet.empty())
        __temporalOrder.pushFront(lastSet);
    }

    return __temporalOrder;
  }
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
