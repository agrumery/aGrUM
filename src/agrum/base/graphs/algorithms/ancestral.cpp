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
 * @brief Algorithms for ancestral graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#include <utility>
#include <vector>

#include <agrum/base/graphs/algorithms/ancestral.h>
#include <agrum/base/graphs/algorithms/generic/reachability.h>
#include <agrum/base/graphs/algorithms/generic/separation.h>
#include <agrum/base/graphs/MAG.h>

namespace gum::graph {

  namespace {
    /// visits the nodes w with an edge w->v (parents of v)
    auto forEachParent(const MarkedGraph& g) {
      return [&g](NodeId v, const auto& visit) {
        for (const auto w: g.neighbours(v))
          if (g.isDefinitelyDirected(w, v)) visit(w);
      };
    }

    /// visits the nodes w with an edge v->w (children of v)
    auto forEachChild(const MarkedGraph& g) {
      return [&g](NodeId v, const auto& visit) {
        for (const auto w: g.neighbours(v))
          if (g.isDefinitelyDirected(v, w)) visit(w);
      };
    }

    /// visits the nodes w with an edge w->v or w-v (a step of an anterior path towards v)
    auto forEachAnteriorStepTo(const MarkedGraph& g) {
      return [&g](NodeId v, const auto& visit) {
        for (const auto w: g.neighbours(v))
          if (g.isDefinitelyDirected(w, v) || g.isUndirected(w, v)) visit(w);
      };
    }

    /// the nodes with a walk of anterior steps to a node of x: ant(x) \ x, but the nodes of x
    /// reached by a walk (enough for the internal uses, cheaper than anteriors)
    NodeSet anteriorWalks(const MarkedGraph& g, const NodeSet& x) {
      return detail::directedClosure(x, forEachAnteriorStepTo(g));
    }
  }   // namespace

  NodeSet ancestors(const MarkedGraph& g, const NodeSet& x) {
    return detail::directedClosure(x, forEachParent(g));
  }

  NodeSet descendants(const MarkedGraph& g, const NodeSet& x) {
    return detail::directedClosure(x, forEachChild(g));
  }

  NodeSet anteriors(const MarkedGraph& g, const NodeSet& x) {
    return detail::pathClosure(x, forEachAnteriorStepTo(g));
  }

  bool isAncestral(const MarkedGraph& g) {
    for (const auto& edge: g.edges())
      if (g.isCircle(edge.first(), edge.second()) || g.isCircle(edge.second(), edge.first()))
        return false;
    for (const auto a: g.nodes()) {
      NodeSet into;   // the parents and the spouses of a
      for (const auto b: g.neighbours(a))
        if (g.isArrowhead(b, a)) into.insert(b);
      if (into.empty()) continue;
      if (!g.undirectedNeighbours(a).empty()) return false;
      // a must not be anterior to a node of into
      if (anteriorWalks(g, into).contains(a)) return false;   // a is not in into
    }
    return true;
  }

  bool isMaximal(const MarkedGraph& g) {
    std::vector< NodeId > nodes(g.nodes().begin(), g.nodes().end());
    for (std::size_t i = 0; i < nodes.size(); ++i)
      for (std::size_t j = i + 1; j < nodes.size(); ++j) {
        const NodeId a = nodes[i];
        const NodeId b = nodes[j];
        if (g.existsEdge(a, b)) continue;
        auto z = anteriorWalks(g, NodeSet{a, b});
        z.erase(a);
        z.erase(b);
        if (!mSeparation(g, NodeSet{a}, NodeSet{b}, z)) return false;
      }
    return true;
  }

  NodeSet ancestors(const MAG& g, const NodeSet& x) { return ancestors(g.graph(), x); }

  NodeSet descendants(const MAG& g, const NodeSet& x) { return descendants(g.graph(), x); }

  NodeSet anteriors(const MAG& g, const NodeSet& x) { return anteriors(g.graph(), x); }

