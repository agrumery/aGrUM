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


/** @file
 * @brief Outlined implementation of the maximally oriented PDAGs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <utility>

#include <agrum/base/graphs/algorithms/orientation.h>
#include <agrum/base/graphs/MPDAG.h>

namespace gum {

  MPDAG::MPDAG(PDAG&& g, DAG&& extension) :
      ImmutableMixedGraph< PDAG >(std::move(g), std::move(extension)) {
    GUM_CONSTRUCTOR(MPDAG);
  }

  MPDAG::MPDAG(const MPDAG& g) : ImmutableMixedGraph< PDAG >(g) { GUM_CONS_CPY(MPDAG); }

  MPDAG::MPDAG(MPDAG&& g) : ImmutableMixedGraph< PDAG >(std::move(g)) { GUM_CONS_MOV(MPDAG); }

  MPDAG::~MPDAG() { GUM_DESTRUCTOR(MPDAG); }

  MPDAG& MPDAG::operator=(const MPDAG& g) {
    if (this != &g) {
      ImmutableMixedGraph< PDAG >::operator=(g);
      GUM_OP_CPY(MPDAG);
    }
    return *this;
  }

  MPDAG& MPDAG::operator=(MPDAG&& g) {
    if (this != &g) {
      ImmutableMixedGraph< PDAG >::operator=(std::move(g));
      GUM_OP_MOV(MPDAG);
    }
    return *this;
  }

  MPDAG MPDAG::meekClosure(const PDAG& pdag) {
    auto extension = graph::consistentExtension(pdag);
    if (!extension.has_value())
      GUM_ERROR(InvalidArgument, "The PDAG is not extendable: it has no consistent extension")
    // the closure of an extendable PDAG is unique (Meek 1995): the extension remains one of it
    auto closure = graph::meekClosure(pdag, *extension);
    return MPDAG(std::move(closure), std::move(*extension));
  }

  MPDAG MPDAG::fromCPDAG(const CPDAG& cpdag, const ArcSet& arcs) {
    // built as a mixed graph then checked once by the PDAG constructor (PDAG::addArc would look for
    // a directed path at each arc)
    // the arcs are checked against the CPDAG, not against the graph being oriented: an edge
    // oriented both ways is a contradiction of the knowledge, not an arc foreign to the CPDAG
    const auto& graph = cpdag.graph();
    MixedGraph  oriented(graph);
    for (const auto& arc: arcs) {
      if (graph.existsArc(arc.tail(), arc.head())) continue;
      if (!graph.existsEdge(arc.tail(), arc.head()))
        GUM_ERROR(InvalidArc, "The arc " << arc << " is not an orientation of an edge of the CPDAG")
      if (!oriented.existsEdge(arc.tail(), arc.head()))
        GUM_ERROR(InvalidArgument,
                  "The background knowledge orients the edge " << arc.tail() << "-" << arc.head()
                                                               << " both ways")
      oriented.eraseEdge(Edge(arc.tail(), arc.head()));
      oriented.addArc(arc.tail(), arc.head());
    }
    try {
      const PDAG pdag(oriented);
      // the knowledge must keep a DAG of the class: an extension with the v-structures of the CPDAG
      auto extension = graph::consistentExtension(pdag);
      if (!extension.has_value() || !(graph::detail::cpdagGraph(*extension) == cpdag.graph()))
        GUM_ERROR(InvalidArgument, "The background knowledge is not consistent with the CPDAG")
      auto closure = graph::meekClosure(pdag, *extension);
      return MPDAG(std::move(closure), std::move(*extension));
    } catch (const InvalidDirectedCycle&) {
      GUM_ERROR(InvalidArgument, "The background knowledge has a directed cycle")
    }
  }

  MPDAG MPDAG::fromPDAG(const PDAG& pdag) {
    auto extension = graph::consistentExtension(pdag);
    if (!extension.has_value())
      GUM_ERROR(InvalidArgument, "The PDAG is not an MPDAG: it has no consistent extension")
    if (!(static_cast< const MixedGraph& >(graph::meekClosure(pdag, *extension))
          == static_cast< const MixedGraph& >(pdag)))
      GUM_ERROR(InvalidArgument, "The PDAG is not an MPDAG: it is not closed under the Meek rules")
    return MPDAG(PDAG(pdag), std::move(*extension));
  }

  bool MPDAG::contains(const DAG& dag) const {
    if (dag.size() != size() || dag.sizeArcs() != sizeArcs() + sizeEdges()) return false;
    for (const auto& arc: arcs())
      if (!dag.existsArc(arc.tail(), arc.head())) return false;
    for (const auto& edge: edges())
      if (!dag.existsArc(edge.first(), edge.second())
          && !dag.existsArc(edge.second(), edge.first()))
        return false;
    // the same v-structures as the class
    return graph::detail::cpdagGraph(dag) == graph::detail::cpdagGraph(extension_);
  }

  CPDAG MPDAG::cpdag() const { return CPDAG::fromDAG(extension_); }

}   // namespace gum
