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
 * @brief Outlined implementation of the maximal ancestral graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <vector>

#include <agrum/base/graphs/algorithms/ancestral.h>
#include <agrum/base/graphs/algorithms/orientation.h>
#include <agrum/base/graphs/MAG.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/MAG_inl.h>
#endif   // GUM_NO_INLINE

namespace gum {

  MAG::MAG(MarkedGraph&& g) : ImmutableGraph< MarkedGraph >(std::move(g)) { GUM_CONSTRUCTOR(MAG); }

  MAG::MAG(const MAG& g) : ImmutableGraph< MarkedGraph >(g) { GUM_CONS_CPY(MAG); }

  MAG::MAG(MAG&& g) : ImmutableGraph< MarkedGraph >(std::move(g)) { GUM_CONS_MOV(MAG); }

  MAG::~MAG() { GUM_DESTRUCTOR(MAG); }

  MAG& MAG::operator=(const MAG& g) {
    if (this != &g) {
      ImmutableGraph< MarkedGraph >::operator=(g);
      GUM_OP_CPY(MAG);
    }
    return *this;
  }

  MAG& MAG::operator=(MAG&& g) {
    if (this != &g) {
      ImmutableGraph< MarkedGraph >::operator=(std::move(g));
      GUM_OP_MOV(MAG);
    }
    return *this;
  }

  MAG MAG::fromDAG(const DAG& dag, const NodeSet& latents, const NodeSet& selection) {
    for (const auto node: latents + selection)
      if (!dag.existsNode(node))
        GUM_ERROR(InvalidArgument, "The node " << node << " is not in the DAG")
    if (!(latents * selection).empty())
      GUM_ERROR(InvalidArgument, "A node can not be both latent and selected")

    // an(v) = the ancestors of v, v included
    NodeProperty< NodeSet > an;
    for (const auto node: dag.nodes()) {
      auto ancestors = dag.ancestors(node);
      ancestors.insert(node);
      an.insert(node, std::move(ancestors));
    }
    NodeSet anSelection;
    for (const auto s: selection)
      anSelection += an[s];

    std::vector< NodeId > observed;
    MarkedGraph           mag;
    for (const auto node: dag.nodes())
      if (!latents.contains(node) && !selection.contains(node)) {
        observed.push_back(node);
        mag.addNodeWithId(node);
        if (dag.hasName(node)) mag.setName(node, dag.nameFromId(node));
      }

    for (std::size_t i = 0; i < observed.size(); ++i)
      for (std::size_t j = i + 1; j < observed.size(); ++j) {
        const NodeId a = observed[i];
        const NodeId b = observed[j];
        // adjacency: no inducing path w.r.t. selection and latents (RS02, Theorem 4.2 (iv))
        auto z = an[a] + an[b] + anSelection;
        z      = z - latents;
        z.erase(a);
        z.erase(b);
        if (dag.dSeparation(a, b, z)) continue;
        // orientation: a tail at a iff a is an ancestor of b or of a selected node
        const bool tailAtA = an[b].contains(a) || anSelection.contains(a);
        const bool tailAtB = an[a].contains(b) || anSelection.contains(b);
        mag.addEdge(a,
                    b,
                    tailAtA ? EdgeMark::Tail : EdgeMark::Arrowhead,
                    tailAtB ? EdgeMark::Tail : EdgeMark::Arrowhead);
      }
    return MAG(std::move(mag));
  }

  MAG MAG::fromPAG(const PAG& pag) {
    // the circle component as a DAG without unshielded collider: a consistent extension
    const auto oriented = graph::consistentExtension(graph::circleComponent(pag));
    if (!oriented.has_value())
      GUM_ERROR(InvalidArgument,
                "The PAG is not valid: its edges o-o can not be oriented without unshielded "
                "collider")
    auto g = graph::representativeGraph(pag, *oriented);

    if (!graph::isAncestral(g) || !graph::isMaximal(g))
      GUM_ERROR(InvalidArgument, "The PAG is not valid: its representative graph is not a MAG")
    return MAG(std::move(g));
  }

  MAG MAG::fromMarkedGraph(const MarkedGraph& g) {
    if (!graph::isAncestral(g))
      GUM_ERROR(InvalidArgument,
                "The marked graph is not a MAG: it has a circle or is not ancestral")
    if (!graph::isMaximal(g))
      GUM_ERROR(InvalidArgument, "The marked graph is not a MAG: it is not maximal")
    return MAG(MarkedGraph(g));
  }

  std::string MAG::toDot() const { return graph_.toDot(); }

  std::string MAG::toDot(const dot::DotOptions& options) const { return graph_.toDot(options); }

}   // namespace gum
