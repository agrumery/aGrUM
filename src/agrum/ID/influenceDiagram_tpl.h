/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Template implementation of InfluenceDiagram/InfluenceDiagram.h classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#include <cstdio>
#include <iostream>
#include <algorithm>

#include <agrum/tools/variables/rangeVariable.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <agrum/tools/variables/integerVariable.h>
#include <agrum/tools/variables/discretizedVariable.h>

#include <agrum/ID/influenceDiagram.h>

namespace gum {
  template < typename GUM_SCALAR >
  NodeId build_node_for_ID(gum::InfluenceDiagram< GUM_SCALAR >& infdiag,
                           std::string                          node,
                           gum::Size                            default_domain_size) {
    auto                       ds        = default_domain_size;
    long                       range_min = 0;
    long                       range_max = long(ds) - 1;
    std::vector< std::string > labels;
    std::vector< GUM_SCALAR >  ticks;
    bool                       isUtil, isDeci, isChanc;
    isUtil  = false;
    isDeci  = false;
    isChanc = false;

    switch (*(node.begin())) {
      case '*':
        isDeci = true;
        node.erase(0, 1);
        break;
      case '$':
        isUtil = true;
        node.erase(0, 1);
        break;
      default:
        isChanc = true;
    }

    std::string name = node;
    if (*(node.rbegin()) == ']') {
      auto posBrack = node.find('[');
      if (posBrack != std::string::npos) {
        name               = node.substr(0, posBrack);
        const auto& s_args = node.substr(posBrack + 1, node.size() - posBrack - 2);
        const auto& args   = split(s_args, ",");
        if (args.empty()) {   // n[]
          GUM_ERROR(InvalidArgument, "Empty range for variable " << node)
        } else if (args.size() == 1) {   // n[4]
          ds        = static_cast< Size >(std::stoi(args[0]));
          range_min = 0;
          range_max = long(ds) - 1;
        } else if (args.size() == 2) {   // n[5,10]
          range_min = std::stol(args[0]);
          range_max = std::stol(args[1]);
          if (1 + range_max - range_min < 2) {
            GUM_ERROR(InvalidArgument, "Invalid range for variable " << node)
          }
          ds = static_cast< Size >(1 + range_max - range_min);
        } else {   // n[3.14,5,10,12]
          for (const auto& tick: args) {
            ticks.push_back(static_cast< GUM_SCALAR >(std::strtod(tick.c_str(), nullptr)));
          }
          ds = static_cast< Size >(args.size() - 1);
        }
      }
    } else if (*(node.rbegin()) == '}') {   // node like "n{one|two|three}"
      auto posBrack = node.find('{');
      if (posBrack != std::string::npos) {
        name   = node.substr(0, posBrack);
        labels = split(node.substr(posBrack + 1, node.size() - posBrack - 2), "|");
        if (labels.size() < 2) { GUM_ERROR(InvalidArgument, "Not enough labels in node " << node) }
        if (!hasUniqueElts(labels)) {
          GUM_ERROR(InvalidArgument, "Duplicate labels in node " << node)
        }
        ds = static_cast< Size >(labels.size());
      }
    }

    if (ds == 0) {
      GUM_ERROR(InvalidArgument, "No value for variable " << name << ".")
    } else if (ds == 1) {
      GUM_ERROR(InvalidArgument,
                "Only one value for variable " << name << " (2 at least are needed).")
    }

    std::vector< int > values;
    if (!labels.empty()) {
      if (std::all_of(labels.begin(), labels.end(), isInteger)) {
        for (const auto& label: labels)
          values.push_back(std::stoi(label));
      }
    }

    // now we add the node in the Influence Diagram
    NodeId idVar;
    trim(name);
    try {
      idVar = infdiag.idFromName(name);
    } catch (gum::NotFound&) {
      if (isChanc) {
        if (!values.empty()) {
          idVar = infdiag.addChanceNode(IntegerVariable(name, name, values));
        } else if (!labels.empty()) {
          idVar = infdiag.addChanceNode(LabelizedVariable(name, name, labels));
        } else if (!ticks.empty()) {
          idVar = infdiag.addChanceNode(DiscretizedVariable< GUM_SCALAR >(name, name, ticks));
        } else {
          idVar = infdiag.addChanceNode(RangeVariable(name, name, range_min, range_max));
        }
      } else if (isDeci) {
        if (!values.empty()) {
          idVar = infdiag.addDecisionNode(IntegerVariable(name, name, values));
        } else if (!labels.empty()) {
          idVar = infdiag.addDecisionNode(LabelizedVariable(name, name, labels));
        } else if (!ticks.empty()) {
          idVar = infdiag.addDecisionNode(DiscretizedVariable< GUM_SCALAR >(name, name, ticks));
        } else {
          idVar = infdiag.addDecisionNode(RangeVariable(name, name, range_min, range_max));
        }
      } else if (isUtil) {
        idVar = infdiag.addUtilityNode(LabelizedVariable(name, name, 1));
      } else {
        GUM_ERROR(FatalError,
                  "No type (chance, decision or utility) for the node '" << node << "'.")
      }
    }

    return idVar;
  }


