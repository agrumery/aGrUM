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
 * @brief Template implementation of BN/BayesNetFragment.h classes.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#include <agrum/BN/BayesNetFragment.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/multidim/potential.h>

namespace gum {
  template <typename GUM_SCALAR>
  BayesNetFragment<GUM_SCALAR>::BayesNetFragment(const BayesNet<GUM_SCALAR> &bn)
      : DiGraphListener(&bn.dag()), __bn(bn) {
    GUM_CONSTRUCTOR(BayesNetFragment);
  }

  template <typename GUM_SCALAR> BayesNetFragment<GUM_SCALAR>::~BayesNetFragment() {
    GUM_DESTRUCTOR(BayesNetFragment);

    for (auto node : nodes())
      if (__localCPTs.exists(node))
        _uninstallCPT(node);
  }

  //============================================================
  // signals to keep consistency with the referred BayesNet
  template <typename GUM_SCALAR>
  INLINE void BayesNetFragment<GUM_SCALAR>::whenNodeAdded(const void *src,
                                                          NodeId id) noexcept {
    // nothing to do
  }
  template <typename GUM_SCALAR>
  INLINE void BayesNetFragment<GUM_SCALAR>::whenNodeDeleted(const void *src,
                                                            NodeId id) noexcept {
    uninstallNode(id);
  }
  template <typename GUM_SCALAR>
  INLINE void BayesNetFragment<GUM_SCALAR>::whenArcAdded(const void *src,
                                                         NodeId from,
                                                         NodeId to) noexcept {
    // nothing to do
  }
  template <typename GUM_SCALAR>
  INLINE void BayesNetFragment<GUM_SCALAR>::whenArcDeleted(const void *src,
                                                           NodeId from,
                                                           NodeId to) noexcept {
    if (dag().existsArc(from, to))
      _uninstallArc(from, to);
  }

  //============================================================
  // IBayesNet interface : BayesNetFragment here is a decorator for the bn

  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR> &
  BayesNetFragment<GUM_SCALAR>::cpt(NodeId id) const {
    if (!isInstalledNode(id))
      GUM_ERROR(NotFound, id << " is not installed");

    if (__localCPTs.exists(id))
      return *__localCPTs[id];
    else
      return __bn.cpt(id);
  }

  template <typename GUM_SCALAR>
  INLINE const VariableNodeMap &
  BayesNetFragment<GUM_SCALAR>::variableNodeMap() const {
    GUM_ERROR(FatalError,
              "Not implemented yet. please use referent bayesnet method");
  }

  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable &
  BayesNetFragment<GUM_SCALAR>::variable(NodeId id) const {
    if (!isInstalledNode(id))
      GUM_ERROR(NotFound, id << " is not installed");

    return __bn.variable(id);
  }

  template <typename GUM_SCALAR>
  INLINE NodeId
      BayesNetFragment<GUM_SCALAR>::nodeId(const DiscreteVariable &var) const {
    NodeId id = __bn.nodeId(var);

    if (!isInstalledNode(id))
      GUM_ERROR(NotFound, "variable " << var.name() << " is not installed");

    return id;
  }

  template <typename GUM_SCALAR>
  INLINE NodeId
      BayesNetFragment<GUM_SCALAR>::idFromName(const std::string &name) const {
    NodeId id = __bn.idFromName(name);

    if (!isInstalledNode(id))
      GUM_ERROR(NotFound, "variable " << name << " is not installed");

    return id;
  }

  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable &
  BayesNetFragment<GUM_SCALAR>::variableFromName(const std::string &name) const {
    NodeId id = __bn.idFromName(name);

    if (!isInstalledNode(id))
      GUM_ERROR(NotFound, "variable " << name << " is not installed");

    return __bn.variable(id);
  }

  //============================================================
  // specific API for BayesNetFragment
  template <typename GUM_SCALAR>
  INLINE bool BayesNetFragment<GUM_SCALAR>::isInstalledNode(NodeId id) const
      noexcept {
    return dag().existsNode(id);
  }

