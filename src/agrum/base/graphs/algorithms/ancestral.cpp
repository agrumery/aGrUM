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

namespace gum::graph {

  namespace {
    /// the nodes from which a node of x is reachable backwards through the edges accepted by follow
    template < typename FOLLOW >
    NodeSet backwardClosure(const MarkedGraph& g, const NodeSet& x, FOLLOW follow) {
      NodeSet               result(x);
      std::vector< NodeId > stack(x.begin(), x.end());
      while (!stack.empty()) {
        const NodeId v = stack.back();
        stack.pop_back();
        for (const auto w: g.neighbours(v))
          if (!result.contains(w) && follow(w, v)) {
            result.insert(w);
            stack.push_back(w);
          }
      }
      return result;
    }
  }   // namespace

  NodeSet ancestors(const MarkedGraph& g, const NodeSet& x) {
    return backwardClosure(g, x, [&g](NodeId w, NodeId v) { return g.isDefinitelyDirected(w, v); });
  }

  NodeSet anteriors(const MarkedGraph& g, const NodeSet& x) {
    return backwardClosure(g, x, [&g](NodeId w, NodeId v) {
      return g.isDefinitelyDirected(w, v) || g.isUndirected(w, v);
    });
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
      if (anteriors(g, into).contains(a)) return false;
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
        auto z = anteriors(g, NodeSet{a, b});
        z.erase(a);
        z.erase(b);
        if (!mSeparation(g, NodeSet{a}, NodeSet{b}, z)) return false;
      }
    return true;
  }

  bool mSeparation(const MarkedGraph& g, const NodeSet& x, const NodeSet& y, const NodeSet& z) {
    if (!(x * y).empty()) GUM_ERROR(InvalidArgument, "NodeSets X and Y must be disjoint.")
    const auto anteriorToZ = anteriors(g, z);
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