  template < typename GUM_SCALAR >
  InfluenceDiagram< GUM_SCALAR >
     InfluenceDiagram< GUM_SCALAR >::fastPrototype(const std::string& dotlike, Size domainSize) {
    gum::InfluenceDiagram< GUM_SCALAR > infdiag;

    for (const auto& chaine: split(remove_newline(dotlike), ";")) {
      NodeId lastId   = 0;
      bool   notfirst = false;
      for (const auto& souschaine: split(chaine, "->")) {
        bool forward = true;
        for (auto& node: split(souschaine, "<-")) {
          trim(node);
          auto idVar = build_node_for_ID(infdiag, node, domainSize);
          if (notfirst) {
            if (forward) {
              infdiag.addArc(lastId, idVar);
              forward = false;
            } else {
              infdiag.addArc(idVar, lastId);
            }
          } else {
            notfirst = true;
            forward  = false;
          }
          lastId = idVar;
        }
      }
    }

    for (const auto n: infdiag.nodes()) {
      if (infdiag.isChanceNode(n))
        infdiag.cpt(n).randomCPT();
      else if (infdiag.isUtilityNode(n)) {
        infdiag.utility(n).random().scale(50).translate(-10);
      }
    }

    infdiag.setProperty("name", "fastPrototype");
    return infdiag;
  }
  // ===========================================================================
  // Constructors / Destructors
  // ===========================================================================

  /*
   * Default constructor.
   */
  template < typename GUM_SCALAR >
  INLINE InfluenceDiagram< GUM_SCALAR >::InfluenceDiagram() : DAGmodel() {
    GUM_CONSTRUCTOR(InfluenceDiagram);
  }

  /*
   * Destructor.
   */
  template < typename GUM_SCALAR >
  InfluenceDiagram< GUM_SCALAR >::~InfluenceDiagram() {
    GUM_DESTRUCTOR(InfluenceDiagram);
    removeTables_();
  }

  /*
   * Copy Constructor
   */
  template < typename GUM_SCALAR >
  InfluenceDiagram< GUM_SCALAR >::InfluenceDiagram(const InfluenceDiagram< GUM_SCALAR >& source) {
    GUM_CONS_CPY(InfluenceDiagram);
    copyStructureAndTables_(source);
  }

  /*
   * Copy Operator
   */
  template < typename GUM_SCALAR >
  InfluenceDiagram< GUM_SCALAR >&
     InfluenceDiagram< GUM_SCALAR >::operator=(const InfluenceDiagram< GUM_SCALAR >& source) {
    if (this != &source) {
      clear();
      // Copying tables and structure
      copyStructureAndTables_(source);
    }

    return *this;
  }

  template < typename GUM_SCALAR >
  void InfluenceDiagram< GUM_SCALAR >::InfluenceDiagram::clear() {
    // Removing previous potentials
    removeTables_();
    _variableMap_.clear();
    dag_.clear();
    _potentialMap_.clear();
    _utilityMap_.clear();
  }

