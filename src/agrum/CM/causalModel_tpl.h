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


/**
 * @file
 * @brief Template implementation of CausalModel/causalModel.h
 */

#pragma once
#include <optional>
#include <sstream>

#include <agrum/CM/causalModel.h>
#include <agrum/CM/doorCriteria.h>

namespace gum {

  // ===============================
  // Constructors with descriptors
  // ===============================

  template < GUM_Numeric GUM_SCALAR >
  CausalModel< GUM_SCALAR >::CausalModel(const BayesNet< GUM_SCALAR >& observationalBN,
                                         const LatentDescriptorVector& latentVarsDescriptor,
                                         bool                          assumeNonSpurious) :
      _observationalBN_(observationalBN), _causalDAG_(observationalBN.dag()) {
    // add each latent (children given as names)
    for (const auto& [latent, children]: latentVarsDescriptor) {
      addLatentVariable(latent, children, assumeNonSpurious);
    }
  }

  /// Copy constructor
  template < GUM_Numeric GUM_SCALAR >
  CausalModel< GUM_SCALAR >::CausalModel(const CausalModel& other) :
      _observationalBN_(other._observationalBN_), _causalDAG_(other._causalDAG_),
      _id2name_(other._id2name_) {
    GUM_CONS_CPY(CausalModel)
  };

  /// Move constructor
  template < GUM_Numeric GUM_SCALAR >
  CausalModel< GUM_SCALAR >::CausalModel(CausalModel&& other) noexcept :
      _observationalBN_(std::move(other._observationalBN_)),
      _causalDAG_(std::move(other._causalDAG_)), _id2name_(std::move(other._id2name_)) {
    GUM_CONS_MOV(CausalModel)
  };

  // ===============================
  // Latent variables
  // ===============================

  template < GUM_Numeric GUM_SCALAR >
  void CausalModel< GUM_SCALAR >::addLatentVariable(
      std::string_view                  latentName,
      const std::vector< std::string >& childrenOfLatent,
      bool                              assumeNonSpurious) {
    std::vector< NodeId > childIds;
    childIds.reserve(childrenOfLatent.size());
    for (const auto& nm: childrenOfLatent) {
      childIds.push_back(idFromName(nm));
    }
    addLatentVariable(latentName, childIds, assumeNonSpurious);
  }

