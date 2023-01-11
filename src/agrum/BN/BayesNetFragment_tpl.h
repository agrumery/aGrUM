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
 * @brief Template implementation of BN/BayesNetFragment.h classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFragment.h>
#include <agrum/tools/multidim/potential.h>

namespace gum {
  template < typename GUM_SCALAR >
  BayesNetFragment< GUM_SCALAR >::BayesNetFragment(const IBayesNet< GUM_SCALAR >& bn) :
      DiGraphListener(&bn.dag()), _bn_(bn) {
    GUM_CONSTRUCTOR(BayesNetFragment)
  }

  template < typename GUM_SCALAR >
  BayesNetFragment< GUM_SCALAR >::~BayesNetFragment() {
    GUM_DESTRUCTOR(BayesNetFragment)

    for (auto node: nodes())
      if (_localCPTs_.exists(node)) uninstallCPT_(node);
  }

  //============================================================
  // signals to keep consistency with the referred BayesNet
  template < typename GUM_SCALAR >
  INLINE void BayesNetFragment< GUM_SCALAR >::whenNodeAdded(const void* src, NodeId id) {
    // nothing to do
  }
  template < typename GUM_SCALAR >
  INLINE void BayesNetFragment< GUM_SCALAR >::whenNodeDeleted(const void* src, NodeId id) {
    uninstallNode(id);
  }
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFragment< GUM_SCALAR >::whenArcAdded(const void* src, NodeId from, NodeId to) {
    // nothing to do
  }
  template < typename GUM_SCALAR >
  INLINE void
     BayesNetFragment< GUM_SCALAR >::whenArcDeleted(const void* src, NodeId from, NodeId to) {
    if (dag().existsArc(from, to)) uninstallArc_(from, to);
  }

  //============================================================
  // IBayesNet interface : BayesNetFragment here is a decorator for the bn

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >& BayesNetFragment< GUM_SCALAR >::cpt(NodeId id) const {
    if (!isInstalledNode(id)) GUM_ERROR(NotFound, "NodeId " << id << " is not installed")

    if (_localCPTs_.exists(id)) return *_localCPTs_[id];
    else return _bn_.cpt(id);
  }