  /*
   * Removing ancient table
   */
  template < typename GUM_SCALAR >
  void InfluenceDiagram< GUM_SCALAR >::removeTables_() {
    for (const auto node: dag_.nodes()) {
      if (isChanceNode(node))
        delete &cpt(node);
      else if (isUtilityNode(node))
        delete &utility(node);
    }
  }

  /*
   * Copying tables from another influence diagram
   */
  template < typename GUM_SCALAR >
  void InfluenceDiagram< GUM_SCALAR >::copyStructureAndTables_(
     const InfluenceDiagram< GUM_SCALAR >& IDsource) {
    for (auto node: IDsource.nodes()) {
      if (IDsource.isChanceNode(node))
        addChanceNode(IDsource.variable(node), node);
      else if (IDsource.isUtilityNode(node))
        addUtilityNode(IDsource.variable(node), node);
      else   // decision node
        addDecisionNode(IDsource.variable(node), node);
    }
    // we add arc in the same order of the potentials
    for (auto node: IDsource.nodes()) {
      const auto& s = IDsource.variable(node).name();
      if (IDsource.isChanceNode(node)) {
        for (Idx par = 1; par <= IDsource.parents(node).size(); par++)
          addArc(IDsource.cpt(node).variable(par).name(), s);
      } else if (IDsource.isUtilityNode(node)) {
        for (Idx par = 1; par <= IDsource.parents(node).size(); par++)
          addArc(IDsource.utility(node).variable(par).name(), s);
      } else {   // decision node
        // here the order does not depends on a Potential
        for (NodeId par: IDsource.parents(node))
          addArc(par, node);
      }
    }

    // Copying potentials
    for (auto node: IDsource.nodes()) {
      const auto& s = IDsource.variable(node).name();
      if (IDsource.isChanceNode(node)) {
        cpt(node).fillWith(IDsource.cpt(s));
      } else if (IDsource.isUtilityNode(node)) {
        utility(node).fillWith(IDsource.utility(s));
      }
    }
  }

  template < typename GUM_SCALAR >
  std::string InfluenceDiagram< GUM_SCALAR >::toDot() const {
    std::stringstream output;
    std::stringstream decisionNode;
    std::stringstream utilityNode;
    std::stringstream chanceNode;
    std::stringstream arcstream;
    output << "digraph \"";

    try {
      output << this->property("name") << "\" {" << std::endl;
    } catch (NotFound&) { output << "no_name\" {" << std::endl; }

    output << "  node [bgcolor=\"#AAAAAA\", style=filled, height=0];" << std::endl;

    decisionNode << "node [shape = box];" << std::endl;

    utilityNode << "node [shape =  hexagon, margin=0];" << std::endl;
    chanceNode << "node [shape = ellipse];" << std::endl;
    std::string tab = "  ";

    for (const auto node: dag_.nodes()) {
      if (isChanceNode(node))
        chanceNode << tab << "\"" << node << "-" << variable(node).name() << "\""
                   << ";";
      else if (isUtilityNode(node))
        utilityNode << tab << "\"" << node << "-" << variable(node).name() << "\""
                    << ";";
      else
        decisionNode << tab << "\"" << node << "-" << variable(node).name() << "\""
                     << ";";

      if (dag_.children(node).size() > 0)
        for (const auto chi: dag_.children(node)) {
          arcstream << "\"" << node << "-" << variable(node).name() << "\""
                    << " -> "
                    << "\"" << chi << "-" << variable(chi).name() << "\"";
          if (isDecisionNode(chi)) { arcstream << " [style=\"tapered, bold\"]"; }
          arcstream << ";" << std::endl;
        }
    }

    output << decisionNode.str() << std::endl
           << utilityNode.str() << std::endl
           << chanceNode.str() << std::endl
           << std::endl
           << arcstream.str() << std::endl
           << "}" << std::endl;

    return output.str();
  }

