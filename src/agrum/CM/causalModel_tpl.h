/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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

#include <sstream>

#include <agrum/CM/causalModel.h>

namespace gum {

// ===============================
// Constructors with descriptors
// ===============================

template <typename GUM_SCALAR>
CausalModel<GUM_SCALAR>::CausalModel(const BayesNet<GUM_SCALAR>& observedBN,
                                     const LatentDescriptorVector& latentVarsDescriptor,
                                     bool keepArcs)
  : _observedBN_(observedBN),
    _causalDAG_(observedBN.dag()) {

  // add each latent (children given as names)
  for (const auto& desc : latentVarsDescriptor) {
      addLatentVariable(desc.first, desc.second, keepArcs);
  }
}


// ===============================
// Latent variables
// ===============================

template <typename GUM_SCALAR>
void CausalModel<GUM_SCALAR>::addLatentVariable(const std::string& latentName,
                                                const std::vector<std::string>& childrenOfLatent,
                                                bool keepArcs) {
  std::vector<NodeId> childIds;
  childIds.reserve(childrenOfLatent.size());
  for (const auto& nm : childrenOfLatent) {
    childIds.push_back(idFromName(nm));
  }
  addLatentVariable(latentName, childIds, keepArcs);
}

template <typename GUM_SCALAR>
void CausalModel<GUM_SCALAR>::addLatentVariable(const std::string& latentName,
                                                const std::vector<NodeId>& childrenOfLatent,
                                                bool keepArcs) {
  // must have at least 2 children
  if (childrenOfLatent.size() < 2) {
    GUM_ERROR(InvalidArgument, "A latent variable must affect at least 2 children.");
  }

  // create a fresh node id for the latent in the causal DAG
  const NodeId id_latent = _causalDAG_.addNode();

  // record bookkeeping
  _id2name_.insert(id_latent, latentName);

  // add arcs latent -> each child
  for (const auto& cid : childrenOfLatent) {
    // children are observed nodes: they must already exist in causal DAG (copied from observed BN)
    _causalDAG_.addArc(id_latent, cid);
  }

  // remove any direct causal arc among children unless keepArcs=true
  if (!keepArcs) {
    for (auto i = childrenOfLatent.begin(); i != childrenOfLatent.end(); ++i) {
      auto j = i; ++j;
      for (auto jj = j; jj != childrenOfLatent.end(); ++jj) {
        const NodeId a = *i;
        const NodeId b = *jj;
        // remove either direction if present
        Arc ab(a,b), ba(b,a);
        if (_causalDAG_.existsArc(ab)) _causalDAG_.eraseArc(ab);
        else if (_causalDAG_.existsArc(ba)) _causalDAG_.eraseArc(ba);
      }
    }
  }
}

// ===============================
// Arc editing
// ===============================

template <typename GUM_SCALAR>
void CausalModel<GUM_SCALAR>::addCausalArc(NodeId x, NodeId y) {
  _causalDAG_.addArc(x, y);
}

template <typename GUM_SCALAR>
void CausalModel<GUM_SCALAR>::addCausalArc(const std::string& x, const std::string& y) {
  // names are resolved in the observed BN (same as pyAgrum)
  const NodeId ix = idFromName(x);
  const NodeId iy = idFromName(y);
  addCausalArc(ix, iy);
}

template <typename GUM_SCALAR>
void CausalModel<GUM_SCALAR>::eraseCausalArc(NodeId x, NodeId y) {
  const Arc a(x, y);
  if (_causalDAG_.existsArc(a)) _causalDAG_.eraseArc(a);
}

template <typename GUM_SCALAR>
void CausalModel<GUM_SCALAR>::eraseCausalArc(const std::string& x, const std::string& y) {
  const NodeId ix = idFromName(x);
  const NodeId iy = idFromName(y);
  eraseCausalArc(ix, iy);
}

template <typename GUM_SCALAR>
bool CausalModel<GUM_SCALAR>::existsArc(NodeId x, NodeId y) const {
  return _causalDAG_.existsArc(Arc(x, y));
}

template <typename GUM_SCALAR>
bool CausalModel<GUM_SCALAR>::existsArc(const std::string& x, const std::string& y) const {
  const NodeId ix = idFromName(x);
  const NodeId iy = idFromName(y);
  return existsArc(ix, iy);
}

// ===============================
// Latent getters
// ===============================

template <typename GUM_SCALAR>
NodeSet CausalModel<GUM_SCALAR>::latentVariablesIds() const {
  NodeSet latentIds;
  for (auto it = _id2name_.begin(); it != _id2name_.end(); ++it) {
       latentIds.insert(it.first());
 }
  return latentIds;
}

template <typename GUM_SCALAR>
Set<std::string> CausalModel<GUM_SCALAR>::latentVariablesNames() const {
  Set<std::string> latentNames;
  for (auto it = _id2name_.begin(); it != _id2name_.end(); ++it) {
      latentNames.insert(it.second());
  }
  return latentNames;
}

// ===============================
// Connected components
// ===============================

// template <typename GUM_SCALAR>
// HashTable<Size, NodeSet>
// CausalModel<GUM_SCALAR>::connectedComponents() const {
//   GUM_ERROR(NotImplemented, "CausalModel::connectedComponents is not implemented yet.");
//   return {};
// }

// ===============================
// DOT export
// ===============================
template <typename GUM_SCALAR>
std::string CausalModel<GUM_SCALAR>::toDot(const bool SHOW_LATENT_NAMES,
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
  for (const auto n : _causalDAG_.nodes()) {
    if (latentVariablesIds().contains(n)) {
      std::string label;
      try { label = nameFromId(n); } catch (gum::NotFound&) { label = "L" + std::to_string(n); }
      out << "  \"" << label << "\" "
          << "[fillcolor=\"" << NODE_BG << "\", "
          <<  "fontcolor=\"" << NODE_FG << "\", "
          <<  "style=filled,shape=" << (SHOW_LATENT_NAMES ? "ellipse" : "point") << "];\n";
    }
  }

  // Then, observed nodes
  for (const auto n : _causalDAG_.nodes()) {
    if (!latentVariablesIds().contains(n)) {
      std::string label;
      try { label = nameFromId(n); } catch (gum::NotFound&) { label = "L" + std::to_string(n); }
      out << "  \"" << label << "\" "
          << "[fillcolor=\"" << NODE_BG << "\", "
          <<  "fontcolor=\"" << NODE_FG << "\", "
          <<  "style=filled,shape=ellipse];\n";
    }
  }

  // Edges
  for (const auto tail : _causalDAG_.nodes()) {
    for (const auto head : _causalDAG_.children(tail)) {
      std::string lt, lh;
      try { lt = nameFromId(tail); } catch (gum::NotFound&) { lt = "L" + std::to_string(tail); }
      try { lh = nameFromId(head); } catch (gum::NotFound&) { lh = "L" + std::to_string(head); }

      const bool dashed = latentVariablesIds().contains(tail) || latentVariablesIds().contains(head);

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

template <typename GUM_SCALAR>
Set<std::string>
CausalModel<GUM_SCALAR>::names() const {
    Set<std::string> names;
    for (const auto n : _observedBN_.nodes()) {
        names.insert(_observedBN_.variable(n).name());
    }
    // latent names (from bijection)
    for (auto it = _id2name_.begin(); it != _id2name_.end(); ++it) {
        names.insert(it.second());
    }
    return names;

}

template <typename GUM_SCALAR>
NodeId CausalModel<GUM_SCALAR>::idFromName(const std::string& name) const {
  if (_id2name_.existsSecond(name)) {
    return _id2name_.first(name);
  }
  else {
   return _observedBN_.idFromName(name);
  }
}

// implement const std::string& nameFromId(NodeId id) const;
template <typename GUM_SCALAR>
std::string CausalModel<GUM_SCALAR>::nameFromId(NodeId id) const {
    if (_id2name_.existsFirst(id)) {
        return _id2name_.second(id);
    }
    else {
        return _observedBN_.variable(id).name();
    }
}


} // namespace gum