  template < typename GUM_SCALAR >
  INLINE const VariableNodeMap& BayesNetFragment< GUM_SCALAR >::variableNodeMap() const {
    return this->_bn_.variableNodeMap();
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable& BayesNetFragment< GUM_SCALAR >::variable(NodeId id) const {
    if (!isInstalledNode(id)) GUM_ERROR(NotFound, "NodeId " << id << " is not installed")

    return _bn_.variable(id);
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNetFragment< GUM_SCALAR >::nodeId(const DiscreteVariable& var) const {
    NodeId id = _bn_.nodeId(var);

    if (!isInstalledNode(id)) GUM_ERROR(NotFound, "variable " << var.name() << " is not installed")

    return id;
  }

  template < typename GUM_SCALAR >
  INLINE NodeId BayesNetFragment< GUM_SCALAR >::idFromName(const std::string& name) const {
    NodeId id = _bn_.idFromName(name);

    if (!isInstalledNode(id)) GUM_ERROR(NotFound, "variable " << name << " is not installed")

    return id;
  }

  template < typename GUM_SCALAR >
  INLINE const DiscreteVariable&
     BayesNetFragment< GUM_SCALAR >::variableFromName(const std::string& name) const {
    NodeId id = idFromName(name);

    if (!isInstalledNode(id)) GUM_ERROR(NotFound, "variable " << name << " is not installed")

    return _bn_.variable(id);
  }

  //============================================================
  // specific API for BayesNetFragment
  template < typename GUM_SCALAR >
  INLINE bool BayesNetFragment< GUM_SCALAR >::isInstalledNode(NodeId id) const {
    return dag().existsNode(id);
  }

  template < typename GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installNode(NodeId id) {
    if (!_bn_.dag().existsNode(id))
      GUM_ERROR(NotFound, "Node " << id << " does not exist in referred BayesNet")

    if (!isInstalledNode(id)) {
      this->dag_.addNodeWithId(id);

      // adding arcs with id as a tail
      for (auto pa: this->_bn_.parents(id)) {
        if (isInstalledNode(pa)) this->dag_.addArc(pa, id);
      }

      // adding arcs with id as a head
      for (auto son: this->_bn_.children(id))
        if (isInstalledNode(son)) this->dag_.addArc(id, son);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFragment< GUM_SCALAR >::installAscendants(NodeId id) {
    installNode(id);

    // bn is a dag => this will have an end ...
    for (auto pa: this->_bn_.parents(id))
      installAscendants(pa);
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFragment< GUM_SCALAR >::uninstallNode(NodeId id) {
    if (isInstalledNode(id)) {
      uninstallCPT(id);
      this->dag_.eraseNode(id);
    }
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFragment< GUM_SCALAR >::uninstallArc_(NodeId from, NodeId to) {
    this->dag_.eraseArc(Arc(from, to));
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFragment< GUM_SCALAR >::installArc_(NodeId from, NodeId to) {
    this->dag_.addArc(from, to);
  }

  template < typename GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installCPT_(NodeId id, const Potential< GUM_SCALAR >& pot) {
    // topology
    const auto& parents = this->parents(id);
    for (auto node_it = parents.beginSafe(); node_it != parents.endSafe();
         ++node_it)   // safe iterator needed here
      uninstallArc_(*node_it, id);

    for (Idx i = 1; i < pot.nbrDim(); i++) {
      NodeId parent = _bn_.idFromName(pot.variable(i).name());

      if (isInstalledNode(parent)) installArc_(parent, id);
    }

    // local cpt
    if (_localCPTs_.exists(id)) uninstallCPT_(id);

    _localCPTs_.insert(id, new gum::Potential< GUM_SCALAR >(pot));
  }

  template < typename GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installCPT(NodeId id, const Potential< GUM_SCALAR >& pot) {
    if (!dag().existsNode(id))
      GUM_ERROR(NotFound, "Node " << id << " is not installed in the fragment")

    if (&(pot.variable(0)) != &(variable(id))) {
      GUM_ERROR(OperationNotAllowed,
                "The potential is not a marginal for  _bn_.variable <" << variable(id).name()
                                                                       << ">")
    }

    const NodeSet& parents = _bn_.parents(id);

    for (Idx i = 1; i < pot.nbrDim(); i++) {
      if (!parents.contains(_bn_.idFromName(pot.variable(i).name())))
        GUM_ERROR(OperationNotAllowed,
                  "Variable <" << pot.variable(i).name() << "> is not in the parents of node "
                               << id)
    }

    installCPT_(id, pot);
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFragment< GUM_SCALAR >::uninstallCPT_(NodeId id) {
    delete _localCPTs_[id];
    _localCPTs_.erase(id);
  }

  template < typename GUM_SCALAR >
  INLINE void BayesNetFragment< GUM_SCALAR >::uninstallCPT(NodeId id) {
    if (_localCPTs_.exists(id)) {
      uninstallCPT_(id);

      // re-create arcs from referred potential
      const Potential< GUM_SCALAR >& pot = cpt(id);

      for (Idx i = 1; i < pot.nbrDim(); i++) {
        NodeId parent = _bn_.idFromName(pot.variable(i).name());

        if (isInstalledNode(parent)) installArc_(parent, id);
      }
    }
  }

  template < typename GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installMarginal(NodeId                         id,
                                                       const Potential< GUM_SCALAR >& pot) {
    if (!isInstalledNode(id)) {
      GUM_ERROR(NotFound, "The node " << id << " is not part of this fragment")
    }

    if (pot.nbrDim() > 1) {
      GUM_ERROR(OperationNotAllowed, "The potential is not a marginal :" << pot)
    }

    if (&(pot.variable(0)) != &(_bn_.variable(id))) {
      GUM_ERROR(OperationNotAllowed,
                "The potential is not a marginal for  _bn_.variable <" << _bn_.variable(id).name()
                                                                       << ">")
    }

    installCPT_(id, pot);
  }

  template < typename GUM_SCALAR >
  bool BayesNetFragment< GUM_SCALAR >::checkConsistency(NodeId id) const {
    if (!isInstalledNode(id))
      GUM_ERROR(NotFound, "The node " << id << " is not part of this fragment")

    const auto& cpt = this->cpt(id);
    NodeSet     cpt_parents;

    for (Idx i = 1; i < cpt.nbrDim(); i++) {
      cpt_parents.insert(_bn_.idFromName(cpt.variable(i).name()));
    }

    return (this->parents(id) == cpt_parents);
  }

  template < typename GUM_SCALAR >
  INLINE bool BayesNetFragment< GUM_SCALAR >::checkConsistency() const {
    for (auto node: nodes())
      if (!checkConsistency(node)) return false;

    return true;
  }

  template < typename GUM_SCALAR >
  std::string BayesNetFragment< GUM_SCALAR >::toDot() const {
    std::stringstream output;
    output << "digraph \"";

    std::string bn_name;

    static std::string inFragmentStyle    = "fillcolor=\"#ffffaa\","
                                            "color=\"#000000\","
                                            "fontcolor=\"#000000\"";
    static std::string styleWithLocalCPT  = "fillcolor=\"#ffddaa\","
                                            "color=\"#000000\","
                                            "fontcolor=\"#000000\"";
    static std::string notConsistantStyle = "fillcolor=\"#ff0000\","
                                            "color=\"#000000\","
                                            "fontcolor=\"#ffff00\"";
    static std::string outFragmentStyle   = "fillcolor=\"#f0f0f0\","
                                            "color=\"#f0f0f0\","
                                            "fontcolor=\"#000000\"";

    try {
      bn_name = _bn_.property("name");
    } catch (NotFound const&) { bn_name = "no_name"; }

    bn_name = "Fragment of " + bn_name;

    output << bn_name << "\" {" << std::endl;
    output << "  graph [bgcolor=transparent,label=\"" << bn_name << "\"];" << std::endl;
    output << "  node [style=filled];" << std::endl << std::endl;

    for (auto node: _bn_.nodes()) {
      output << "\"" << _bn_.variable(node).name() << "\" [comment=\"" << node << ":"
             << _bn_.variable(node) << ", \"";

      if (isInstalledNode(node)) {
        if (!checkConsistency(node)) {
          output << notConsistantStyle;
        } else if (_localCPTs_.exists(node)) output << styleWithLocalCPT;
        else output << inFragmentStyle;
      } else output << outFragmentStyle;

      output << "];" << std::endl;
    }

    output << std::endl;

    std::string tab = "  ";

    for (auto node: _bn_.nodes()) {
      if (_bn_.children(node).size() > 0) {
        for (auto child: _bn_.children(node)) {
          output << tab << "\"" << _bn_.variable(node).name() << "\" -> "
                 << "\"" << _bn_.variable(child).name() << "\" [";

          if (dag().existsArc(Arc(node, child))) output << inFragmentStyle;
          else output << outFragmentStyle;

          output << "];" << std::endl;
        }
      }
    }

    output << "}" << std::endl;

    return output.str();
  }

  template < typename GUM_SCALAR >
  gum::BayesNet< GUM_SCALAR > BayesNetFragment< GUM_SCALAR >::toBN() const {
    if (!checkConsistency()) {
      GUM_ERROR(OperationNotAllowed, "The fragment contains un-consistent node(s)")
    }
    gum::BayesNet< GUM_SCALAR > res;
    for (const auto nod: nodes()) {
      res.add(variable(nod), nod);
    }
    for (const auto& arc: dag().arcs()) {
      res.addArc(arc.tail(), arc.head());
    }
    for (const auto nod: nodes()) {
      res.cpt(nod).fillWith(cpt(nod));
    }

    return res;
  }
}   // namespace gum