  bool isAncestral(const MAG& g) { return isAncestral(g.graph()); }

  bool isMaximal(const MAG& g) { return isMaximal(g.graph()); }

  bool mSeparation(const MarkedGraph& g, const NodeSet& x, const NodeSet& y, const NodeSet& z) {
    if (!(x * y).empty()) GUM_ERROR(InvalidArgument, "NodeSets X and Y must be disjoint.")
    detail::checkSeparationNodes(g, x, y, z);
    const auto anteriorToZ = anteriorWalks(g, z) + z;   // ant(z) of Richardson and Spirtes
    // as graph::dSeparated: an observed node of x or y is separated from any node (it remains a
    // node of z on the paths between the other ones)
    const auto sources = x - z;
    const auto targets = y - z;
    // the reachable states (node, an arrowhead at the node on the edge used to reach it)
    Set< std::pair< NodeId, bool > >         visited;
    std::vector< std::pair< NodeId, bool > > stack;
    for (const auto a: sources)
      for (const auto w: g.neighbours(a)) {
        const std::pair< NodeId, bool > state{w, g.isArrowhead(a, w)};
        if (!visited.contains(state)) {
          visited.insert(state);
          stack.push_back(state);
        }
      }
    while (!stack.empty()) {
      const auto [v, arrowheadAtV] = stack.back();
      stack.pop_back();
      if (targets.contains(v)) return false;
      if (sources.contains(v)) continue;
      for (const auto w: g.neighbours(v)) {
        const bool collider = arrowheadAtV && g.isArrowhead(w, v);
        if (collider ? !anteriorToZ.contains(v) : z.contains(v)) continue;
        const std::pair< NodeId, bool > state{w, g.isArrowhead(v, w)};
        if (!visited.contains(state)) {
          visited.insert(state);
          stack.push_back(state);
        }
      }
    }
    return true;
  }

  PDAG circleComponent(const MarkedGraph& g) {
    PDAG circles;
    for (const auto node: g.nodes())
      circles.addNodeWithId(node);
    for (const auto& edge: g.edges())
      if (g.isCircle(edge.first(), edge.second()) && g.isCircle(edge.second(), edge.first()))
        circles.addEdge(edge.first(), edge.second());
    return circles;
  }

  MarkedGraph representativeGraph(const MarkedGraph& g, const DAG& circleOrientation) {
    MarkedGraph result(g);
    Size        oriented = 0;
    for (const auto& edge: g.edges()) {
      const NodeId x         = edge.first();
      const NodeId y         = edge.second();
      const bool   circleAtX = g.isCircle(y, x);
      const bool   circleAtY = g.isCircle(x, y);
      if (circleAtX && circleAtY) {
        if (circleOrientation.existsArc(x, y)) {
          result.setMarkAt(y, x, EdgeMark::Tail);
          result.setMarkAt(x, y, EdgeMark::Arrowhead);
        } else if (circleOrientation.existsArc(y, x)) {
          result.setMarkAt(x, y, EdgeMark::Tail);
          result.setMarkAt(y, x, EdgeMark::Arrowhead);
        } else {
          GUM_ERROR(InvalidArgument, "The edge " << x << " o-o " << y << " is not oriented")
        }
        ++oriented;
      } else if (circleAtX) {
        // x o-> y: tail at x; y --o x: arrowhead at x
        result.setMarkAt(y, x, g.isArrowhead(x, y) ? EdgeMark::Tail : EdgeMark::Arrowhead);
      } else if (circleAtY) {
        result.setMarkAt(x, y, g.isArrowhead(y, x) ? EdgeMark::Tail : EdgeMark::Arrowhead);
      }
    }
    if (oriented != circleOrientation.sizeArcs())
      GUM_ERROR(InvalidArgument, "The orientation has arcs which are not edges o-o of the graph")
    return result;
  }

}   // namespace gum::graph