  template < GUM_Numeric GUM_SCALAR >
  void CausalModel< GUM_SCALAR >::addLatentVariable(std::string_view             latentName,
                                                    const std::vector< NodeId >& childrenOfLatent,
                                                    bool assumeNonSpurious) {
    // must have at least 2 children
    if (childrenOfLatent.size() < 2) {
      GUM_ERROR(InvalidArgument, "A latent variable must affect at least 2 children.");
    }

    // create a fresh node id for the latent in the causal DAG
    const NodeId id_latent = _causalDAG_.addNode();

    // record bookkeeping
    _id2name_.insert(id_latent, std::string(latentName));

    // add arcs latent -> each child
    for (const auto& cid: childrenOfLatent) {
      // children are observed nodes: they must already exist in causal DAG (copied from observed
      // BN)
      _causalDAG_.addArc(id_latent, cid);
    }

    // remove any direct causal arc among children unless assumeNonSpurious=true
    if (!assumeNonSpurious) {
      for (auto i = childrenOfLatent.begin(); i != childrenOfLatent.end(); ++i) {
        auto j = i;
        ++j;
        for (auto jj = j; jj != childrenOfLatent.end(); ++jj) {
          const NodeId a = *i;
          const NodeId b = *jj;
          // remove either direction if present
          Arc ab(a, b), ba(b, a);
          if (_causalDAG_.existsArc(ab)) _causalDAG_.eraseArc(ab);
          else if (_causalDAG_.existsArc(ba)) _causalDAG_.eraseArc(ba);
        }
      }
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  bool CausalModel< GUM_SCALAR >::existsArc(NodeId x, NodeId y) const {
    return _causalDAG_.existsArc(Arc(x, y));
  }

  template < GUM_Numeric GUM_SCALAR >
  bool CausalModel< GUM_SCALAR >::existsArc(std::string_view x, std::string_view y) const {
    const NodeId ix = idFromName(x);
    const NodeId iy = idFromName(y);
    return existsArc(ix, iy);
  }

  // ===============================
  // Latent getters
  // ===============================

  template < GUM_Numeric GUM_SCALAR >
  NodeSet CausalModel< GUM_SCALAR >::latentVariablesIds() const {
    NodeSet latentIds;
    for (auto it = _id2name_.begin(); it != _id2name_.end(); ++it) {
      latentIds.insert(it.first());
    }
    return latentIds;
  }

  template < GUM_Numeric GUM_SCALAR >
  Set< std::string > CausalModel< GUM_SCALAR >::latentVariablesNames() const {
    Set< std::string > latentNames;
    for (auto it = _id2name_.begin(); it != _id2name_.end(); ++it) {
      latentNames.insert(it.second());
    }
    return latentNames;
  }

  // ===============================
  // Spurious arc management
  // ===============================

  template < GUM_Numeric GUM_SCALAR >
  void CausalModel< GUM_SCALAR >::assumeSpurious(NodeId x, NodeId y) {
    // Validate arc in observationalBN
    if (!_observationalBN_.dag().existsArc(Arc(x, y))) {
      GUM_ERROR(InvalidArgument,
                "Arc(" + std::to_string(x) + "," + std::to_string(y)
                    + ") not present in observationalBN");
    }
    // Remove arc from causalDAG if present
    if (_causalDAG_.existsArc(Arc(x, y))) { _causalDAG_.eraseArc(Arc(x, y)); }
  }

  template < GUM_Numeric GUM_SCALAR >
  void CausalModel< GUM_SCALAR >::assumeSpurious(std::string_view x, std::string_view y) {
    assumeSpurious(idFromName(x), idFromName(y));
  }

  template < GUM_Numeric GUM_SCALAR >
  void CausalModel< GUM_SCALAR >::assumeNonSpurious(NodeId x, NodeId y) {
    // Validate arc in observationalBN
    if (!_observationalBN_.dag().existsArc(Arc(x, y))) {
      GUM_ERROR(InvalidArgument,
                "Arc(" + std::to_string(x) + "," + std::to_string(y)
                    + ") not present in observationalBN");
    }
    // Add arc to causalDAG if absent
    if (!_causalDAG_.existsArc(Arc(x, y))) { _causalDAG_.addArc(x, y); }
  }

  template < GUM_Numeric GUM_SCALAR >
  void CausalModel< GUM_SCALAR >::assumeNonSpurious(std::string_view x, std::string_view y) {
    assumeNonSpurious(idFromName(x), idFromName(y));
  }

  template < GUM_Numeric GUM_SCALAR >
  bool CausalModel< GUM_SCALAR >::isAssumedSpurious(NodeId x, NodeId y) const {
    return _observationalBN_.dag().existsArc(Arc(x, y)) && !_causalDAG_.existsArc(Arc(x, y));
  }

  template < GUM_Numeric GUM_SCALAR >
  bool CausalModel< GUM_SCALAR >::isAssumedSpurious(std::string_view x, std::string_view y) const {
    return isAssumedSpurious(idFromName(x), idFromName(y));
  }

  // ===============================
  // Connected components
  // ===============================

  // -----------------------------------------------------------------------------
  // Connected components (weak connectivity) on the CAUSAL DAG.
  // This adapts the inner pyAgrum implementation:
  // - Work over _causalDAG_.nodes()
  // - Traverse using parents() and children()
  // - Keep a "remaining" set; pop an arbitrary root for each component
  // -----------------------------------------------------------------------------
  template < GUM_Numeric GUM_SCALAR >
  HashTable< NodeId, NodeSet > CausalModel< GUM_SCALAR >::connectedComponents() const {
    HashTable< NodeId, NodeSet > comps;

    // Build a mutable "remaining" set from the DAG's node view
    NodeSet remaining;
    for (auto n: _causalDAG_.nodes())
      remaining.insert(n);

    while (!remaining.empty()) {
      const NodeId root = *remaining.begin();

      NodeSet               cc;
      std::vector< NodeId > stack{root};
      remaining.erase(root);

      while (!stack.empty()) {
        const NodeId u = stack.back();
        stack.pop_back();
        cc.insert(u);

        // weak neighbours = parents ∪ children
        for (auto v: _causalDAG_.parents(u)) {
          if (remaining.contains(v)) {
            remaining.erase(v);
            stack.push_back(v);
          }
        }
        for (auto v: _causalDAG_.children(u)) {
          if (remaining.contains(v)) {
            remaining.erase(v);
            stack.push_back(v);
          }
        }
      }

      comps.insert(root, std::move(cc));
    }

    return comps;
  }

  // causalModel_tpl.h
  template < GUM_Numeric GUM_SCALAR >
  CausalModel< GUM_SCALAR >
      CausalModel< GUM_SCALAR >::inducedCausalSubModel(const CausalModel< GUM_SCALAR >& cm,
                                                       NodeSet subset) const {
    // If caller gave an empty subset, use "all observed" nodes
    if (subset.empty()) {
      for (auto n: cm.observationalBN().nodes())
        subset.insert(n);
    }

    // Keep only OBSERVED nodes (drop latents explicitly)
    NodeSet nodes = subset;
    for (auto L: cm.latentVariablesIds())
      nodes.erase(L);

    // --- Build an observational BN restricted to `nodes`
    BayesNet< GUM_SCALAR >      bn;
    HashTable< NodeId, NodeId > idmap;   // oldId -> newId

    // add variables
    for (auto n: nodes) {
      const auto&  var = cm.observationalBN().variable(n);
      const NodeId nn  = bn.add(var, n);   // API accepts "add(var, id)"
      idmap.insert(n, nn);
    }

    // add arcs among the kept observed nodes (from the *causal* DAG)
    for (const auto& a: cm.causalDAG().arcs()) {
      const NodeId x = a.tail();
      const NodeId y = a.head();
      if (nodes.contains(x) && nodes.contains(y)) { bn.addArc(idmap[x], idmap[y]); }
    }

    // --- Rebuild latent descriptor restricted to kept nodes
    LatentDescriptorVector latentDesc;
    for (auto L: cm.latentVariablesIds()) {
      std::vector< NodeId > mapped_children;
      mapped_children.reserve(4);

      // intersect latent children with kept observed nodes
      for (auto c: cm.children(L)) {
        if (nodes.contains(c)) mapped_children.push_back(idmap[c]);
      }

      // keep the latent ONLY if it still has at least 2 children
      if (mapped_children.size() >= 2) {
        latentDesc.emplace_back(cm.nameFromId(L), std::move(mapped_children));
      }
    }


    // assumeNonSpurious=true : pyagrum uses True here (do not remove arcs among children)
    return CausalModel< GUM_SCALAR >(bn, latentDesc, /*assumeNonSpurious=*/true);
  }

  // ===============================
  // Parents / Children (by id / by name)
  // ===============================

  template < GUM_Numeric GUM_SCALAR >
  NodeSet CausalModel< GUM_SCALAR >::parents(NodeId x) const {
    // delegate to the causal DAG (includes latent->observed arcs)
    return _causalDAG_.parents(x);
  }

  template < GUM_Numeric GUM_SCALAR >
  NodeSet CausalModel< GUM_SCALAR >::parents(std::string_view name) const {
    return parents(idFromName(name));
  }

  template < GUM_Numeric GUM_SCALAR >
  NodeSet CausalModel< GUM_SCALAR >::children(NodeId x) const {
    return _causalDAG_.children(x);
  }

  template < GUM_Numeric GUM_SCALAR >
  NodeSet CausalModel< GUM_SCALAR >::children(std::string_view name) const {
    return children(idFromName(name));
  }

  /* =======================================================================
   * Backdoor / Frontdoor conveniences (IDs only)
   * ======================================================================= */

  template < GUM_Numeric GUM_SCALAR >
  std::optional< NodeSet > CausalModel< GUM_SCALAR >::backDoor(NodeId cause, NodeId effect) const {
    // Preconditions: cause/effect must be observed (non-latent).
    const NodeSet lat = latentVariablesIds();
    if (lat.contains(cause) || lat.contains(effect)) {
      GUM_ERROR(InvalidArgument,
                "CausalModel::backDoor: 'cause' and 'effect' must be observed (non-latent).");
    }

    const auto candidates = DoorCriteria::enumerateBackdoorSets(_causalDAG_, cause, effect, lat);
    if (candidates.empty()) return std::nullopt;
    return candidates.front();   // first valid set (may itself be empty)
  }

  template < GUM_Numeric GUM_SCALAR >
  std::optional< NodeSet > CausalModel< GUM_SCALAR >::frontDoor(NodeId cause, NodeId effect) const {
    // Preconditions: cause/effect must be observed (non-latent).
    const NodeSet lat = latentVariablesIds();
    if (lat.contains(cause) || lat.contains(effect)) {
      GUM_ERROR(InvalidArgument,
                "CausalModel::frontDoor: 'cause' and 'effect' must be observed (non-latent).");
    }

    const auto candidates = DoorCriteria::enumerateFrontdoorSets(_causalDAG_, cause, effect, lat);
    if (candidates.empty()) return std::nullopt;
    return candidates.front();   // first valid set (may itself be empty)
  }

  // ===============================
  // DOT export
  // ===============================
  template < GUM_Numeric GUM_SCALAR >
  std::string CausalModel< GUM_SCALAR >::toDot(const bool  SHOW_LATENT_NAMES,
                                               const char* NODE_BG,
                                               const char* NODE_FG,
                                               const char* EDGE_COL) const {
    //   SHOW_LATENT_NAMES => causal.show_latent_names
    //   NODE_BG           => causal.default_node_bgcolor
    //   NODE_FG           => causal.default_node_fgcolor
    //   EDGE_COL          => notebook.default_arc_color

    std::stringstream out;
    out << "digraph {\n";

    // First, latent nodes
    for (const auto n: _causalDAG_.nodes()) {
      if (latentVariablesIds().contains(n)) {
        std::string label;
        try {
          label = nameFromId(n);
        } catch (gum::NotFound&) { label = "L" + std::to_string(n); }
        out << "  \"" << label << "\" "
            << "[fillcolor=\"" << NODE_BG << "\", "
            << "fontcolor=\"" << NODE_FG << "\", "
            << "style=filled,shape=" << (SHOW_LATENT_NAMES ? "ellipse" : "point") << "];\n";
      }
    }

    // Then, observed nodes
    for (const auto n: _causalDAG_.nodes()) {
      if (!latentVariablesIds().contains(n)) {
        std::string label;
        try {
          label = nameFromId(n);
        } catch (gum::NotFound&) { label = "L" + std::to_string(n); }
        out << "  \"" << label << "\" "
            << "[fillcolor=\"" << NODE_BG << "\", "
            << "fontcolor=\"" << NODE_FG << "\", "
            << "style=filled,shape=ellipse];\n";
      }
    }

    // Edges
    for (const auto tail: _causalDAG_.nodes()) {
      for (const auto head: _causalDAG_.children(tail)) {
        std::string lt, lh;
        try {
          lt = nameFromId(tail);
        } catch (gum::NotFound&) { lt = "L" + std::to_string(tail); }
        try {
          lh = nameFromId(head);
        } catch (gum::NotFound&) { lh = "L" + std::to_string(head); }

        const bool dashed
            = latentVariablesIds().contains(tail) || latentVariablesIds().contains(head);

        out << "  \"" << lt << "\"->\"" << lh << "\" ";
        if (dashed) {
          out << "[style=dashed]";
        } else {
          // match Python: color="#4A4A4A:#4A4A4A"
          out << "[color=\"" << EDGE_COL << ":" << EDGE_COL << "\"]";
        }
        out << ";\n";
      }
    }

    out << "}\n";
    return out.str();
  }

  // ===============================
  // Names and IDs Getters
  // ===============================

  template < GUM_Numeric GUM_SCALAR >
  Set< std::string > CausalModel< GUM_SCALAR >::names() const {
    Set< std::string > names;
    for (const auto n: _observationalBN_.nodes()) {
      names.insert(_observationalBN_.variable(n).name());
    }
    // latent names (from bijection)
    for (auto it = _id2name_.begin(); it != _id2name_.end(); ++it) {
      names.insert(it.second());
    }
    return names;
  }

  template < GUM_Numeric GUM_SCALAR >
  NodeId CausalModel< GUM_SCALAR >::idFromName(std::string_view name) const {
    if (_id2name_.existsSecond(std::string(name))) {
      return _id2name_.first(std::string(name));
    } else {
      return _observationalBN_.idFromName(name);
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string CausalModel< GUM_SCALAR >::nameFromId(NodeId id) const {
    if (_id2name_.existsFirst(id)) {
      return _id2name_.second(id);
    } else {
      return _observationalBN_.variable(id).name();
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  Bijection< NodeId, std::string >
      CausalModel< GUM_SCALAR >::id2name(bool includeLatentVariables) const {
    Bijection< NodeId, std::string > result;

    // Add observed variables
    for (const auto n: _observationalBN_.nodes()) {
      result.insert(n, _observationalBN_.variable(n).name());
    }

    // Add latent variables if requested
    if (includeLatentVariables) {
      for (auto iter = _id2name_.beginSafe(); iter != _id2name_.endSafe(); ++iter) {
        result.insert(iter.first(), iter.second());
      }
    }

    return result;
  }

}   // namespace gum