  template <typename GUM_SCALAR>
  void BayesNetFragment<GUM_SCALAR>::installNode(NodeId id) {
    if (!__bn.dag().existsNode(id))
      GUM_ERROR(NotFound, "Node " << id << " does not exist in referred BayesNet");

    if (!isInstalledNode(id)) {
      this->_dag.addNode(id);

      // adding arcs with id as a tail
      for (auto pa : this->__bn.dag().parents(id)) {
        if (isInstalledNode(pa))
          this->_dag.addArc(pa, id);
      }

      // addin arcs with id as a head
      for (auto son : this->__bn.dag().children(id))
        if (isInstalledNode(son))
          this->_dag.addArc(id, son);
    }
  }

  template <typename GUM_SCALAR>
  INLINE void BayesNetFragment<GUM_SCALAR>::installAscendants(NodeId id) {
    installNode(id);

    // bn is a dag => this will have an end ...
    for (auto pa : this->__bn.dag().parents(id))
      installAscendants(pa);
  }

  template <typename GUM_SCALAR>
  INLINE void BayesNetFragment<GUM_SCALAR>::uninstallNode(NodeId id) noexcept {
    if (isInstalledNode(id)) {
      this->_dag.eraseNode(id);
      uninstallCPT(id);
    }
  }

  template <typename GUM_SCALAR>
  INLINE void BayesNetFragment<GUM_SCALAR>::_uninstallArc(NodeId from,
                                                          NodeId to) noexcept {
    this->_dag.eraseArc(Arc(from, to));
  }

  template <typename GUM_SCALAR>
  INLINE void BayesNetFragment<GUM_SCALAR>::_installArc(NodeId from,
                                                        NodeId to) noexcept {
    this->_dag.addArc(from, to);
  }

  template <typename GUM_SCALAR>
  void BayesNetFragment<GUM_SCALAR>::_installCPT(
      NodeId id, const Potential<GUM_SCALAR> *pot) noexcept {
    // topology
    const auto &parents = dag().parents(id);
    for (auto node_it = parents.beginSafe(); node_it != parents.endSafe();
         ++node_it) // safe iterator needed here
      _uninstallArc(*node_it, id);

    for (Idx i = 1; i < pot->nbrDim(); i++) {
      NodeId parent = __bn.idFromName(pot->variable(i).name());

      if (isInstalledNode(parent))
        _installArc(parent, id);
    }

    // local cpt
    if (__localCPTs.exists(id))
      _uninstallCPT(id);

    __localCPTs.insert(id, pot);
  }

  template <typename GUM_SCALAR>
  void BayesNetFragment<GUM_SCALAR>::installCPT(NodeId id,
                                                const Potential<GUM_SCALAR> *pot) {
    if (!dag().existsNode(id))
      GUM_ERROR(NotFound, "Node " << id << " is not installed in the fragment");

    if (&(pot->variable(0)) != &(variable(id))) {
      GUM_ERROR(OperationNotAllowed,
                "The potential is not a marginal for __bn.variable <"
                    << variable(id).name() << ">");
    }

    const NodeSet &parents = __bn.dag().parents(id);

    for (Idx i = 1; i < pot->nbrDim(); i++) {
      if (!parents.contains(__bn.idFromName(pot->variable(i).name())))
        GUM_ERROR(OperationNotAllowed,
                  "Variable <" << pot->variable(i).name()
                               << "> is not in the parents of node " << id);
    }

    _installCPT(id, pot);
  }

  template <typename GUM_SCALAR>
  INLINE void BayesNetFragment<GUM_SCALAR>::_uninstallCPT(NodeId id) noexcept {
    delete __localCPTs[id];
    __localCPTs.erase(id);
  }

  template <typename GUM_SCALAR>
  INLINE void BayesNetFragment<GUM_SCALAR>::uninstallCPT(NodeId id) noexcept {
    if (__localCPTs.exists(id)) {
      _uninstallCPT(id);

      // re-create arcs from referred potential
      const Potential<GUM_SCALAR> &pot = cpt(id);

      for (Idx i = 1; i < pot.nbrDim(); i++) {
        NodeId parent = __bn.idFromName(pot.variable(i).name());

        if (isInstalledNode(parent))
          _installArc(parent, id);
      }
    }
  }