  template < typename GUM_SCALAR >
  std::string InfluenceDiagram< GUM_SCALAR >::toString() const {
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
  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >& InfluenceDiagram< GUM_SCALAR >::cpt(NodeId varId) const {
    return *(_potentialMap_[varId]);
  }

  /*
   * Returns the utility table of a utility node.
   */
  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               InfluenceDiagram< GUM_SCALAR >::utility(NodeId varId) const {
    return *(_utilityMap_[varId]);
  }

  /*
   * Return true if node is a utility one
   */
  template < typename GUM_SCALAR >
  INLINE bool InfluenceDiagram< GUM_SCALAR >::isUtilityNode(NodeId varId) const {
    return _utilityMap_.exists(varId);
  }

  /*
   * Return true if node is a utility one
   */
  template < typename GUM_SCALAR >
  INLINE bool InfluenceDiagram< GUM_SCALAR >::isDecisionNode(NodeId varId) const {
    bool ret = true;

    if (isUtilityNode(varId) || isChanceNode(varId)) ret = false;

    return ret;
  }

  /*
   * Return true if node is a chance one
   */
  template < typename GUM_SCALAR >
  INLINE bool InfluenceDiagram< GUM_SCALAR >::isChanceNode(NodeId varId) const {
    return _potentialMap_.exists(varId);
  }

  /*
   * Returns the number of utility nodes
   */
  template < typename GUM_SCALAR >
  INLINE Size InfluenceDiagram< GUM_SCALAR >::utilityNodeSize() const {
    return _utilityMap_.size();
  }

  /*
   * Returns the number of chance nodes
   */
  template < typename GUM_SCALAR >
  INLINE Size InfluenceDiagram< GUM_SCALAR >::chanceNodeSize() const {
    return _potentialMap_.size();
  }

  /*
   * Returns the number of decision nodes
   */
  template < typename GUM_SCALAR >
  INLINE Size InfluenceDiagram< GUM_SCALAR >::decisionNodeSize() const {
    return (size() - _utilityMap_.size() - _potentialMap_.size());
  }

  /*
   * Returns a constant reference to the VariableNodeMap of this Influence
   * Diagram
   */
  template < typename GUM_SCALAR >
  INLINE const VariableNodeMap& InfluenceDiagram< GUM_SCALAR >::variableNodeMap() const {
    return _variableMap_;
  }

  /*
   * Returns a constant reference over a variable given it's node id.
   */
  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable& InfluenceDiagram< GUM_SCALAR >::variable(NodeId id) const {
    return _variableMap_[id];
  }

  /*
   * Return id node from discrete var pointer.
   */
  template < typename GUM_SCALAR >
  INLINE NodeId InfluenceDiagram< GUM_SCALAR >::nodeId(const DiscreteVariable& var) const {
    return _variableMap_.get(var);
  }

  // Getter by name
  template < typename GUM_SCALAR >
  INLINE NodeId InfluenceDiagram< GUM_SCALAR >::idFromName(const std::string& name) const {
    return _variableMap_.idFromName(name);
  }

  // Getter by name
  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
               InfluenceDiagram< GUM_SCALAR >::variableFromName(const std::string& name) const {
    return _variableMap_.variableFromName(name);
  }

  /*
   * Add a chance variable, it's associate node and it's CPT. The id of the new
   * variable is automatically generated.
   */
  template < typename GUM_SCALAR >
  NodeId InfluenceDiagram< GUM_SCALAR >::add(const DiscreteVariable& var, NodeId varId) {
    return addChanceNode(var, varId);
  }

  /*
   * Add a utility variable, it's associate node and it's UT. The id of the new
   * variable is automatically generated.
   * @Throws : Gum::InvalidArgument if var has more than one state
   */
  template < typename GUM_SCALAR >
  NodeId InfluenceDiagram< GUM_SCALAR >::addUtilityNode(const DiscreteVariable& var, NodeId varId) {
    auto   newMultiDim = new MultiDimArray< GUM_SCALAR >();
    NodeId res;

    try {
      res = addUtilityNode(var, newMultiDim, varId);
    } catch (Exception&) {
      if (newMultiDim != nullptr) delete newMultiDim;
      throw;
    }

    return res;
  }

  /*
   * Add a decision variable. The id of the new
   * variable is automatically generated.
   */
  template < typename GUM_SCALAR >
  NodeId InfluenceDiagram< GUM_SCALAR >::addDecisionNode(const DiscreteVariable& var,
                                                         NodeId                  varId) {
    return addNode_(var, varId);
  }

  /*
   * Add a chance variable, it's associate node and it's CPT. The id of the new
   * variable is automatically generated.
   */
  template < typename GUM_SCALAR >
  NodeId InfluenceDiagram< GUM_SCALAR >::addChanceNode(const DiscreteVariable& var, NodeId varId) {
    auto   newMultiDim = new MultiDimArray< GUM_SCALAR >();
    NodeId res;

    try {
      res = addChanceNode(var, newMultiDim, varId);
    } catch (Exception&) {
      delete newMultiDim;
      throw;
    }

    return res;
  }

  /*
   * Add a chance variable, it's associate node and it's CPT. The id of the new
   * variable is automatically generated.
   */
  template < typename GUM_SCALAR >
  NodeId
     InfluenceDiagram< GUM_SCALAR >::addChanceNode(const DiscreteVariable&               var,
                                                   MultiDimImplementation< GUM_SCALAR >* aContent,
                                                   NodeId DesiredId) {
    NodeId proposedId = addNode_(var, DesiredId);

    auto varcpt = new Potential< GUM_SCALAR >(aContent);
    (*varcpt) << variable(proposedId);
    _potentialMap_.insert(proposedId, varcpt);

    return proposedId;
  }

  /*
   * Add a utility variable, it's associate node and it's UT. The id of the new
   * variable is automatically generated.
   * @Throws : Gum::InvalidArgument if var has more than one state
   */
  template < typename GUM_SCALAR >
  NodeId
     InfluenceDiagram< GUM_SCALAR >::addUtilityNode(const DiscreteVariable&               var,
                                                    MultiDimImplementation< GUM_SCALAR >* aContent,
                                                    NodeId DesiredId) {
    if (var.domainSize() != 1) {
      GUM_ERROR(InvalidArgument,
                "Utility var have no state ( which implicates a "
                "single label for data output reasons ).")
    }

    NodeId proposedId = addNode_(var, DesiredId);

    auto varut = new Potential< GUM_SCALAR >(aContent);

    (*varut) << variable(proposedId);

    _utilityMap_.insert(proposedId, varut);

    return proposedId;
  }

  /*
   * Add a node
   */
  template < typename GUM_SCALAR >
  NodeId InfluenceDiagram< GUM_SCALAR >::addNode_(const DiscreteVariable& variableType,
                                                  NodeId                  DesiredId) {
    // None thread safe code!
    NodeId proposedId;

    if (DesiredId == 0)
      proposedId = dag_.nextNodeId();
    else
      proposedId = DesiredId;

    _variableMap_.insert(proposedId, variableType);

    dag_.addNodeWithId(proposedId);

    // end critical section
    return proposedId;
  }

  /*
   * Erase a Variable from the network and remove the variable from
   * all children of id.
   * If no variable matches the id, then nothing is done.
   */
  template < typename GUM_SCALAR >
  void InfluenceDiagram< GUM_SCALAR >::erase(NodeId varId) {
    if (_variableMap_.exists(varId)) {
      // Reduce the variable child's CPT or Utility Table if necessary
      for (const auto chi: dag_.children(varId))
        if (isChanceNode(chi))
          _potentialMap_[chi]->erase(variable(varId));
        else if (isUtilityNode(chi))
          _utilityMap_[chi]->erase(variable(varId));

      if (isChanceNode(varId)) {
        delete _potentialMap_[varId];
        _potentialMap_.erase(varId);
      } else if (isUtilityNode(varId)) {
        delete _utilityMap_[varId];
        _utilityMap_.erase(varId);
      }

      _variableMap_.erase(varId);
      dag_.eraseNode(varId);
    }
  }

  /*
   * Erase a Variable from the network and remove the variable from
   * all children of var.
   * If no variable matches, then nothing is done.
   */
  template < typename GUM_SCALAR >
  INLINE void InfluenceDiagram< GUM_SCALAR >::erase(const DiscreteVariable& var) {
    erase(_variableMap_.get(var));
  }

  /* we allow the user to change the name of a variable
   */
  template < typename GUM_SCALAR >
  INLINE void InfluenceDiagram< GUM_SCALAR >::changeVariableName(NodeId             id,
                                                                 const std::string& new_name) {
    _variableMap_.changeName(id, new_name);
  }

  // ===========================================================================
  // @name Arc manipulation methods.
  // ===========================================================================
  /*
   * Add an arc in the ID, and update diagram's chance nodes cpt if necessary.
   */
  template < typename GUM_SCALAR >
  INLINE void InfluenceDiagram< GUM_SCALAR >::addArc(NodeId tail, NodeId head) {
    if (isUtilityNode(tail)) { GUM_ERROR(InvalidArc, "Tail cannot be a utility node") }

    dag_.addArc(tail, head);

    if (isChanceNode(head))
      // Add parent in the child's CPT
      (*(_potentialMap_[head])) << variable(tail);
    else if (isUtilityNode(head)) {
      // Add parent in the child's UT
      (*(_utilityMap_[head])) << variable(tail);
    }
  }

  /*
   * Removes an arc in the ID, and update diagram chance nodes cpt if necessary.
   *
   * If (tail, head) doesn't exist, the nothing happens.
   */
  template < typename GUM_SCALAR >
  INLINE void InfluenceDiagram< GUM_SCALAR >::eraseArc(const Arc& arc) {
    if (dag_.existsArc(arc)) {
      NodeId head = arc.head(), tail = arc.tail();
      dag_.eraseArc(arc);

      if (isChanceNode(head))
        // Removes parent in the child's CPT
        (*(_potentialMap_[head])) >> variable(tail);
      else if (isUtilityNode(head))
        // Removes parent in the child's UT
        (*(_utilityMap_[head])) >> variable(tail);
    }
  }

  /*
   * Removes an arc in the ID, and update diagram chance nodes cpt if necessary.
   *
   * If (tail, head) doesn't exist, the nothing happens.
   */
  template < typename GUM_SCALAR >
  INLINE void InfluenceDiagram< GUM_SCALAR >::eraseArc(NodeId tail, NodeId head) {
    eraseArc(Arc(tail, head));
  }

  // ===========================================================================
  // Graphical methods
  // ===========================================================================

  /*
   * The node's id are coherent with the variables and nodes of the topology.
   */
  template < typename GUM_SCALAR >
  void InfluenceDiagram< GUM_SCALAR >::moralGraph_(UndiGraph& graph) const {
    for (const auto node: dag_.nodes())
      if (!isUtilityNode(node)) graph.addNodeWithId(node);

    for (const auto node: dag_.nodes()) {
      if (!isDecisionNode(node))
        for (const auto par: dag_.parents(node)) {
          if (isChanceNode(node)) graph.addEdge(node, par);

          for (const auto par2: dag_.parents(node))
            if (par != par2) graph.addEdge(par, par2);
        }
    }
  }

  /*
   * True if a directed path exist with all decision nodes
   */
  template < typename GUM_SCALAR >
  bool InfluenceDiagram< GUM_SCALAR >::decisionOrderExists() const {
    const Sequence< NodeId >& order = topologicalOrder(true);

    // Finding first decision node
    Sequence< NodeId >::const_iterator orderIter = order.begin();

    while ((orderIter != order.end()) && (!isDecisionNode(*orderIter)))
      ++orderIter;

    if (orderIter == order.end()) return true;

    NodeId parentDecision = (*orderIter);
    ++orderIter;

    // Checking path between decisions nodes
    while (orderIter != order.end()) {
      if (isDecisionNode(*orderIter)) {
        if (!existsPathBetween(parentDecision, *orderIter)) return false;

        parentDecision = *orderIter;
      }

      ++orderIter;
    }

    return true;
  }

  /*
   * Returns true if a path exists between source and destination
   */
  template < typename GUM_SCALAR >
  bool InfluenceDiagram< GUM_SCALAR >::existsPathBetween(NodeId src, NodeId dest) const {
    List< NodeId > nodeFIFO;
    // mark[node] contains 0 if not visited
    // mark[node] = predecessor if visited
    NodeProperty< int > mark = dag_.nodesProperty((int)-1);
    NodeId              current;

    mark[src] = (int)src;
    nodeFIFO.pushBack(src);

    while (!nodeFIFO.empty()) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      for (const auto new_one: dag_.children(current)) {
        if (mark[new_one] != -1) continue;   // if this node is already marked, continue

        mark[new_one] = (int)current;

        if (new_one == dest) break;   // if we reach *orderIter, stop.

        nodeFIFO.pushBack(new_one);
      }
    }

    if (mark[dest] == -1) return false;

    return true;
  }

