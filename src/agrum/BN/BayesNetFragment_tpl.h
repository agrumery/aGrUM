/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


/**
 * @file
 * @brief Template implementation of BN/BayesNetFragment.h classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <agrum/base/core/dot/dotElements.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFragment.h>

namespace gum {
  template < GUM_Numeric GUM_SCALAR >
  BayesNetFragment< GUM_SCALAR >::BayesNetFragment(const IBayesNet< GUM_SCALAR >& bn) :
      DiGraphListener(&bn.internalDag()), _bn_(bn) {
    GUM_CONSTRUCTOR(BayesNetFragment)
  }

  template < GUM_Numeric GUM_SCALAR >
  BayesNetFragment< GUM_SCALAR >::~BayesNetFragment() {
    GUM_DESTRUCTOR(BayesNetFragment)

    for (auto node: nodes())
      if (_localCPTs_.exists(node)) uninstallCPT_(node);
  }

  //============================================================
  // signals to keep consistency with the referred BayesNet
  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::whenNodeAdded(const void* src, NodeId id) {
    // nothing to do
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::whenNodeDeleted(const void* src, NodeId id) {
    uninstallNode(id);
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::whenArcAdded(const void* src, NodeId from, NodeId to) {
    // nothing to do
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::whenArcDeleted(const void* src, NodeId from, NodeId to) {
    if (this->internalDag().existsArc(from, to)) uninstallArc_(from, to);
  }

  //============================================================
  // IBayesNet interface : BayesNetFragment here is a decorator for the bn

  template < GUM_Numeric GUM_SCALAR >
  const Tensor< GUM_SCALAR >& BayesNetFragment< GUM_SCALAR >::cpt(NodeId id) const {
    if (!isInstalledNode(id)) GUM_ERROR(NotFound, "NodeId " << id << " is not installed")

    if (_localCPTs_.exists(id)) return *_localCPTs_[id];
    else return _bn_.cpt(id);
  }

  template < GUM_Numeric GUM_SCALAR >
  const VariableNodeMap& BayesNetFragment< GUM_SCALAR >::variableNodeMap() const {
    return this->_bn_.variableNodeMap();
  }

  template < GUM_Numeric GUM_SCALAR >
  const DiscreteVariable& BayesNetFragment< GUM_SCALAR >::variable(NodeId id) const {
    if (!isInstalledNode(id)) GUM_ERROR(NotFound, "NodeId " << id << " is not installed")

    return _bn_.variable(id);
  }

  template < GUM_Numeric GUM_SCALAR >
  NodeId BayesNetFragment< GUM_SCALAR >::nodeId(const DiscreteVariable& var) const {
    NodeId id = _bn_.nodeId(var);

    if (!isInstalledNode(id)) GUM_ERROR(NotFound, "variable " << var.name() << " is not installed")

    return id;
  }

  template < GUM_Numeric GUM_SCALAR >
  NodeId BayesNetFragment< GUM_SCALAR >::idFromName(std::string_view name) const {
    NodeId id = _bn_.idFromName(name);

    if (!isInstalledNode(id)) GUM_ERROR(NotFound, "variable " << name << " is not installed")

    return id;
  }

  template < GUM_Numeric GUM_SCALAR >
  const DiscreteVariable&
      BayesNetFragment< GUM_SCALAR >::variableFromName(std::string_view name) const {
    NodeId id = idFromName(name);

    if (!isInstalledNode(id)) GUM_ERROR(NotFound, "variable " << name << " is not installed")

    return _bn_.variable(id);
  }

  //============================================================
  // specific API for BayesNetFragment
  template < GUM_Numeric GUM_SCALAR >
  bool BayesNetFragment< GUM_SCALAR >::isInstalledNode(NodeId id) const {
    return this->internalDag().existsNode(id);
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installNode(NodeId id) {
    if (!_bn_.internalDag().existsNode(id))
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

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installAscendants(NodeId id) {
    installNode(id);

    // bn is a dag => this will have an end ...
    for (auto pa: this->_bn_.parents(id))
      installAscendants(pa);
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::uninstallNode(NodeId id) {
    if (isInstalledNode(id)) {
      uninstallCPT(id);
      this->dag_.eraseNode(id);
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::uninstallArc_(NodeId from, NodeId to) {
    this->dag_.eraseArc(Arc(from, to));
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installArc_(NodeId from, NodeId to) {
    this->dag_.addArc(from, to);
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installCPT_(NodeId id, const Tensor< GUM_SCALAR >& pot) {
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

    _localCPTs_.insert(id, new gum::Tensor< GUM_SCALAR >(pot));
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installCPT(NodeId id, const Tensor< GUM_SCALAR >& pot) {
    if (!this->internalDag().existsNode(id))
      GUM_ERROR(NotFound, "Node " << id << " is not installed in the fragment")

    if (&(pot.variable(0)) != &(variable(id))) {
      GUM_ERROR(OperationNotAllowed,
                "The tensor is not a marginal for  _bn_.variable <" << variable(id).name() << ">")
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

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::uninstallCPT_(NodeId id) {
    delete _localCPTs_[id];
    _localCPTs_.erase(id);
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::uninstallCPT(NodeId id) {
    if (_localCPTs_.exists(id)) {
      uninstallCPT_(id);

      // re-create arcs from referred tensor
      const Tensor< GUM_SCALAR >& pot = cpt(id);

      for (Idx i = 1; i < pot.nbrDim(); i++) {
        NodeId parent = _bn_.idFromName(pot.variable(i).name());

        if (isInstalledNode(parent)) installArc_(parent, id);
      }
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installMarginal(NodeId id, const Tensor< GUM_SCALAR >& pot) {
    if (!isInstalledNode(id)) {
      GUM_ERROR(NotFound, "The node " << id << " is not part of this fragment")
    }

    if (pot.nbrDim() > 1) {
      GUM_ERROR(OperationNotAllowed, "The tensor is not a marginal :" << pot)
    }

    if (&(pot.variable(0)) != &(_bn_.variable(id))) {
      GUM_ERROR(OperationNotAllowed,
                "The tensor is not a marginal for  _bn_.variable <" << _bn_.variable(id).name()
                                                                    << ">")
    }

    installCPT_(id, pot);
  }

  template < GUM_Numeric GUM_SCALAR >
  bool BayesNetFragment< GUM_SCALAR >::checkConsistency(NodeId id) const {
    if (!isInstalledNode(id))
      GUM_ERROR(NotFound, "The node " << id << " is not part of this fragment")

    const auto& node_cpt = this->cpt(id);
    NodeSet     cpt_parents;

    for (Idx i = 1; i < node_cpt.nbrDim(); i++) {
      cpt_parents.insert(_bn_.idFromName(node_cpt.variable(i).name()));
    }

    return (this->parents(id) == cpt_parents);
  }

  template < GUM_Numeric GUM_SCALAR >
  bool BayesNetFragment< GUM_SCALAR >::checkConsistency() const {
    for (auto node: nodes())
      if (!checkConsistency(node)) return false;

    return true;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string BayesNetFragment< GUM_SCALAR >::toDot(const dot::DotOptions& options) const {
    // the colors of the nodes, which tell what they are for the fragment
    struct NodeColors {
      const char* fill;
      const char* border;
      const char* font;
    };

    static constexpr NodeColors inFragment{"#ffffaa", "#000000", "#000000"};
    static constexpr NodeColors withLocalCPT{"#ffddaa", "#000000", "#000000"};
    static constexpr NodeColors notConsistent{"#ff0000", "#000000", "#ffff00"};
    static constexpr NodeColors outFragment{"#f0f0f0", "#f0f0f0", "#000000"};

    const auto     style = dot::DotStyle::read();
    const auto     title = "Fragment of " + _bn_.propertyWithDefault("name", "no_name");
    dot::DotWriter writer(true, title);
    dot::writePreamble(writer, style, options, title);

    for (const auto node: _bn_.nodes()) {
      const NodeColors* colors = &outFragment;
      if (isInstalledNode(node)) {
        if (!checkConsistency(node)) colors = &notConsistent;
        else if (_localCPTs_.exists(node)) colors = &withLocalCPT;
        else colors = &inFragment;
      }

      const auto& name       = _bn_.variable(node).name();
      auto        attributes = dot::nodeAttributes(style,
                                                   options.node(node),
                                                   "",
                                                   "(" + std::to_string(node) + ") " + name);
      dot::addDefaultAttribute(attributes, "fontcolor", colors->font);
      dot::addDefaultAttribute(attributes, "color", colors->border);
      dot::addDefaultAttribute(attributes, "fillcolor", colors->fill);
      writer.node(name, attributes);
    }

    for (const auto node: _bn_.nodes()) {
      for (const auto child: _bn_.children(node)) {
        auto attributes = dot::arcAttributes(options.arc(node, child));
        dot::addDefaultAttribute(attributes,
                                 "tooltip",
                                 std::to_string(node) + "&nbsp;&rarr;&nbsp;"
                                     + std::to_string(child));
        dot::addDefaultAttribute(attributes, "fontsize", "10");
        dot::addDefaultAttribute(attributes,
                                 "color",
                                 this->internalDag().existsArc(Arc(node, child)) ? "#000000"
                                                                                 : "#f0f0f0");
        writer.edge(_bn_.variable(node).name(), _bn_.variable(child).name(), attributes);
      }
    }

    return writer.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  gum::BayesNet< GUM_SCALAR > BayesNetFragment< GUM_SCALAR >::toBN() const {
    if (!checkConsistency()) {
      GUM_ERROR(OperationNotAllowed, "The fragment contains un-consistent node(s)")
    }
    gum::BayesNet< GUM_SCALAR > res;
    for (const auto nod: nodes()) {
      res.add(variable(nod), nod);
    }
    for (const auto& arc: this->internalDag().arcs()) {
      res.addArc(arc.tail(), arc.head());
    }
    for (const auto nod: nodes()) {
      res.cpt(nod).fillWith(cpt(nod));
    }

    return res;
  }

  template < GUM_Numeric GUM_SCALAR >
  const Tensor< GUM_SCALAR >& BayesNetFragment< GUM_SCALAR >::cpt(std::string_view name) const {
    return cpt(idFromName(name));
  }

  template < GUM_Numeric GUM_SCALAR >
  const DiscreteVariable& BayesNetFragment< GUM_SCALAR >::variable(std::string_view name) const {
    return variable(idFromName(name));
  }

  template < GUM_Numeric GUM_SCALAR >
  bool BayesNetFragment< GUM_SCALAR >::isInstalledNode(std::string_view name) const {
    return isInstalledNode(idFromName(name));
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installNode(std::string_view name) {
    installNode(_bn_.idFromName(name));
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installAscendants(std::string_view name) {
    installAscendants(_bn_.idFromName(name));
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::uninstallNode(std::string_view name) {
    uninstallNode(idFromName(name));
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installMarginal(std::string_view            name,
                                                       const Tensor< GUM_SCALAR >& pot) {
    installMarginal(_bn_.idFromName(name), pot);
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::installCPT(std::string_view            name,
                                                  const Tensor< GUM_SCALAR >& pot) {
    installCPT(_bn_.idFromName(name), pot);
  }

  template < GUM_Numeric GUM_SCALAR >
  void BayesNetFragment< GUM_SCALAR >::uninstallCPT(std::string_view name) {
    uninstallCPT(idFromName(name));
  }

  template < GUM_Numeric GUM_SCALAR >
  bool BayesNetFragment< GUM_SCALAR >::checkConsistency(std::string_view name) const {
    return checkConsistency(idFromName(name));
  }

}   // namespace gum