  template <typename GUM_SCALAR>
  void
  BayesNetFragment<GUM_SCALAR>::installMarginal(NodeId id,
                                                const Potential<GUM_SCALAR> *pot) {
    if (!isInstalledNode(id)) {
      GUM_ERROR(NotFound, "The node " << id << " is not part of this fragment");
    }

    if (pot->nbrDim() > 1) {
      GUM_ERROR(OperationNotAllowed, "The potential is not a marginal :" << pot);
    }

    if (&(pot->variable(0)) != &(__bn.variable(id))) {
      GUM_ERROR(OperationNotAllowed,
                "The potential is not a marginal for __bn.variable <"
                    << __bn.variable(id).name() << ">");
    }

    _installCPT(id, pot);
  }

  template <typename GUM_SCALAR>
  bool BayesNetFragment<GUM_SCALAR>::checkConsistency(NodeId id) const {
    if (!isInstalledNode(id))
      GUM_ERROR(NotFound, "The node " << id << " is not part of this fragment");

    const auto &parents = dag().parents(id);
    const auto &cpt = this->cpt(id);
    NodeSet cpt_parents;

    for (Idx i = 1; i < cpt.nbrDim(); i++) {
      cpt_parents.insert(__bn.idFromName(cpt.variable(i).name()));
    }

    return (parents == cpt_parents);
  }

  template <typename GUM_SCALAR>
  INLINE bool BayesNetFragment<GUM_SCALAR>::checkConsistency() const noexcept {
    for (auto node : nodes())
      if (!checkConsistency(node))
        return false;

    return true;
  }

  template <typename GUM_SCALAR>
  std::string BayesNetFragment<GUM_SCALAR>::toDot(void) const {
    std::stringstream output;
    output << "digraph \"";

    std::string bn_name;

    static std::string inFragmentStyle = "fillcolor=\"#ffffaa\","
                                         "color=\"#000000\","
                                         "fontcolor=\"#000000\"";
    static std::string styleWithLocalCPT = "fillcolor=\"#ffddaa\","
                                           "color=\"#000000\","
                                           "fontcolor=\"#000000\"";
    static std::string notConsistantStyle = "fillcolor=\"#ff0000\","
                                            "color=\"#000000\","
                                            "fontcolor=\"#ffff00\"";
    static std::string outFragmentStyle = "fillcolor=\"#f0f0f0\","
                                          "color=\"#f0f0f0\","
                                          "fontcolor=\"#000000\"";

    try {
      bn_name = __bn.property("name");
    } catch (NotFound &) {
      bn_name = "no_name";
    }

    bn_name = "Fragment of " + bn_name;

    output << bn_name << "\" {" << std::endl;
    output << "  graph [bgcolor=transparent,label=\"" << bn_name << "\"];"
           << std::endl;
    output << "  node [style=filled];" << std::endl << std::endl;

    for (auto node : __bn.nodes()) {
      output << "\"" << __bn.variable(node).name() << "\" [comment=\"" << node << ":"
             << __bn.variable(node) << ", \"";

      if (isInstalledNode(node)) {
        if (!checkConsistency(node)) {
          output << notConsistantStyle;
        } else if (__localCPTs.exists(node))
          output << styleWithLocalCPT;
        else
          output << inFragmentStyle;
      } else
        output << outFragmentStyle;

      output << "];" << std::endl;
    }

    output << std::endl;

    std::string tab = "  ";

    for (auto node : __bn.nodes()) {
      if (__bn.dag().children(node).size() > 0) {
        for (auto child : __bn.dag().children(node)) {
          output << tab << "\"" << __bn.variable(node).name() << "\" -> "
                 << "\"" << __bn.variable(child).name() << "\" [";

          if (dag().existsArc(Arc(node, child)))
            output << inFragmentStyle;
          else
            output << outFragmentStyle;

          output << "];" << std::endl;
        }
      }
    }

    output << "}" << std::endl;

    return output.str();
  }
} // gum