  /*
   * Returns the decision graph
   */
  template < typename GUM_SCALAR >
  gum::DAG* InfluenceDiagram< GUM_SCALAR >::getDecisionGraph() const {
    auto temporalGraph = new gum::DAG();

    for (const auto node: dag_.nodes()) {
      if (isDecisionNode(node)) {
        if (!temporalGraph->existsNode(node)) temporalGraph->addNodeWithId(node);

        for (const auto chi: getChildrenDecision_(node)) {
          if (!temporalGraph->existsNode(chi)) temporalGraph->addNodeWithId(chi);

          temporalGraph->addArc(node, chi);
        }
      }
    }

    return temporalGraph;
  }

  /*
   * Returns the list of children decision for a given nodeId
   */
  template < typename GUM_SCALAR >
  Sequence< NodeId >
     InfluenceDiagram< GUM_SCALAR >::getChildrenDecision_(NodeId parentDecision) const {
    Sequence< NodeId > childrenSeq;

    List< NodeId > nodeFIFO;
    NodeId         current;

    // mark[node] contains false if not visited
    // mark[node] contains true if visited
    NodeProperty< bool > mark = dag_.nodesProperty(false);

    mark[parentDecision] = true;

    nodeFIFO.pushBack(parentDecision);

    while (!nodeFIFO.empty()) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      for (const auto new_one: dag_.children(current)) {
        if (mark[new_one]) continue;   // if this node is already marked, continue

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
  template < typename GUM_SCALAR >
  std::vector< NodeId > InfluenceDiagram< GUM_SCALAR >::decisionOrder() const {
    if (!decisionOrderExists()) { GUM_ERROR(NotFound, "No decision path exists") }

    std::vector< NodeId > decisionSequence;

    for (const auto elt: topologicalOrder(false))
      if (isDecisionNode(elt)) decisionSequence.push_back(elt);

    return decisionSequence;
  }

  /*
   * Returns partial temporal ordering
   * @throw NotFound if such a sequence does not exist
   */
  template < typename GUM_SCALAR >
  const List< NodeSet >& InfluenceDiagram< GUM_SCALAR >::getPartialTemporalOrder(bool clear) const {
    if (clear) {
      _temporalOrder_.clear();

      std::vector< NodeId > order    = decisionOrder();
      NodeSet               nodeList = dag_.asNodeSet();

      for (auto i: order) {
        NodeSet partialOrderedSet;

        for (const auto par: dag_.parents(i)) {
          if (nodeList.contains(par) && isChanceNode(par)) {
            partialOrderedSet.insert(par);
            nodeList.erase(par);
          }
        }

        if (!partialOrderedSet.empty()) _temporalOrder_.pushFront(partialOrderedSet);

        NodeSet decisionSet;

        decisionSet.insert(i);

        _temporalOrder_.pushFront(decisionSet);
      }

      NodeSet lastSet;   //= new gum::NodeSet();

      for (const auto node: nodeList)
        if (isChanceNode(node)) lastSet.insert(node);

      if (!lastSet.empty()) _temporalOrder_.pushFront(lastSet);
    }

    return _temporalOrder_;
  }
}   // namespace gum
